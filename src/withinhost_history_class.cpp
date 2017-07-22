/*
 * withinhost_history_class.cpp
 *
 *  Created on: Oct 26, 2012
 *      Author: chris
 */

#include "withinhost_history_class.hpp"


withinhost_history::withinhost_history() {
	cyclicIterator = summaryList.begin();
}

withinhost_history::~withinhost_history() {
	std::list<withinhost_statesummary*>::iterator histIt = summaryList.begin();
	for ( ; histIt != summaryList.end(); histIt++ ) {
		delete (*histIt);
	}
}

void withinhost_history::push_back(statesummary* statesum) {
	//std::cout << "push_back, cumulative beta: ";
	withinhost_statesummary* tcStatesum = (withinhost_statesummary*) statesum;
	if ( !summaryList.empty() ) {
		withinhost_statesummary* previousSummary = (withinhost_statesummary*) summaryList.back();
		ext_double newCumulativeBeta = previousSummary->getCumulativeBeta()
				+ (tcStatesum->getTime() - previousSummary->getTime())
				* previousSummary->getBeta();
		tcStatesum->assignCumulativeBeta(newCumulativeBeta);
		//std::cout << tcStatesum->getCumulativeBeta();

		// Now add the cumulativeExpVL
		ext_double newCumulativeExpVL = previousSummary->getCumulativeExpVL()
						+ (tcStatesum->getTime() - previousSummary->getTime())
						* pow(ext_double(10.0),previousSummary->getVL());
		tcStatesum->assignCumulativeExpVL(newCumulativeExpVL);
	}
	summaryList.push_back(tcStatesum);
}

withinhost_statesummary* withinhost_history::getStatesummary(ext_double x, ss_memfun f) {
	/* returns a statesummary based on a number and a withinhost_statesummary method
	 * the first state summary SHOULD ALWAYS HAVE TIME 0!!!!!
	 */
	// check that the list is non-empty and that x is valid
	if ( summaryList.empty() || x < ext_double(0.0) || x != x ) {
		/* give warning */
		std::cout << "WARNING: in withinhost_history::getStatesummary: ";
		if ( summaryList.empty() ) std::cout << " Empty summaryList. ";
		if ( x < ext_double(0.0) ) std::cout << " Negative function value x. ";
		if ( x != x ) std::cout << " Not a number: x = " << x;
		std::cout << std::endl;
		return NULL;
	}
	// check that the cyclicIterator is valid
	if ( cyclicIterator == summaryList.end() ) cyclicIterator = summaryList.begin();
	// declare the to-be-returned pointer
	withinhost_statesummary* correctStatesum = NULL;
	// decide on doing a backward or a forward search
	if ( ((*cyclicIterator)->*f)() <= x ) {
		// do a forward search
		//std::cout << "do forward search...\n";
		do {
			//std::cout << ((*cyclicIterator)->*f)() << " ";
			cyclicIterator++;
			if ( cyclicIterator == summaryList.end() ) break;
		} while ( ((*cyclicIterator)->*f)() <= x );
		cyclicIterator--; // in all cases, the iterator is too high.
		correctStatesum = *cyclicIterator;
		//std::cout << std::endl;
	}
	else { // ((*cyclicIterator)->*f)() > x
		// do a backward search
		//std::cout << "do backward search...\n";
		while ( x < ((*cyclicIterator)->*f)() ) {
			//std::cout << ((*cyclicIterator)->*f)() << " ";
			if ( cyclicIterator == summaryList.begin() ) break;
			else cyclicIterator--;
		}
		//std::cout << ((*cyclicIterator)->*f)() << " ";
		correctStatesum = *cyclicIterator;
		//std::cout << std::endl;
	}

	if ( correctStatesum == NULL ) { /* give warning */
		std::cout 	<< "WARNING: in withinhost_history::getStatesummary: "
					<< "Could not find the right statesummary.\n";
	}

	return correctStatesum;
}

withinhost_statesummary* withinhost_history::getFinalStatesummary() {
	if ( summaryList.empty() ) {
		std::cout << "WARNING: summaryList is empty\n";
		return NULL;
	}
	else return summaryList.back();
}



void withinhost_history::print(std::ostream & os) const {
	os << "<within_host_history >\n";
	std::list<withinhost_statesummary*>::const_iterator It = summaryList.begin();
	for ( ; It != summaryList.end(); It++ ) {
		os << (*It) << " ";
	}
	os << "</within_host_history >\n";
}

std::ostream & operator<<(std::ostream & os, const withinhost_history* hist) {
	hist->print(os);
	return os;
}
