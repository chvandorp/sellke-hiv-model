/*
 * cl_argreader.cpp
 *
 *  Created on: Nov 16, 2012
 *      Author: chris
 */

#include "cl_argreader.hpp"

parstruct cl_argreader(int argc, char* argv[]) {
	/* initiate the flag map. i.e. couple the enumeration of flags to the strings
	 * so that we may use a switch statement on flags.
	 */
	std::map<std::string,flags> flagMap; initFlagMap(flagMap);

	/* walk though the array of c_strings and assign arguments
	 * of flags to their variables
	 */
	int seed;
	bool seedSet = false;

	std::string paramFilename;
	bool paramFilenameSet = false;

	/* additional arguments for the division of the parameter sweep
	 * ps_divider = number of parts
	 * ps_remainder = index of this part
	 */

	int ps_divider;
	bool ps_dividerSet = false;

	int ps_remainder;
	bool ps_remainderSet = false;

	char** argp = &argv[0]; // a pointer to the strings

	while ( argp != &argv[argc] ) {
		argp++;
		if ( argp == &argv[argc] ) break;
		std::string flag(*argp);
		//std::cout << "read flag : " << flag << "\n";
		switch ( flagMap[flag] ) {
			case sFlag: { // seed
				argp++;
				if ( argp != &argv[argc] ) {
					std::string arg(*argp);
					std::istringstream ( arg ) >> seed;
					seedSet = true;
					break;
				}
				else {
					std::cout << "WARNING: error occurred while parsing.\n";
					break;
				}
			}
			case pFlag: { // parameter file
				argp++;
				if ( argp != &argv[argc] ) {
					std::string arg(*argp);
					paramFilename = arg;
					paramFilenameSet = true;
					break;
				}
				else {
					std::cout << "WARNING: error occurred while parsing (parameter file).\n";
					break;
				}
			}
			case dFlag: { // divide parameter sweeps in individual runs
				argp++;
				if ( argp != &argv[argc] ) {
					std::string arg(*argp);
					std::istringstream ( arg ) >> ps_divider;
					ps_dividerSet = true;
					break;
				}
				else {
					std::cout << "WARNING: error occurred while parsing (parsweep divider).\n";
					break;
				}
			}
			case rFlag: { // the index of this run
				argp++;
				if ( argp != &argv[argc] ) {
					std::string arg(*argp);
					std::istringstream ( arg ) >> ps_remainder;
					ps_remainderSet = true;
					break;
				}
				else {
					std::cout << "WARNING: error occurred while parsing (parsweep remainder).\n";
					break;
				}
			}
			case invalidFlag: {
				std::cout << "WARNING: invalid flag used for passing parameters: " << flag << "\n";
				break;
			}
			default: {
				std::cout << "WARNING: error occurred while parsing. (default)\n";
				break;
			}
		}
	} // while

	if ( !paramFilenameSet ) { // prompt for parameter file name
		std::cout << "give name of a parameter file. ";
		char line[256];
		std::cin.getline(line,256,'\n');
		paramFilename = std::string(line);
	}

	if ( !seedSet ) { // prompt for seed
		std::cout << "give a seed for RNG. ";
		std::cin >> seed;
	}

	/* read the parameter file */
	filereader paramFile(paramFilename);

	/* re-seed the RNG */
	RNG.seed(seed);

	/* init parameters in the parameter struct */
	parstruct allParams(&paramFile); // PASS THE ADRESS!!!!!

	/* if ps_remainder and ps_divider are set, then replace then in the parstruct */

	if ( ps_remainderSet || ps_dividerSet ) {
		if ( ps_dividerSet ) { allParams.RANGE_DIVIDER = ps_divider; }
		else {
			std::cout << "WARNING (in cl_argreader): taking RANGE_DIVIDER form " << paramFilename << ".";
		}
		if ( ps_remainderSet ) { allParams.RANGE_REMAINDER = ps_remainder; }
		else {
			std::cout << "WARNING (in cl_argreader): taking RANGE_REMAINDER form " << paramFilename << ".";
		}
	}

	/* write the parameters to the outputFile */
	//paramFile.writeNumbersToFile(allParams.EPISIM_WRITE_FILENAME);

	return allParams;
}



void initFlagMap(std::map<std::string,flags> & flagMap) {
	/* aux function for main. i.e. for binding passed values
	 * to their variables.
	 */
	flagMap["-s"] = sFlag;
	flagMap["-p"] = pFlag;
	flagMap["-d"] = dFlag;
	flagMap["-r"] = rFlag;
}



