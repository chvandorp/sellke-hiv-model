/*
 * individual_class.cpp
 *
 *  Created on: Oct 23, 2012
 *      Author: chris
 */

#include "individual_class.hpp"

/*********** general methods for individual ********************************/

individual::individual() {
	deleteByEventBool = false;
}

individual::~individual() {
	//std::cout << "deleting individual\n"; // testing
	disableMyEvents();
}

void individual::addEvent(event* ev) {
	myEvents.push_back(ev); // TODO
}

void individual::disableMyEvents() {
	std::list<event*>::iterator evIt = myEvents.begin();
	for ( ; evIt != myEvents.end(); evIt++ ) {
		/* individuals that have a NULL pointer in their eventlist
		 * will not be deleted by the sellke construction (which may be desired)
		 */
		if ( *evIt != NULL ) (*evIt)->disable();
	}
}

void individual::clearEvents() {
	myEvents.clear();
}

bool individual::detachEvent(event* ev) {
	// remove any pointers to *ev from my events
	myEvents.remove(ev);
	/*
	std::cout << "events that subject points at: ";
	for ( std::list<event*>::iterator i = myEvents.begin(); i != myEvents.end(); i++ ) {
		std::cout << (*i) << " ";
	}
	std::cout << std::endl;
	// testing...
	*/
	return myEvents.empty(); // if empty, return true. In that case the individual will be deleted
}

bool individual::deleteByEvent() { return deleteByEventBool; }



