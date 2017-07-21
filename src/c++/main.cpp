/**
 * @file main.cpp
 * @brief
 * @author Carlos Sosa Marrero
 * @author Nicolas Ciferri
 * @author Alfredo Hernandez
 * @date 05.19.17
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <stdlib.h>
#include <string>

#include "coupler.hpp"
#include "diffusion3D.hpp"
#include "prostTissue.hpp"
#include "model.hpp"
#include "prostateCell.hpp"
#include "prostCell.hpp"
#include "rootSimulator.hpp"
#include "simulator.hpp"
#include "treatment.hpp"
#include <QApplication>
#include <QPushButton>
#include <QPicture>
#include "MaFenetre.hpp"

using namespace std;

int main(int argc, char *argv[]){
  Coupler *coupler;
  ProstTissue *model1;
  diffusion3D *model2;
  int nrow, ncol, nlayer;
  double doubTime;
  double apopDeadTime, apopProb, necDeadTime;
  double KConso, Vmax;
  int schedule;
  double fraction, interval, totalDose;
  string nFInTum, nFInVes, nFTissueDim;
  vector<double> alpha(7, 0.0);
  vector<double> beta(7, 0.0);
  vector<double> cycDistrib(4, 0.0);
  vector<double> cycDur(4, 0.0);
  Treatment *treatment;
  RootSimulator *sim;
  double DT1, DT2, simTime;

  if(argc != 33){
    cout << "Incorrect number of parameters. " <<
      "32 parameters expected." << endl;
    cout << "doubTime G1Dur SDur G2Dur MDur G1Distrib SDistrib " <<
      "G2Distrib MDistrib alphaAlive alphaTumG1 alphaTumS " <<
      "alphaTumG2 alphaTumM alphaDead alphaVes betaAlive " <<
      "betaTumG1 betaTumS betaTumG2 betaTumM betaDead betaVes " <<
      "apopProb apopDeadTime necDeadTime Vmax KConso fraction " <<
      "interval totalDose schedule" << endl;
    return EXIT_FAILURE;
    }
  
  
  nFTissueDim = "./src/data/tissueDim.dat";
  nFInTum = "./src/data/inTum.dat";
  nFInVes = "./src/data/inVes.dat";

  ifstream fTissueDim(nFTissueDim.c_str());
  if(fTissueDim.is_open()){
    fTissueDim >> nrow;
    fTissueDim >> ncol;
    fTissueDim >> nlayer;
    fTissueDim.close();
    cout << "Tissue dimensions: "  << endl;
    cout << nrow << " row";
    if(nrow > 1){
      cout << "s";
    }
    cout << endl;
    cout << ncol << " column";
    if(ncol > 1){
      cout << "s";
    }
    cout << endl;
    cout << nlayer << " layer";
    if(nlayer > 1){
      cout << "s";
    }
    cout << endl;
    cout << "---------------------------------------------" << endl;
  }
  else{
    cout << "An error occurred while opening tissue dimensions file"
	 << endl;
  }
  doubTime = atof(argv[1]);

  cycDur[0] = atof(argv[2]);
  cycDur[1] = atof(argv[3]);
  cycDur[2] = atof(argv[4]);
  cycDur[3] = atof(argv[5]);

  cycDistrib[0] = atof(argv[6]);
  cycDistrib[1] = atof(argv[7]);
  cycDistrib[2] = atof(argv[8]);
  cycDistrib[3] = atof(argv[9]);
  
  alpha[0] = atof(argv[10]);
  alpha[1] = atof(argv[11]);
  alpha[2] = atof(argv[12]);
  alpha[3] = atof(argv[13]);
  alpha[4] = atof(argv[14]);
  alpha[5] = atof(argv[15]);
  alpha[6] = atof(argv[16]);

  beta[0] = atof(argv[17]);
  beta[1] = atof(argv[18]);
  beta[2] = atof(argv[19]);
  beta[3] = atof(argv[20]);
  beta[4] = atof(argv[21]);
  beta[5] = atof(argv[22]);
  beta[6] = atof(argv[23]);

  apopProb     = atof(argv[24]);
  apopDeadTime = atof(argv[25]);
  necDeadTime  = atof(argv[26]);

  Vmax   = atof(argv[27]);
  KConso = atof(argv[28]);

  fraction  = atof(argv[29]);
  totalDose = atof(argv[30]);
  interval  = atof(argv[31]);
  schedule  = atoi(argv[32]);
  
  treatment = new Treatment(fraction, totalDose, interval,
			    schedule);
  cout<<treatment<<endl;
  
  model1 = new ProstTissue(nrow, ncol, nlayer, nFInTum, nFInVes,
			   doubTime, cycDur, cycDistrib,
			   apopDeadTime, necDeadTime, apopProb,
			   alpha, beta, treatment);
  model2 = new diffusion3D(nrow, ncol, nlayer, nFInVes, Vmax,
			   KConso);
  coupler = new Coupler(model1, model2);
  DT1 = 1; //h;
  DT2 = 1; //s;
  sim = new RootSimulator(coupler, DT1, DT2);
  simTime = treatment->getDuration();
  sim->initSim();
  sim->simulate(0.0, simTime);
  sim->stop();

	//Interface graphique 
	QApplication app(argc, argv);
	MaFenetre *fenetre;
	fenetre = new MaFenetre(ncol,nrow,simTime);
	fenetre->show();
	
	return app.exec() && EXIT_SUCCESS;
}
