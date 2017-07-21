/**
 * @file simulator.cpp
 * @brief
 * @author Carlos Sosa Marrero
 * @author Nicolas Ciferri
 * @author Alfredo Hernandez
 * @date 05.19.17
 */

#include <stdlib.h>
#include <string>
#include "simulator.hpp"

using namespace std;

Simulator::Simulator(){
  m_model = 0;
  m_currentTime = 0.0;
  m_DT = 1; //h
  m_simMeth = new SimMeth(euler);
}


Simulator::Simulator(Model *model, const double DT){
  m_model = model;
  m_currentTime = 0.0;
  m_DT = DT;
  m_simMeth = new SimMeth(euler);
}


Simulator::~Simulator(){
  delete m_model;
}


void Simulator::setModel(Model *model){
  m_model = model;
}


void Simulator::initSim(){ 
  m_model->initModel(m_DT);
      
  //It does nothing for the moment
  m_model->startModel();
}


void Simulator::simulate(const double currentTime,
			 const double simTime){
  int numIter;
  
  numIter = simTime / m_DT;
  m_currentTime = currentTime;
  for(int j(0); j < numIter; j++){
    //Update of the state of every cell composing the tissue
    if(m_model->updateModel(m_currentTime, m_DT)){
      break;
    }
    m_model->calcModelOut();
    m_currentTime += m_DT;
  }
  
}



void Simulator::stop(){
  m_model->terminateModel();
}
