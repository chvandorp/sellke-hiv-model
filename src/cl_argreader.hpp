/*
 * cl_argreader.hpp
 *
 *  Created on: Nov 16, 2012
 *      Author: chris
 */

#ifndef CL_ARGREADER_HPP_
#define CL_ARGREADER_HPP_

#include <map>
#include <string>
#include <iostream>
#include <sstream>

#include "RngClass.hpp" // for re-seeding the RNG
#include "filereader_class.hpp"
#include "parameters.hpp"



enum flags { // enumeration for flags used for program arguments
	invalidFlag=0, // map will return 0 at request of unknown flag
	sFlag, // "-s" for the seed
	pFlag, // "-p" for the parameter file name
	dFlag, // "-d" the paramsweep is divided into a number of independent threads
	rFlag  // "-r" and this is the index (remainder)
};

void initFlagMap(std::map<std::string,flags> & flagMap); // couple enum flags to strings (e.g. -s, -p)

parstruct cl_argreader(int, char** );

#endif /* CL_ARGREADER_HPP_ */
