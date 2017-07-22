/*
 * parametersweep_class.cpp
 *
 *  Created on: Nov 26, 2012
 *      Author: chris
 */

#include "parametersweep_class.hpp"

parametersweep::parametersweep(parstruct* allParams) {
	this->allParams = allParams;

	std::pair< std::pair<double,double>, int > vlmax = allParams->RANGE_VLMAX;
	std::pair< std::pair<double,double>, int > pp = allParams->RANGE_LOGPESC;
	std::pair< std::pair<double,double>, int > nn = allParams->RANGE_N;

	double ppStepsize = (pp.first.second - pp.first.first);
	if ( pp.second > 1 ) ppStepsize /= double(pp.second-1);
	double vlmaxStepsize = (vlmax.first.second - vlmax.first.first);
	if ( vlmax.second > 1 ) vlmaxStepsize /= double(vlmax.second-1);
	int nnStepsize = (int) floor((nn.first.second - nn.first.first));
	if (nn.second > 1 ) nnStepsize /= double(nn.second-1);

	if ( vlmax.second <= 1 ) vlmaxRange.push_back(vlmax.first.first);
	else {
		for ( int i = 0; i < vlmax.second; i++ ) {
			vlmaxRange.push_back(vlmax.first.first + i*vlmaxStepsize);
		}
	}
	if ( pp.second <= 1 ) pescRange.push_back(pp.first.first);
	else {
		for ( int i = 0; i < pp.second; i++ ) {
			pescRange.push_back(pp.first.first + i*ppStepsize);
		}
	}
	if ( nn.second <= 1 ) nRange.push_back((int) nn.first.first);
	else {
		for ( int i = 0; i < nn.second; ++i ) {
			nRange.push_back(int(nn.first.first) + i*nnStepsize);
		}
	}

	/* try to load a parameter-curve from a file specified in the parameters
	 * If this is successful, loadedCurve will be true, else false
	 */
	readCurveFile();
}

