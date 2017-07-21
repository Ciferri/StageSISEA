/**
 * @file prostTissue.hpp
 * @brief
 * @author Carlos Sosa Marrero
 * @author Nicolas Ciferri
 * @author Alfredo Hernandez
 * @date 05.19.17
 */

#ifndef DEF_ProstTissue
#define DEF_ProstTissue

#include <vector>
#include <string>

#include "model.hpp"
#include "prostCell.hpp"
#include "treatment.hpp"

//Outputs
#define OUT_TUM_DENS m_out->at(0)
#define OUT_VES_DENS m_out->at(1)

//Internal parameters
#define PAR_INIT_TUM_DENS m_param->at(0)
#define PAR_INIT_VES_DENS m_param->at(1)


class ProstTissue : public Model {
public:
	ProstTissue(const int nrow, const int ncol,
				const int nlayer, Treatment *const treatment);
	ProstTissue(const int nrow, const int ncol, const int nlayer,
				const std::string nFInTum, const std::string nFInVes,
				const double doubTime, std::vector<double> cycDur,
				std::vector<double> cycDistrib,
				const double apopDeadTime, const double necDeadTime,
				const double apopProb, std::vector<double> alpha,
				std::vector<double> beta,
				Treatment *const treatment);
	~ProstTissue();
	virtual int calcModelOut();
	virtual int initModel(const double DT);
	virtual int startModel();
	virtual int terminateModel();
	virtual int updateModel(const double currentTime,
				const double DT);
	int getNumAlive() const;
	int getNumCol() const;
	int getNumDead() const;
	int getNumG1() const;
	int getNumG2() const;
	int getNumLayer() const;
	int getNumM() const;
	int getNumRow() const;
	int getNumS() const;
	int getNumTum() const;
	int getNumVes() const;
	Treatment *getTreatment() const;
	void printNeededDose() const;
	
protected:
	int m_ncol, m_nlayer, m_nrow;
	std::vector<Model ***> m_map;
	int m_flag;
	Treatment *m_treatment;
};

#endif
