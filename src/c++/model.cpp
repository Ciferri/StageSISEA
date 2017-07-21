/**
 * @file model.cpp
 * @brief
 * @author Carlos Sosa Marrero
 * @author Nicolas Ciferri
 * @author Alfredo Hernandez
 * @date 05.19.17
 */

#include "model.hpp"
#include <iostream>

using namespace std;

Model::Model(const int numIn, const int numSt, const int numOut,
	     const int numParam, const int numComp){
  m_numIn    = numIn;
  m_numSt    = numSt;
  m_numOut   = numOut;
  m_numParam = numParam;
  m_numComp  = numComp;

  m_in       = new DoubleVect(numIn, 0);
  m_st       = new DoubleVect(numSt, 0);
  m_derivSt  = new DoubleVect(numSt, 0);
  m_out      = new DoubleVect(numOut, 0);
  m_param    = new DoubleVect(numParam, 0);
  m_comp     = new vector<Model *>((unsigned int)numComp, 0);
}


Model::~Model(){
  delete m_in;
  delete m_st;
  delete m_derivSt;
  delete m_out;
  delete m_param;
  delete m_comp;
}


int Model::startModel(){
  return 0;
}


DoubleVect *Model::getIn() const{
  return m_in;
}


DoubleVect *Model::getSt() const{
  return m_st;
}


DoubleVect *Model::getDerivSt() const{
  return m_derivSt;
}


DoubleVect *Model::getOut() const{
  return m_out;
}


DoubleVect *Model::getParam() const{
  return m_param;
}


vector<Model*> *Model::getComp() const{
  return m_comp;
}


int Model::getNumIn() const{
  return m_numIn;
}


int Model::getNumSt() const{
  return m_numSt;
}


int Model::getNumOut() const{
  return m_numOut;

}

int Model::getNumParam() const{
  return m_numParam;
}


int Model::getNumComp() const{
  return m_numComp;
}
