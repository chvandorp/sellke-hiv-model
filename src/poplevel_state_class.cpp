/*
 * poplevel_state_class.cpp
 *
 *  Created on: Oct 26, 2012
 *      Author: chris
 */

#include "poplevel_state_class.hpp"

poplevel_state::poplevel_state(parstruct* allParams) {
	// set the population size etc.
	S0 = allParams->POPSIZE;
	S = S0; I = 0;
	deathRate = allParams->NATURAL_DEATH_RATE;
	birthRate = ext_double(S0) * deathRate;
	currentReceiver = NULL; currentTransmitter = NULL;
	saveThisStateBool = true; // TODO just for testing!!!!!
	this->allParams = allParams;
}

poplevel_state::~poplevel_state() {
	//std::cout << "delete a poplevel_state\n";
}

// TODO: improve the handling of data here... Maybe use ostreams??

statesummary* poplevel_state::makeStatesummary() {
	// make a state summary, i.e. I, time, V, TODO virus, heritablilty
	/* a bit ugly, but this is the only place to test if the state is the final state */
	if ( ext_double(allParams->MAX_TIME) <= time || I <= 0 ) finalStateBool = true;

	statesummary* newSummary = NULL;
	if ( currentTransmitter != NULL && currentReceiver != NULL ) {
		ext_double spVL1 = currentTransmitter->getSpVLtimeAverage();
		ext_double spVL2 = currentReceiver->getSpVLtimeAverage();
		ext_double k1 = (double) currentTransmitter->getK();
		ext_double k2 = (double) currentReceiver->getK();
		pathogen* virus1 = new pathogen(*(currentTransmitter->getInfectingVirus()));
		virus1->clearEvents();
		pathogen* virus2 = new pathogen(*(currentReceiver->getInfectingVirus()));
		virus2->clearEvents();
		newSummary = new poplevel_statesummary(time, I, S+I, spVL1, spVL2, k1, k2, virus1, virus2);
	}
	else {
		newSummary = new poplevel_statesummary(time, I, S+I);
	}
	return newSummary;
}

void poplevel_state::addInfected() {
	if ( S > 0 ) {
		--S; ++I;
	}
	else {
		std::cout << "WARNING (in poplevel_state::addInfected()): error with population size.\n";
	}
}

void poplevel_state::addSusceptible() {
	++S;
}

void poplevel_state::removeInfected() {
	if ( I > 0 ) {
		--I;
	}
	else {
		std::cout << "WARNING (in poplevel_state::removeInfected()): error with population size.\n";
	}
}

void poplevel_state::removeSusceptible() {
	if ( S > 0 ) {
		--S;
	}
	else {
		std::cout << "WARNING (in poplevel_state::removeSusceptible()): error with population size.\n";
	}
}

ext_double poplevel_state::getFractionSusceptibles() const {
	return ext_double(double(S)/double(S + I));
}

ext_double poplevel_state::getDensitySusceptibles() const {
	return ext_double(double(S)/double(S0));
}

void poplevel_state::addCouple(host* transmitter, host* receiver) {
	currentReceiver = receiver;
	currentTransmitter = transmitter;
}

ext_double poplevel_state::getNumberSusceptibles() const { return S; }
ext_double poplevel_state::getPopulationSize() const { return S + I; }
ext_double poplevel_state::getNumberInfecteds() const { return I; }
ext_double poplevel_state::getBirthRate() const { return birthRate; }
ext_double poplevel_state::getDeathRate() const { return deathRate; }



