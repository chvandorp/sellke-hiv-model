/*
 * main_parametersweep.cpp
 *
 *  Created on: Nov 27, 2012
 *      Author: chris
 */

#include "main_parametersweep.hpp"


int main(int argc, char* argv[]) {
	/* the program accepts a seed given as argument
	 * if no seed is given, the program will prompt for one.
	 * the same holds for the parameter name
	 * syntax: <program name> -p <parameter file name> -s <seed>
	 */

	parstruct allParams = cl_argreader(argc, argv);

	std::cout << "# running...\n";

	parametersweep PS(&allParams);

	//PS.doNGMbasedAlongCurve(); // WARNING: MAKE SURE TO SET PESC and PREV right!
	PS.doNGMbased();

	std::cout << "# finished...\n";
	return 0;
}
