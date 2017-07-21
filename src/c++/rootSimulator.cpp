/**
 * @file rootSimulator.cpp
 * @brief
 * @author Carlos Sosa Marrero
 * @author Nicolas Ciferri
 * @author Alfredo Hernandez
 * @date 05.19.17
 */

#include "rootSimulator.hpp"

using namespace std;

RootSimulator::RootSimulator(){
}


RootSimulator::RootSimulator(Coupler *coupler, const double DT1,
			     const double DT2){
  m_coupler = coupler;
  m_DT1 = DT1;
  m_DT2 = DT2;
  m_currentTime = 0.0;
  m_sim1 = new Simulator(m_coupler->getModel1(), DT1);
  m_sim2 = new Simulator(m_coupler->getModel2(), DT2);
}


void RootSimulator::initSim(){
  m_sim1->initSim();
  m_sim2->initSim();
}


void RootSimulator::simulate(const double currentTime,
														 const double simTime){
  int numIter;
  ofstream fTumDens("./src/data/tumDens.dat");
  ofstream fState("./src/data/state.dat");
  ofstream fTimer("./src/data/timer.dat");
  ofstream fPO2("./src/data/po2.dat");
  
  vector<double> VectPO2;
  vector<double> VectState;
  vector<double> VectTimer;
  vector<double> VectCycle;

  //ColorBar : Jet, cycle, state, Hot,etc..., .map
  Image3D image("./src/data/colourMaps/state.map",
								((ProstTissue *)(m_coupler)->getModel1())->getNumCol(),
								((ProstTissue *)(m_coupler)->getModel1())->getNumRow(),
								((ProstTissue *)(m_coupler)->getModel1())->getNumLayer(),
								1,1,1);
	

  numIter = simTime / m_DT1;
  for(int j(0); j < numIter; j++){
    m_sim2->simulate(m_currentTime, 3600);
    m_coupler->updateModel();
    m_sim1->simulate(m_currentTime, m_DT1);
    fTumDens << m_currentTime << " " << m_coupler->getModel1()->getOut()->at(0) << endl;
    
    VectPO2.clear();
    VectTimer.clear();
    VectState.clear();
    VectCycle.clear();
        
    for(int i(0); i < m_coupler->getModel1()->getNumComp(); i++){
      fState << m_coupler->getModel1()->getComp()->at(i)->getOut()->at(0) << "\t";
      fTimer << m_coupler->getModel1()->getComp()->at(i)->getParam()->at(0) << "\t";
      fPO2 << m_coupler->getModel2()->getComp()->at(i)->getOut()->at(0) << "\t";
      
      VectPO2.push_back(m_coupler->getModel2()->getComp()->at(i)->getOut()->at(0));
      VectState.push_back(m_coupler->getModel1()->getComp()->at(i)->getOut()->at(0));
      VectTimer.push_back(m_coupler->getModel1()->getComp()->at(i)->getParam()->at(0));
      
      if(((ProstCell *)(m_coupler->getModel1()->getComp()->at(i)))->getVes()){VectCycle.push_back(0);}
      else if(((ProstCell *)(m_coupler->getModel1()->getComp()->at(i)))->getG1()){VectCycle.push_back(1);}
      else if(((ProstCell *)(m_coupler->getModel1()->getComp()->at(i)))->getS()){VectCycle.push_back(2);}
      else if(((ProstCell *)(m_coupler->getModel1()->getComp()->at(i)))->getG2()){VectCycle.push_back(3);}
      else if(((ProstCell *)(m_coupler->getModel1()->getComp()->at(i)))->getM()){VectCycle.push_back(4);}
      else if(((ProstCell *)(m_coupler->getModel1()->getComp()->at(i)))->getDead()){VectCycle.push_back(5);}
      else{VectCycle.push_back(6);}
    }
    
		//~ image.printImageWithVector(VectPO2,VectState,VectTimer,j,image); //créer une image pgm
		//~ image.printImageBMP(VectPO2,VectState,VectTimer,VectCycle,j,image); //créer une image bmp
		image.printImagePNG(VectPO2,VectState,VectTimer,VectCycle,j,image); //créer une image png
 
    fState << endl;
    fTimer << endl;
    fPO2 << endl;

    m_coupler->updateModel();
    m_currentTime += m_DT1;
  }
  fTumDens.close();
  fState.close();
  fTimer.close();
  fPO2.close();
}


  
void RootSimulator::stop(){
  m_sim1->stop();
  m_sim2->stop();
}
