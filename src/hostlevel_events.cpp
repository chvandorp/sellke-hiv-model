/*
 * hostlevel_events.cpp
 *
 *  Created on: Oct 26, 2012
 *      Author: chris
 */

#include "hostlevel_events.hpp"

/************ phasechange_event methods **************************/

phasechange_event::phasechange_event(phase* currentPhase, parstruct* allPars) {
	// assign the subject
	subject = currentPhase;
	// bind the parstruct
	this->allPars = allPars;

	// choose an appropriate threshold
	switch ( currentPhase->getPhaseType() ) {
	case phase::ACUTE_PHASE:
		threshold = ext_double(1.0);
		break;
	case phase::ASYM_PHASE:
		do {
			threshold = ext_double(RNG.Gamma(1/allPars->FRASERS_RHO,allPars->FRASERS_RHO));
		} while ( threshold <= ext_double(0.0) );
		break;
	case phase::AIDS_PHASE:
		threshold = ext_double(1.0);
		break;
	default:
		threshold = INFTY;
		break;
	} // switch ( phase::currentPhaseType )

}

std::list<event*> phasechange_event::modifyState(state* s) {
	// do something with the state... (and the subject...)
	s->increaseTimeWith(EWTcache);
	//s->rememberMe();
	s->rememberMe();

	/* increase the phase type of phase
	 * pass the time of the event so that
	 * phase can remember the timing of the events
	 * in order to calculate the length of the asym phase.
	 */
	phase* currentPhase = (phase*) subject;
	currentPhase->changePhase(s->getTime());

	/* if the phase changes from ACUTE_PHASE
	 * to ASYM_PHASE, the state must assign the spVLonsetAsymPhase variable
	 * Also a copy of the virus at this time point should be saved
	 */
	if ( currentPhase->isAsym() ) {
		((withinhost_state*) s)->assignSpVLonsetAsymPhase();
		((withinhost_state*) s)->assignVirusEndAcutePhase();
		//s->forgetMe();
	}

	/* return a new phase change event (if not in REMOVED_PHASE;
	 * in that case, make final state)
	 */
	if ( currentPhase->isRemoved() ) {
		s->makeFinalState();
		return std::list<event*>(0);
	}
	else {
		event* newPhaseChange = new phasechange_event((phase*) subject, allPars);
		subject->addEvent(newPhaseChange);
		return std::list<event*>(1,newPhaseChange);
	}
}

ext_double phasechange_event::calcEWT(state* s) {
	withinhost_state* whedState = (withinhost_state*) s;
	EWTcache = (threshold-load)/(whedState->getDelta());
	return EWTcache;
}

void phasechange_event::updateLoad(ext_double passedTime, state* s) {
	withinhost_state* whedState = (withinhost_state*) s;
	load += (whedState->getDelta()) * passedTime;
}

/************ revmutation_event methods **************************/

revmutation_event::revmutation_event(pathogen* currentPathogen) {
	// assign the subject
	subject = currentPathogen;
	int sites = ((pathogen*) subject)->getF() + ((pathogen*) subject)->getD();

	// set threshold
	if ( sites <= 0 ) {
		threshold = INFTY;
		if ( sites < 0 ) std::cout << "WARNING: pathogen has negative number of mutations.\n";
	}
	else {
		threshold = ext_double(RNG.Exponential((double) sites));
	}
}

std::list<event*> revmutation_event::modifyState(state* s) {
	// do something with the state (including the currentPathogen)...
	((pathogen* ) subject)->doRevMutation();
	s->increaseTimeWith(EWTcache);
	s->rememberMe();

	// make a new revmutation_event
	event* newEvent = new revmutation_event((pathogen*) subject);
	// inform the virus of this newEvent
	subject->addEvent(newEvent);
	return std::list<event*>(1,newEvent);
}

ext_double revmutation_event::calcEWT(state* s) {
	withinhost_state* whedState = (withinhost_state*) s;
	EWTcache = (threshold-load)/(whedState->getLambdaRev());
	return EWTcache;
}

void revmutation_event::updateLoad(ext_double passedTime, state* s) {
	withinhost_state* whedState = (withinhost_state*) s;
	load += (whedState->getLambdaRev()) * passedTime;
}

/************ escmutation_event methods **************************/

escmutation_event::escmutation_event(pathogen* currentPathogen) {
	// assign the subject
	subject = currentPathogen;

	// set threshold

	int sites = ((pathogen*) subject)->getK() - ((pathogen*) subject)->getE();
	if ( sites <= 0 ) {
		threshold = INFTY;
		if ( sites < 0 ) std::cout << "WARNING: pathogen has negative number of mutations (2).\n";
	}
	else {
		threshold = ext_double(RNG.Exponential((double) sites));
	}
}

std::list<event*> escmutation_event::modifyState(state* s) {
	// do something with the state (including the currentPathogen)...
	((pathogen* ) subject)->doEscMutation();
	s->increaseTimeWith(EWTcache);
	s->rememberMe();

	// make a new revmutation_event
	event* newEvent = new escmutation_event((pathogen*) subject);
	// inform the virus of this event
	subject->addEvent(newEvent);
	return std::list<event*>(1,newEvent);
}


ext_double escmutation_event::calcEWT(state* s) {
	withinhost_state* whedState = (withinhost_state*) s;
	EWTcache = (threshold-load)/(whedState->getLambdaEsc());
	return EWTcache;
}

void escmutation_event::updateLoad(ext_double passedTime, state* s) {
	withinhost_state* whedState = (withinhost_state*) s;
	load += (whedState->getLambdaEsc()) * passedTime;
}
