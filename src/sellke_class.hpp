/*
 * sellke_class.hpp
 *
 *  Created on: Oct 23, 2012
 *      Author: chris
 */

#ifndef SELLKE_CLASS_HPP_
#define SELLKE_CLASS_HPP_

#include <list>
#include <iostream> // for testing...
class event;
#include "event_class.hpp"
#include "state_class.hpp"
class statesummary;
#include "statesummary_class.hpp"
#include "history_class.hpp"


class sellke {
public:
	sellke();
	void run();
	virtual ~sellke();
protected:
	/* search the eventlist for the first event, remove it from the
	 * list and return then pointer-to-event
	 */
	event* getFirstEvent();
	history* oldStates;
	std::list<event*> eventList;
	state* currentState;
	// functions for testing
	void printStats() const;
};


#endif /* SELLKE_CLASS_HPP_ */
