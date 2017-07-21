/**
 * @file prostateCell.hpp
 * @brief
 * @author Nicolas Ciferri
 * @author Carlos Sosa Marrero
 * @author Alfredo Hernandez
 * @date 05.19.17
 */

#ifndef DEF_PROSTACELL
#define DEF_PROSTACELL

#include "prostTissue.hpp"
#include "diffusion3D.hpp"
#include "model.hpp"
#include "treatment.hpp"

//inputs
#define IN_PO2        m_in->at(0)
#define IN_CONS_PO2		m_in->at(1)

//State variables
#define ST_ALIVE      m_st->at(0)
#define ST_TUM        m_st->at(1)
#define ST_DEAD       m_st->at(2)
#define ST_VES        m_st->at(3)
#define ST_PO2_INT 		m_st->at(4)

//output
#define OUT_CONS_O2 	m_out->at(0)

//Internal parameter
#define PAR_Vmax  m_param->at(0)
#define PAR_Km	  m_param->at(1)

//state variables
#define VAR_NUM_ALIVE	variable->at(0)
#define VAR_NUM_DEAD	variable->at(1)
#define VAR_NUM_TUMOR	variable->at(2)
#define VAR_NUM_VES		variable->at(3)
#define VAR_PO2_INT		variable->at(4)
#define VAR_STATE_1		variable->at(5)


class prostateCell: public Model{
public :
  prostateCell();
  prostateCell(Model *const parent);
  virtual ~prostateCell();
  virtual int calcModelOut();
  virtual int initModel(const double DT);
  virtual int initModel(double alive,double dead, double tumor, double ves, double state);
  virtual int startModel();
  virtual int terminateModel();
  virtual int updateModel(const double currentTime, const double DT);
  double calcOER() const;
  double calcSF() const;
  double getAlive() const;
  double getDead() const;
  double getDeadTime() const;
  double getDoubTime() const;
  double getInAlive() const;
  int getOutState() const;
  double getTum() const;
  double getVes() const;
  double getPO2() const;
  double getST_X();
  void setInAlive(const double input);
  void setInDead(const double input);
  void setInPO2(const double input);
  void setInTum(const double input);
  void setInVes(const double input);
  void setVmax(double param);
  void setKm(double param);
  void setIN_Z(double input);
  void setIN_X(double input);
  void setST_X(double state);
};
#endif
