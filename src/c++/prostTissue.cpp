/**
 * @file prostTissue.cpp
 * @brief
 * @author Carlos Sosa Marrero
 * @author Nicolas Ciferri
 * @author Alfredo Hernandez
 * @date 05.19.17
 */

#include <iostream>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <vector>

#include "prostTissue.hpp"

using namespace std;

ProstTissue::ProstTissue(const int nrow, const int ncol,
			 const int nlayer,
			 Treatment *const treatment) :
	Model(0, 0, 2, 2, nrow * ncol * nlayer){
	m_nrow   = nrow;
	m_ncol   = ncol;
	m_nlayer = nlayer;
	
	m_treatment = treatment;
		
	//Creation of the cells composing the tissue model
	for(int k(0); k < m_numComp; k++){
		m_comp->at(k) = new ProstCell(this);
		m_numOut += (m_comp->at(k))->getNumOut();
	}
}


ProstTissue::ProstTissue(const int nrow, const int ncol,
			 const int nlayer,
			 const string nFInTum, const string nFInVes,
			 const double doubTime,
			 vector<double> cycDur,
			 vector<double> cycDistrib,
			 const double apopDeadTime,
			 const double necDeadTime,
			 const double apopProb,
			 vector<double> alpha, vector<double> beta,
			 Treatment *const treatment) :
	Model(0, 0, 2, 2, nrow * ncol * nlayer){
	double inputTimer, inputTum, inputVes, selInitPhase;
	vector<Model **> map2D;
	ifstream fInTum(nFInTum.c_str());
	ifstream fInVes(nFInVes.c_str());

	m_nrow   = nrow;
	m_ncol   = ncol;
	m_nlayer = nlayer;
	
	m_treatment = treatment;
	
	//Creation of the cells composing the tissue model
	for(int k(0); k < m_numComp; k++){
		m_comp->at(k) = new ProstCell(doubTime, cycDur, apopDeadTime,
					necDeadTime, apopProb, alpha,
					beta, this);
		m_numOut += (m_comp->at(k))->getNumOut();
	}
	for(int i(0); i < m_nrow * m_nlayer; i++){
		map2D.push_back(&(m_comp->at(i * m_ncol)));
	}
	for(int l(0); l < m_nlayer; l++){
		m_map.push_back(&(map2D[l * m_nrow]));
	}
	
	//Definition of each cell's edge
	for(int l(0); l < m_nlayer; l++){
		for(int i(0); i < m_nrow; i++){
			for(int j(0); j < m_ncol; j++){
	for(int ll(-1); ll <= 1; ll++){
		for(int ii(-1); ii <= 1; ii++){
			for(int jj(-1); jj <= 1; jj++){
				if(ii != 0 || jj != 0 || ll != 0){
		if(l + ll >= 0 && l + ll < m_nlayer && i + ii >= 0
			 && i + ii < m_nrow && j + jj >= 0 &&
			 j + jj < m_ncol){
			((ProstCell *)m_map[l][i][j])
				->addToEdge(((ProstCell *)m_map[l + ll][i + ii]
				 [j + jj]));
		}
				}
			}
		}
	}
			}
		}
	}
	
	//Initialization of the cells state 
	if(fInTum.is_open() == 0){
		cout << "An error occurred while opening initial tumor" <<
			"data file" << endl;
	}
	else if(fInVes.is_open() == 0){
		cout << "An error occurred while opening initial vessel" <<
			"data file" << endl;
	}
	else{
		srand(time(NULL));
		for(int k(0); k < m_numComp; k++){
			if(fInTum >> inputTum){
	((ProstCell *)m_comp->at(k))->setInTum(inputTum);
			}
			else{
	cout << "Insufficient data in tumor file" << endl;
	break;
			}
			if(fInVes >> inputVes){
	if(inputTum && inputVes){
		cout << "Conflict between initial data. Cell "<< k <<
			" is both tumor and vessel" << endl;
		break;
	}
	else{
		((ProstCell *)m_comp->at(k))->setInVes(inputVes);
	}
			}
			else{
	cout << "Insufficient data in vessel file" << endl;
	break;
			}
			
			selInitPhase = (double)rand() / (double)(RAND_MAX);
			if(selInitPhase < cycDistrib.at(0)){
	inputTimer = rand() % (int)(cycDur.at(0) * doubTime);
			}
			else if(selInitPhase < cycDistrib.at(0) + cycDistrib.at(1)){
	inputTimer = cycDur.at(0) * doubTime +
		rand() % (int)(cycDur.at(1) * doubTime);
			}
			else if(selInitPhase < cycDistrib.at(0) + cycDistrib.at(1) +
				cycDistrib.at(2)){
	inputTimer = (cycDur.at(0) + cycDur.at(1)) * doubTime +
		rand() % (int)(cycDur.at(2) * doubTime);
			}
			else{
	inputTimer = (cycDur.at(0) + cycDur.at(1) + cycDur.at(2)) *
		doubTime + rand() % (int)(cycDur.at(3) * doubTime);
			}
			((ProstCell *)m_comp->at(k))->setInTimer(inputTimer);
			(m_comp->at(k))->initModel();
			((ProstCell *)m_comp->at(k))->setInTum(0.0);
			((ProstCell *)m_comp->at(k))->setInVes(0.0);
		}
		fInTum.close();
		fInVes.close();
	}
}


