/**
 * @file treatment.cpp
 * @brief
 * @author Carlos Sosa Marrero
 * @date 05.19.17 
 */

#include <iostream>
#include <string>
#include <vector>

#include "treatment.hpp"

using namespace std;

Treatment::Treatment(){
  int accSession(0), i(0), numSession;
  
  m_fraction  = 2.0; //Gy
  m_interval  = 24.0; //h
  m_totalDose = 80.0; //Gy

  m_scheDescrip = "Mon-Fri";
  numSession=(int)(m_totalDose/m_fraction);
  while(accSession<=numSession){
    if((i+1)%7==6||(i+1)%7==0){
      m_schedule.push_back(false);
    }
    else{
      m_schedule.push_back(true);
      accSession++;
    }
    i++;
  }
}


Treatment::Treatment(const double fraction, const double totalDose,
		     const double interval, const int schedule){
  int accSession(0), i(0), numSession;
  
  m_fraction  = fraction;
  m_interval  = interval;
  m_totalDose = totalDose;

  switch(schedule){
  case 1:
    m_scheDescrip = "Mon-Sun";
    while(accSession<=numSession){
      m_schedule.push_back(true);
      accSession++;
    }
    break;
    
  case 2:
    m_scheDescrip = "Mon-Fri";
    numSession=(int)(m_totalDose/m_fraction);
    while(accSession<=numSession){
      if((i+1)%7==6||(i+1)%7==0){
	m_schedule.push_back(false);
      }
      else{
	m_schedule.push_back(true);
	accSession++;
      }
      i++;
    }
    break;
    
  default:
    m_scheDescrip = "Mon-Fri";
    numSession=(int)(m_totalDose/m_fraction);
    while(accSession<=numSession){
      if((i+1)%7==6||(i+1)%7==0){
	m_schedule.push_back(false);
      }
      else{
	m_schedule.push_back(true);
	accSession++;
      }
      i++;
    }
    break;
  }
}


double Treatment::getDuration() const{
  return m_schedule.size() * m_interval;
}


double Treatment::getFraction() const{
  return m_fraction;
}


double Treatment::getInterval() const{
  return m_interval;
}


vector<bool> Treatment::getSchedule() const{
  return m_schedule;
}


string Treatment::getScheDescrip() const{
  return m_scheDescrip;
}


double Treatment::getTotalDose() const{
  return m_totalDose;
}


ostream &operator<<(ostream &stream, Treatment *treatment){
  stream << "Total dose = " << treatment->getTotalDose() <<
    " Gy" << endl;
  stream << "Fraction = " << treatment->getFraction() << " Gy" <<
    endl;
  stream << "Interval = " << treatment->getInterval() << " h" <<
    endl;
  stream << "Schedule = " << treatment->getScheDescrip() <<  endl;
  stream << "Duration = " << treatment->getDuration() <<  endl;
  stream << "---------------------------------------------";
  return stream;
}
