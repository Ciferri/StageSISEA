/**
 * @file simMeth.hpp
 * @brief
 * @author Alfredo Hernandez
 */

#ifndef SimMeth_h_h
#define SimMeth_h_h

#include "model.hpp"

enum methName {rk4,euler,ac};

class SimMeth{	
 public:
  SimMeth(methName meth, Model* model);
  SimMeth(methName meth);
  ~SimMeth();
  void setModel(Model* model);
  methName getMeth() const;
  void initMeth();
  int setMeth(methName meth);
  void simulate(double globalTime, double DT);
  void simRk4(double x, double h);
  void euler(double x, double h);

private:
  methName m_meth;	
  Model* m_model; 
};

#endif
