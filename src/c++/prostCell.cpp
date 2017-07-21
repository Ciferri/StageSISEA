/**
 * @file prostCell.cpp
 * @brief
 * @author Carlos Sosa Marrero
 * @author Nicolas Ciferri
 * @author Alfredo Hernandez
 * @date 05.19.17
 */

#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "prostCell.hpp"

using namespace std;

ProstCell::ProstCell(Model *const parent) : Model(6, 8, 1, 29, 0){
  ST_ALIVE = 1.0;
  ST_DEAD  = 0.0;
  ST_TUM   = 0.0;
  ST_VES   = 0.0;

  PAR_TIMER     = 0; //h
  PAR_DOUB_TIME = 1008; //h
  
  PAR_LIM_G1S = 0.55 * PAR_DOUB_TIME;
  PAR_LIM_SG2 = 0.75 * PAR_DOUB_TIME;
  PAR_LIM_G2M = 0.9  * PAR_DOUB_TIME;
  
  ST_G1 = 0           <= PAR_TIMER && PAR_TIMER <  PAR_LIM_G1S;
  ST_S  = PAR_LIM_G1S <= PAR_TIMER && PAR_TIMER <  PAR_LIM_SG2;
  ST_G2 = PAR_LIM_SG2 <= PAR_TIMER && PAR_TIMER <  PAR_LIM_G2M;
  ST_M  = PAR_LIM_G2M <= PAR_TIMER && PAR_TIMER <= PAR_DOUB_TIME;
  
  PAR_M     = 3.0; //adim.
  PAR_K     = 3.0; //mmHg
  PAR_PO2   = 3.5; //mmHg
  
  PAR_ALPHA_ALIVE = 0; //Gy^-1
  PAR_ALPHA_G1    = 0.158; //Gy^-1
  PAR_ALPHA_S     = 0.113; //Gy^-1
  PAR_ALPHA_G2    = 0.169; //Gy^-1
  PAR_ALPHA_M     = 0.189; //Gy^-1
  PAR_ALPHA_DEAD  = 0; //Gy^-1
  PAR_ALPHA_VES   = 0; //Gy^-1

  PAR_ALPHA = PAR_ALPHA_ALIVE;

  PAR_BETA_ALIVE = 0; //Gy^-2
  PAR_BETA_G1    = 0.051; //Gy^-2
  PAR_BETA_S     = 0.037; //Gy^-2
  PAR_BETA_G2    = 0.055; //Gy^-2
  PAR_BETA_M     = 0.061; //Gy^-2
  PAR_BETA_DEAD  = 0; //Gy^-2
  PAR_BETA_VES   = 0; //Gy^-2

  PAR_BETA = PAR_BETA_ALIVE;

  PAR_APOP_PROB      = 0.8;
  PAR_APOP_DEAD_TIME = 234; //h
  PAR_NEC_DEAD_TIME  = 468; //h
  PAR_DEAD_TIME      = PAR_APOP_DEAD_TIME; //h
    
  PAR_ACC_DOSE = 0; //Gy
  
  m_parent = parent;
  m_edge = new vector<ProstCell *>((unsigned int)0, 0);
  m_treatment = 0;
}


