/**
 * @file prostateCell.cpp
 * @brief
 * @author Nicolas Ciferri
 * @author Carlos Sosa Marrero
 * @author Alfredo Hernandez
 * @date 05.19.17
 */

#include "prostateCell.hpp"
#include "treatment.hpp"

#include <iostream>
#include <math.h>

using namespace std;

prostateCell::prostateCell() : Model(2, 5, 1, 2, 0){
  //Model(int numIn= ,int numSt= ,int numOut = ,int numParam = , int numComp= );

}

prostateCell::prostateCell(Model *const parent) : Model(7, 5, 2, 8, 0){

}


prostateCell::~prostateCell(){
}


int prostateCell::calcModelOut(){
  OUT_CONS_O2 = ST_PO2_INT;
  //cout << OUT_CONS_O2 << endl;
  return OUT_CONS_O2;
}


int prostateCell::initModel(const double DT){

  ST_ALIVE = 0.0;
  ST_DEAD  = 0.0;
  ST_TUM   = 0.0;
  ST_VES   = 0.0;
  ST_PO2_INT	=0;

  return 0;
}

int prostateCell::initModel(double alive,double dead, double tumor, double ves, double state)
{
  ST_PO2_INT	= state;
  return 0;
}

int prostateCell::startModel(){
  return 0;
}


int prostateCell::terminateModel(){
  return 0;
}


int prostateCell::updateModel(const double currentTime, const double DT){
  ST_PO2_INT	= IN_PO2 - IN_CONS_PO2;
  return 0;
}


double prostateCell::calcOER() const{

  return 0;
}


double prostateCell::calcSF() const{

}


double prostateCell::getAlive() const{
  return 0;
}


double prostateCell::getDead() const{
  return 0;
}


double prostateCell::getDeadTime() const{
  return 0;
}


double prostateCell::getDoubTime() const{
  return 0;
}


double prostateCell::getInAlive() const{
  return 0;
}


double prostateCell::getTum() const{
  return ST_TUM;
}


double prostateCell::getVes() const{
  return ST_VES;
}

double prostateCell::getPO2() const
{
  return ST_PO2_INT;
}

double prostateCell::getST_X()
{
  return OUT_CONS_O2;
}

void prostateCell::setInAlive(const double input){
}


void prostateCell::setInDead(const double input){
}


void prostateCell::setInPO2(const double input){
  IN_PO2 = input;

}


void prostateCell::setInTum(const double input){
}


void prostateCell::setInVes(const double input){
}

void prostateCell::setIN_Z(double input)
{
  IN_PO2=input;
}

void prostateCell::setIN_X(double input)
{
  IN_CONS_PO2 = input;
}

void prostateCell::setST_X(double state)
{
  OUT_CONS_O2 = state;
}
void prostateCell::setVmax(double param)
{
  PAR_Vmax = param;
}
void prostateCell::setKm(double param)
{
  PAR_Km = param;
}
