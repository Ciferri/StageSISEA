/**
 * @file Image3D.cpp
 * @brief
 * @author Nicolas Ciferri
 * @date 05.19.17
 */

#include "Image3D.hpp"
#include "Header_pgm.hpp"


using namespace std;
using MyImage::Image3D;
using My_io_pgm::Header;

Image3D::Image3D(){
	int valeur = 0;
	(*this).nbx = 101;
	(*this).nby = 62;
	(*this).nbz = 1;
	(*this).number_grey_level = 255;
	(*this).dx = 1;
	(*this).dy = 1;
	(*this).dz = 1;
	(*this).size = (*this).nbx*(*this).nby*(*this).nbz;
	alloue_partie_dynamique(nbx,nby,nbz);
	init(valeur);
}

Image3D::Image3D(int _nbx,int _nby,int _nbz, double _dx, double _dy, double _dz){
	(*this).nbx = _nbx;
	(*this).nby = _nby;
	(*this).nbz = _nbz;
	(*this).number_grey_level = pow(2,20);
	(*this).dx = _dx;
	(*this).dy = _dy;
	(*this).dz = _dz;
	(*this).size = nbx*nby*nbz;
	alloue_partie_dynamique(nbx,nby,nbz);
	init(0);
}

Image3D::Image3D(const string colorMap,int _nbx,int _nby,int _nbz, double _dx, double _dy, double _dz){
	(*this).nbx = _nbx;
	(*this).nby = _nby;
	(*this).nbz = _nbz;
	(*this).number_grey_level = pow(2,20);
	(*this).dx = _dx;
	(*this).dy = _dy;
	(*this).dz = _dz;
	(*this).size = nbx*nby*nbz;
	alloue_partie_dynamique(nbx,nby,nbz);
	init(0);

	ifstream vectcolor(colorMap.c_str());
	double tmp;
	int i=0;

	if(vectcolor){
		while ( vectcolor >> tmp){
			vect.push_back(tmp);
			i++;
		}
	}
}

Image3D::Image3D(const char* filepath_pgm, double _dx,double _dy, double _dz){
	Header header(filepath_pgm);
	(*this).nbx = header.get_dimx();
	(*this).nby = header.get_dimy();
	(*this).nbz = header.get_dimz();
	(*this).number_grey_level = header.get_NumberGreyLevel();
	(*this).size = dx*dy*dz;
	(*this).dx = _dx;
	(*this).dy = _dy;
	(*this).dz = _dz;
	alloue_partie_dynamique(nbx,nby,nbz);
	ifstream pgm_image(filepath_pgm, ios_base::in);//l'inverse c'est : ofstream(char* , iosbase::out)
	if(!pgm_image){
		cerr << "Is not a file : " << filepath_pgm << endl;
		exit(-1);
	}
	char buffer[512];
	int dim;
	pgm_image.getline(buffer,512);
	pgm_image.getline(buffer,512);
	pgm_image.getline(buffer,512);
	pgm_image >> dim;
	pgm_image >> dim;
	pgm_image >> dim;
	int val;

	for(int k=0;k< ((*this).nbz);k++)
		for(int i=0;i< ((*this).nbx);i++)
			for(int j=0;j< ((*this).nby);j++){
				pgm_image >> val;
				(*this).ptr[i][j][k]=val;
			}
	pgm_image.close();
}


Image3D::Image3D( const Image3D& ima ){
	(*this).nbx = ima.nbx;
	(*this).nby = ima.nby;
	(*this).nbz = ima.nbz;
	(*this).number_grey_level = ima.number_grey_level;
	(*this).dx = ima.dx;
	(*this).dy = ima.dy;
	(*this).dz = ima.dz;
	(*this).size = ima.size;
	(*this).alloue_partie_dynamique(nbx,nby,nbz);
	for(int k=0;k<((*this).nbz);k++)
		for(int i=0;i< ((*this).nbx);i++)
			for(int j=0;j< ((*this).nby);j++)
				(*this).ptr[i][j][k] = ima.ptr[i][j][k];
}

Image3D::~Image3D(){
	(*this).libere_partie_dynamique();
}

void Image3D::alloue_partie_dynamique(int dimx, int dimy, int dimz){
	if(dimx<=0 || dimy<=0 || dimz<=0){
		std::cout<<"Dimension de l'allocation anormales" << endl;
		std::cout<<"dimx : "<<dimx<<", dimy : " << dimy << ", dimz : " << dimz << endl;
		exit(1);
	}
	ptr = new int** [dimx];
	for(int i=0;i<dimx;i++){
		ptr[i] = new int*[dimy];
		for(int j=0;j<dimy;j++){
			ptr[i][j] = new int[dimz];
			if(ptr == NULL || ptr[i]==NULL || ptr[i][j]==NULL){
				std::cout<<"probleme allocation memoire"<<endl;
				exit(1);
			}
		}
	}
}

