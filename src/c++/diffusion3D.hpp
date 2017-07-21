/**
 * @file diffusion3D.hpp
 * @brief
 * @author Carlos Sosa Marrero
 * @author Nicolas Ciferri
 * @author Alfredo Hernandez
 * @date 05.19.17
 */

#ifndef diffusion3D_H
#define diffusion3D_H

#include "prostateCell.hpp"
#include <iostream>
#include "math.h"
#include <cmath>
#include <fstream>
#include <sstream>

#define FILTERSIZE 3
#define INIT_VASCULAR_PO2 42.0

using namespace std;

class diffusion3D : public Model {

public:
	// constructor / destructor
	diffusion3D(const int nrow, const int ncol, const int nlayer, const string nFInVes
				,double Vmax, double K_conso);
	~diffusion3D();

	//variables
	double Tm, Lm;
	double coupParam;
	double m_Vmax;
	double m_K_conso;
	double coeff_diff;
	double size;
	double time_step_s;
	double mat_diff;
	double pO2;
	double conso_pO2;
	double delta;

	double filterIn[FILTERSIZE][FILTERSIZE][FILTERSIZE];
	double filterOut[FILTERSIZE][FILTERSIZE][FILTERSIZE];



	// methods
	virtual int initModel(const double DT);
	virtual int initModel(int x,int y,int z,double pO2);
	virtual int calcModelOut();
	virtual int updateModel(double time, const double DT);
	virtual int terminateModel();
	virtual int startModel();
	virtual int Coord_XY_to_K(int x, int y);
	
	int getNumCol() const;
	int getNumRow() const;
	int getNumLayer() const;

	int m_ncol, m_nlayer, m_nrow;
	vector<double> vectorTissue;
	vector<Model *> Tissue;

};


#endif
