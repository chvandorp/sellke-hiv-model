/*
 * withinhost_statesummary_class.cpp
 *
 *  Created on: Oct 26, 2012
 *      Author: chris
 */

#include "withinhost_statesummary_class.hpp"

/************** methods for withinhost_statesummary ********/

withinhost_statesummary::withinhost_statesummary
(ext_double t, ext_double b, ext_double vl, pathogen* v ) :
beta(b) , VL(vl) , virus(v) {
	time = t;
	cumulativeBeta = ext_double(0.0);
	cumulativeExpVL = ext_double(0.0);
}
withinhost_statesummary::~withinhost_statesummary() {
	delete virus;
}

ext_double withinhost_statesummary::getBeta() { return beta; }
ext_double withinhost_statesummary::getCumulativeBeta() { return cumulativeBeta; }

void withinhost_statesummary::assignCumulativeBeta(ext_double cb) {
	cumulativeBeta = cb;
}

ext_double withinhost_statesummary::getVL() { return VL; }
ext_double withinhost_statesummary::getCumulativeExpVL() { return cumulativeExpVL; }

void withinhost_statesummary::assignCumulativeExpVL(ext_double cvl) {
	cumulativeExpVL = cvl;
}

pathogen* withinhost_statesummary::getPathogen() { return virus; }


void withinhost_statesummary::print(std::ostream & os) const {
	os 	<< "<within_host_statesummary "
			<< "time='" << time << "' "
			<< "V='" << virus->calcSpVL() << "' "
			<< "beta='" << beta << "' > "
			<< virus << " "
	 	<< "</within_host_statesummary > \n";
}

std::ostream & operator<<(std::ostream & os, const withinhost_statesummary* sum) {
	sum->print(os);
	return os;
}


