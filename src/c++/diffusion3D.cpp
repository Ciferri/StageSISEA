/**
 * @file diffusion3D.cpp
 * @brief
 * @author Carlos Sosa Marrero
 * @author Nicolas Ciferri
 * @author Alfredo Hernandez
 * @date 05.19.17
 */

#include "diffusion3D.hpp"


/**
 * Contructeur.
 * Création d'un vecteur de prostateCell
 */
diffusion3D::diffusion3D(const int nrow, const int ncol,const int nlayer,
			 const string nFInVes,
			 double Vmax,
			 double K_conso) : Model(0,0,0,0,ncol*nrow*nlayer)
{
	m_nrow   = nrow;
	m_ncol   = ncol;
	m_nlayer = nlayer;
	double vectTissue[ncol*nrow*nlayer];
	// creation of the cells composing the tissu model
	for (int i=0;i<getNumComp();i++)
	{
		m_comp->at(i) = new prostateCell(this);				/**< Création du vecteur de prostateCell */
		m_numOut += (m_comp->at(i))->getNumOut();
	}
	ifstream tissueLoad(nFInVes.c_str());
	if(tissueLoad){
		int i=0;
		string valeur;
		while (std::getline(tissueLoad, valeur)){
			istringstream ( valeur ) >> vectTissue[i];
			vectorTissue.push_back(vectTissue[i]);
			i++;
		}
	}
	int count = 0;
	for (int k=0; k<m_nlayer;k++){
		for (int i=0;i<m_nrow;i++){
			for (int j=0;j<m_ncol;j++){
				Tissue.push_back( m_comp->at(count) );	/**< tableau du tissue */
				count += 1;
			}
		}
	}

	coeff_diff = 1.835*pow(10,-9);
	size = 20;
	time_step_s = 0.07;
	coupParam = (coeff_diff*pow(10,12)*time_step_s)/(size*size);
	m_Vmax = Vmax;
	m_K_conso = K_conso;

	for(int i=0;i<FILTERSIZE;i++){
		for(int j=0;j<FILTERSIZE;j++){
			for(int k=0;k<FILTERSIZE;k++){
				if( ((k==1) && (j==1)) && (i==1) ){
					filterIn[i][j][k] = -26;
				}else{
					filterIn[i][j][k] = 1;
				}
				filterOut[i][j][k] = -filterIn[i][j][k];
			}
		}
	}
}

/**
 *
 * Destructeur
 *
 */
diffusion3D::~diffusion3D()
{
}

int diffusion3D::startModel()
{
	for (int i=0;i<getNumComp();i++)
		(m_comp->at(i))->startModel();
	return 0;
}

/**
 *
 * Initialisation du tissue.
 *
 * */
int diffusion3D::initModel(const double DT)
{
	for (int i=0;i<getNumComp();i++)
	{
		((prostateCell *)(m_comp->at(i)))->initModel(DT);	/**< Fonction qui initialise toute les pO2 à zéro */
	}
	return 1;
}

/**
 *
 * Surcharge de la classe initModel
 *
 * */
int diffusion3D::initModel(int x, int y, int z, double pO2)
{
	int indice = Coord_XY_to_K(x,y) + z*m_ncol*m_nrow;		/**< Fonction qui prend en paramètre x et y et renvoie un indice */
	((prostateCell *)(m_comp->at(indice)))->initModel(0.0, 0.0, 0.0, 1.0, pO2); 	/**< Pour un vaisseau, la pO2 est de 42mmHg */
	return 1;
}

int diffusion3D::calcModelOut()
{
	for (int i=0;i<getNumComp();i++)
		(m_comp->at(i))->calcModelOut();
	return 0;
}

/**
 *
 * Mise à jour du tissue
 *
 * */