void Image3D::libere_partie_dynamique(){
	if(ptr != NULL){
		for(int i=0;i< ((*this).nbx);i++){
			if(ptr[i]!=NULL){
	delete [] ptr[i];
			}
		}
		delete [] ptr;
		ptr=NULL;
	}
}

int Image3D::get_Nbx() const{
	return (*this).nbx;
}
int Image3D::get_Nby() const{
	return (*this).nby;
}
int Image3D::get_Nbz() const{
	return (*this).nbz;
}
int Image3D::get_NumberGreyLevel() const{
	return (*this).number_grey_level;
}
long int Image3D::get_Size() const{
	return (*this).size;
}
double Image3D::get_Dx() const{
	return (*this).dx;
}
double Image3D::get_Dy() const{
	return (*this).dy;
}
double Image3D::get_Dz() const{
	return (*this).dz;
}
int Image3D::get_PixelValue(int k,int i,int j) const{
	assert(i<(*this).nbx);
	assert(j<(*this).nby);
	assert(k<(*this).nbz);
	int val_i_j_k = 0;
	val_i_j_k = (*this).ptr[i][j][k];
	return val_i_j_k;
}
void Image3D::init(int valeur_init){
	for (int k = 0;k<(*this).nbz ; k++)
		for (int i = 0;i<(*this).nbx ; i++)
			for(int j=0;j<(*this).nby;j++)
	(*this).ptr[i][j][k] = valeur_init;
}
void Image3D::set_PixelValue(int i,int j,int k, int val){
	assert(i< (*this).nbx);
	assert(j< (*this).nby);
	assert(k< (*this).nbz);
	(*this).ptr[i][j][k] = val;
}
void Image3D::set_Nbx(int n){
	nbx = n;
}
void Image3D::set_Nby(int n){
	nby = n;
}
void Image3D::set_Nbz(int n){
	nbz = n;
}
void Image3D::set_Size(long int s){
	size = s;
}
void Image3D::set_NumberGreyLevel(int n){
	number_grey_level = n;
}
void Image3D::set_Dx(double d){
	dx = d;
}
void Image3D::set_Dy(double d){
	dy = d;
}
void Image3D::set_Dz(double d){
	dz = d;
}
void Image3D::print() const{
	cout << "************Image3D************\n";
	cout << "nbx : " << (*this).nbx << endl;
	cout << "nby : " << (*this).nby << endl;
	cout << "nbz : " << (*this).nbz << endl;
	cout << "Nombre de niveaux de gris :  " << (*this).number_grey_level << endl;
	cout << "dx : " << (*this).dx << endl;
	cout << "dy : " << (*this).dy << endl;
	cout << "dz : " << (*this).dz << endl;
}

void Image3D::write_pgm(const char* filename) const{
	FILE *file;
	file=fopen(filename,"w");
	if( file == NULL){
		cout << "probleme d'ouverture du fichier" << filename << endl;
		exit(1);
	}
	fprintf(file,"P2\n%s\n%d %d\n255\n","# File produced by me", (*this).nbx,(*this).nby );
	int val_current;
	for(int k=0;k<(*this).nbz;k++)
		for(int i=0;i<(*this).nbx;i++)
			for(int j=0;j<(*this).nby;j++){
				val_current = (*this).ptr[i][j][k];
				if(val_current <0)
					val_current = 0;
				if(val_current > (*this).number_grey_level )
					val_current = (*this).number_grey_level;
				fprintf(file,"%d\n",val_current);
			}
	fclose(file);
}

void Image3D::write_pgm_with_index(const char* filename, const char* type, const int nb) const{
	std::stringstream fichier;
	fichier << filename << nb << type;

	FILE *file;
	file=fopen(fichier.str().c_str(),"w");
	if( file == NULL){
		cout << "probleme d'ouverture du fichier" << filename << endl;
		exit(1);
	}
	fprintf(file,"P2\n%s\n%d %d\n255\n","# File produced by me", (*this).nbx,(*this).nby);

	int val_current;
	for(int k=0;k<(*this).nbz;k++)
		for(int i=0;i<(*this).nbx;i++)
			for(int j=0;j<(*this).nby;j++){
				val_current = (*this).ptr[i][j][k];
				if(val_current <0)
					val_current = 0;
				if(val_current > (*this).number_grey_level )
					val_current = (*this).number_grey_level;
				fprintf(file,"%d\n",val_current);
			}
	fclose(file);
}


