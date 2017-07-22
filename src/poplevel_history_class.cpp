/*
 * poplevel_history_class.cpp
 *
 *  Created on: Oct 31, 2012
 *      Author: chris
 */

#include "poplevel_history_class.hpp"


poplevel_history::poplevel_history(parstruct* allParams) {
	nextSampleTime = ext_double(0.0);
	next_echo_time = 1;
	echo_bool = ( allParams->EPISIM_ECHO_BOOL == 1 );
	sample_constant = ext_double(allParams->EPISIM_SAMPLE_CONSTANT);
	maxtime = (int) allParams->MAX_TIME;
	time(&start_clock_time);

	fileLink.open((allParams->EPISIM_WRITE_FILENAME).c_str());
	fileLink << "<epidemicsimulation "
				<< "POPSIZE='" << allParams->POPSIZE << "' "
				<< "VLMAX='" << allParams->VLMAX <<  "' "
				<< "SIGMA='" << allParams->SIGMA << "' "
				<< "PHI='" << allParams->PHI << "' "
				<< "LAMBDA1='" << allParams->LAMBDA1 << "' "
				<< "LAMBDA2='" << allParams->LAMBDA2 << "' "
				<< "LAMBDA3='" << allParams->LAMBDA3 << "' "
				<< "PREV='" << allParams->PREV << "' "
				<< "PESC='" << allParams->PESC << "' "
				<< "initialNumberOfMutations='" << (allParams->INITIAL_E + allParams->INITIAL_F) << "' "
				<< "PEPTIDES_N='" << allParams->PEPTIDES_N << "' "
				<< "NONEPITOPE_M='" << allParams->NONEPITOPE_M << "' "
				<< "MEAN_K='" << allParams->MEAN_K << "' "
				<< "STANDARD_DEV_K='" << allParams->STANDARD_DEV_K << "' "
				<< "FLAT_TP='" << (allParams->FLAT_TP ? "TRUE" : "FALSE") << "' > \n";
}


poplevel_history::~poplevel_history() {
	//std::cout << "deleting poplevel_history\n";

	fileLink << "</epidemicsimulation>";

	fileLink.close(); // flushes the ofstream...
}

void poplevel_history::push_back(statesummary* statsum) {
	ext_double t = statsum->getTime();

	/* compute heritability in the epidemic simulation */

	/* (1) remember all spVL pairs between t-CALC_HERIT_INTERVAL and t */

	std::pair<ext_double,ext_double> spVLpair = ((poplevel_statesummary*) statsum)->getSpVLpair();
	std::pair<ext_double,ext_double> noMutsPair = ((poplevel_statesummary*) statsum)->getNoMutspair();
	if ( isNUM(spVLpair.first) && isNUM(spVLpair.second) ) { /* test if the pair comes from an infection event */
		heritlist.push_back(triple(t,spVLpair.first,spVLpair.second));
		heritlistNoMuts.push_back(triple(t,noMutsPair.first,noMutsPair.second));
		while ( !heritlist.empty() ) {
			if ( heritlist.front().fst >= t-CALC_HERIT_INTERVAL ) {
				break;
			}
			else {
				heritlist.pop_front();
				heritlistNoMuts.pop_front();
			}
		}

	}

	/* write the state summary to a file (now specified in the header
	 * of poplevel_history_class...
	 * and otherwise take a smaller number for # events/year
	 */
	if ( nextSampleTime <= t ) {
		/* (2) compute heritability using the heritlist */

		correlation heritObject(heritlist);
		correlation heritObjectNoMuts(heritlistNoMuts);

		/* add running stats to statsum */

		((poplevel_statesummary*) statsum)->addRunningStats(heritObject,heritObjectNoMuts,CALC_HERIT_INTERVAL);


		/* write the statesummary to file */
		((poplevel_statesummary*) statsum)->writeToFile(fileLink);

		/* sample in regular intervals */
		while ( nextSampleTime <= t ) {
			nextSampleTime += sample_constant;
		}
	}
	if ( echo_bool ) {
		if ( ext_double(next_echo_time) <= t ) {
			time_t current_clock_time; time(&current_clock_time);
			double DT = difftime(current_clock_time,start_clock_time) / double(next_echo_time);
			long int remaining_seconds = (long int)(double(maxtime-next_echo_time) * DT);

			long int remaining_days = remaining_seconds / 86400; remaining_seconds -= 86400 * remaining_days;
			long int remaining_hours = remaining_seconds / 3600; remaining_seconds -= 3600 * remaining_hours;
			long int remaining_minutes = remaining_seconds / 60; remaining_seconds -= 60 * remaining_minutes;

			std::cout << "# --- ETA: "
					  << remaining_days << " days, "
					  << remaining_hours << " hours, "
					  << remaining_minutes << " minutes and "
					  << remaining_seconds << " seconds.\r";
			fflush(stdout);

			while ( ext_double(next_echo_time) <= statsum->getTime() ) {
				next_echo_time++;
			}
		}
	}
	delete statsum; // always delete this!!!
}