ProstCell::ProstCell(const double doubTime, vector <double> cycDur,
		     const double apopDeadTime,
		     const double necDeadTime,
		     const double apopProb, vector<double> alpha,
		     vector<double> beta, Model *const parent) :
  Model(6, 8, 1, 29, 0){
  ST_ALIVE = 1.0;		
  ST_DEAD  = 0.0;
  ST_TUM   = 0.0;		
  ST_VES   = 0.0;

  PAR_TIMER    = 0; //h
  PAR_DOUB_TIME = doubTime; //h

  
  PAR_LIM_G1S = cycDur.at(0) * PAR_DOUB_TIME;
  PAR_LIM_SG2 = PAR_LIM_G1S + cycDur.at(1) * PAR_DOUB_TIME;
  PAR_LIM_G2M = PAR_LIM_SG2 + cycDur.at(2) * PAR_DOUB_TIME;
  
  ST_G1 = 0           <= PAR_TIMER && PAR_TIMER <  PAR_LIM_G1S;
  ST_S  = PAR_LIM_G1S <= PAR_TIMER && PAR_TIMER <  PAR_LIM_SG2;
  ST_G2 = PAR_LIM_SG2 <= PAR_TIMER && PAR_TIMER <  PAR_LIM_G2M;
  ST_M  = PAR_LIM_G2M <= PAR_TIMER && PAR_TIMER <= PAR_DOUB_TIME;
  
  PAR_M     = 3.0; //adim.
  PAR_K     = 3.0; //mmHg
  PAR_PO2   = 3.5; //mmHg
  
  PAR_ALPHA_ALIVE = alpha.at(0); //Gy^-1
  PAR_ALPHA_G1    = alpha.at(1); //Gy^-1
  PAR_ALPHA_S     = alpha.at(2); //Gy^-1
  PAR_ALPHA_G2    = alpha.at(3); //Gy^-1
  PAR_ALPHA_M     = alpha.at(4); //Gy^-1
  PAR_ALPHA_DEAD  = alpha.at(5); //Gy^-1
  PAR_ALPHA_VES   = alpha.at(6); //Gy^-1

  PAR_ALPHA = PAR_ALPHA_ALIVE;

  PAR_BETA_ALIVE = beta.at(0); //Gy^-2
  PAR_BETA_G1    = beta.at(1); //Gy^-2
  PAR_BETA_S     = beta.at(2); //Gy^-2
  PAR_BETA_G2    = beta.at(3); //Gy^-2
  PAR_BETA_M     = beta.at(4); //Gy^-2
  PAR_BETA_DEAD  = beta.at(5); //Gy^-2
  PAR_BETA_VES   = beta.at(6); //Gy^-2

  PAR_BETA = PAR_BETA_ALIVE;

  PAR_APOP_PROB      = apopProb;
  PAR_APOP_DEAD_TIME = apopDeadTime; //h
  PAR_NEC_DEAD_TIME  = necDeadTime; //h
  PAR_DEAD_TIME      = PAR_APOP_DEAD_TIME; //h
    
  PAR_ACC_DOSE = 0; //Gy
  
  m_parent = parent;
  m_edge = new vector<ProstCell *>((unsigned int)0, 0);
  m_treatment = ((ProstTissue *)m_parent)->getTreatment();
}


ProstCell::~ProstCell(){
}


int ProstCell::calcModelOut(){
  OUT_STATE = ST_ALIVE + 2 * ST_TUM + 3 * ST_VES + 4 * ST_DEAD;
  return 0;
}


int ProstCell::initModel(const double DT){
  ST_ALIVE = (ST_ALIVE || IN_ALIVE) && !IN_TUM && !IN_DEAD &&
    !IN_VES;   
  ST_TUM   = (ST_TUM || IN_TUM) && !ST_ALIVE && !IN_DEAD;
  ST_DEAD  = (ST_DEAD || IN_DEAD) && !ST_ALIVE && !ST_TUM;
  ST_VES   = (ST_VES || IN_VES) && !IN_DEAD;
  setInTum(0.0);
  setInDead(0.0);
  setInVes(0.0);
  
  PAR_TIMER = IN_TIMER;
  if(!ST_DEAD){
    ST_G1 = 0           <= PAR_TIMER && PAR_TIMER <  PAR_LIM_G1S;
    ST_S  = PAR_LIM_G1S <= PAR_TIMER && PAR_TIMER <  PAR_LIM_SG2;
    ST_G2 = PAR_LIM_SG2 <= PAR_TIMER && PAR_TIMER <  PAR_LIM_G2M;
    ST_M  = PAR_LIM_G2M <= PAR_TIMER && PAR_TIMER <= PAR_DOUB_TIME;
  }

  if(ST_ALIVE){
    PAR_ALPHA = PAR_ALPHA_ALIVE;
    PAR_BETA  = PAR_BETA_ALIVE;
  }
  if(ST_DEAD){
    PAR_ALPHA = PAR_ALPHA_DEAD;
    PAR_BETA  = PAR_BETA_DEAD;
  }
  if(ST_VES){
    PAR_ALPHA = PAR_ALPHA_VES;
    PAR_BETA  = PAR_BETA_VES;
  }
  if(ST_TUM){
    if(ST_G1){
      PAR_ALPHA = PAR_ALPHA_G1;
      PAR_BETA  = PAR_BETA_G1;
    }
    if(ST_S){
      PAR_ALPHA = PAR_ALPHA_S;
      PAR_BETA  = PAR_BETA_S;
    }
    if(ST_G2){
      PAR_ALPHA = PAR_ALPHA_G2;
      PAR_BETA  = PAR_BETA_G2;
    }
    if(ST_M){
      PAR_ALPHA = PAR_ALPHA_M;
      PAR_BETA  = PAR_BETA_M;
    }
  }

  //PAR_PO2 = IN_PO2;
  PAR_PO2 = m_in->at(4);
  
  return 0;
}


