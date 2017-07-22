/*
 * parameters.hpp
 *
 *  Created on: Nov 9, 2012
 *      Author: chris
 */

#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

#include <string>
#include "filereader_class.hpp"

class parstruct {
public:
	parstruct(filereader* );

	double FRASERS_BETA1;
	double FRASERS_BETAMAX;
	double FRASERS_BETA50;
	double FRASERS_BETAK;
	double FRASERS_BETA3;
	double FRASERS_D1;
	double FRASERS_DMAX;
	double FRASERS_DMIN;
	double FRASERS_D50;
	double FRASERS_DK;
	double FRASERS_D3;
	double FRASERS_RHO;

	int FLAT_TP; // a switch for a flat transmission potential or not.

	double LAMBDA1;
	double LAMBDA2;
	double LAMBDA3;

	double VLMAX;
	double SIGMA; // immune response fitness cost
	double PHI; // fitness cost
	double PREV;
	double PESC;
	int PEPTIDES_N; // size of combined binding repertoire
	int NONEPITOPE_M; // non-epitope mutations

	int MEAN_K; // mean size of individual binding repertoire
	int STANDARD_DEV_K;

	int SAMPLESIZE;
	int SAMPLESIZE_LARGE;
	int POPSIZE;
	double NATURAL_DEATH_RATE;

	double MAX_TIME;

	std::string EPISIM_WRITE_FILENAME;
	double EPISIM_SAMPLE_CONSTANT;
	int EPISIM_ECHO_BOOL;

	int INITIAL_E;
	int INITIAL_F;
	int INITIAL_D;

	std::pair< std::pair<double,double>, int > RANGE_LOGPESC;
	std::pair< std::pair<double,double>, int > RANGE_VLMAX;
	std::pair< std::pair<double,double>, int > RANGE_N;

	std::string NGMPARSWEEP_WRITE_FILENAME;
	std::string GENPARSWEEP_WRITE_FILENAME;
	std::string PARSWEEP_CURVE_FILENAME;

	int RANGE_DIVIDER; // the parsweep is divided into multiple treads
	int RANGE_REMAINDER; // only do the simulation for this remainder
};

#endif /* PARAMETERS_HPP_ */
