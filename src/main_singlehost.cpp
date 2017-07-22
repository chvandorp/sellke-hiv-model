/*
 * main_singlehost.cpp
 *
 *  Created on: Nov 7, 2012
 *      Author: chris
 */

#include "main_singlehost.hpp"


int main(int argc, char* argv[]) {
	/* the program accepts a seed given as argument
	 * if no seed is given, the program will prompt for one.
	 * the same holds for the parameter name
	 * syntax: <program name> -p <parameter file name> -s <seed>
	 */

	parstruct allParams = cl_argreader(argc, argv);

	std::cout << "# running...\n";

	singlehostsim mySingleHostSimulation(&allParams);
	//mySingleHostSimulation.makeNGM();
	//mySingleHostSimulation.computeEigs();
	//mySingleHostSimulation.WHEDsamplesFromDist();
	mySingleHostSimulation.WHEDsamples();


	string filename = "data/singleHostsSims.xml";
	std::ofstream filestream(filename.c_str());
	if ( filestream.good() ) {
		filestream << mySingleHostSimulation;
		filestream.close();
	}
	else {
		std::cout << "WARNING: unable to write to file " << filename << ".\n";
	}

	/*
	string NGMfilename = "data/generatedNGM.dat";
	filestream.open(NGMfilename.c_str());
	if ( filestream.good() ) {
		mySingleHostSimulation.printNGM(filestream);
		filestream.close();
	}
	else {
		std::cout << "WARNING: unable to write to file (NGM)" << filename << ".\n";
	}
	*/


	std::cout << "# finished.\n";

	return 0;
}