int ProstCell::startModel(){
  return 0;
}


int ProstCell::terminateModel(){
  return 0;
}


int ProstCell::updateModel(const double currentTime,
			   const double DT){
  ST_ALIVE = (ST_ALIVE || IN_ALIVE) && !IN_TUM && !IN_DEAD &&
    !IN_VES;   
  ST_TUM   = (ST_TUM || IN_TUM) && !ST_ALIVE && !IN_DEAD;
  ST_DEAD  = (ST_DEAD || IN_DEAD) && !ST_ALIVE && !ST_TUM;
  ST_VES   = (ST_VES || IN_VES) && !IN_DEAD;
  setInAlive(0.0);
  setInTum(0.0);
  setInDead(0.0);
  setInVes(0.0);

  PAR_TIMER += DT;
  if(!ST_DEAD){
    ST_G1 = 0           <= PAR_TIMER && PAR_TIMER <  PAR_LIM_G1S;
    ST_S  = PAR_LIM_G1S <= PAR_TIMER && PAR_TIMER <  PAR_LIM_SG2;
    ST_G2 = PAR_LIM_SG2 <= PAR_TIMER && PAR_TIMER <  PAR_LIM_G2M;
    ST_M  = PAR_LIM_G2M <= PAR_TIMER && PAR_TIMER <= PAR_DOUB_TIME;
  }

  if(ST_ALIVE){
    PAR_ALPHA = PAR_ALPHA_ALIVE;
    PAR_BETA  = PAR_BETA_ALIVE;
  }
  if(ST_DEAD){
    PAR_ALPHA = PAR_ALPHA_DEAD;
    PAR_BETA  = PAR_BETA_DEAD;
  }
  if(ST_VES){
    PAR_ALPHA = PAR_ALPHA_VES;
    PAR_BETA  = PAR_BETA_VES;
  }
  if(ST_TUM){
    if(ST_G1){
      PAR_ALPHA = PAR_ALPHA_G1;
      PAR_BETA  = PAR_BETA_G1;
    }
    if(ST_S){
      PAR_ALPHA = PAR_ALPHA_S;
      PAR_BETA  = PAR_BETA_S;
    }
    if(ST_G2){
      PAR_ALPHA = PAR_ALPHA_G2;
      PAR_BETA  = PAR_BETA_G2;
    }
    if(ST_M){
      PAR_ALPHA = PAR_ALPHA_M;
      PAR_BETA  = PAR_BETA_M;
    }
  }

  PAR_PO2 = m_in->at(4);
  
  calcTumGrowth();

  if(m_treatment){
    if(fmod(currentTime, m_treatment->getInterval()) == 0){
      int i(currentTime / m_treatment->getInterval());
      if((m_treatment->getSchedule()).at(i)){
	calcRespToIrr();
      }
    }
  }

  if(ST_DEAD){
    calcDeadCellsResor(DT);
  }
  return 0;
}


void ProstCell::addToEdge(ProstCell *const cell){
  m_edge->push_back(cell);
}


