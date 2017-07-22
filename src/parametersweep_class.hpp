/*
 * parametersweep_class.hpp
 *
 *  Created on: Nov 26, 2012
 *      Author: chris
 */

#ifndef PARAMETERSWEEP_CLASS_HPP_
#define PARAMETERSWEEP_CLASS_HPP_

#include "parameters.hpp"
#include "singlehostsim_class.hpp"
#include <cmath>
#include <fstream>
#include <iostream>


class parametersweep {
public:
	parametersweep(parstruct* );
	void doNGMbased();
	/* the following function takes a list of parameter pairs
	 * loaded from a file specified in the parameters file
	 * (seperated by spaces and newlines, e.g. "0.0 1.0\n0.1 1.2\n0.3 2.0")
	 * and computes he steady states and staistics at these parameters
	 */
	void doNGMbasedAlongCurve();
protected:
	parstruct* allParams;
	std::vector<double> llRange;
	std::vector<double> pescRange;
	std::vector<double> vlmaxRange;
	std::vector<int> nRange;
	std::vector< std::pair<double, double> > curve;
	void readCurveFile();
	bool loadedCurve;
};


#endif /* PARAMETERSWEEP_CLASS_HPP_ */
