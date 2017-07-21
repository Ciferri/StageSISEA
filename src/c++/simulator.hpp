/**
 * @file simulator.hpp
 * @brief
 * @author Carlos Sosa Marrero
 * @author Nicolas Ciferri
 * @author Alfredo Hernandez
 * @date 05.19.17 
 */

#ifndef DEF_SIMULATOR
#define DEF_SIMULATOR

#include "model.hpp"
#include "simMeth.hpp"
#include <string>
#include <iostream>
#include <fstream>

class Simulator{	
public:
  Simulator();
  Simulator(Model *model, const double DT);
  ~Simulator();
  void initSim();
  void setModel(Model *model);
  void simulate(const double currentTime, const double simTime);
  void stop();

  
private:
  SimMeth *m_simMeth;
  double m_DT;	
  double m_currentTime;
  Model *m_model;
};

#endif
