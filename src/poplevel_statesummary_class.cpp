/*
 * poplevel_statesummary_class.cpp
 *
 *  Created on: Oct 31, 2012
 *      Author: chris
 */

#include "poplevel_statesummary_class.hpp"

// TODO: handle data better for epidemics (in the statesummary)

poplevel_statesummary::poplevel_statesummary
(ext_double time, int prevalence, int popsize) {
	this->time = time;
	this->prevalence = prevalence;
	this->popsize = popsize;
	spVLtrans = NANUM;
	spVLrec = NANUM;
	kRec = NANUM;
	kTrans = NANUM;
	numberOfMutationsRec = NANUM;
	numberOfMutationsTrans = NANUM;
	this->dRec = NANUM;
	this->dTrans = NANUM;
	virusRec = NULL;
	virusTrans = NULL;

	initStatsMap();
}

poplevel_statesummary::poplevel_statesummary
(ext_double time, int prevalence, int popsize,
		ext_double spVLtrans, ext_double spVLrec,
		ext_double kTrans, ext_double kRec,
		pathogen* virusTrans, pathogen* virusRec) {
	this->time = time;
	this->prevalence = prevalence;
	this->popsize = popsize;
	this->spVLrec = spVLrec;
	this->spVLtrans = spVLtrans;
	this->kRec = kRec;
	this->kTrans = kTrans;
	if ( virusTrans != NULL ) {
		numberOfMutationsTrans = virusTrans->getNumberOfMutations();
		dTrans = virusTrans->getD();
	}
	else {
		numberOfMutationsTrans = NANUM;
		dTrans = NANUM;
	}
	if ( virusRec != NULL ) {
		numberOfMutationsRec = virusRec->getNumberOfMutations();
		dRec = virusRec->getD();
	}
	else {
		numberOfMutationsRec = NANUM;
		dRec = NANUM;
	}
	this->virusRec = virusRec;
	this->virusTrans = virusTrans;

	initStatsMap();
}

poplevel_statesummary::~poplevel_statesummary() {
	delete virusTrans;
	delete virusRec;
}

std::pair<ext_double,ext_double> poplevel_statesummary::getSpVLpair() {
	return std::pair<ext_double,ext_double>(spVLtrans,spVLrec);
}

std::pair<ext_double,ext_double> poplevel_statesummary::getNoMutspair() {
	ext_double mutstrans = NANUM; ext_double mutsrec = NANUM;
	if ( virusTrans != NULL && virusRec != NULL ) {
		mutstrans = virusTrans->getE() + virusTrans->getF() + virusTrans->getD();
		mutsrec = virusRec->getE() + virusRec->getF() + virusRec->getD();
	}
	return std::pair<ext_double,ext_double>(mutstrans,mutsrec);
}

void poplevel_statesummary::writeToFile(std::ostream & os) {
	os << "<sample "
			<< "time='" << time << "' "
			<< "prevalence='" << prevalence << "' "
			<< "popsize='" << popsize << "' "
			<< "spVLrec='" << spVLrec << "' "
			<< "spVLtrans='" << spVLtrans << "' "
			<< "kRec='" << kRec << "' "
			<< "kTrans='" << kTrans << "' "
			<< "numberOfMutationsRec='" << numberOfMutationsRec << "' "
			<< "numberOfMutationsTrans='" << numberOfMutationsTrans << "' "
			<< "dRec='" << dRec << "' "
			<< "dTrans='" << dTrans << "' ";
		/* write entries of the statsMap to file */
	for ( std::map<std::string,ext_double>::iterator it = statsMap.begin(); it != statsMap.end(); it++ ) {
		os  << it->first << "='" << it->second << "' ";
	}
	os << ">\n";
	if ( virusTrans != NULL ) {
		os << "<virusTrans>"
				<< virusTrans
				<< "</virusTrans>\n";
	}
	if ( virusRec != NULL ) {
		os << "<virusRec>"
				<< virusRec
				<< "</virusRec>\n";
	}
	os << "</sample>\n";
}


void poplevel_statesummary::addRunningStats(const correlation & heritObject,
		const correlation & heritObjectNoMuts,
		ext_double interval_length) {
	statsMap["running_slopeSPVL"] = heritObject.get_slope();
	statsMap["running_interceptSPVL"] = heritObject.get_intercept();
	statsMap["running_R2SPVL"] = heritObject.get_R2();
	statsMap["running_samplesize"] = heritObject.get_N();
	statsMap["running_incidence"] = heritObject.get_N() / interval_length;
	statsMap["running_meanSPVL"] = heritObject.get_mean(); // time average (tav)
	statsMap["running_varSPVL"] = heritObject.get_var(); // time average (tav)
	statsMap["running_medianSPVL"] = heritObject.get_median();
	statsMap["running_top_percentileSPVL"] = heritObject.get_top_percentile();
	statsMap["running_bottom_percentileSPVL"] = heritObject.get_bottom_percentile();
	statsMap["running_slopeNoMuts"] = heritObjectNoMuts.get_slope();
	statsMap["running_interceptNoMuts"] = heritObjectNoMuts.get_intercept();
	statsMap["running_R2NoMuts"] = heritObjectNoMuts.get_R2();
	statsMap["running_meanNoMuts"] = heritObjectNoMuts.get_mean();
	statsMap["running_varNoMuts"] = heritObjectNoMuts.get_var();
	statsMap["running_medianNoMuts"] = heritObjectNoMuts.get_median();
	statsMap["running_top_percentileNoMuts"] = heritObjectNoMuts.get_top_percentile();
	statsMap["running_bottom_percentileNoMuts"] = heritObjectNoMuts.get_bottom_percentile();
}

void poplevel_statesummary::initStatsMap() {
	statsMap["running_slopeSPVL"] = NANUM;
	statsMap["running_interceptSPVL"] = NANUM;
	statsMap["running_R2SPVL"] = NANUM;
	statsMap["running_samplesize"] = NANUM;
	statsMap["running_incidence"] = NANUM;
	statsMap["running_meanSPVL"] = NANUM; // time average (tav)
	statsMap["running_varSPVL"] = NANUM; // time average (tav)
	statsMap["running_medianSPVL"] = NANUM;
	statsMap["running_top_percentileSPVL"] = NANUM;
	statsMap["running_bottom_percentileSPVL"] = NANUM;
	statsMap["running_slopeNoMuts"] = NANUM;
	statsMap["running_interceptNoMuts"] = NANUM;
	statsMap["running_R2NoMuts"] = NANUM;
	statsMap["running_meanNoMuts"] = NANUM;
	statsMap["running_varNoMuts"] = NANUM;
	statsMap["running_medianNoMuts"] = NANUM;
	statsMap["running_top_percentileNoMuts"] = NANUM;
	statsMap["running_bottom_percentileNoMuts"] = NANUM;
}