int diffusion3D::updateModel(double time, const double DT)
{
	double tempGradIn;
	double tempGradOut;

	double resDiffusionIn[m_nlayer][m_ncol][m_nrow];
	double resDiffusionOut[m_nlayer][m_ncol][m_nrow];
	double initImageTissue[m_nlayer][m_ncol][m_nrow];
	double Conso_PO2[m_nlayer][m_ncol][m_nrow];

	Model *tissue[m_nlayer][m_ncol][m_nrow];

	int count = 0;
	for (int k=0; k<m_nlayer;k++)
	{
		for (int i=0;i<m_nrow;i++)
		{
			for (int j=0;j<m_ncol;j++)
			{
				tissue[k][j][i] = Tissue.at(count);
				count += 1;
			}
		}
	}
	//updating of all cells
	for (int i=0;i<getNumComp();i++)
		(m_comp->at(i))->updateModel(time,DT);
		
	int cpt = 0;
	for (int k=0;k<m_nlayer;k++){
		for (int i=0;i<m_nrow;i++){
			for (int j=0;j<m_ncol;j++){
				if(vectorTissue.at(cpt) != 0)
					initModel(j,i,k,INIT_VASCULAR_PO2);
				cpt += 1;
			}
		}
	}
	
	for (int k=0;k<m_nlayer;k++)
		for (int i=0;i<m_ncol;i++)
			for (int j=0;j<m_nrow;j++){
				initImageTissue[k][i][j] = ((prostateCell*)tissue[k][i][j])->getST_X();
				Conso_PO2[k][i][j] = (initImageTissue[k][i][j]*m_Vmax*time_step_s)/(m_K_conso + initImageTissue[k][i][j]);
			}

	// /////////////////// //
	// --Cas particulier-- //
	// /////////////////// //

	/*
	 * Représentation d'un cube avec 6 faces (nord,sud est, ouest, seuil et top)
	 * */
	///////////////////
	//  Sommets‎ (8)  //
	///////////////////

	/**
	 * First row / First col / First height
	 * */
	if(m_nlayer >= 3){
		tempGradIn =
			filterIn[1][1][2]*((prostateCell*)tissue[0][1][0])->getST_X() +
			filterIn[1][2][1]*((prostateCell*)tissue[0][0][1])->getST_X() +
			filterIn[1][2][2]*((prostateCell*)tissue[0][1][1])->getST_X() +
			filterIn[2][1][1]*((prostateCell*)tissue[1][0][0])->getST_X() +
			filterIn[2][1][2]*((prostateCell*)tissue[1][1][0])->getST_X() +
			filterIn[2][2][1]*((prostateCell*)tissue[1][0][1])->getST_X() +
			filterIn[2][2][2]*((prostateCell*)tissue[1][1][1])->getST_X() -
			(filterIn[1][1][2]+filterIn[1][2][1]+filterIn[1][2][2]+
			 filterIn[2][1][1]+filterIn[2][1][2]+filterIn[2][2][1]+filterIn[2][2][2])*
			((prostateCell*)tissue[0][0][0])->getST_X();
		resDiffusionIn[0][0][0] = tempGradIn*coupParam/7;

		tempGradOut =
			filterOut[1][1][2]*((prostateCell*)tissue[0][1][0])->getST_X() +
			filterOut[1][2][1]*((prostateCell*)tissue[0][0][1])->getST_X() +
			filterOut[1][2][2]*((prostateCell*)tissue[0][1][1])->getST_X() +
			filterOut[2][1][1]*((prostateCell*)tissue[1][0][0])->getST_X() +
			filterOut[2][1][2]*((prostateCell*)tissue[1][1][0])->getST_X() +
			filterOut[2][2][1]*((prostateCell*)tissue[1][0][1])->getST_X() +
			filterOut[2][2][2]*((prostateCell*)tissue[1][1][1])->getST_X() -
			(filterOut[1][1][2]+filterOut[1][2][1]+filterOut[1][2][2]+
			filterOut[2][1][1]+filterOut[2][1][2]+filterOut[2][2][1]+filterOut[2][2][2])*
			((prostateCell*)tissue[0][0][0])->getST_X();
		resDiffusionOut[0][0][0] = tempGradOut*coupParam/7;
	

		/**
		 * First row / Last col / First Height
		 * */
		tempGradIn =
			filterIn[1][1][0]*((prostateCell*)tissue[0][m_ncol-2][0])->getST_X() +
			filterIn[1][2][0]*((prostateCell*)tissue[0][m_ncol-2][1])->getST_X() +
			filterIn[1][2][1]*((prostateCell*)tissue[0][m_ncol-1][1])->getST_X() +
			filterIn[2][1][1]*((prostateCell*)tissue[1][m_ncol-1][0])->getST_X() +
			filterIn[2][1][0]*((prostateCell*)tissue[1][m_ncol-2][0])->getST_X() +
			filterIn[2][2][0]*((prostateCell*)tissue[1][m_ncol-2][1])->getST_X() +
			filterIn[2][2][1]*((prostateCell*)tissue[1][m_ncol-1][1])->getST_X() -
			(filterIn[1][1][0]+filterIn[1][2][1]+filterIn[1][2][0]+
			filterIn[2][1][1]+filterIn[2][1][0]+filterIn[2][2][0]+filterIn[2][2][1])*
			((prostateCell*)tissue[0][m_ncol-1][0])->getST_X();
		resDiffusionIn[0][m_ncol-1][0] = tempGradIn*coupParam/7;

		tempGradOut =
			filterOut[1][1][0]*((prostateCell*)tissue[0][m_ncol-2][0])->getST_X() +
			filterOut[1][2][0]*((prostateCell*)tissue[0][m_ncol-2][1])->getST_X() +
			filterOut[1][2][1]*((prostateCell*)tissue[0][m_ncol-1][1])->getST_X() +
			filterOut[2][1][1]*((prostateCell*)tissue[1][m_ncol-1][0])->getST_X() +
			filterOut[2][1][0]*((prostateCell*)tissue[1][m_ncol-2][0])->getST_X() +
			filterOut[2][2][0]*((prostateCell*)tissue[1][m_ncol-2][1])->getST_X() +
			filterOut[2][2][1]*((prostateCell*)tissue[1][m_ncol-1][1])->getST_X() -
			(filterOut[1][1][0]+filterOut[1][2][1]+filterOut[1][2][0]+
			filterOut[2][1][1]+filterOut[2][1][0]+filterOut[2][2][0]+filterOut[2][2][1])*
			((prostateCell*)tissue[0][m_ncol-1][0])->getST_X();
		resDiffusionOut[0][m_ncol-1][0] = tempGradOut*coupParam/7;

		/**
		 * Last row / First col / First height
		 * */
		tempGradIn =
			filterIn[1][0][1]*((prostateCell*)tissue[0][0][m_nrow-2])->getST_X() +
			filterIn[1][0][2]*((prostateCell*)tissue[0][1][m_nrow-2])->getST_X() +
			filterIn[1][1][2]*((prostateCell*)tissue[0][1][m_nrow-1])->getST_X() +
			filterIn[2][1][1]*((prostateCell*)tissue[1][0][m_nrow-1])->getST_X() +
			filterIn[2][0][1]*((prostateCell*)tissue[1][0][m_nrow-2])->getST_X() +
			filterIn[2][0][2]*((prostateCell*)tissue[1][1][m_nrow-2])->getST_X() +
			filterIn[2][1][2]*((prostateCell*)tissue[1][1][m_nrow-1])->getST_X() -
			(filterIn[1][0][1]+filterIn[1][0][2]+filterIn[1][1][2]+
			filterIn[2][1][1]+filterIn[2][0][1]+filterIn[2][0][2]+filterIn[2][1][2])*
			((prostateCell*)tissue[0][0][m_nrow-1])->getST_X();
		resDiffusionIn[0][0][m_nrow-1] = tempGradIn*coupParam/7;

		tempGradOut =
			filterOut[1][0][1]*((prostateCell*)tissue[0][0][m_nrow-2])->getST_X() +
			filterOut[1][0][2]*((prostateCell*)tissue[0][1][m_nrow-2])->getST_X() +
			filterOut[1][1][2]*((prostateCell*)tissue[0][1][m_nrow-1])->getST_X() +
			filterOut[2][1][1]*((prostateCell*)tissue[1][0][m_nrow-1])->getST_X() +
			filterOut[2][0][1]*((prostateCell*)tissue[1][0][m_nrow-2])->getST_X() +
			filterOut[2][0][2]*((prostateCell*)tissue[1][1][m_nrow-2])->getST_X() +
			filterOut[2][1][2]*((prostateCell*)tissue[1][1][m_nrow-1])->getST_X() -
			(filterOut[1][0][1]+filterOut[1][0][2]+filterOut[1][1][2]+
			filterOut[2][1][1]+filterOut[2][0][1]+filterOut[2][0][2]+filterOut[2][1][2])*
			((prostateCell*)tissue[0][0][m_nrow-1])->getST_X();
		resDiffusionOut[0][0][m_nrow-1] = tempGradOut*coupParam/7;

		/**
		 * Last row / Last col / First height
		 * */
		tempGradIn =
			filterIn[1][0][0]*((prostateCell*)tissue[0][m_ncol-2][m_nrow-2])->getST_X() +
			filterIn[1][0][1]*((prostateCell*)tissue[0][m_ncol-1][m_nrow-2])->getST_X() +
			filterIn[1][1][0]*((prostateCell*)tissue[0][m_ncol-2][m_nrow-1])->getST_X() +
			filterIn[2][1][1]*((prostateCell*)tissue[1][m_ncol-1][m_nrow-1])->getST_X() +
			filterIn[2][0][0]*((prostateCell*)tissue[1][m_ncol-2][m_nrow-2])->getST_X() +
			filterIn[2][0][1]*((prostateCell*)tissue[1][m_ncol-2][m_nrow-2])->getST_X() +
			filterIn[2][1][0]*((prostateCell*)tissue[1][m_ncol-1][m_nrow-1])->getST_X() -
			(filterIn[1][0][0]+filterIn[1][0][1]+filterIn[1][1][0]+
			filterIn[2][1][1]+filterIn[2][0][1]+filterIn[2][0][0]+filterIn[2][1][0])*
			((prostateCell*)tissue[0][m_ncol-1][m_nrow-1])->getST_X();
		resDiffusionIn[0][m_ncol-1][m_nrow-1] = tempGradIn*coupParam/7;

		tempGradOut =
			filterOut[1][0][0]*((prostateCell*)tissue[0][m_ncol-2][m_nrow-2])->getST_X() +
			filterOut[1][0][1]*((prostateCell*)tissue[0][m_ncol-1][m_nrow-2])->getST_X() +
			filterOut[1][1][0]*((prostateCell*)tissue[0][m_ncol-2][m_nrow-1])->getST_X() +
			filterOut[2][1][1]*((prostateCell*)tissue[1][m_ncol-1][m_nrow-1])->getST_X() +
			filterOut[2][0][0]*((prostateCell*)tissue[1][m_ncol-2][m_nrow-2])->getST_X() +
			filterOut[2][0][1]*((prostateCell*)tissue[1][m_ncol-2][m_nrow-2])->getST_X() +
			filterOut[2][1][0]*((prostateCell*)tissue[1][m_ncol-1][m_nrow-1])->getST_X() -
			(filterOut[1][0][0]+filterOut[1][0][1]+filterOut[1][1][0]+
			 filterOut[2][1][1]+filterOut[2][0][1]+filterOut[2][0][0]+filterOut[2][1][0])*
			((prostateCell*)tissue[0][m_ncol-1][m_nrow-1])->getST_X();
		resDiffusionOut[0][m_ncol-1][m_nrow-1] = tempGradOut*coupParam/7;

		/**
		 * First row / First col / Last height
		 * */
		tempGradIn =
			filterIn[1][1][2]*((prostateCell*)tissue[m_nlayer-1][1][0])->getST_X() +
			filterIn[1][2][1]*((prostateCell*)tissue[m_nlayer-1][0][1])->getST_X() +
			filterIn[1][2][2]*((prostateCell*)tissue[m_nlayer-1][1][1])->getST_X() +
			filterIn[2][1][1]*((prostateCell*)tissue[m_nlayer-2][0][0])->getST_X() +
			filterIn[2][1][2]*((prostateCell*)tissue[m_nlayer-2][1][0])->getST_X() +
			filterIn[2][2][1]*((prostateCell*)tissue[m_nlayer-2][0][1])->getST_X() +
			filterIn[2][2][2]*((prostateCell*)tissue[m_nlayer-2][1][1])->getST_X() -
			(filterIn[1][1][2]+filterIn[1][2][1]+filterIn[1][2][2]+
			 filterIn[2][1][1]+filterIn[2][1][2]+filterIn[2][2][1]+filterIn[2][2][2])*
			((prostateCell*)tissue[m_nlayer-1][0][0])->getST_X();
		resDiffusionIn[m_nlayer-1][0][0] = tempGradIn*coupParam/7;

		tempGradOut =
			filterOut[1][1][2]*((prostateCell*)tissue[m_nlayer-1][1][0])->getST_X() +
			filterOut[1][2][1]*((prostateCell*)tissue[m_nlayer-1][0][1])->getST_X() +
			filterOut[1][2][2]*((prostateCell*)tissue[m_nlayer-1][1][1])->getST_X() +
			filterOut[2][1][1]*((prostateCell*)tissue[m_nlayer-2][0][0])->getST_X() +
			filterOut[2][1][2]*((prostateCell*)tissue[m_nlayer-2][1][0])->getST_X() +
			filterOut[2][2][1]*((prostateCell*)tissue[m_nlayer-2][0][1])->getST_X() +
			filterOut[2][2][2]*((prostateCell*)tissue[m_nlayer-2][1][1])->getST_X() -
			(filterOut[1][1][2]+filterOut[1][2][1]+filterOut[1][2][2]+
			 filterOut[2][1][1]+filterOut[2][1][2]+filterOut[2][2][1]+filterOut[2][2][2])*
			((prostateCell*)tissue[m_nlayer-1][0][0])->getST_X();
		resDiffusionOut[m_nlayer-1][0][0] = tempGradOut*coupParam/7;


		/**
		 * First row / Last col / Last Height
		 * */
		tempGradIn =
			filterIn[1][1][0]*((prostateCell*)tissue[m_nlayer-1][m_ncol-2][0])->getST_X() +
			filterIn[1][2][0]*((prostateCell*)tissue[m_nlayer-1][m_ncol-2][1])->getST_X() +
			filterIn[1][2][1]*((prostateCell*)tissue[m_nlayer-1][m_ncol-1][1])->getST_X() +
			filterIn[2][1][1]*((prostateCell*)tissue[m_nlayer-2][m_ncol-1][0])->getST_X() +
			filterIn[2][1][0]*((prostateCell*)tissue[m_nlayer-2][m_ncol-2][0])->getST_X() +
			filterIn[2][2][0]*((prostateCell*)tissue[m_nlayer-2][m_ncol-2][1])->getST_X() +
			filterIn[2][2][1]*((prostateCell*)tissue[m_nlayer-2][m_ncol-1][1])->getST_X() -
			(filterIn[1][1][0]+filterIn[1][2][1]+filterIn[1][2][0]+
			 filterIn[2][1][1]+filterIn[2][1][0]+filterIn[2][2][0]+filterIn[2][2][1])*
			((prostateCell*)tissue[m_nlayer-1][m_ncol-1][0])->getST_X();
		resDiffusionIn[m_nlayer-1][m_ncol-1][0] = tempGradIn*coupParam/7;

		tempGradOut =
			filterOut[1][1][0]*((prostateCell*)tissue[m_nlayer-1][m_ncol-2][0])->getST_X() +
			filterOut[1][2][0]*((prostateCell*)tissue[m_nlayer-1][m_ncol-2][1])->getST_X() +
			filterOut[1][2][1]*((prostateCell*)tissue[m_nlayer-1][m_ncol-1][1])->getST_X() +
			filterOut[2][1][1]*((prostateCell*)tissue[m_nlayer-2][m_ncol-1][0])->getST_X() +
			filterOut[2][1][0]*((prostateCell*)tissue[m_nlayer-2][m_ncol-2][0])->getST_X() +
			filterOut[2][2][0]*((prostateCell*)tissue[m_nlayer-2][m_ncol-2][1])->getST_X() +
			filterOut[2][2][1]*((prostateCell*)tissue[m_nlayer-2][m_ncol-1][1])->getST_X() -
			(filterOut[1][1][0]+filterOut[1][2][1]+filterOut[1][2][0]+
			 filterOut[2][1][1]+filterOut[2][1][0]+filterOut[2][2][0]+filterOut[2][2][1])*
			((prostateCell*)tissue[m_nlayer-1][m_ncol-1][0])->getST_X();
		resDiffusionOut[m_nlayer-1][m_ncol-1][0] = tempGradOut*coupParam/7;

		/**
		 * Last row / First col / Last height
		 * */
		tempGradIn =
			filterIn[1][0][1]*((prostateCell*)tissue[m_nlayer-1][0][m_nrow-2])->getST_X() +
			filterIn[1][0][2]*((prostateCell*)tissue[m_nlayer-1][1][m_nrow-2])->getST_X() +
			filterIn[1][1][2]*((prostateCell*)tissue[m_nlayer-1][1][m_nrow-1])->getST_X() +
			filterIn[2][1][1]*((prostateCell*)tissue[m_nlayer-2][0][m_nrow-1])->getST_X() +
			filterIn[2][0][1]*((prostateCell*)tissue[m_nlayer-2][0][m_nrow-2])->getST_X() +
			filterIn[2][0][2]*((prostateCell*)tissue[m_nlayer-2][1][m_nrow-2])->getST_X() +
			filterIn[2][1][2]*((prostateCell*)tissue[m_nlayer-2][1][m_nrow-1])->getST_X() -
			(filterIn[1][0][1]+filterIn[1][0][2]+filterIn[1][1][2]+
			 filterIn[2][1][1]+filterIn[2][0][1]+filterIn[2][0][2]+filterIn[2][1][2])*
			((prostateCell*)tissue[m_nlayer-1][0][m_nrow-1])->getST_X();
		resDiffusionIn[m_nlayer-1][0][m_nrow-1] = tempGradIn*coupParam/7;

		tempGradOut =
			filterOut[1][0][1]*((prostateCell*)tissue[m_nlayer-1][0][m_nrow-2])->getST_X() +
			filterOut[1][0][2]*((prostateCell*)tissue[m_nlayer-1][1][m_nrow-2])->getST_X() +
			filterOut[1][1][2]*((prostateCell*)tissue[m_nlayer-1][1][m_nrow-1])->getST_X() +
			filterOut[2][1][1]*((prostateCell*)tissue[m_nlayer-2][0][m_nrow-1])->getST_X() +
			filterOut[2][0][1]*((prostateCell*)tissue[m_nlayer-2][0][m_nrow-2])->getST_X() +
			filterOut[2][0][2]*((prostateCell*)tissue[m_nlayer-2][1][m_nrow-2])->getST_X() +
			filterOut[2][1][2]*((prostateCell*)tissue[m_nlayer-2][1][m_nrow-1])->getST_X() -
			(filterOut[1][0][1]+filterOut[1][0][2]+filterOut[1][1][2]+
			 filterOut[2][1][1]+filterOut[2][0][1]+filterOut[2][0][2]+filterOut[2][1][2])*
			((prostateCell*)tissue[m_nlayer-1][0][m_nrow-1])->getST_X();
		resDiffusionOut[m_nlayer-1][0][m_nrow-1] = tempGradOut*coupParam/7;

		/**
		 * Last row / Last col / Last height
		 * */
		tempGradIn =
			filterIn[1][0][0]*((prostateCell*)tissue[m_nlayer-1][m_ncol-2][m_nrow-2])->getST_X() +
			filterIn[1][0][1]*((prostateCell*)tissue[m_nlayer-1][m_ncol-1][m_nrow-2])->getST_X() +
			filterIn[1][1][0]*((prostateCell*)tissue[m_nlayer-1][m_ncol-2][m_nrow-1])->getST_X() +
			filterIn[2][1][1]*((prostateCell*)tissue[m_nlayer-2][m_ncol-1][m_nrow-1])->getST_X() +
			filterIn[2][0][0]*((prostateCell*)tissue[m_nlayer-2][m_ncol-2][m_nrow-2])->getST_X() +
			filterIn[2][0][1]*((prostateCell*)tissue[m_nlayer-2][m_ncol-2][m_nrow-2])->getST_X() +
			filterIn[2][1][0]*((prostateCell*)tissue[m_nlayer-2][m_ncol-1][m_nrow-1])->getST_X() -
			(filterIn[1][0][0]+filterIn[1][0][1]+filterIn[1][1][0]+
			 filterIn[2][1][1]+filterIn[2][0][1]+filterIn[2][0][0]+filterIn[2][1][0])*
			((prostateCell*)tissue[m_nlayer-1][m_ncol-1][m_nrow-1])->getST_X();
		resDiffusionIn[m_nlayer-1][m_ncol-1][m_nrow-1] = tempGradIn*coupParam/7;

		tempGradOut =
			filterOut[1][0][0]*((prostateCell*)tissue[m_nlayer-1][m_ncol-2][m_nrow-2])->getST_X() +
			filterOut[1][0][1]*((prostateCell*)tissue[m_nlayer-1][m_ncol-1][m_nrow-2])->getST_X() +
			filterOut[1][1][0]*((prostateCell*)tissue[m_nlayer-1][m_ncol-2][m_nrow-1])->getST_X() +
			filterOut[2][1][1]*((prostateCell*)tissue[m_nlayer-2][m_ncol-1][m_nrow-1])->getST_X() +
			filterOut[2][0][0]*((prostateCell*)tissue[m_nlayer-2][m_ncol-2][m_nrow-2])->getST_X() +
			filterOut[2][0][1]*((prostateCell*)tissue[m_nlayer-2][m_ncol-2][m_nrow-2])->getST_X() +
			filterOut[2][1][0]*((prostateCell*)tissue[m_nlayer-2][m_ncol-1][m_nrow-1])->getST_X() -
			(filterOut[1][0][0]+filterOut[1][0][1]+filterOut[1][1][0]+
			 filterOut[2][1][1]+filterOut[2][0][1]+filterOut[2][0][0]+filterOut[2][1][0])*
			((prostateCell*)tissue[m_nlayer-1][m_ncol-1][m_nrow-1])->getST_X();
		resDiffusionOut[m_nlayer-1][m_ncol-1][m_nrow-1] = tempGradOut*coupParam/7;


		///////////////////
		//  Arêtes (12)  //
		///////////////////
		/**
		 * Seuil / Nord
		 * */
		for (int i=1;i<m_ncol-1;i++)
			{
	tempGradIn =
		filterIn[0][0][0]*((prostateCell*)tissue[0][i-1][0])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[0][i-1][1])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[1][i-1][0])->getST_X() +
		filterIn[0][1][0]*((prostateCell*)tissue[1][i-1][1])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[0][i][1])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[1][i][0])->getST_X() +
		filterIn[0][1][0]*((prostateCell*)tissue[1][i][1])->getST_X() +
		filterIn[0][0][0]*((prostateCell*)tissue[0][i+1][0])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[0][i+1][1])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[1][i+1][0])->getST_X() +
		filterIn[1][2][2]*((prostateCell*)tissue[1][i+1][1])->getST_X() -
		11*((prostateCell*)tissue[0][i][0])->getST_X();
	resDiffusionIn[0][i][0] = tempGradIn*coupParam/11;

	tempGradOut =
		filterOut[0][0][0]*((prostateCell*)tissue[0][i-1][0])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[0][i-1][1])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[1][i-1][0])->getST_X() +
		filterOut[0][1][0]*((prostateCell*)tissue[1][i-1][1])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[0][i][1])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[1][i][0])->getST_X() +
		filterOut[0][1][0]*((prostateCell*)tissue[1][i][1])->getST_X() +
		filterOut[0][0][0]*((prostateCell*)tissue[0][i+1][0])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[0][i+1][1])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[1][i+1][0])->getST_X() +
		filterOut[1][2][2]*((prostateCell*)tissue[1][i+1][1])->getST_X() +
		11*((prostateCell*)tissue[0][i][0])->getST_X();
	resDiffusionOut[0][i][0] = tempGradOut*coupParam/11;
			}

		/**
		 * Seuil / Sud
		 * */
		for (int i=1;i<m_ncol-1;i++)
			{
	tempGradIn =
		filterIn[0][0][0]*((prostateCell*)tissue[0][i-1][m_nrow-1])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[0][i-1][m_nrow-2])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[1][i-1][m_nrow-1])->getST_X() +
		filterIn[0][1][0]*((prostateCell*)tissue[1][i-1][m_nrow-2])->getST_X()+
		filterIn[0][0][1]*((prostateCell*)tissue[0][i][m_nrow-2])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[1][i][m_nrow-1])->getST_X() +
		filterIn[0][1][0]*((prostateCell*)tissue[1][i][m_nrow-2])->getST_X() +
		filterIn[0][0][0]*((prostateCell*)tissue[0][i+1][m_nrow-1])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[0][i+1][m_nrow-2])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[1][i+1][m_nrow-1])->getST_X() +
		filterIn[1][2][2]*((prostateCell*)tissue[1][i+1][m_nrow-2])->getST_X() -
		11*((prostateCell*)tissue[0][i][m_nrow-1])->getST_X() ;
	resDiffusionIn[0][i][m_nrow-1] = tempGradIn*coupParam/11;

	tempGradOut =
		filterOut[0][0][0]*((prostateCell*)tissue[0][i-1][m_nrow-1])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[0][i-1][m_nrow-2])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[1][i-1][m_nrow-1])->getST_X() +
		filterOut[0][1][0]*((prostateCell*)tissue[1][i-1][m_nrow-2])->getST_X()+
		filterOut[0][0][1]*((prostateCell*)tissue[0][i][m_nrow-2])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[1][i][m_nrow-1])->getST_X() +
		filterOut[0][1][0]*((prostateCell*)tissue[1][i][m_nrow-2])->getST_X() +
		filterOut[0][0][0]*((prostateCell*)tissue[0][i+1][m_nrow-1])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[0][i+1][m_nrow-2])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[1][i+1][m_nrow-1])->getST_X() +
		filterOut[1][2][2]*((prostateCell*)tissue[1][i+1][m_nrow-2])->getST_X() +
		11*((prostateCell*)tissue[0][i][m_nrow-1])->getST_X() ;
	resDiffusionOut[0][i][m_nrow-1] = tempGradOut*coupParam/11;
			}

		/**
		 * Seuil / Est
		 * */
		for (int i=1;i<m_nrow-1;i++)
			{
	tempGradIn =
		filterIn[0][0][0]*((prostateCell*)tissue[0][m_ncol-1][i-1])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[0][m_ncol-2][i-1])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[1][m_ncol-1][i-1])->getST_X() +
		filterIn[0][1][0]*((prostateCell*)tissue[1][m_ncol-2][i-1])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[0][m_ncol-2][i])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[1][m_ncol-1][i])->getST_X() +
		filterIn[0][1][0]*((prostateCell*)tissue[1][m_ncol-2][i])->getST_X() +
		filterIn[0][0][0]*((prostateCell*)tissue[0][m_ncol-1][i+1])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[0][m_ncol-2][i+1])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[1][m_ncol-1][i+1])->getST_X() +
		filterIn[1][2][2]*((prostateCell*)tissue[1][m_ncol-2][i+1])->getST_X() -
		11*((prostateCell*)tissue[0][m_ncol-1][i])->getST_X();
	resDiffusionIn[0][m_ncol-1][i] = tempGradIn*coupParam/11;

	tempGradOut =
		filterOut[0][0][0]*((prostateCell*)tissue[0][m_ncol-1][i-1])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[0][m_ncol-2][i-1])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[1][m_ncol-1][i-1])->getST_X() +
		filterOut[0][1][0]*((prostateCell*)tissue[1][m_ncol-2][i-1])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[0][m_ncol-2][i])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[1][m_ncol-1][i])->getST_X() +
		filterOut[0][1][0]*((prostateCell*)tissue[1][m_ncol-2][i])->getST_X() +
		filterOut[0][0][0]*((prostateCell*)tissue[0][m_ncol-1][i+1])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[0][m_ncol-2][i+1])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[1][m_ncol-1][i+1])->getST_X() +
		filterOut[1][2][2]*((prostateCell*)tissue[1][m_ncol-2][i+1])->getST_X() +
		11*((prostateCell*)tissue[0][m_ncol-1][i])->getST_X();
	resDiffusionOut[0][m_ncol-1][i] = tempGradOut*coupParam/11;
			}

		/**
		 * Seuil / Ouest
		 * */
		for (int i=1;i<m_nrow-1;i++)
			{
	tempGradIn =
		filterIn[0][0][0]*((prostateCell*)tissue[0][0][i-1])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[0][1][i-1])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[1][0][i-1])->getST_X() +
		filterIn[0][1][0]*((prostateCell*)tissue[1][1][i-1])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[0][1][i])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[1][0][i])->getST_X() +
		filterIn[0][1][0]*((prostateCell*)tissue[1][1][i])->getST_X() +
		filterIn[0][0][0]*((prostateCell*)tissue[0][0][i+1])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[0][1][i+1])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[1][0][i+1])->getST_X() +
		filterIn[1][2][2]*((prostateCell*)tissue[1][1][i+1])->getST_X() -
		11*((prostateCell*)tissue[0][0][i])->getST_X();
	resDiffusionIn[0][0][i] = tempGradIn*coupParam/11;

	tempGradOut =
		filterOut[0][0][0]*((prostateCell*)tissue[0][0][i-1])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[0][1][i-1])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[1][0][i-1])->getST_X() +
		filterOut[0][1][0]*((prostateCell*)tissue[1][1][i-1])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[0][1][i])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[1][0][i])->getST_X() +
		filterOut[0][1][0]*((prostateCell*)tissue[1][1][i])->getST_X() +
		filterOut[0][0][0]*((prostateCell*)tissue[0][0][i+1])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[0][1][i+1])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[1][0][i+1])->getST_X() +
		filterOut[1][2][2]*((prostateCell*)tissue[1][1][i+1])->getST_X() +
		11*((prostateCell*)tissue[0][0][i])->getST_X();
	resDiffusionOut[0][0][i] = tempGradOut*coupParam/11;
			}

		/**
		 * Top / Nord
		 * */
		for (int i=1;i<m_ncol-1;i++)
			{
	tempGradIn =
		filterIn[0][0][0]*((prostateCell*)tissue[m_nlayer-1][i-1][0])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[m_nlayer-1][i-1][1])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[m_nlayer-2][i-1][0])->getST_X() +
		filterIn[0][1][0]*((prostateCell*)tissue[m_nlayer-2][i-1][1])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[m_nlayer-1][i][1])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[m_nlayer-2][i][0])->getST_X() +
		filterIn[0][1][0]*((prostateCell*)tissue[m_nlayer-2][i][1])->getST_X() +
		filterIn[0][0][0]*((prostateCell*)tissue[m_nlayer-1][i+1][0])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[m_nlayer-1][i+1][1])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[m_nlayer-2][i+1][0])->getST_X() +
		filterIn[1][2][2]*((prostateCell*)tissue[m_nlayer-2][i+1][1])->getST_X() -
		11*((prostateCell*)tissue[m_nlayer-1][i][0])->getST_X();
	resDiffusionIn[m_nlayer-1][i][0] = tempGradIn*coupParam/11;

	tempGradOut =
		filterOut[0][0][0]*((prostateCell*)tissue[m_nlayer-1][i-1][0])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[m_nlayer-1][i-1][1])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[m_nlayer-2][i-1][0])->getST_X() +
		filterOut[0][1][0]*((prostateCell*)tissue[m_nlayer-2][i-1][1])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[m_nlayer-1][i][1])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[m_nlayer-2][i][0])->getST_X() +
		filterOut[0][1][0]*((prostateCell*)tissue[m_nlayer-2][i][1])->getST_X() +
		filterOut[0][0][0]*((prostateCell*)tissue[m_nlayer-1][i+1][0])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[m_nlayer-1][i+1][1])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[m_nlayer-2][i+1][0])->getST_X() +
		filterOut[1][2][2]*((prostateCell*)tissue[m_nlayer-2][i+1][1])->getST_X() +
		11*((prostateCell*)tissue[m_nlayer-1][i][0])->getST_X();
	resDiffusionOut[m_nlayer-1][i][0] = tempGradOut*coupParam/11;
			}

		/**
		 * Top / Sud
		 * */
		for (int i=1;i<m_ncol-1;i++)
			{
	tempGradIn =
		filterIn[0][0][0]*((prostateCell*)tissue[m_nlayer-1][i-1][m_nrow-1])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[m_nlayer-1][i-1][m_nrow-2])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[m_nlayer-2][i-1][m_nrow-1])->getST_X() +
		filterIn[0][1][0]*((prostateCell*)tissue[m_nlayer-2][i-1][m_nrow-2])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[m_nlayer-1][i][m_nrow-2])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[m_nlayer-2][i][m_nrow-1])->getST_X() +
		filterIn[0][1][0]*((prostateCell*)tissue[m_nlayer-2][i][m_nrow-2])->getST_X() +
		filterIn[0][0][0]*((prostateCell*)tissue[m_nlayer-1][i+1][m_nrow-1])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[m_nlayer-1][i+1][m_nrow-2])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[m_nlayer-2][i+1][m_nrow-1])->getST_X() +
		filterIn[1][2][2]*((prostateCell*)tissue[m_nlayer-2][i+1][m_nrow-2])->getST_X() -
		11*((prostateCell*)tissue[m_nlayer-1][i][m_nrow-1])->getST_X();
	resDiffusionIn[m_nlayer-1][i][m_nrow-1] = tempGradIn*coupParam/11;

	tempGradOut =
		filterOut[0][0][0]*((prostateCell*)tissue[m_nlayer-1][i-1][m_nrow-1])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[m_nlayer-1][i-1][m_nrow-2])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[m_nlayer-2][i-1][m_nrow-1])->getST_X() +
		filterOut[0][1][0]*((prostateCell*)tissue[m_nlayer-2][i-1][m_nrow-2])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[m_nlayer-1][i][m_nrow-2])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[m_nlayer-2][i][m_nrow-1])->getST_X() +
		filterOut[0][1][0]*((prostateCell*)tissue[m_nlayer-2][i][m_nrow-2])->getST_X() +
		filterOut[0][0][0]*((prostateCell*)tissue[m_nlayer-1][i+1][m_nrow-1])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[m_nlayer-1][i+1][m_nrow-2])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[m_nlayer-2][i+1][m_nrow-1])->getST_X() +
		filterOut[1][2][2]*((prostateCell*)tissue[m_nlayer-2][i+1][m_nrow-2])->getST_X() +
		11*((prostateCell*)tissue[m_nlayer-1][i][m_nrow-1])->getST_X();
	resDiffusionOut[m_nlayer-1][i][m_nrow-1] = tempGradOut*coupParam/11;
			}

		/**
		 * Top / Est
		 * */
		for (int i=1;i<m_nrow-1;i++)
			{
	tempGradIn =
		filterIn[0][0][0]*((prostateCell*)tissue[m_nlayer-1][m_ncol-1][i-1])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[m_nlayer-1][m_ncol-2][i-1])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[m_nlayer-2][m_ncol-1][i-1])->getST_X() +
		filterIn[0][1][0]*((prostateCell*)tissue[m_nlayer-2][m_ncol-2][i-1])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[m_nlayer-1][m_ncol-2][i])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[m_nlayer-2][m_ncol-1][i])->getST_X() +
		filterIn[0][1][0]*((prostateCell*)tissue[m_nlayer-2][m_ncol-2][i])->getST_X() +
		filterIn[0][0][0]*((prostateCell*)tissue[m_nlayer-1][m_ncol-1][i+1])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[m_nlayer-1][m_ncol-2][i+1])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[m_nlayer-2][m_ncol-1][i+1])->getST_X() +
		filterIn[1][2][2]*((prostateCell*)tissue[m_nlayer-2][m_ncol-2][i+1])->getST_X() -
		11*((prostateCell*)tissue[m_nlayer-1][m_ncol-1][i])->getST_X();
	resDiffusionIn[m_nlayer-1][m_ncol-1][i] = tempGradIn*coupParam/11;

	tempGradOut =
		filterOut[0][0][0]*((prostateCell*)tissue[m_nlayer-1][m_ncol-1][i-1])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[m_nlayer-1][m_ncol-2][i-1])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[m_nlayer-2][m_ncol-1][i-1])->getST_X() +
		filterOut[0][1][0]*((prostateCell*)tissue[m_nlayer-2][m_ncol-2][i-1])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[m_nlayer-1][m_ncol-2][i])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[m_nlayer-2][m_ncol-1][i])->getST_X() +
		filterOut[0][1][0]*((prostateCell*)tissue[m_nlayer-2][m_ncol-2][i])->getST_X() +
		filterOut[0][0][0]*((prostateCell*)tissue[m_nlayer-1][m_ncol-1][i+1])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[m_nlayer-1][m_ncol-2][i+1])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[m_nlayer-2][m_ncol-1][i+1])->getST_X() +
		filterOut[1][2][2]*((prostateCell*)tissue[m_nlayer-2][m_ncol-2][i+1])->getST_X() +
		11*((prostateCell*)tissue[m_nlayer-1][m_ncol-1][i])->getST_X();
	resDiffusionOut[m_nlayer-1][m_ncol-1][i] = tempGradOut*coupParam/11;
			}

		/**
		 * Top / Ouest
		 * */
		for (int i=1;i<m_nrow-1;i++)
			{
	tempGradIn =
		filterIn[0][0][0]*((prostateCell*)tissue[m_nlayer-1][0][i-1])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[m_nlayer-1][1][i-1])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[m_nlayer-2][0][i-1])->getST_X() +
		filterIn[0][1][0]*((prostateCell*)tissue[m_nlayer-2][1][i-1])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[m_nlayer-1][1][i])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[m_nlayer-2][0][i])->getST_X() +
		filterIn[0][1][0]*((prostateCell*)tissue[m_nlayer-2][1][i])->getST_X() +
		filterIn[0][0][0]*((prostateCell*)tissue[m_nlayer-1][0][i+1])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[m_nlayer-1][1][i+1])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[m_nlayer-2][0][i+1])->getST_X() +
		filterIn[1][2][2]*((prostateCell*)tissue[m_nlayer-2][1][i+1])->getST_X() -
		11*((prostateCell*)tissue[m_nlayer-1][0][i])->getST_X();
	resDiffusionIn[m_nlayer-1][0][i] = tempGradIn*coupParam/11;

	tempGradOut =
		filterOut[0][0][0]*((prostateCell*)tissue[m_nlayer-1][0][i-1])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[m_nlayer-1][1][i-1])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[m_nlayer-2][0][i-1])->getST_X() +
		filterOut[0][1][0]*((prostateCell*)tissue[m_nlayer-2][1][i-1])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[m_nlayer-1][1][i])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[m_nlayer-2][0][i])->getST_X() +
		filterOut[0][1][0]*((prostateCell*)tissue[m_nlayer-2][1][i])->getST_X() +
		filterOut[0][0][0]*((prostateCell*)tissue[m_nlayer-1][0][i+1])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[m_nlayer-1][1][i+1])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[m_nlayer-2][0][i+1])->getST_X() +
		filterOut[1][2][2]*((prostateCell*)tissue[m_nlayer-2][1][i+1])->getST_X() +
		11*((prostateCell*)tissue[m_nlayer-1][0][i])->getST_X();
	resDiffusionOut[m_nlayer-1][0][i] = tempGradOut*coupParam/11;
			}


		/**
		 * Ouest / Nord
		 * */
		for (int i=1;i<m_nlayer-1;i++)
			{
	tempGradIn =
		filterIn[0][0][0]*((prostateCell*)tissue[i-1][0][0])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[i-1][0][1])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[i-1][1][0])->getST_X() +
		filterIn[0][1][0]*((prostateCell*)tissue[i-1][1][1])->getST_X()  +
		filterIn[0][0][1]*((prostateCell*)tissue[i][0][1])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[i][1][0])->getST_X() +
		filterIn[0][1][0]*((prostateCell*)tissue[i][1][1])->getST_X() +
		filterIn[0][0][0]*((prostateCell*)tissue[i+1][0][0])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[i+1][0][1])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[i+1][1][0])->getST_X() +
		filterIn[1][2][2]*((prostateCell*)tissue[i+1][1][1])->getST_X() -
		11*((prostateCell*)tissue[i][0][0])->getST_X();
	resDiffusionIn[i][0][0] = tempGradIn*coupParam/11;

	tempGradOut =
		filterOut[0][0][0]*((prostateCell*)tissue[i-1][0][0])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[i-1][0][1])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[i-1][1][0])->getST_X() +
		filterOut[0][1][0]*((prostateCell*)tissue[i-1][1][1])->getST_X()  +
		filterOut[0][0][1]*((prostateCell*)tissue[i][0][1])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[i][1][0])->getST_X() +
		filterOut[0][1][0]*((prostateCell*)tissue[i][1][1])->getST_X() +
		filterOut[0][0][0]*((prostateCell*)tissue[i+1][0][0])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[i+1][0][1])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[i+1][1][0])->getST_X() +
		filterOut[1][2][2]*((prostateCell*)tissue[i+1][1][1])->getST_X() +
		11*((prostateCell*)tissue[i][0][0])->getST_X();
	resDiffusionOut[i][0][0] = tempGradOut*coupParam/11;
			}

		/**
		 * Ouest / Sud
		 * */
		for (int i=1;i<m_nlayer-1;i++)
			{
	tempGradIn =
		filterIn[0][0][0]*((prostateCell*)tissue[i-1][0][m_nrow-1])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[i-1][0][m_nrow-2])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[i-1][1][m_nrow-1])->getST_X() +
		filterIn[0][1][0]*((prostateCell*)tissue[i-1][1][m_nrow-2])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[i][0][m_nrow-2])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[i][1][m_nrow-1])->getST_X() +
		filterIn[0][1][0]*((prostateCell*)tissue[i][1][m_nrow-2])->getST_X() +
		filterIn[0][0][0]*((prostateCell*)tissue[i+1][0][m_nrow-1])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[i+1][0][m_nrow-2])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[i+1][1][m_nrow-1])->getST_X() +
		filterIn[1][2][2]*((prostateCell*)tissue[i+1][1][m_nrow-2])->getST_X() -
		11*((prostateCell*)tissue[i][0][m_nrow-1])->getST_X();
	resDiffusionIn[i][0][m_nrow-1] = tempGradIn*coupParam/11;

	tempGradOut =
		filterOut[0][0][0]*((prostateCell*)tissue[i-1][0][m_nrow-1])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[i-1][0][m_nrow-2])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[i-1][1][m_nrow-1])->getST_X() +
		filterOut[0][1][0]*((prostateCell*)tissue[i-1][1][m_nrow-2])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[i][0][m_nrow-2])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[i][1][m_nrow-1])->getST_X() +
		filterOut[0][1][0]*((prostateCell*)tissue[i][1][m_nrow-2])->getST_X() +
		filterOut[0][0][0]*((prostateCell*)tissue[i+1][0][m_nrow-1])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[i+1][0][m_nrow-2])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[i+1][1][m_nrow-1])->getST_X() +
		filterOut[1][2][2]*((prostateCell*)tissue[i+1][1][m_nrow-2])->getST_X()+
		11*((prostateCell*)tissue[i][0][m_nrow-1])->getST_X();
	resDiffusionOut[i][0][m_nrow-1] = tempGradOut*coupParam/11;
			}

		/**
		 * Est / Nord
		 * */
		for (int i=1;i<m_nlayer-1;i++)
			{
	tempGradIn =
		filterIn[0][0][0]*((prostateCell*)tissue[i-1][m_ncol-1][0])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[i-1][m_ncol-1][1])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[i-1][m_ncol-2][0])->getST_X() +
		filterIn[0][1][0]*((prostateCell*)tissue[i-1][m_ncol-2][1])->getST_X()+
		filterIn[0][0][1]*((prostateCell*)tissue[i][m_ncol-1][1])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[i][m_ncol-2][0])->getST_X() +
		filterIn[0][1][0]*((prostateCell*)tissue[i][m_ncol-2][1])->getST_X() +
		filterIn[0][0][0]*((prostateCell*)tissue[i+1][m_ncol-1][0])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[i+1][m_ncol-1][1])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[i+1][m_ncol-2][0])->getST_X() +
		filterIn[1][2][2]*((prostateCell*)tissue[i+1][m_ncol-2][1])->getST_X() -
		11*((prostateCell*)tissue[i][m_ncol-1][0])->getST_X();
	resDiffusionIn[i][m_ncol-1][0] = tempGradIn*coupParam/11;

	tempGradOut =
		filterOut[0][0][0]*((prostateCell*)tissue[i-1][m_ncol-1][0])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[i-1][m_ncol-1][1])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[i-1][m_ncol-2][0])->getST_X() +
		filterOut[0][1][0]*((prostateCell*)tissue[i-1][m_ncol-2][1])->getST_X()+
		filterOut[0][0][1]*((prostateCell*)tissue[i][m_ncol-1][1])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[i][m_ncol-2][0])->getST_X() +
		filterOut[0][1][0]*((prostateCell*)tissue[i][m_ncol-2][1])->getST_X() +
		filterOut[0][0][0]*((prostateCell*)tissue[i+1][m_ncol-1][0])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[i+1][m_ncol-1][1])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[i+1][m_ncol-2][0])->getST_X() +
		filterOut[1][2][2]*((prostateCell*)tissue[i+1][m_ncol-2][1])->getST_X() +
		11*((prostateCell*)tissue[i][m_ncol-1][0])->getST_X();
	resDiffusionOut[i][m_ncol-1][0] = tempGradOut*coupParam/11;
			}

		/**
		 * Est / Sud
		 * */
		for (int i=1;i<m_nlayer-1;i++)
			{
	tempGradIn =
		filterIn[0][0][0]*((prostateCell*)tissue[i-1][m_ncol-1][m_nrow-1])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[i-1][m_ncol-1][m_nrow-2])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[i-1][m_ncol-2][m_nrow-1])->getST_X() +
		filterIn[0][1][0]*((prostateCell*)tissue[i-1][m_ncol-2][m_nrow-2])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[i][m_ncol-1][m_nrow-2])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[i][m_ncol-2][m_nrow-1])->getST_X() +
		filterIn[0][1][0]*((prostateCell*)tissue[i][m_ncol-2][m_nrow-2])->getST_X() +
		filterIn[0][0][0]*((prostateCell*)tissue[i+1][m_ncol-1][m_nrow-1])->getST_X() +
		filterIn[0][0][1]*((prostateCell*)tissue[i+1][m_ncol-1][m_nrow-2])->getST_X() +
		filterIn[0][0][2]*((prostateCell*)tissue[i+1][m_ncol-2][m_nrow-1])->getST_X() +
		filterIn[1][2][2]*((prostateCell*)tissue[i+1][m_ncol-2][m_nrow-2])->getST_X()-
		11*((prostateCell*)tissue[i][m_ncol-1][m_nrow-1])->getST_X() ;
	resDiffusionIn[i][m_ncol-1][m_nrow-1] = tempGradIn*coupParam/11;

	tempGradOut =
		filterOut[0][0][0]*((prostateCell*)tissue[i-1][m_ncol-1][m_nrow-1])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[i-1][m_ncol-1][m_nrow-2])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[i-1][m_ncol-2][m_nrow-1])->getST_X() +
		filterOut[0][1][0]*((prostateCell*)tissue[i-1][m_ncol-2][m_nrow-2])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[i][m_ncol-1][m_nrow-2])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[i][m_ncol-2][m_nrow-1])->getST_X() +
		filterOut[0][1][0]*((prostateCell*)tissue[i][m_ncol-2][m_nrow-2])->getST_X() +
		filterOut[0][0][0]*((prostateCell*)tissue[i+1][m_ncol-1][m_nrow-1])->getST_X() +
		filterOut[0][0][1]*((prostateCell*)tissue[i+1][m_ncol-1][m_nrow-2])->getST_X() +
		filterOut[0][0][2]*((prostateCell*)tissue[i+1][m_ncol-2][m_nrow-1])->getST_X() +
		filterOut[1][2][2]*((prostateCell*)tissue[i+1][m_ncol-2][m_nrow-2])->getST_X()+
		11*((prostateCell*)tissue[i][m_ncol-1][m_nrow-1])->getST_X() ;
	resDiffusionOut[i][m_ncol-1][m_nrow-1] = tempGradOut*coupParam/11;
			}

		/////////////////
		//  Faces‎ (6)  //
		/////////////////

		/**
		 * Seuil
		 * */
		for (int i=1;i<m_ncol-1;i++)
			{
	for (int j=1;j<m_nrow-1;j++)
		{
			tempGradIn =
				filterIn[0][0][0]*((prostateCell*)tissue[1][i-1][j-1])->getST_X() +
				filterIn[0][0][1]*((prostateCell*)tissue[1][i-1][j])->getST_X() +
				filterIn[0][0][2]*((prostateCell*)tissue[1][i-1][j+1])->getST_X() +
				filterIn[0][1][0]*((prostateCell*)tissue[1][i][j-1])->getST_X() +
				filterIn[0][1][1]*((prostateCell*)tissue[1][i][j])->getST_X() +
				filterIn[0][1][2]*((prostateCell*)tissue[1][i][j+1])->getST_X() +
				filterIn[0][2][0]*((prostateCell*)tissue[1][i+1][j-1])->getST_X() +
				filterIn[0][2][1]*((prostateCell*)tissue[1][i+1][j])->getST_X() +
				filterIn[0][2][2]*((prostateCell*)tissue[1][i+1][j+1])->getST_X() +
				filterIn[1][0][0]*((prostateCell*)tissue[0][i-1][j-1])->getST_X() +
				filterIn[1][0][1]*((prostateCell*)tissue[0][i-1][j])->getST_X() +
				filterIn[1][0][2]*((prostateCell*)tissue[0][i-1][j+1])->getST_X() +
				filterIn[1][1][0]*((prostateCell*)tissue[0][i][j-1])->getST_X() +
				filterIn[1][1][2]*((prostateCell*)tissue[0][i][j+1])->getST_X() +
				filterIn[1][2][0]*((prostateCell*)tissue[0][i+1][j-1])->getST_X() +
				filterIn[1][2][1]*((prostateCell*)tissue[0][i+1][j])->getST_X() +
				filterIn[1][2][2]*((prostateCell*)tissue[0][i+1][j+1])->getST_X() -
				17*((prostateCell*)tissue[0][i][j])->getST_X();
			resDiffusionIn[0][i][j] = tempGradIn*coupParam/17;

			tempGradOut =
				filterOut[0][0][0]*((prostateCell*)tissue[1][i-1][j-1])->getST_X() +
				filterOut[0][0][1]*((prostateCell*)tissue[1][i-1][j])->getST_X() +
				filterOut[0][0][2]*((prostateCell*)tissue[1][i-1][j+1])->getST_X() +
				filterOut[0][1][0]*((prostateCell*)tissue[1][i][j-1])->getST_X() +
				filterOut[0][1][1]*((prostateCell*)tissue[1][i][j])->getST_X() +
				filterOut[0][1][2]*((prostateCell*)tissue[1][i][j+1])->getST_X() +
				filterOut[0][2][0]*((prostateCell*)tissue[1][i+1][j-1])->getST_X() +
				filterOut[0][2][1]*((prostateCell*)tissue[1][i+1][j])->getST_X() +
				filterOut[0][2][2]*((prostateCell*)tissue[1][i+1][j+1])->getST_X() +
				filterOut[1][0][0]*((prostateCell*)tissue[0][i-1][j-1])->getST_X() +
				filterOut[1][0][1]*((prostateCell*)tissue[0][i-1][j])->getST_X() +
				filterOut[1][0][2]*((prostateCell*)tissue[0][i-1][j+1])->getST_X() +
				filterOut[1][1][0]*((prostateCell*)tissue[0][i][j-1])->getST_X() +
				filterOut[1][1][2]*((prostateCell*)tissue[0][i][j+1])->getST_X() +
				filterOut[1][2][0]*((prostateCell*)tissue[0][i+1][j-1])->getST_X() +
				filterOut[1][2][1]*((prostateCell*)tissue[0][i+1][j])->getST_X() +
				filterOut[1][2][2]*((prostateCell*)tissue[0][i+1][j+1])->getST_X() +
				17*((prostateCell*)tissue[0][i][j])->getST_X();
			resDiffusionOut[0][i][j] = tempGradOut*coupParam/17;
		}
			}

		/**
		 * top
		 * */
		for (int i=1;i<m_ncol-1;i++)
			{
	for (int j=1;j<m_nrow-1;j++)
		{
			tempGradIn =
				filterIn[1][0][0]*((prostateCell*)tissue[m_nlayer-1][i-1][j-1])->getST_X() +
				filterIn[1][0][1]*((prostateCell*)tissue[m_nlayer-1][i-1][j])->getST_X() +
				filterIn[1][0][2]*((prostateCell*)tissue[m_nlayer-1][i-1][j+1])->getST_X() +
				filterIn[1][1][0]*((prostateCell*)tissue[m_nlayer-1][i][j-1])->getST_X() -
				17*((prostateCell*)tissue[m_nlayer-1][i][j])->getST_X() +
				filterIn[1][1][2]*((prostateCell*)tissue[m_nlayer-1][i][j+1])->getST_X() +
				filterIn[1][2][0]*((prostateCell*)tissue[m_nlayer-1][i+1][j-1])->getST_X() +
				filterIn[1][2][1]*((prostateCell*)tissue[m_nlayer-1][i+1][j])->getST_X() +
				filterIn[1][2][2]*((prostateCell*)tissue[m_nlayer-1][i+1][j+1])->getST_X() +
				filterIn[2][0][0]*((prostateCell*)tissue[m_nlayer-2][i-1][j-1])->getST_X() +
				filterIn[2][0][1]*((prostateCell*)tissue[m_nlayer-2][i-1][j])->getST_X() +
				filterIn[2][0][2]*((prostateCell*)tissue[m_nlayer-2][i-1][j+1])->getST_X() +
				filterIn[2][1][0]*((prostateCell*)tissue[m_nlayer-2][i][j-1])->getST_X() +
				filterIn[2][1][0]*((prostateCell*)tissue[m_nlayer-2][i][j])->getST_X() +
				filterIn[2][1][2]*((prostateCell*)tissue[m_nlayer-2][i][j+1])->getST_X() +
				filterIn[2][2][0]*((prostateCell*)tissue[m_nlayer-2][i+1][j-1])->getST_X() +
				filterIn[2][2][1]*((prostateCell*)tissue[m_nlayer-2][i+1][j])->getST_X() +
				filterIn[2][2][2]*((prostateCell*)tissue[m_nlayer-2][i+1][j+1])->getST_X();
			resDiffusionIn[m_nlayer-1][i][j] = tempGradIn*coupParam/17;

			tempGradOut =
				filterOut[1][0][0]*((prostateCell*)tissue[m_nlayer-1][i-1][j-1])->getST_X() +
				filterOut[1][0][1]*((prostateCell*)tissue[m_nlayer-1][i-1][j])->getST_X() +
				filterOut[1][0][2]*((prostateCell*)tissue[m_nlayer-1][i-1][j+1])->getST_X() +
				filterOut[1][1][0]*((prostateCell*)tissue[m_nlayer-1][i][j-1])->getST_X() +
				17*((prostateCell*)tissue[m_nlayer-1][i][j])->getST_X() +
				filterOut[1][1][2]*((prostateCell*)tissue[m_nlayer-1][i][j+1])->getST_X() +
				filterOut[1][2][0]*((prostateCell*)tissue[m_nlayer-1][i+1][j-1])->getST_X() +
				filterOut[1][2][1]*((prostateCell*)tissue[m_nlayer-1][i+1][j])->getST_X() +
				filterOut[1][2][2]*((prostateCell*)tissue[m_nlayer-1][i+1][j+1])->getST_X() +
				filterOut[2][0][0]*((prostateCell*)tissue[m_nlayer-2][i-1][j-1])->getST_X() +
				filterOut[2][0][1]*((prostateCell*)tissue[m_nlayer-2][i-1][j])->getST_X() +
				filterOut[2][0][2]*((prostateCell*)tissue[m_nlayer-2][i-1][j+1])->getST_X() +
				filterOut[2][1][0]*((prostateCell*)tissue[m_nlayer-2][i][j-1])->getST_X() +
				filterOut[2][1][0]*((prostateCell*)tissue[m_nlayer-2][i][j])->getST_X() +
				filterOut[2][1][2]*((prostateCell*)tissue[m_nlayer-2][i][j+1])->getST_X() +
				filterOut[2][2][0]*((prostateCell*)tissue[m_nlayer-2][i+1][j-1])->getST_X() +
				filterOut[2][2][1]*((prostateCell*)tissue[m_nlayer-2][i+1][j])->getST_X() +
				filterOut[2][2][2]*((prostateCell*)tissue[m_nlayer-2][i+1][j+1])->getST_X();
			resDiffusionOut[m_nlayer-1][i][j] = tempGradOut*coupParam/17;
		}
			}
		/**
		 * Nord
		 * */
		for (int i=1;i<m_nlayer-1;i++)
			{
	for (int j=1;j<m_ncol-1;j++)
		{
			tempGradIn =
				filterIn[0][0][0]*((prostateCell*)tissue[i-1][j-1][1])->getST_X() +
				filterIn[0][1][0]*((prostateCell*)tissue[i-1][j][1])->getST_X() +
				filterIn[0][2][0]*((prostateCell*)tissue[i-1][j+1][1])->getST_X() +
				filterIn[1][0][0]*((prostateCell*)tissue[i][j-1][1])->getST_X() +
				filterIn[1][1][0]*((prostateCell*)tissue[i][j][1])->getST_X() +
				filterIn[1][2][0]*((prostateCell*)tissue[i][j+1][1])->getST_X() +
				filterIn[2][0][0]*((prostateCell*)tissue[i+1][j-1][1])->getST_X() +
				filterIn[2][1][0]*((prostateCell*)tissue[i+1][j][1])->getST_X() +
				filterIn[2][2][0]*((prostateCell*)tissue[i+1][j+1][1])->getST_X() +
				filterIn[0][0][1]*((prostateCell*)tissue[i-1][j-1][0])->getST_X() +
				filterIn[0][1][1]*((prostateCell*)tissue[i-1][j][0])->getST_X() +
				filterIn[0][2][1]*((prostateCell*)tissue[i-1][j+1][0])->getST_X() +
				filterIn[1][0][1]*((prostateCell*)tissue[i][j-1][0])->getST_X() -
				17*((prostateCell*)tissue[i][j][0])->getST_X() +
				filterIn[1][2][1]*((prostateCell*)tissue[i][j+1][0])->getST_X() +
				filterIn[2][0][1]*((prostateCell*)tissue[i+1][j-1][0])->getST_X() +
				filterIn[2][1][1]*((prostateCell*)tissue[i+1][j][0])->getST_X() +
				filterIn[2][2][1]*((prostateCell*)tissue[i+1][j+1][0])->getST_X();
			resDiffusionIn[i][j][0] = tempGradIn*coupParam/17;

			tempGradOut =
				filterOut[0][0][0]*((prostateCell*)tissue[i-1][j-1][1])->getST_X() +
				filterOut[0][1][0]*((prostateCell*)tissue[i-1][j][1])->getST_X() +
				filterOut[0][2][0]*((prostateCell*)tissue[i-1][j+1][1])->getST_X() +
				filterOut[1][0][0]*((prostateCell*)tissue[i][j-1][1])->getST_X() +
				filterOut[1][1][0]*((prostateCell*)tissue[i][j][1])->getST_X() +
				filterOut[1][2][0]*((prostateCell*)tissue[i][j+1][1])->getST_X() +
				filterOut[2][0][0]*((prostateCell*)tissue[i+1][j-1][1])->getST_X() +
				filterOut[2][1][0]*((prostateCell*)tissue[i+1][j][1])->getST_X() +
				filterOut[2][2][0]*((prostateCell*)tissue[i+1][j+1][1])->getST_X() +
				filterOut[0][0][1]*((prostateCell*)tissue[i-1][j-1][0])->getST_X() +
				filterOut[0][1][1]*((prostateCell*)tissue[i-1][j][0])->getST_X() +
				filterOut[0][2][1]*((prostateCell*)tissue[i-1][j+1][0])->getST_X() +
				filterOut[1][0][1]*((prostateCell*)tissue[i][j-1][0])->getST_X() +
				17*((prostateCell*)tissue[i][j][0])->getST_X() +
				filterOut[1][2][1]*((prostateCell*)tissue[i][j+1][0])->getST_X() +
				filterOut[2][0][1]*((prostateCell*)tissue[i+1][j-1][0])->getST_X() +
				filterOut[2][1][1]*((prostateCell*)tissue[i+1][j][0])->getST_X() +
				filterOut[2][2][1]*((prostateCell*)tissue[i+1][j+1][0])->getST_X();
			resDiffusionOut[i][j][0] = tempGradOut*coupParam/17;
		}
			}

		/**
		 * Sud
		 * */
		for (int i=1;i<m_nlayer-1;i++)
			{
	for (int j=1;j<m_ncol-1;j++)
		{
			tempGradIn =
				filterIn[0][0][2]*((prostateCell*)tissue[i-1][j-1][m_nrow-2])->getST_X() +
				filterIn[0][1][2]*((prostateCell*)tissue[i-1][j][m_nrow-2])->getST_X() +
				filterIn[0][2][2]*((prostateCell*)tissue[i-1][j+1][m_nrow-2])->getST_X() +
				filterIn[1][0][2]*((prostateCell*)tissue[i][j-1][m_nrow-2])->getST_X() +
				filterIn[1][1][2]*((prostateCell*)tissue[i][j][m_nrow-2])->getST_X() +
				filterIn[1][2][2]*((prostateCell*)tissue[i][j+1][m_nrow-2])->getST_X() +
				filterIn[2][0][2]*((prostateCell*)tissue[i+1][j-1][m_nrow-2])->getST_X() +
				filterIn[2][1][2]*((prostateCell*)tissue[i+1][j][m_nrow-2])->getST_X() +
				filterIn[2][2][2]*((prostateCell*)tissue[i+1][j+1][m_nrow-2])->getST_X() +
				filterIn[0][0][1]*((prostateCell*)tissue[i-1][j-1][m_nrow-1])->getST_X() +
				filterIn[0][1][1]*((prostateCell*)tissue[i-1][j][m_nrow-1])->getST_X() +
				filterIn[0][2][1]*((prostateCell*)tissue[i-1][j+1][m_nrow-1])->getST_X() +
				filterIn[1][0][1]*((prostateCell*)tissue[i][j-1][m_nrow-1])->getST_X() -
				17*((prostateCell*)tissue[i][j][m_nrow-1])->getST_X() +
				filterIn[1][2][1]*((prostateCell*)tissue[i][j+1][m_nrow-1])->getST_X() +
				filterIn[2][0][1]*((prostateCell*)tissue[i+1][j-1][m_nrow-1])->getST_X() +
				filterIn[2][1][1]*((prostateCell*)tissue[i+1][j][m_nrow-1])->getST_X() +
				filterIn[2][2][1]*((prostateCell*)tissue[i+1][j+1][m_nrow-1])->getST_X();
			resDiffusionIn[i][j][m_nrow-1] = tempGradIn*coupParam/17;

			tempGradOut =
				filterOut[0][0][2]*((prostateCell*)tissue[i-1][j-1][m_nrow-2])->getST_X() +
				filterOut[0][1][2]*((prostateCell*)tissue[i-1][j][m_nrow-2])->getST_X() +
				filterOut[0][2][2]*((prostateCell*)tissue[i-1][j+1][m_nrow-2])->getST_X() +
				filterOut[1][0][2]*((prostateCell*)tissue[i][j-1][m_nrow-2])->getST_X() +
				filterOut[1][1][2]*((prostateCell*)tissue[i][j][m_nrow-2])->getST_X() +
				filterOut[1][2][2]*((prostateCell*)tissue[i][j+1][m_nrow-2])->getST_X() +
				filterOut[2][0][2]*((prostateCell*)tissue[i+1][j-1][m_nrow-2])->getST_X() +
				filterOut[2][1][2]*((prostateCell*)tissue[i+1][j][m_nrow-2])->getST_X() +
				filterOut[2][2][2]*((prostateCell*)tissue[i+1][j+1][m_nrow-2])->getST_X() +
				filterOut[0][0][1]*((prostateCell*)tissue[i-1][j-1][m_nrow-1])->getST_X() +
				filterOut[0][1][1]*((prostateCell*)tissue[i-1][j][m_nrow-1])->getST_X() +
				filterOut[0][2][1]*((prostateCell*)tissue[i-1][j+1][m_nrow-1])->getST_X() +
				filterOut[1][0][1]*((prostateCell*)tissue[i][j-1][m_nrow-1])->getST_X() +
				17*((prostateCell*)tissue[i][j][m_nrow-1])->getST_X() +
				filterOut[1][2][1]*((prostateCell*)tissue[i][j+1][m_nrow-1])->getST_X() +
				filterOut[2][0][1]*((prostateCell*)tissue[i+1][j-1][m_nrow-1])->getST_X() +
				filterOut[2][1][1]*((prostateCell*)tissue[i+1][j][m_nrow-1])->getST_X() +
				filterOut[2][2][1]*((prostateCell*)tissue[i+1][j+1][m_nrow-1])->getST_X();
			resDiffusionOut[i][j][m_nrow-1] = tempGradOut*coupParam/17;
		}
			}

		/**
		 * Est
		 * */
		for (int i=1;i<m_nlayer-1;i++)
			{
	for (int j=1;j<m_nrow-1;j++)
		{
			tempGradIn =
				filterIn[0][0][2]*((prostateCell*)tissue[i-1][m_ncol-2][j-1])->getST_X() +
				filterIn[0][1][2]*((prostateCell*)tissue[i-1][m_ncol-2][j])->getST_X() +
				filterIn[0][2][2]*((prostateCell*)tissue[i-1][m_ncol-2][j+1])->getST_X() +
				filterIn[1][0][2]*((prostateCell*)tissue[i][m_ncol-2][j-1])->getST_X() +
				filterIn[1][1][2]*((prostateCell*)tissue[i][m_ncol-2][j])->getST_X() +
				filterIn[1][2][2]*((prostateCell*)tissue[i][m_ncol-2][j+1])->getST_X() +
				filterIn[2][0][2]*((prostateCell*)tissue[i+1][m_ncol-2][j-1])->getST_X() +
				filterIn[2][1][2]*((prostateCell*)tissue[i+1][m_ncol-2][j])->getST_X() +
				filterIn[2][2][2]*((prostateCell*)tissue[i+1][m_ncol-2][j+1])->getST_X() +
				filterIn[0][0][1]*((prostateCell*)tissue[i-1][m_ncol-1][j-1])->getST_X() +
				filterIn[0][1][1]*((prostateCell*)tissue[i-1][m_ncol-1][j])->getST_X() +
				filterIn[0][2][1]*((prostateCell*)tissue[i-1][m_ncol-1][j+1])->getST_X() +
				filterIn[1][0][1]*((prostateCell*)tissue[i][m_ncol-1][j-1])->getST_X() -
				17*((prostateCell*)tissue[i][m_ncol-1][j])->getST_X() +
				filterIn[1][2][1]*((prostateCell*)tissue[i][m_ncol-1][j+1])->getST_X() +
				filterIn[2][0][1]*((prostateCell*)tissue[i+1][m_ncol-1][j-1])->getST_X() +
				filterIn[2][1][1]*((prostateCell*)tissue[i+1][m_ncol-1][j])->getST_X() +
				filterIn[2][2][1]*((prostateCell*)tissue[i+1][m_ncol-1][j+1])->getST_X();
			resDiffusionIn[i][m_ncol-1][j] = tempGradIn*coupParam/17;

			tempGradOut =
				filterOut[0][0][2]*((prostateCell*)tissue[i-1][m_ncol-2][j-1])->getST_X() +
				filterOut[0][1][2]*((prostateCell*)tissue[i-1][m_ncol-2][j])->getST_X() +
				filterOut[0][2][2]*((prostateCell*)tissue[i-1][m_ncol-2][j+1])->getST_X() +
				filterOut[1][0][2]*((prostateCell*)tissue[i][m_ncol-2][j-1])->getST_X() +
				filterOut[1][1][2]*((prostateCell*)tissue[i][m_ncol-2][j])->getST_X() +
				filterOut[1][2][2]*((prostateCell*)tissue[i][m_ncol-2][j+1])->getST_X() +
				filterOut[2][0][2]*((prostateCell*)tissue[i+1][m_ncol-2][j-1])->getST_X() +
				filterOut[2][1][2]*((prostateCell*)tissue[i+1][m_ncol-2][j])->getST_X() +
				filterOut[2][2][2]*((prostateCell*)tissue[i+1][m_ncol-2][j+1])->getST_X() +
				filterOut[0][0][1]*((prostateCell*)tissue[i-1][m_ncol-1][j-1])->getST_X() +
				filterOut[0][1][1]*((prostateCell*)tissue[i-1][m_ncol-1][j])->getST_X() +
				filterOut[0][2][1]*((prostateCell*)tissue[i-1][m_ncol-1][j+1])->getST_X() +
				filterOut[1][0][1]*((prostateCell*)tissue[i][m_ncol-1][j-1])->getST_X() +
				17*((prostateCell*)tissue[i][m_ncol-1][j])->getST_X() +
				filterOut[1][2][1]*((prostateCell*)tissue[i][m_ncol-1][j+1])->getST_X() +
				filterOut[2][0][1]*((prostateCell*)tissue[i+1][m_ncol-1][j-1])->getST_X() +
				filterOut[2][1][1]*((prostateCell*)tissue[i+1][m_ncol-1][j])->getST_X() +
				filterOut[2][2][1]*((prostateCell*)tissue[i+1][m_ncol-1][j+1])->getST_X();
			resDiffusionOut[i][m_ncol-1][j] = tempGradOut*coupParam/17;
		}
			}

		/**
		 * Ouest
		 * */
		for (int i=1;i<m_nlayer-1;i++)
			{
	for (int j=1;j<m_ncol-1;j++)
		{
			tempGradIn =
				filterIn[0][0][2]*((prostateCell*)tissue[i-1][1][j-1])->getST_X() +
				filterIn[0][1][2]*((prostateCell*)tissue[i-1][1][j])->getST_X() +
				filterIn[0][2][2]*((prostateCell*)tissue[i-1][1][j+1])->getST_X() +
				filterIn[1][0][2]*((prostateCell*)tissue[i][1][j-1])->getST_X() +
				filterIn[1][1][2]*((prostateCell*)tissue[i][1][j])->getST_X() +
				filterIn[1][2][2]*((prostateCell*)tissue[i][1][j+1])->getST_X() +
				filterIn[2][0][2]*((prostateCell*)tissue[i+1][1][j-1])->getST_X() +
				filterIn[2][1][2]*((prostateCell*)tissue[i+1][1][j])->getST_X() +
				filterIn[2][2][2]*((prostateCell*)tissue[i+1][1][j+1])->getST_X() +
				filterIn[0][0][1]*((prostateCell*)tissue[i-1][0][j-1])->getST_X() +
				filterIn[0][1][1]*((prostateCell*)tissue[i-1][0][j])->getST_X() +
				filterIn[0][2][1]*((prostateCell*)tissue[i-1][0][j+1])->getST_X() +
				filterIn[1][0][1]*((prostateCell*)tissue[i][0][j-1])->getST_X() -
				17*((prostateCell*)tissue[i][0][j])->getST_X() +
				filterIn[1][2][1]*((prostateCell*)tissue[i][0][j+1])->getST_X() +
				filterIn[2][0][1]*((prostateCell*)tissue[i+1][0][j-1])->getST_X() +
				filterIn[2][1][1]*((prostateCell*)tissue[i+1][0][j])->getST_X() +
				filterIn[2][2][1]*((prostateCell*)tissue[i+1][0][j+1])->getST_X();
			resDiffusionIn[i][0][j] = tempGradIn*coupParam/17;

			tempGradOut =
				filterOut[0][0][2]*((prostateCell*)tissue[i-1][1][j-1])->getST_X() +
				filterOut[0][1][2]*((prostateCell*)tissue[i-1][1][j])->getST_X() +
				filterOut[0][2][2]*((prostateCell*)tissue[i-1][1][j+1])->getST_X() +
				filterOut[1][0][2]*((prostateCell*)tissue[i][1][j-1])->getST_X() +
				filterOut[1][1][2]*((prostateCell*)tissue[i][1][j])->getST_X() +
				filterOut[1][2][2]*((prostateCell*)tissue[i][1][j+1])->getST_X() +
				filterOut[2][0][2]*((prostateCell*)tissue[i+1][1][j-1])->getST_X() +
				filterOut[2][1][2]*((prostateCell*)tissue[i+1][1][j])->getST_X() +
				filterOut[2][2][2]*((prostateCell*)tissue[i+1][1][j+1])->getST_X() +
				filterOut[0][0][1]*((prostateCell*)tissue[i-1][0][j-1])->getST_X() +
				filterOut[0][1][1]*((prostateCell*)tissue[i-1][0][j])->getST_X() +
				filterOut[0][2][1]*((prostateCell*)tissue[i-1][0][j+1])->getST_X() +
				filterOut[1][0][1]*((prostateCell*)tissue[i][0][j-1])->getST_X() +
				17*((prostateCell*)tissue[i][0][j])->getST_X() +
				filterOut[1][2][1]*((prostateCell*)tissue[i][0][j+1])->getST_X() +
				filterOut[2][0][1]*((prostateCell*)tissue[i+1][0][j-1])->getST_X() +
				filterOut[2][1][1]*((prostateCell*)tissue[i+1][0][j])->getST_X() +
				filterOut[2][2][1]*((prostateCell*)tissue[i+1][0][j+1])->getST_X();
			resDiffusionOut[i][0][j] = tempGradOut*coupParam/17;
		}
			}

		///////////
		// corps //
		///////////
		/**
		 * coupling between middle cells
		 * */
		for (int k=1;k<m_nlayer-1;k++)
			{
	for (int i=1;i<m_ncol-1;i++)
		{
			for (int j=1;j<m_nrow-1;j++)
				{
		tempGradIn =
			filterIn[0][0][0]*((prostateCell*)tissue[k-1][i-1][j-1])->getST_X() +
			filterIn[0][0][1]*((prostateCell*)tissue[k-1][i-1][j])->getST_X() +
			filterIn[0][0][2]*((prostateCell*)tissue[k-1][i-1][j+1])->getST_X() +
			filterIn[0][1][0]*((prostateCell*)tissue[k-1][i][j-1])->getST_X() +
			filterIn[0][1][1]*((prostateCell*)tissue[k-1][i][j])->getST_X() +
			filterIn[0][1][2]*((prostateCell*)tissue[k-1][i][j+1])->getST_X() +
			filterIn[0][2][0]*((prostateCell*)tissue[k-1][i+1][j-1])->getST_X() +
			filterIn[0][2][1]*((prostateCell*)tissue[k-1][i+1][j])->getST_X() +
			filterIn[0][2][2]*((prostateCell*)tissue[k-1][i+1][j+1])->getST_X() +
			filterIn[1][0][0]*((prostateCell*)tissue[k][i-1][j-1])->getST_X() +
			filterIn[1][0][1]*((prostateCell*)tissue[k][i-1][j])->getST_X() +
			filterIn[1][0][2]*((prostateCell*)tissue[k][i-1][j+1])->getST_X() +
			filterIn[1][1][0]*((prostateCell*)tissue[k][i][j-1])->getST_X() +
			filterIn[1][1][1]*((prostateCell*)tissue[k][i][j])->getST_X() +
			filterIn[1][1][2]*((prostateCell*)tissue[k][i][j+1])->getST_X() +
			filterIn[1][2][0]*((prostateCell*)tissue[k][i+1][j-1])->getST_X() +
			filterIn[1][2][1]*((prostateCell*)tissue[k][i+1][j])->getST_X() +
			filterIn[1][2][2]*((prostateCell*)tissue[k][i+1][j+1])->getST_X() +
			filterIn[2][0][0]*((prostateCell*)tissue[k+1][i-1][j-1])->getST_X() +
			filterIn[2][0][1]*((prostateCell*)tissue[k+1][i-1][j])->getST_X() +
			filterIn[2][0][2]*((prostateCell*)tissue[k+1][i-1][j+1])->getST_X() +
			filterIn[2][1][0]*((prostateCell*)tissue[k+1][i][j-1])->getST_X() +
			filterIn[2][1][1]*((prostateCell*)tissue[k+1][i][j])->getST_X() +
			filterIn[2][1][2]*((prostateCell*)tissue[k+1][i][j+1])->getST_X() +
			filterIn[2][2][0]*((prostateCell*)tissue[k+1][i+1][j-1])->getST_X() +
			filterIn[2][2][1]*((prostateCell*)tissue[k+1][i+1][j])->getST_X() +
			filterIn[2][2][2]*((prostateCell*)tissue[k+1][i+1][j+1])->getST_X();
		resDiffusionIn[k][i][j] = tempGradIn*coupParam/26;

		tempGradOut =
			filterOut[0][0][0]*((prostateCell*)tissue[k-1][i-1][j-1])->getST_X() +
			filterOut[0][0][1]*((prostateCell*)tissue[k-1][i-1][j])->getST_X() +
			filterOut[0][0][2]*((prostateCell*)tissue[k-1][i-1][j+1])->getST_X() +
			filterOut[0][1][0]*((prostateCell*)tissue[k-1][i][j-1])->getST_X() +
			filterOut[0][1][1]*((prostateCell*)tissue[k-1][i][j])->getST_X() +
			filterOut[0][1][2]*((prostateCell*)tissue[k-1][i][j+1])->getST_X() +
			filterOut[0][2][0]*((prostateCell*)tissue[k-1][i+1][j-1])->getST_X() +
			filterOut[0][2][1]*((prostateCell*)tissue[k-1][i+1][j])->getST_X() +
			filterOut[0][2][2]*((prostateCell*)tissue[k-1][i+1][j+1])->getST_X() +
			filterOut[1][0][0]*((prostateCell*)tissue[k][i-1][j-1])->getST_X() +
			filterOut[1][0][1]*((prostateCell*)tissue[k][i-1][j])->getST_X() +
			filterOut[1][0][2]*((prostateCell*)tissue[k][i-1][j+1])->getST_X() +
			filterOut[1][1][0]*((prostateCell*)tissue[k][i][j-1])->getST_X() +
			filterOut[1][1][1]*((prostateCell*)tissue[k][i][j])->getST_X() +
			filterOut[1][1][2]*((prostateCell*)tissue[k][i][j+1])->getST_X() +
			filterOut[1][2][0]*((prostateCell*)tissue[k][i+1][j-1])->getST_X() +
			filterOut[1][2][1]*((prostateCell*)tissue[k][i+1][j])->getST_X() +
			filterOut[1][2][2]*((prostateCell*)tissue[k][i+1][j+1])->getST_X() +
			filterOut[2][0][0]*((prostateCell*)tissue[k+1][i-1][j-1])->getST_X() +
			filterOut[2][0][1]*((prostateCell*)tissue[k+1][i-1][j])->getST_X() +
			filterOut[2][0][2]*((prostateCell*)tissue[k+1][i-1][j+1])->getST_X() +
			filterOut[2][1][0]*((prostateCell*)tissue[k+1][i][j-1])->getST_X() +
			filterOut[2][1][1]*((prostateCell*)tissue[k+1][i][j])->getST_X() +
			filterOut[2][1][2]*((prostateCell*)tissue[k+1][i][j+1])->getST_X() +
			filterOut[2][2][0]*((prostateCell*)tissue[k+1][i+1][j-1])->getST_X() +
			filterOut[2][2][1]*((prostateCell*)tissue[k+1][i+1][j])->getST_X() +
			filterOut[2][2][2]*((prostateCell*)tissue[k+1][i+1][j+1])->getST_X();
		resDiffusionOut[k][i][j] = tempGradOut*coupParam/26;
				}
		}
			}
	}else{
		/**
		 * First row / First Cell
		 * */   
		tempGradIn = filterIn[1][1][2]*((prostateCell*)tissue[0][1][0])->getST_X() +
			filterIn[1][2][1]*((prostateCell*)tissue[0][0][1])->getST_X() +
			filterIn[1][2][2]*((prostateCell*)tissue[0][1][1])->getST_X() -
			(filterIn[1][1][2]+filterIn[1][2][1]+filterIn[1][2][2])*((prostateCell*)tissue[0][0][0])->getST_X();  
		resDiffusionIn[0][0][0] = tempGradIn*coupParam/3;
		
		tempGradOut = filterOut[1][1][2]*((prostateCell*)tissue[0][1][0])->getST_X() +
			filterOut[1][2][1]*((prostateCell*)tissue[0][0][1])->getST_X() +
			filterOut[1][2][2]*((prostateCell*)tissue[0][1][1])->getST_X() -
			(filterOut[1][1][2]+filterOut[1][2][1]+filterOut[1][2][2])*((prostateCell*)tissue[0][0][0])->getST_X();  
		resDiffusionOut[0][0][0] = tempGradOut*coupParam/3;

		/**
		 * First row / Last Cell
		 * */   
		tempGradIn = filterIn[1][1][0]*((prostateCell*)tissue[0][m_ncol-2][0])->getST_X() +
			filterIn[1][2][0]*((prostateCell*)tissue[0][m_ncol-2][1])->getST_X() +
			filterIn[1][2][1]*((prostateCell*)tissue[0][m_ncol-1][1])->getST_X() -
			(filterIn[1][1][0]+filterIn[1][2][0]+filterIn[1][2][1])*((prostateCell*)tissue[0][m_ncol-1][0])->getST_X();  
		resDiffusionIn[0][m_ncol-1][0] = tempGradIn*coupParam/3;
		
		tempGradOut = filterOut[1][1][0]*((prostateCell*)tissue[0][m_ncol-2][0])->getST_X() +
			filterOut[1][2][0]*((prostateCell*)tissue[0][m_ncol-2][1])->getST_X() +
			filterOut[1][2][1]*((prostateCell*)tissue[0][m_ncol-1][1])->getST_X() -
			(filterOut[1][1][0]+filterOut[1][2][0]+filterOut[1][2][1])*((prostateCell*)tissue[0][m_ncol-1][0])->getST_X();  
		resDiffusionOut[0][m_ncol-1][0] = tempGradOut*coupParam/3;

		/**
		 * Last row / First Cell
		 * */   
		tempGradIn = filterIn[1][0][1]*((prostateCell*)tissue[0][0][m_nrow-2])->getST_X() +
			filterIn[1][0][2]*((prostateCell*)tissue[0][1][m_nrow-2])->getST_X() +
			filterIn[1][1][2]*((prostateCell*)tissue[0][1][m_nrow-1])->getST_X() -
			(filterIn[1][0][1]+filterIn[1][0][2]+filterIn[1][1][2])*((prostateCell*)tissue[0][0][m_nrow-1])->getST_X();  
		resDiffusionIn[0][0][m_nrow-1] = tempGradIn*coupParam/3;
		
		tempGradOut = filterOut[1][0][1]*((prostateCell*)tissue[0][0][m_nrow-2])->getST_X() +
			filterOut[1][0][2]*((prostateCell*)tissue[0][1][m_nrow-2])->getST_X() +
			filterOut[1][1][2]*((prostateCell*)tissue[0][1][m_nrow-1])->getST_X() -
			(filterOut[1][0][1]+filterOut[1][0][2]+filterOut[1][1][2])*((prostateCell*)tissue[0][0][m_nrow-1])->getST_X();  
		resDiffusionOut[0][0][m_nrow-1] = tempGradOut*coupParam/3;


		/**
		 * Last row / Last Cell
		 * */   
		tempGradIn = filterIn[1][0][0]*((prostateCell*)tissue[0][m_ncol-2][m_nrow-2])->getST_X() +
			filterIn[1][0][1]*((prostateCell*)tissue[0][m_ncol-1][m_nrow-2])->getST_X() +
			filterIn[1][1][0]*((prostateCell*)tissue[0][m_ncol-2][m_nrow-1])->getST_X() -
			(filterIn[1][0][0]+filterIn[1][0][1]+filterIn[1][1][0])*((prostateCell*)tissue[0][m_ncol-1][m_nrow-1])->getST_X();
		resDiffusionIn[0][m_ncol-1][m_nrow-1] = tempGradIn*coupParam/3;
		
		tempGradOut = filterOut[1][0][0]*((prostateCell*)tissue[0][m_ncol-2][m_nrow-2])->getST_X() +
			filterOut[1][0][1]*((prostateCell*)tissue[0][m_ncol-1][m_nrow-2])->getST_X() +
			filterOut[1][1][0]*((prostateCell*)tissue[0][m_ncol-2][m_nrow-1])->getST_X() -
			(filterOut[1][0][0]+filterOut[1][0][1]+filterOut[1][1][0])*((prostateCell*)tissue[0][m_ncol-1][m_nrow-1])->getST_X();
		resDiffusionOut[0][m_ncol-1][m_nrow-1] = tempGradOut*coupParam/3;

		/**
		 * First row / Middle cells
		 * */
		for (int j=1;j<m_ncol-1;j++)
			{	
	tempGradIn = filterIn[1][1][0]*((prostateCell*)tissue[0][j-1][0])->getST_X() +
		filterIn[1][1][2]*((prostateCell*)tissue[0][j+1][0])->getST_X() +
		filterIn[1][2][0]*((prostateCell*)tissue[0][j-1][1])->getST_X() +
		filterIn[1][2][1]*((prostateCell*)tissue[0][j][1])->getST_X() +
		filterIn[1][2][2]*((prostateCell*)tissue[0][j+1][1])->getST_X() -
		(filterIn[1][0][0]+filterIn[1][0][1]+filterIn[1][0][2]+filterIn[1][1][0]+filterIn[1][1][2])*((prostateCell*)tissue[0][j][0])->getST_X();    
	resDiffusionIn[0][j][0] = tempGradIn*coupParam/5; 

	tempGradOut = filterOut[1][1][0]*((prostateCell*)tissue[0][j-1][0])->getST_X() +
		filterOut[1][1][2]*((prostateCell*)tissue[0][j+1][0])->getST_X() +
		filterOut[1][2][0]*((prostateCell*)tissue[0][j+1][1])->getST_X() +
		filterOut[1][2][1]*((prostateCell*)tissue[0][j][1])->getST_X() +
		filterOut[1][2][2]*((prostateCell*)tissue[0][j+1][1])->getST_X() -
		(filterOut[1][0][0]+filterOut[1][0][1]+filterOut[1][0][2]+filterOut[1][1][0]+filterOut[1][1][2])*((prostateCell*)tissue[0][j][0])->getST_X();    
	resDiffusionOut[0][j][0] = tempGradOut*coupParam/5; 
			
			}

		/**
		 * Last row / Middle Cells
		 * */
		for (int j=1;j<m_ncol-1;j++)
			{	
	tempGradIn = filterIn[1][0][0]*((prostateCell*)tissue[0][j-1][m_nrow-2])->getST_X() +
		filterIn[1][0][1]*((prostateCell*)tissue[0][j][m_nrow-2])->getST_X() +
		filterIn[1][0][2]*((prostateCell*)tissue[0][j+1][m_nrow-2])->getST_X() +
		filterIn[1][1][0]*((prostateCell*)tissue[0][j-1][m_nrow-1])->getST_X() +
		filterIn[1][1][2]*((prostateCell*)tissue[0][j+1][m_nrow-1])->getST_X() -
		(filterIn[1][0][0]+filterIn[1][0][1]+filterIn[1][0][2]+filterIn[1][1][0]+filterIn[1][1][2])*((prostateCell*)tissue[0][j][m_nrow-1])->getST_X();    
	resDiffusionIn[0][j][m_nrow-1] = tempGradIn*coupParam/5;
			
	tempGradOut = filterOut[1][0][0]*((prostateCell*)tissue[0][j-1][m_nrow-2])->getST_X() +
		filterOut[1][0][1]*((prostateCell*)tissue[0][j][m_nrow-2])->getST_X() +
		filterOut[1][0][2]*((prostateCell*)tissue[0][j+1][m_nrow-2])->getST_X() +
		filterOut[1][1][0]*((prostateCell*)tissue[0][j-1][m_nrow-1])->getST_X() +
		filterOut[1][1][2]*((prostateCell*)tissue[0][j+1][m_nrow-1])->getST_X() -
		(filterOut[1][0][0]+filterOut[1][0][1]+filterOut[1][0][2]+filterOut[1][1][0]+filterOut[1][1][2])*((prostateCell*)tissue[0][j][m_nrow-1])->getST_X();    
	resDiffusionOut[0][j][m_nrow-1] = tempGradOut*coupParam/5;
			

			}

		/**
		 * Last column / Middle Cells  
		 * */
		for (int i=1;i<m_nrow-1;i++)
			{	  
	tempGradIn = filterIn[1][0][0]*((prostateCell*)tissue[0][m_ncol-2][i-1])->getST_X() +
		filterIn[1][0][1]*((prostateCell*)tissue[0][m_ncol-1][i-1])->getST_X() +
		filterIn[1][1][0]*((prostateCell*)tissue[0][m_ncol-2][i])->getST_X() +
		filterIn[1][2][0]*((prostateCell*)tissue[0][m_ncol-2][i+1])->getST_X() +
		filterIn[1][2][1]*((prostateCell*)tissue[0][m_ncol-1][i+1])->getST_X() -
		(filterIn[1][0][0]+filterIn[1][0][1]+filterIn[1][1][0]+filterIn[1][2][0]+filterIn[1][2][1])*((prostateCell*)tissue[0][m_ncol-1][i])->getST_X();
	resDiffusionIn[0][m_ncol-1][i] = tempGradIn*coupParam/5;
			
	tempGradOut = filterOut[1][0][0]*((prostateCell*)tissue[0][m_ncol-2][i-1])->getST_X() +
		filterOut[1][0][1]*((prostateCell*)tissue[0][m_ncol-1][i-1])->getST_X() +
		filterOut[1][1][0]*((prostateCell*)tissue[0][m_ncol-2][i])->getST_X() +
		filterOut[1][2][0]*((prostateCell*)tissue[0][m_ncol-2][i+1])->getST_X() +
		filterOut[1][2][1]*((prostateCell*)tissue[0][m_ncol-1][i+1])->getST_X() -
		(filterOut[1][0][0]+filterOut[1][0][1]+filterOut[1][1][0]+filterOut[1][2][0]+filterOut[1][2][1])*((prostateCell*)tissue[0][m_ncol-1][i])->getST_X();
	resDiffusionOut[0][m_ncol-1][i] = tempGradOut*coupParam/5;

			}

		/**
		 * First column / Middle Cells
		 * */
		for (int i=1;i<m_nrow-1;i++) 
			{	  
	tempGradIn = filterIn[1][0][1]*((prostateCell*)tissue[0][0][i-1])->getST_X() +
		filterIn[1][0][2]*((prostateCell*)tissue[0][1][i-1])->getST_X() +
		filterIn[1][1][2]*((prostateCell*)tissue[0][1][i])->getST_X() +
		filterIn[1][2][1]*((prostateCell*)tissue[0][0][i+1])->getST_X() +
		filterIn[1][2][2]*((prostateCell*)tissue[0][1][i+1])->getST_X() -
		(filterIn[1][0][1]+filterIn[1][0][2]+filterIn[1][1][2]+filterIn[1][2][1]+filterIn[1][2][2])*((prostateCell*)tissue[0][0][i])->getST_X();
	resDiffusionIn[0][0][i] = tempGradIn*coupParam/5;
			
	tempGradOut = filterOut[1][0][1]*((prostateCell*)tissue[0][0][i-1])->getST_X() +
		filterOut[1][0][2]*((prostateCell*)tissue[0][1][i-1])->getST_X() +
		filterOut[1][1][2]*((prostateCell*)tissue[0][1][i])->getST_X() +
		filterOut[1][2][1]*((prostateCell*)tissue[0][0][i+1])->getST_X() +
		filterOut[1][2][2]*((prostateCell*)tissue[0][1][i+1])->getST_X() -
		(filterOut[1][0][1]+filterOut[1][0][2]+filterOut[1][1][2]+filterOut[1][2][1]+filterOut[1][2][2])*((prostateCell*)tissue[0][0][i])->getST_X();
	resDiffusionOut[0][0][i] = tempGradOut*coupParam/5;
			}

		for (int i=1;i<m_ncol-1;i++)
		{
			for (int j=1;j<m_nrow-1;j++)
			{
					tempGradIn =
						filterIn[1][0][0]*((prostateCell*)tissue[0][i-1][j-1])->getST_X() +
						filterIn[1][0][1]*((prostateCell*)tissue[0][i-1][j])->getST_X() +
						filterIn[1][0][2]*((prostateCell*)tissue[0][i-1][j+1])->getST_X() +
						filterIn[1][1][0]*((prostateCell*)tissue[0][i][j-1])->getST_X() -
						8*((prostateCell*)tissue[0][i][j])->getST_X() +
						filterIn[1][1][2]*((prostateCell*)tissue[0][i][j+1])->getST_X() +
						filterIn[1][2][0]*((prostateCell*)tissue[0][i+1][j-1])->getST_X() +
						filterIn[1][2][1]*((prostateCell*)tissue[0][i+1][j])->getST_X() +
						filterIn[1][2][2]*((prostateCell*)tissue[0][i+1][j+1])->getST_X();
					resDiffusionIn[0][i][j] = tempGradIn*coupParam/8;

					tempGradOut =
						filterOut[1][0][0]*((prostateCell*)tissue[0][i-1][j-1])->getST_X() +
						filterOut[1][0][1]*((prostateCell*)tissue[0][i-1][j])->getST_X() +
						filterOut[1][0][2]*((prostateCell*)tissue[0][i-1][j+1])->getST_X() +
						filterOut[1][1][0]*((prostateCell*)tissue[0][i][j-1])->getST_X() +
						8*((prostateCell*)tissue[0][i][j])->getST_X() +
						filterOut[1][1][2]*((prostateCell*)tissue[0][i][j+1])->getST_X() +
						filterOut[1][2][0]*((prostateCell*)tissue[0][i+1][j-1])->getST_X() +
						filterOut[1][2][1]*((prostateCell*)tissue[0][i+1][j])->getST_X() +
						filterOut[1][2][2]*((prostateCell*)tissue[0][i+1][j+1])->getST_X();
					resDiffusionOut[0][i][j] = tempGradOut*coupParam/8;
			}
		}
	}

	for (int k=0;k<m_nlayer;k++){
		for (int i=0;i<m_ncol;i++){
			for (int j=0;j<m_nrow;j++){
				((prostateCell*)tissue[k][i][j])->setIN_Z(initImageTissue[k][i][j] + resDiffusionIn[k][i][j] - resDiffusionOut[k][i][j]);
				((prostateCell*)tissue[k][i][j])->setIN_X(Conso_PO2[k][i][j]);
			}
		}
	}
	bool stable = true;
	for (int k=0;k<m_nlayer;k++)
		for (int i=0;i<m_ncol;i++)
			for (int j=0;j<m_nrow;j++)
				if(1.01*initImageTissue[k][i][j] <= ((prostateCell*)tissue[k][i][j])->getST_X())
					stable = false;

	if(stable){
		return 1;
	}
	else{
		return 0;
	}
}

int diffusion3D::terminateModel()
{
	for (int i=0;i<getNumComp();i++)
		(m_comp->at(i))->terminateModel();
	return 0;
}

/**
 * Fonction qui prend en paramètre des coordonné x et y et rend une position de vecteur.
 * */
int diffusion3D::Coord_XY_to_K(int x, int y)
{
	return y*m_ncol + x;
}
	
int diffusion3D::getNumRow() const{
	return m_nrow;
}
int diffusion3D::getNumCol() const{
	return m_ncol;
}
int diffusion3D::getNumLayer() const{
	return m_nlayer;
}