bool Image3D::operator== ( const Image3D& ima){
	if(((*this).nbx!=ima.nbx)||(*this).nby!=ima.nby||(*this).nbz!=ima.nbz){
		std::cout << "pb de dim\n";
		return false;
	}else{
		for(int k=0;k<nbz;k++)
			for(int i=0;i<nbx;i++)
				for(int j=0;j<nby;j++)
					if((*this).ptr[i][j][k]!=ima.ptr[i][j][k])
						return false;
		return true;
	}
}



void Image3D::printImageWithVector(vector<double>& PO2,
																	 vector<double>& state,
																	 vector<double>& Timer,
																	 int nb, Image3D image){
	int ncol = image.get_Nbx();
	int nrow = image.get_Nby();
	int nlayer = image.get_Nbz();
	Image3D imagePO2(ncol,nrow,nlayer,1,1,1);
	Image3D imageState(ncol,nrow,nlayer,1,1,1);
	Image3D imageTimer(ncol,nrow,nlayer,1,1,1);
	int cpt=0;
	for(int l=0;l<nlayer;l++)
		for(int i=0;i<ncol;i++)
			for(int r=0;r<nrow;r++){
				imagePO2.set_PixelValue(i,r,l,PO2.at(cpt));
				imageState.set_PixelValue(i,r,l,state.at(cpt));
				imageTimer.set_PixelValue(i,r,l,Timer.at(cpt));
				cpt += 1;
			}
	imagePO2.write_pgm_with_index("./data_out/imgPO2/imgPO2_",".pgm",nb);
	imageState.write_pgm_with_index("./data_out/imgState/imgState_",".pgm",nb);
	imageTimer.write_pgm_with_index("./data_out/imgTimer/imgTimer_",".pgm",nb);
}

Pixel Image3D::colorBar(double val,double vmin,double vmax){
	double dv = vmax - vmin;
	Pixel pixel = {255,255,255};
	if (val < (0.25 * dv)) {
		pixel.r = 0;
		pixel.g = (4 * val / dv)*255;
	}else if (val < (0.5 * dv)) {
		pixel.r = 0;
		pixel.b = (1 + 4 * (0.25 * dv - val) / dv)*255;
	}else if (val < (0.75 * dv)) {
		pixel.r = (4 * (val - 0.5 * dv) / dv)*255;
		pixel.b = 0;
	}else {
		pixel.g = (1 + 4 * (0.75 * dv - val) / dv)*255;
		pixel.b = 0;
	}
	return pixel;
}


Pixel Image3D::colorBar(double val,double dv){
	Pixel pixel = {255,255,255};
	if(val < (0.25*dv)){
		pixel.r = 255;
		pixel.g = 255;
		pixel.b = 255;
	}else if(val < 0.5*dv){
		pixel.r = 0;
		pixel.g = 0;
		pixel.b = 255;
	}else if(val < 0.75*dv){
		pixel.r = 255;
		pixel.g = 0;
		pixel.b = 0;
	}else{
		pixel.r = 0;
		pixel.g = 0;
		pixel.b = 0;
	}
	return pixel;
}
/**
 * Fonction qui prend en paramètre une valeur et un colormap et lui rend un pixel
 *
 * */
Pixel Image3D::colorMap(double val,double vmin,double vmax){

	Pixel pixel = {255,255,255};
	double dv = vmax - vmin;
	double size = vect.size();
	double valeur = (val*size)/dv;
	int indice = floor(valeur);
	int reste = indice%3;

	if(indice >= size)
		indice = size -3;

	pixel.r = (vect.at(indice-reste))*255;
	pixel.g = (vect.at(indice+1-reste))*255;
	pixel.b = (vect.at(indice+2-reste))*255;

	return pixel;
}

