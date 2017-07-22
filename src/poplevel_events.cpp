/*
 * poplevel_events.cpp
 *
 *  Created on: Oct 26, 2012
 *      Author: chris
 */

#include "poplevel_events.hpp"

/************* methods for transmission_event:public event **********/

transmission_event::transmission_event(host* focalHost, parstruct* allPars) {
	this->allPars = allPars;
	subject = focalHost;
	threshold = ext_double(RNG.Exponential(1.0));
}

std::list<event*> transmission_event::modifyState(state* curState) {
	//std::cout << "transmission event" << std::endl;

	poplevel_state* popState = (poplevel_state*) curState;
	host* transmittingHost = (host*) subject;
	// update the global time to the time of the event
	popState->increaseTimeWith(EWTcache);
	// std::cout << EWTcache << " " << popState->getTime() << "\n";
	popState->rememberMe();
	// initiate a list to return
	std::list<event*> newEvents;
	// make a new transmission event for the transmitting host
	event* newTransTransEvent = new transmission_event(transmittingHost,allPars);
	// inform the transmittingHost of this event and add to returned list
	transmittingHost->addEvent(newTransTransEvent);
	newEvents.push_back(newTransTransEvent);
	// get the right virus for the transmitting host
	pathogen* transmittedVirus = transmittingHost->getPathogen(popState->getTime());
	if ( transmittedVirus == NULL ) {
		std::cout << "WARNING: in transmission_event::modifyState: getPathogen returned NULL\n";

		popState->addCouple(NULL,NULL);
	}
	else {
		// increase the number of infected individuals
		popState->addInfected();
		pathogen* copyTransmittedVirus = new pathogen(*transmittedVirus);
		copyTransmittedVirus->clearEvents();
		// make a new host. NB: time was increased above.
		host* newHost = new host(copyTransmittedVirus, popState->getTime(), allPars);
		// make a transmission event for the new host
		event* newTransRecEvent = new transmission_event(newHost,allPars);
		// inform the receiving host of this event and add to returned list
		newHost->addEvent(newTransRecEvent);
		newEvents.push_back(newTransRecEvent);
		// make a death event for the new host
		event* newDeathEvent = new death_event(newHost,allPars);
		// inform the new host of this death event and add to returned list
		newHost->addEvent(newDeathEvent);
		newEvents.push_back(newDeathEvent);
		// pass the transmission couple to the state
		popState->addCouple(transmittingHost,newHost);
	}
	// return the list
	return newEvents;
}

ext_double transmission_event::calcEWT(state* curState) {
	ext_double globalTime = curState->getTime();
#ifdef DENSITY_DEPENDENT_TRANSMISSION
	ext_double densitySusc = ((poplevel_state*) curState)->getDensitySusceptibles();
	ext_double correctedThreshold = (threshold - load) / densitySusc;
#else
	ext_double fractionSusc = ((poplevel_state*) curState)->getFractionSusceptibles();
	ext_double correctedThreshold = (threshold - load) / fractionSusc;
#endif
	if ( allPars->FLAT_TP ) {
		ext_double c = ext_double(0.5) * ((host*) subject)->getTotalBeta();
		correctedThreshold *= c;
	}
	EWTcache = ((host*) subject)->calcEWTofTransmission(globalTime,correctedThreshold);
	return EWTcache;
}

void transmission_event::updateLoad(ext_double lengthInterval, state* curState) {
	ext_double c = 1.0;
	if ( allPars->FLAT_TP ) {
		c = ext_double(0.5) * ((host*) subject)->getTotalBeta();
	}
	ext_double globalTime = curState->getTime();
#ifdef DENSITY_DEPENDENT_TRANSMISSION
	ext_double densitySusc = ((poplevel_state*) curState)->getDensitySusceptibles();
	load += densitySusc / c * ( ((host*) subject)->calcIntegralBeta(globalTime,lengthInterval) );
#else
	ext_double fractionSusc = ((poplevel_state*) curState)->getFractionSusceptibles();
	load += fractionSusc / c * ( ((host*) subject)->calcIntegralBeta(globalTime,lengthInterval) );
#endif
}



/********************* methods for death_event **********************/

death_event::death_event(host* focalHost, parstruct* allPars) {
	subject = focalHost;
	ext_double thr_nat = ext_double(RNG.Exponential(allPars->NATURAL_DEATH_RATE));
	ext_double thr_aids = focalHost->getTimeOfDeath();
	threshold = min(thr_nat,thr_aids);
}

std::list<event*> death_event::modifyState(state* curState) {
	//std::cout << "death event" << std::endl;
	poplevel_state* popState = (poplevel_state*) curState;
	// increase the global time
	popState->increaseTimeWith(EWTcache);
	popState->forgetMe();
	// decrease the number of Infectious Individuals
	popState->removeInfected();
 	// popState->addSusceptible(); // ONLY IF POPULATIONSIZE IS CONSTANT
	/* the host will be deleted if the event is deleted...
	 * but only if no events are left pointing to it. Therefore, we will
	 * delete it here (which disables its events)
	 */
	delete subject; subject = NULL;
	// clear the pointers to individuals in the state
	popState->addCouple(NULL,NULL);
	// return an empty list
	return std::list<event*>(0);
}

ext_double death_event::calcEWT(state* curState) {
	EWTcache = threshold - load;
	return EWTcache;
}

void death_event::updateLoad(ext_double WT, state* curState) { load += WT; }


// birth event of a new susceptible person

birth_event::birth_event() {
	threshold = ext_double(RNG.Exponential(1.0));
}
std::list<event*> birth_event::modifyState(state* s) {
	poplevel_state* tc_s = (poplevel_state*) s;
	tc_s->addSusceptible();
	// update the global time to the time of the event
	s->increaseTimeWith(EWTcache);
	s->forgetMe();
	// clear the pointers to individuals in the state
	tc_s->addCouple(NULL,NULL);
	// initiate a list to return
	std::list<event*> ret;
	event* ev = new birth_event;
	ret.push_back(ev);
	return ret;
}
ext_double birth_event::calcEWT(state* s) {
	poplevel_state* tc_s = (poplevel_state*) s;
	EWTcache = (threshold - load) / tc_s->getBirthRate();
	return EWTcache;
}
void birth_event::updateLoad(ext_double WT, state* s) {
	poplevel_state* tc_s = (poplevel_state*) s;
	load += WT * tc_s->getBirthRate();
}


// death event of a susceptible person

natural_death_event::natural_death_event() {
	threshold = ext_double(RNG.Exponential(1.0));
}
std::list<event*> natural_death_event::modifyState(state* s) {
	poplevel_state* tc_s = (poplevel_state*) s;
	tc_s->removeSusceptible();
	// update the global time to the time of the event
	s->increaseTimeWith(EWTcache);
	s->forgetMe();
	// clear the pointers to individuals in the state
	tc_s->addCouple(NULL,NULL);
	// initiate a list to return
	std::list<event*> ret;
	event* ev = new natural_death_event;
	ret.push_back(ev);
	return ret;
}
ext_double natural_death_event::calcEWT(state* s) {
	poplevel_state* tc_s = (poplevel_state*) s;
	ext_double totalRate = tc_s->getDeathRate() * tc_s->getNumberSusceptibles();
	EWTcache = (threshold - load) / totalRate;
	return EWTcache;
}
void natural_death_event::updateLoad(ext_double WT, state* s) {
	poplevel_state* tc_s = (poplevel_state*) s;
	ext_double totalRate = tc_s->getDeathRate() * tc_s->getNumberSusceptibles();
	load += WT * totalRate;
}
