/**
 * @file simMeth.cpp
 * @brief
 * @author Alfredo Hernandez
 */

#include "simMeth.hpp"
#include <stdlib.h>

SimMeth::SimMeth(methName meth, Model* model){
  m_meth = meth;
  m_model = model;  
}


SimMeth::SimMeth(methName meth){
  m_meth = meth;
}

SimMeth::~SimMeth(){
  delete m_model;
}

void SimMeth::setModel(Model* model){
  m_model = model;
}


methName SimMeth::getMeth() const{
  return m_meth;
}


int SimMeth::setMeth(methName meth){
  m_meth = meth;
  return 0;
}


void SimMeth::initMeth(){
}


void SimMeth::simulate(double globalTime, double DT){
  switch(m_meth){
  default:
    euler(globalTime, DT);
    break;
  }
}


void SimMeth::euler(double x, double h){
}