void Image3D::printImageBMP(vector<double>& PO2,
														vector<double>& state,
														vector<double>& Timer,
														vector<double>& cycle,
														int nb, Image3D image){
	int ncol = image.get_Nbx();
	int nrow = image.get_Nby();
	int nlayer = image.get_Nbz();
	int i,j;
	int cpt =0;

	Image* imgState = NouvelleImage(ncol,nrow);
	Image* imgPO2 = NouvelleImage(ncol,nrow);
	Image* imgCycle = NouvelleImage(ncol,nrow);

	Image* ColourBar = NouvelleImage(255,100);
	Pixel pixelColourBar;

	for(int k=0;k<nlayer;k++)
		for(i=0;i<nrow;i++)
		{
			for(j=0;j<ncol;j++)
			{
				Pixel pixelPO2 = colorMap(PO2.at(cpt),0,42);
				Pixel pixelState = colorMap(state.at(cpt),0,3);
				Pixel pixelCycle = colorMap(cycle.at(cpt),0,6);

				SetPixel(imgCycle,j,i,pixelCycle);
				SetPixel(imgPO2,j,i ,pixelPO2);
				SetPixel(imgState,j,i,pixelState);
				cpt++;
			}
		}

	for(int i =0;i<255;i++){
		for(int j = 0;j<100;j++){
			pixelColourBar = colorMap(i,0,255);
			SetPixel(ColourBar,i,j,pixelColourBar);
		}
	}

	Sauver(imgState,"./data_out/imgState/imgState_",".bmp",nb);
	Sauver(imgPO2,"./data_out/imgPO2/imgPO2_",".bmp",nb);
	Sauver(imgCycle,"./data_out/imgCycle/imgCycle_",".bmp",nb);
	Sauver(ColourBar,"./data_out/colourBar",".bmp",1);
}

void Image3D::printImagePNG(vector<double>& PO2,
														vector<double>& state,
														vector<double>& Timer,
														vector<double>& cycle,
														int nb, Image3D image){

	unsigned width = image.get_Nbx(), height = image.get_Nby();
	std::vector<unsigned char> vectImagePO2, vectImageState, vectImageCycle,vectImageColorBar;

	// nécessaire car RGBA, donc 4 canaux
	vectImagePO2.resize(width * height * 4);
	vectImageState.resize(width * height * 4);
	vectImageCycle.resize(width * height * 4);
	vectImageColorBar.resize(100*255*4);

	int cpt = 0;
	for(unsigned y = 0; y < height; y++)
		for(unsigned x = 0; x < width; x++)
		{
			Pixel pixelPO2 = colorMap(PO2.at(cpt),0,42);
			vectImagePO2[4*x + 4*y * width + 0]= pixelPO2.r;        // R
			vectImagePO2[4*x + 4*y * width + 1]= pixelPO2.g;        // G
			vectImagePO2[4*x + 4*y * width + 2]= pixelPO2.b;        // B
			vectImagePO2[4*x + 4*y * width + 3]= 255;               // A

			Pixel pixelState = colorMap(state.at(cpt),0,3);
			vectImageState[4*x + 4*y * width + 0]= pixelState.r;    // R
			vectImageState[4*x + 4*y * width + 1]= pixelState.g;    // G
			vectImageState[4*x + 4*y * width + 2]= pixelState.b;    // B
			vectImageState[4*x + 4*y * width + 3]= 255;             // A

			Pixel pixelCycle = colorMap(cycle.at(cpt),0,6);
			vectImageCycle[4*x + 4*y * width + 0]= pixelCycle.r;    // R
			vectImageCycle[4*x + 4*y * width + 1]= pixelCycle.g;    // G
			vectImageCycle[4*x + 4*y * width + 2]= pixelCycle.b;    // B
			vectImageCycle[4*x + 4*y * width + 3]= 255;             // A

			cpt += 1;
		}

	for(int i =254;i>=0;i--){
		for(int j = 99;j>=0;j--){
			Pixel pixelColourBar = colorMap(i,0,255);
			vectImageColorBar[4*j + 4*i * 100 + 0]= pixelColourBar.r;		// R
			vectImageColorBar[4*j + 4*i * 100 + 1]= pixelColourBar.g;		// G
			vectImageColorBar[4*j + 4*i * 100 + 2]= pixelColourBar.b;		// B
			vectImageColorBar[4*j + 4*i * 100 + 3]= 255;								// A
		}
	}

	std::stringstream fichierPO2,fichierState,fichierTimer,fichierCycle,fichierColorBar;
	fichierPO2 << "./data_out/imgPO2/imgPO2_" << nb << ".png";
	string cheminPO2 = fichierPO2.str();
	fichierState << "./data_out/imgState/imgState_" << nb << ".png";
	string cheminState = fichierState.str();
	fichierCycle << "./data_out/imgCycle/imgCycle_" << nb << ".png";
	string cheminCycle = fichierCycle.str();

	lodepng::encode(cheminPO2, vectImagePO2, width, height);
	lodepng::encode(cheminState, vectImageState, width, height);
	lodepng::encode(cheminCycle, vectImageCycle, width, height);
	lodepng::encode("./data_out/ColorBar.png", vectImageColorBar, 100, 255);

}



