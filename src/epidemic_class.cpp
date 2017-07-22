/*
 * epidemic_class.cpp
 *
 *  Created on: Oct 25, 2012
 *      Author: chris
 */

#include "epidemic_class.hpp"


epidemic::epidemic(parstruct* allParams) {
	// make a population_state object
	currentState = new poplevel_state(allParams);
	pathogen foundingVirus(allParams);
	for ( int i = 0; i < INITIAL_NUMBER_OF_HOSTS; i++ ) { // start with some hosts...
		double startTime = 0.0; // TODO make something that allows for positive start times
		pathogen* copyOfFoundingVirus = new pathogen(foundingVirus);
		host* patientZero = new host(copyOfFoundingVirus,ext_double(startTime),allParams);
		// initiate the eventList with infection_events and death_events
		event* firstInfectionEvent = new transmission_event(patientZero,allParams);
		event* firstDeathEvent = new death_event(patientZero,allParams);
		// inform patient zero about his events
		patientZero->addEvent(firstInfectionEvent);
		patientZero->addEvent(firstDeathEvent);
		// and add the events to the eventlist
		eventList.push_back(firstInfectionEvent);
		eventList.push_back(firstDeathEvent);
		// adjust the currentState for the initial infections
		((poplevel_state*) currentState)->addInfected();
	}
	event* firstNaturalDeathEvent = new natural_death_event;
	eventList.push_back(firstNaturalDeathEvent);
	event* firstBirthEvent = new birth_event;
	eventList.push_back(firstBirthEvent);

	oldStates = new poplevel_history(allParams);
	run();
}

epidemic::~epidemic() {
	// most is done in sellke::~sellke()
}