void ProstCell::calcDeadCellsResor(const double DT){
  double p;
  
  p = (double)rand() / (double)(RAND_MAX);
  if(calcRF(DT) > p){
    PAR_TIMER = 0;
    setInAlive(1.0);
  }
}

  
double ProstCell::calcOER() const{
  double OER;
  
  OER = (PAR_M * PAR_PO2 + PAR_K) / (PAR_PO2 + PAR_K); //mmHg
  return OER;
}


void ProstCell::calcRespToIrr(){
  double p;
  
  p = (double)rand() / (double)(RAND_MAX);
  if(calcSF() < p){
    setInDead (1.0);
    p = (double)rand() / (double)(RAND_MAX);
    if(p < PAR_APOP_PROB){
      PAR_DEAD_TIME = PAR_APOP_DEAD_TIME;
    }
    else{
      PAR_DEAD_TIME = PAR_NEC_DEAD_TIME;
    }
    PAR_TIMER = 0;
  }
  PAR_ACC_DOSE += m_treatment->getFraction();
}


double ProstCell::calcRF(const double DT) const{
  double RF;

  RF = 1.0 - pow(2.0, -DT / PAR_DEAD_TIME);
  return RF;
}


double ProstCell::calcSF() const{
  double fraction, SF;
  
  fraction = m_treatment->getFraction();
  SF = exp(-PAR_ALPHA / PAR_M * fraction * calcOER() -
	   PAR_BETA / (PAR_M * PAR_M) * fraction * fraction *
	   calcOER() * calcOER());
  return SF;
}


void ProstCell::calcTumGrowth(){
  ProstCell *newTumCell(0);
  
  if(PAR_TIMER >= PAR_DOUB_TIME){
    PAR_TIMER = 0;
    if(ST_TUM && ((ProstTissue *)m_parent)->getNumAlive()){
      newTumCell = searchSpace();
      newTumCell->setInTum(1.0);
      newTumCell->PAR_TIMER = 0;
    }     
  }
}


double ProstCell::getAlive() const{
  return ST_ALIVE;
}


double ProstCell::getAccDose() const{
  return PAR_ACC_DOSE;
}


double ProstCell::getDead() const{
  return ST_DEAD;
}


double ProstCell::getDeadTime() const{
  return PAR_DEAD_TIME;
}


double ProstCell::getDoubTime() const{
  return PAR_DOUB_TIME;
}


vector<ProstCell *> *ProstCell::getEdge() const{
  return m_edge;
}


double ProstCell::getG1() const{
  return ST_G1 && ST_TUM;
}


double ProstCell::getG2() const{
  return ST_G2 && ST_TUM;
}


double ProstCell::getM() const{
  return ST_M && ST_TUM;
}


int ProstCell::getOutState() const{
  return (int)OUT_STATE;
}


double ProstCell::getS() const{
  return ST_S && ST_TUM;
}


double ProstCell::getTum() const{
  return ST_TUM;
}


double ProstCell::getVes() const{
  return ST_VES;
}


ProstCell *ProstCell::searchSpace() const{
  int edgeSize, m;
  
  edgeSize = m_edge->size();
  m = rand() % edgeSize;
  for(int n(0); n < edgeSize; n++){
    if(((ProstCell *)m_edge->at(m))->ST_ALIVE){
      return ((ProstCell *)m_edge->at(m));
    }
    m++;
    if(m == edgeSize){
      m = 0;
    }
  }
  return ((ProstCell *)m_edge->at(m))->searchSpace();
}


void ProstCell::setInAlive(const double input){
  IN_ALIVE = input;
}


void ProstCell::setInDead(const double input){
  IN_DEAD = input;
}


void ProstCell::setInPO2(const double input){
  m_in->at(4) = input;
  
}


void ProstCell::setInTimer(const double input){
  IN_TIMER = input;
}


void ProstCell::setInTum(const double input){
  IN_TUM = input;
}


void ProstCell::setInVes(const double input){
  IN_VES = input;
}