ProstTissue::~ProstTissue(){
}


int ProstTissue::calcModelOut(){
	for(int k(0); k < m_numComp; k++){
		(m_comp->at(k))->calcModelOut();
	}
	OUT_TUM_DENS = (double)getNumTum() / (double)m_numComp * 100;
	OUT_VES_DENS = (double)getNumVes() / (double)m_numComp * 100;
	return 0;
}


int ProstTissue::initModel(const double DT){
	PAR_INIT_TUM_DENS = (double)getNumTum() / (double)m_numComp * 100;
	OUT_TUM_DENS = PAR_INIT_TUM_DENS;
	PAR_INIT_VES_DENS = (double)getNumVes() / (double)m_numComp * 100;
	OUT_VES_DENS = PAR_INIT_VES_DENS;
	m_flag = 0;
	cout << "Total number of cells = " << m_numComp << endl;
	cout << "Initial number of cells at G1 = " << getNumG1() << endl;
	cout << "Initial number of cells at S = " << getNumS() << endl;
	cout << "Initial number of cells at G2 = "<< getNumG2() << endl;
	cout << "Initial number of cells at M = "<< getNumM() << endl;
	cout << "Initial number of living cells = "
			 << getNumAlive() << endl;
	cout << "Initial number of tumor cells = " << getNumTum() << endl;
	cout << "Initial tumor density: " << PAR_INIT_TUM_DENS << "%"
			 << endl;
	cout << "Initial number of vessels = " << getNumVes() << endl;
	cout << "Initial vascular density: " << PAR_INIT_VES_DENS << "%"
			 << endl;
	cout << "Initial number of dead cells = " << getNumDead() << endl;
	cout << "---------------------------------------------" << endl;
	return 0;
}


//It does nothing for the moment
int ProstTissue::startModel(){
	for (int k(0) ;k < m_numComp; k++){
		(m_comp->at(k))->startModel();
	}
	return 0;
}


int ProstTissue::terminateModel(){
	for(int k(0); k < m_numComp; k++){
		(m_comp->at(k))->terminateModel();
	}
	cout << "---------------------------------------------" << endl;
	cout << "Final number of cells at G1 = " << getNumG1() << endl;
	cout << "Final number of cells at S = " << getNumS() << endl;
	cout << "Final number of cells at G2 = " << getNumG2() << endl;
	cout << "Final number of cells at M = " << getNumM() << endl;
	cout << "Final number of living cells = "
			 << getNumAlive() << endl;
	cout << "Final number of tumor cells = " << getNumTum() << endl;
	cout << "Final tumor density: " << OUT_TUM_DENS << "%" << endl;
	cout << (PAR_INIT_TUM_DENS - OUT_TUM_DENS) / PAR_INIT_TUM_DENS
		* 100 << "% of initial tumor cells killed" << endl;
	cout << "Final number of vessels = " << getNumVes() << endl;
	cout << "Final vascular density: " << OUT_VES_DENS << "%" << endl;
	cout << "Final number of dead cells = "<< getNumDead() << endl;

	cout << "---------------------------------------------" << endl;
	return 0;
}


