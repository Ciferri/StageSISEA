/**
 * @file rootSimulator.hpp
 * @brief
 * @author Carlos Sosa Marrero
 * @author Nicolas Ciferri
 * @author Alfredo Hernandez
 * @date 05.19.17
 */

#ifndef DEF_ROOTSIMULATOR
#define DEF_ROOTSIMULATOR

#include "coupler.hpp"
#include "model.hpp"
#include "simulator.hpp"
#include "Image3D.hpp"
#include "Header_pgm.hpp"
#include <string>
#include <sstream>
#pragma pack(1)  // desative l'alignement mémoire
typedef int int32;
typedef short int16;

using My_io_pgm::Header;
using MyImage::Image3D;

class RootSimulator{
public:
  RootSimulator();
  RootSimulator(Coupler *coupler, const double DT1,
		const double DT2);
  ~RootSimulator();
  void initSim();
  void simulate(const double currentTime, const double simTime);
  void stop();
private:	
  double m_DT1, m_DT2;
  double m_currentTime;
  Coupler *m_coupler;
  Simulator *m_sim1, *m_sim2;
};
#endif
