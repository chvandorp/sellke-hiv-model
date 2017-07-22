/*
 * parameters.cpp
 *
 *  Created on: Nov 14, 2012
 *      Author: chris
 */

#include "parameters.hpp"

parstruct::parstruct(filereader* paramFile) {
	FRASERS_BETA1 = paramFile->getParameterDouble("FRASERS_BETA1");
	FRASERS_BETAMAX = paramFile->getParameterDouble("FRASERS_BETAMAX");
	FRASERS_BETA50 = paramFile->getParameterDouble("FRASERS_BETA50");
	FRASERS_BETAK = paramFile->getParameterDouble("FRASERS_BETAK");
	FRASERS_BETA3 = paramFile->getParameterDouble("FRASERS_BETA3");
	FRASERS_D1 = paramFile->getParameterDouble("FRASERS_D1");
	FRASERS_DMAX = paramFile->getParameterDouble("FRASERS_DMAX");
	FRASERS_DMIN = paramFile->getParameterDouble("FRASERS_DMIN");
	FRASERS_D50 = paramFile->getParameterDouble("FRASERS_D50");
	FRASERS_DK = paramFile->getParameterDouble("FRASERS_DK");
	FRASERS_D3 = paramFile->getParameterDouble("FRASERS_D3");
	FRASERS_RHO = paramFile->getParameterDouble("FRASERS_RHO");
	LAMBDA1 = paramFile->getParameterDouble("LAMBDA1");
	LAMBDA2 = paramFile->getParameterDouble("LAMBDA2");
	LAMBDA3 = paramFile->getParameterDouble("LAMBDA3");
	VLMAX = paramFile->getParameterDouble("VLMAX");
	SIGMA = paramFile->getParameterDouble("SIGMA");
	PHI = paramFile->getParameterDouble("PHI");
	PREV = paramFile->getParameterDouble("PREV");
	PESC = paramFile->getParameterDouble("PESC");
	PEPTIDES_N = paramFile->getParameterInt("PEPTIDES_N");
	NONEPITOPE_M = paramFile->getParameterInt("NONEPITOPE_M");
	MEAN_K = paramFile->getParameterInt("MEAN_K");
	STANDARD_DEV_K = paramFile->getParameterInt("STANDARD_DEV_K");
	SAMPLESIZE = paramFile->getParameterInt("SAMPLESIZE");
	SAMPLESIZE_LARGE = paramFile->getParameterInt("SAMPLESIZE_LARGE");
	POPSIZE = paramFile->getParameterInt("POPSIZE");
	NATURAL_DEATH_RATE = paramFile->getParameterDouble("NATURAL_DEATH_RATE");
	MAX_TIME = paramFile->getParameterDouble("MAX_TIME");
	EPISIM_SAMPLE_CONSTANT = paramFile->getParameterDouble("EPISIM_SAMPLE_CONSTANT");
	EPISIM_WRITE_FILENAME = paramFile->getParameterString("EPISIM_WRITE_FILENAME");
	EPISIM_ECHO_BOOL = paramFile->getParameterInt("EPISIM_ECHO_BOOL");
	INITIAL_E = paramFile->getParameterInt("INITIAL_E");
	INITIAL_F = paramFile->getParameterInt("INITIAL_F");
	INITIAL_D = paramFile->getParameterInt("INITIAL_D");
	RANGE_VLMAX = paramFile->getParameterRange("RANGE_VLMAX");
	RANGE_LOGPESC = paramFile->getParameterRange("RANGE_LOGPESC");
	RANGE_N = paramFile->getParameterRange("RANGE_N");
	NGMPARSWEEP_WRITE_FILENAME = paramFile->getParameterString("NGMPARSWEEP_WRITE_FILENAME");
	GENPARSWEEP_WRITE_FILENAME = paramFile->getParameterString("GENPARSWEEP_WRITE_FILENAME");
	PARSWEEP_CURVE_FILENAME = paramFile->getParameterString("PARSWEEP_CURVE_FILENAME");
	RANGE_DIVIDER = paramFile->getParameterInt("RANGE_DIVIDER"); // multithread the parametersweep
	RANGE_REMAINDER = paramFile->getParameterInt("RANGE_REMAINDER"); // multithread the parametersweep
	FLAT_TP = paramFile->getParameterInt("FLAT_TP");
}
