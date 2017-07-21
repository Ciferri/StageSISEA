/**
 * @file coupler.cpp
 * @brief
 * @author Carlos Sosa Marrero
 * @author Nicolas Ciferri
 * @author Alfredo Hernandez
 * @date 05.19.17 
 */

#include "coupler.hpp"

using namespace std;

Coupler::Coupler(Model *model1, Model *model2):Model(0, 0, 1, 0, 2){
  m_comp->at(0) = model1;
  m_comp->at(1) = model2;
}


Coupler::~Coupler(){
}


int Coupler::calcModelOut(){

}


int Coupler::initModel(const double DT){
}


int Coupler::startModel(){
}


int Coupler::terminateModel(){
}


int Coupler::updateModel(const double currentTime,
			 const double DT){
  if(m_comp->at(0)->getNumComp() ==  m_comp->at(0)->getNumComp()){
    for(int k(0); k <  m_comp->at(0)->getNumComp(); k++){
      ((ProstCell *) m_comp->at(0)->getComp()->at(k))->
				setInPO2(m_comp->at(1)->getComp()->at(k)->getOut()->at(0));
    }
  }
}


Model *Coupler::getModel1() const{
  return  m_comp->at(0);
}


Model *Coupler::getModel2() const{
  return  m_comp->at(1);
}
