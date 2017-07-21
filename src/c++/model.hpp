/**
 * @file model.hpp
 * @brief
 * @author Carlos Sosa Marrero
 * @author Nicolas Ciferri
 * @author Alfredo Hernandez
 * @date 05.19.17
 */

#ifndef DEF_MODEL
#define DEF_MODEL

#include <vector>

typedef std::vector<double> DoubleVect;

class Model {
	public :
		Model(const int numIn, const int numSt, const int numOut,
		const int numParam, const int numComp);
		virtual ~Model();
		virtual int calcModelOut() = 0;
		virtual int initModel(const double DT = 0) = 0;
		virtual int updateModel(const double currentTime = 0,
					const double DT = 0 ) = 0;
		virtual int terminateModel() = 0;
		virtual int startModel();
		DoubleVect *getIn() const;
		DoubleVect *getSt() const;
		DoubleVect *getDerivSt() const;
		DoubleVect *getOut() const;
		DoubleVect *getParam() const;
		std::vector<Model*> *getComp() const;
		int getNumIn() const;
		int getNumSt() const;
		int getNumOut() const;
		int getNumParam() const;
		int getNumComp() const;

	protected:
		int m_numIn;
		int m_numSt;
		int m_numOut;
		int m_numParam;
		int m_numComp;
		DoubleVect *m_in;	
		DoubleVect *m_st;   
		DoubleVect *m_derivSt; //cache the value of states during integ
		DoubleVect *m_out; 
		DoubleVect *m_param; 
		std::vector<Model *>  *m_comp;
		Model	*m_parent; 
};

#endif
