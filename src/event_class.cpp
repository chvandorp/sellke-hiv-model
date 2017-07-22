/*
 * event_class.cpp
 *
 *  Created on: Oct 23, 2012
 *      Author: chris
 */

#include "event_class.hpp"

event::event() {
	enabled = true;
	subject = NULL;
	load = ext_double(0.0);
	threshold = INFTY;
	EWTcache = INFTY;
}

event::event(individual* subject) {
	// enable the event
	this->subject = subject;
	enabled = true;

	// set load and threshold
	load = ext_double(0.0);
	threshold = INFTY;
	EWTcache = INFTY;
}

event::~event() {
	/* deleteByEvent() decides if the subject must be deleted
	 * if no events remain (e.g. if it is not bound to the state)
	 * detachEvent(this) will remove this from subject->myEvents
	 */
	// std::cout << "event address that is to be deleted " << this << std::endl; // testing
	if ( subject != NULL ) {
		if ( subject->detachEvent(this) && subject->deleteByEvent() ) {
			delete subject;
		}
	}
}

void event::disable() {
	enabled = false;
	subject = NULL;
}

bool event::isEnabled() { return enabled; }
ext_double event::getEWT() { return EWTcache; }

bool compareEvents(event* ev1, event* ev2) {
	return ev1->getEWT() <= ev2->getEWT();
}