void parametersweep::doNGMbased() {
	/* catch a possible error with the range divisor/remainder */
	if ( allParams->RANGE_DIVIDER <= 0
			|| allParams->RANGE_REMAINDER < 0
			|| allParams->RANGE_REMAINDER >= allParams->RANGE_DIVIDER ) {
		std::cout << "WARNING (in parametersweep::doNGMbased): "
				  << "invalid RANGLE_REMAINDER of RANGE_DIVISOR given (using 0/1).\n";
		allParams->RANGE_DIVIDER = 1;
		allParams->RANGE_REMAINDER = 0;
	}


	std::string filename = allParams->NGMPARSWEEP_WRITE_FILENAME;
	/* modify the file name if the RANGE_DIVISOR > 1 */
	if ( allParams->RANGE_DIVIDER > 1 ) {
		std::stringstream ss;
		ss << "[" << allParams->RANGE_REMAINDER << "(" << allParams->RANGE_DIVIDER << ")]";
		std::string indexString;
		ss >> indexString;
		unsigned found = filename.find(".xml"); // get the position of the file extention (NB .xml)
		filename.insert(found,indexString); // and put [remainder/divider] before .xml
	}


	std::ofstream os(filename.c_str());

	if ( !os.good() ) {
		os.close();
		std::cout << "WARNING: unable to write to file " << filename << ".\n";
		return;
	}

	/* write root tag to file */
	os << "<parameter_sweep_NGM "
			<< "SAMPLESIZE='" << allParams->SAMPLESIZE << "' "
			<< "SAMPLESIZE_LARGE='" << allParams->SAMPLESIZE_LARGE << "' "
			<< "PREV='" << allParams->PREV << "' "
			<< "N='" << allParams->PEPTIDES_N << "' "
			<< "MEAN_K='" << allParams->MEAN_K << "' "
			<< "STANDARD_DEV_K='" << allParams->STANDARD_DEV_K << "' "
			<< "SIGMA='" << allParams->SIGMA << "' "
			<< "PHI='" << allParams->PHI << "' "
			<< "FLAT_TP='" << (allParams->FLAT_TP ? "TRUE" : "FALSE") << "' "
			<< ">\n";

	int number_of_steps =
			pescRange.size() *
			nRange.size() *
			vlmaxRange.size() / allParams->RANGE_DIVIDER;
	int steps_finished = 0; // not entirely accurate, but who cares

	/* a counter for the division of the parametersweep */
	int modCounter = 0;


	/* NB: the program takes the defined PESC and PREV in the parameter file and scales both values
	 * with the values in the range
	 * so for instance, if PESC = 0.001586 and PREV = 0.000559, and the range is given by
	 * -8.0 ... 0.0, then the sweeped values of PESC are
	 * 0.001586 * 10^(-8.0) ... 0.001586 * 10^(0.0)
	 * and for PREV:
	 * 0.000559 * 10^(-8.0) ... 0.000559 * 10^(0.0)
	 * 
	 * The parametersweep can be done over several ranges, but this must be determined 
	 * at compile time. TODO: let the user choose what he/she wants at run time!!!
	 */

	// NB: sweep is over Vmax and n.
	for ( std::vector<double>::iterator vlmax = vlmaxRange.begin(); vlmax != vlmaxRange.end(); vlmax++ ) {
		for ( std::vector<double>::iterator pp = pescRange.begin(); pp != pescRange.end(); pp++ ) {
		//for ( std::vector<int>::iterator nn = nRange.begin(); nn != nRange.end(); nn++ ) { // uncomment when sweeping n!
			/* the constants RANGE_DIVIDER and RANGE_REMAINDER
			 * determine if a simulation with the particular parameters should be in this
			 * parameter sweep. This is the case if
			 * full_number_of_steps \cong RANGE_REMIANDER (\mod RANGE_DIVIDER)
			 */
			if ( modCounter % allParams->RANGE_DIVIDER == allParams->RANGE_REMAINDER ) {
				// make a new parstruct (nicer than changing the old one)
				parstruct newAllParams(*allParams); // exists only in this scope!!!!
				/* change the parameters in newAllParams */
				newAllParams.PESC = allParams->PESC * pow(10.0,*pp);
				newAllParams.PREV = allParams->PREV * pow(10.0,*pp);
				newAllParams.VLMAX = (*vlmax);
				//newAllParams.PEPTIDES_N = (*nn); // uncomment when sweeping n!

				singlehostsim SHS(&newAllParams); // pass the address
				SHS.makeNGM();
				SHS.computeEigs();
				SHS.WHEDsamplesFromDist();
				SHS.computeStatistics();
				os << SHS.getStatistics();

				/* ETA */
				steps_finished++;
				std::cout << "# " << steps_finished << " of " << number_of_steps << " finished.\n";
			} // newAllParams is destructed here...
			modCounter++;
		}
	}

	/* close root tag */
	os << "</parameter_sweep_NGM >";


	os.close();
}

