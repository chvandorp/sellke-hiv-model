/*
 * sellke_class.cpp
 *
 *  Created on: Oct 23, 2012
 *      Author: chris
 */

#include "sellke_class.hpp"

sellke::sellke() {
	currentState = NULL;
	oldStates = NULL;
}

sellke::~sellke() {
	//std::cout << "deleting a sellke construction " << eventList.size() << "\n";

	// delete the current state
	delete currentState;

	// delete any remaining events
	std::list<event*>::iterator evIt = eventList.begin();
	for ( ; evIt != eventList.end(); evIt++ ) {
		delete (*evIt);
	}

	// delete the history
	delete oldStates;
}

void sellke::run() {
	if ( currentState == NULL ) {
		std::cout << "WARNING: (in sellke::run()): currentState is not initialized.\n";
		return; // to prevent errors...
	}
	// the main loop....
	while ( !currentState->isFinalState() ) {
// (1a) sort eventList and remove disabled events
		//std::cout << "sellke: sort eventList\n";

		std::list<event*>::iterator evIt = eventList.begin();
		while ( evIt != eventList.end() ) {
			if ( (*evIt)->isEnabled() ) {
				(*evIt)->calcEWT(currentState);
				evIt++; // :-)
			}
			else {
				delete (*evIt); // disabled events point to NULL!!!
				evIt = eventList.erase(evIt);
			}
		}
// (1b) test if any events are left over...
		if ( eventList.empty() ) break; // simulation is over

// (2) take out first event
		//std::cout << "sellke: take out first event " << eventList.size() << "\n";
/*
		eventList.sort(compareEvents);
		event* firstEvent = eventList.front();
		eventList.pop_front();
*/
// (2') leave the eventList unsorted and just take out the first event
		event* firstEvent = getFirstEvent();

// (3) update the other events
		//std::cout << "sellke: update other events\n";

		ext_double WT = firstEvent->getEWT();
		if ( WT == INFTY ) break;
		for ( evIt = eventList.begin(); evIt != eventList.end(); evIt++ ) {
			(*evIt)->updateLoad(WT,currentState);
		}

// (4) add a statesummary to the history
		//std::cout << "sellke: add a statesummary to the history\n";


		if ( currentState->saveThisState() ) {
			oldStates->push_back(currentState->makeStatesummary());
		}

// (5) modify the state with the first event
		//std::cout << "sellke: modify state with first event\n";


		std::list<event*> newEvents = firstEvent->modifyState(currentState);

// (6) add new events to the eventList
		//std::cout << "sellke: add new events to the eventList\n";



		eventList.insert(eventList.end(),newEvents.begin(),newEvents.end());

// (7) delete the first event
		//std::cout << "sellke: delete the first event\n";


		delete firstEvent; // deletes subject if OK to delete...


		// TESTING!!!
		// printStats();
	} // while ( !currentState->isFinalState() )

// make a summary of the final state.
	if ( currentState->saveThisState() ) {
		oldStates->push_back(currentState->makeStatesummary());
	}
}

event* sellke::getFirstEvent() {
	if ( eventList.empty() ) return NULL;
	else {
		std::list<event*>::iterator it = eventList.begin();
		std::list<event*>::iterator jt = eventList.begin();
		for ( ; it != eventList.end(); ++it ) {
			if ( (*it)->getEWT() <= (*jt)->getEWT() ) {
				jt = it;
			}
		}
		event* fstEvent = *jt;
		eventList.erase(jt);
		return fstEvent;
	}
}

void sellke::printStats() const {
	ext_double meanEWT = 0.0;
	int numValidEvents = 0;
	int numDisabledEvents = 0;
	int numInfEvents = 0;
	int numNanEvents = 0;
	for ( std::list<event*>::const_iterator it = eventList.begin(); it != eventList.end(); ++it ) {
		if ( (*it)->isEnabled() ) {
			if ( isNUM((*it)->getEWT()) ) {
				if ( isFinite((*it)->getEWT()) ) {
					meanEWT += (*it)->getEWT();
					++numValidEvents;
				}
				else ++numInfEvents;
			}
			else ++numNanEvents;
		}
		else ++numDisabledEvents;
	}
	if ( numValidEvents > 0 ) meanEWT /= numValidEvents;
	if ( eventList.size() > 10 ) {
		std::cout << currentState->getTime() << " "
				  << meanEWT << " "
				  << eventList.size() << " "
				  << numValidEvents << " "
				  << numDisabledEvents << " "
				  << numInfEvents << " "
				  << numNanEvents << "\n";
	}
}