int ProstTissue::updateModel(const double currentTime,
					 const double DT){
	for(int k(0); k < m_numComp; k++){
		(m_comp->at(k))->updateModel(currentTime, DT);
	}
	
	if(m_treatment){
		int print(0);
		double tumSurv;
		tumSurv = OUT_TUM_DENS / PAR_INIT_TUM_DENS;
		if(tumSurv < 0.5){
			print = 1;
		}
		if(tumSurv < 0.2){
			print = 2;
		}
		if(tumSurv < 0.1){
			print = 3;
		}
		if(tumSurv < 0.05){
			print = 4;
		}
		if(tumSurv < 0.01){
			print = 5;
		}
		if(tumSurv < 0.001){
			print = 6;
		}
		if(print > m_flag){
			printNeededDose();
			m_flag++;
			cout << currentTime << endl;
		}

	}
	return 0;
}


int ProstTissue::getNumAlive() const{
	int count(0);
	for(int k(0); k < m_numComp; k++){
		if(((ProstCell *)m_comp->at(k))->getAlive()){
			count++;
		}
	}
	return count;
}

int ProstTissue::getNumCol() const{
	return m_ncol;
}

int ProstTissue::getNumDead() const{
	int count(0);
	for(int k(0); k < m_numComp; k++){
		if(((ProstCell *)m_comp->at(k))->getDead()){
			count++;
		}
	}
	return count;
}


int ProstTissue::getNumG1() const{
	int count(0);
	for(int k(0); k < m_numComp; k++){
		if(((ProstCell *)m_comp->at(k))->getG1()){
			count++;
		}
	}
	return count;
}


int ProstTissue::getNumG2() const{
	int count(0);
	for(int k(0); k < m_numComp; k++){
		if(((ProstCell *)m_comp->at(k))->getG2()){
			count++;
		}
	}
	return count;
}

int ProstTissue::getNumLayer() const{
	return m_nlayer;
}

int ProstTissue::getNumM() const{
	int count(0);
	for(int k(0); k < m_numComp; k++){
		if(((ProstCell *)m_comp->at(k))->getM()){
			count++;
		}
	}
	return count;
}

int ProstTissue::getNumRow() const{
	return m_nrow;
}

int ProstTissue::getNumS() const{
	int count(0);
	for(int k(0); k < m_numComp; k++){
		if(((ProstCell *)m_comp->at(k))->getS()){
			count++;
		}
	}
	return count;
}


int ProstTissue::getNumTum() const{
	int count(0);
	for(int k(0); k < m_numComp; k++){
		if(((ProstCell *)m_comp->at(k))->getTum()){
			count++;
		}
	}
	return count;
}


int ProstTissue::getNumVes() const{
	int count(0);
	for(int k(0); k < m_numComp; k++){
		if(((ProstCell *)m_comp->at(k))->getVes()){
			count++;
		}
	}
	return count;
}


Treatment *ProstTissue::getTreatment() const{
	return m_treatment;
}


void ProstTissue::printNeededDose() const{
	string perc;
	
	switch(m_flag){
	case 0:
		perc = "50";
		break;
	case 1:
		perc = "80";
		break;
	case 2:
		perc = "90";
		break;
	case 3:
		perc = "95";
		break;
	case 4:
		perc = "99";
		break;
	case 5:
		perc = "99.9";
		break;
	}
 
	cout << "Total dose needed to kill " << perc <<
		"% of tumor cells = " <<
		((ProstCell*)m_comp->at(0))->getAccDose() << endl;
}