void parametersweep::doNGMbasedAlongCurve() {
	if ( !loadedCurve ) {
		std::cout << "WARNING (in parametersweep::doNGMbasedAlongCurve()): no curve has been loaded.\n";
		return;
	}

	std::string filename = allParams->NGMPARSWEEP_WRITE_FILENAME;
	/* modify the file name if the RANGE_DIVISOR > 1 */
	if ( allParams->RANGE_DIVIDER > 1 ) {
		std::stringstream ss;
		ss << "[" << allParams->RANGE_REMAINDER << "(" << allParams->RANGE_DIVIDER << ")]";
		std::string indexString;
		ss >> indexString;
		unsigned found = filename.find(".xml"); // get the position of the file extention (NB .xml)
		filename.insert(found,indexString); // and put [remainder/divider] before .xml
	}


	std::ofstream os(filename.c_str());

	if ( !os.good() ) {
		os.close();
		std::cout << "WARNING: unable to write to file " << filename << ".\n";
		return;
	}

	/* write root tag to file */
	os << "<parameter_sweep_NGM "
			<< "SAMPLESIZE='" << allParams->SAMPLESIZE << "' "
			<< "SAMPLESIZE_LARGE='" << allParams->SAMPLESIZE_LARGE << "' "
			<< "PREV='" << allParams->PREV << "' "
			<< "N='" << allParams->PEPTIDES_N << "' "
			<< "MEAN_K='" << allParams->MEAN_K << "' "
			<< "STANDARD_DEV_K='" << allParams->STANDARD_DEV_K << "' "
			<< "SIGMA='" << allParams->SIGMA << "' "
			<< "PHI='" << allParams->PHI << "' "
			<< "FLAT_TP='" << (allParams->FLAT_TP ? "TRUE" : "FALSE") << "' "
			<< ">\n";

	int number_of_steps = curve.size() / allParams->RANGE_DIVIDER;
	int steps_finished = 0; // not entirely accurate...

	/* a counter for the division of the parametersweep */
	int modCounter = 0;

	/* curves only specify lambda_1esc and the parameter file
	 * specifies the ratios, so first get them...
	 */
	double ratio1 = allParams->LAMBDA1 / allParams->LAMBDA1; /* :-) */
	double ratio2 = allParams->LAMBDA2 / allParams->LAMBDA1;
	double ratio3 = allParams->LAMBDA3 / allParams->LAMBDA1;

	for ( std::vector< std::pair<double, double> >::iterator cit = curve.begin(); cit != curve.end(); ++cit ) {
		if ( modCounter % allParams->RANGE_DIVIDER == allParams->RANGE_REMAINDER ) {
			// make a new parstruct (nicer than changing the old one)
			parstruct newAllParams(*allParams); // exists only in this scope!!!!
			/* change the parameters in newAllParams */
			newAllParams.VLMAX = cit->second;
			double lambda = cit->first;
			newAllParams.LAMBDA1 = lambda * ratio1;
			newAllParams.LAMBDA2 = lambda * ratio2;
			newAllParams.LAMBDA3 = lambda * ratio3;

			singlehostsim SHS(&newAllParams); // pass the address
			SHS.makeNGM();
			SHS.computeEigs();
			SHS.WHEDsamplesFromDist();
			SHS.computeStatistics();
			os << SHS.getStatistics();

			/* write a transmission-couple data set to file */
			std::stringstream tcFilenameStream;
			tcFilenameStream << "data-hsq/herit-data-" << modCounter << ".dat";
			std::ofstream os_tc(tcFilenameStream.str().c_str(),std::ios_base::out);
			if ( os_tc) {
				// some info as a comment...
				os_tc << "# VLMAX = " << newAllParams.VLMAX << ", LAMBDA1 = " << newAllParams.LAMBDA1 << "\n";
				// a header for R...
				os_tc << "'Vtavr' 'Vminr' 'kr' 'mr' 'Vtavt' 'Vmint' 'kt' 'mt'\n";
				SHS.printTCs(os_tc);
				os_tc.close();
			}
			else {
				std::cout << "WARNING (in parametersweep::doNGMbasedAlongCurve()): unable to open herit-data file.\n";
			}

			/* ETA */
			steps_finished++;
			std::cout << "# " << steps_finished << " of " << number_of_steps << " finished.\n";
		} // newAllParams is destructed here...
		modCounter++;
	}
	/* close root tag */
	os << "</parameter_sweep_NGM >";
	os.close();
}

void parametersweep::readCurveFile() {
	loadedCurve = false;
	std::string filename = allParams->PARSWEEP_CURVE_FILENAME;
	std::ifstream fileStream(filename.c_str());
	if ( fileStream ) {
		/* a char* to write to */
		char line[256];
		/* loop through the file */
		while ( fileStream.good() ) {
			/* get the name of the parameter */
			fileStream.getline(line,256,'\n');
			std::string lineString(line);
			if ( !lineString.empty() ) {
				/* put the lineString into an istringstream */
				std::istringstream lineStream(lineString);
				std::pair<double,double> pr;
				char value_ch[256];
				lineStream.getline(value_ch,256,' ');
				std::istringstream(std::string(value_ch)) >> pr.first;
				lineStream.getline(value_ch,256,' ');
				std::istringstream(std::string(value_ch)) >> pr.second;
				curve.push_back(pr);

				// std::cout << pr.first << " " << pr.second << "\n"; // TESTING!!
			}
		}
		if ( !curve.empty() ) loadedCurve = true;
		/* close the file */
		fileStream.close();
	}
}






