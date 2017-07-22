/*
 * hostlevel_events.hpp
 *
 *  Created on: Oct 26, 2012
 *      Author: chris
 */

#ifndef HOSTLEVEL_EVENTS_HPP_
#define HOSTLEVEL_EVENTS_HPP_

#include "event_class.hpp"
#include "state_class.hpp"
#include "ext_double_class.hpp"
#include "RngClass.hpp"
#include "pathogen_class.hpp"
#include "phase_class.hpp"
#include "withinhost_state_class.hpp"
#include "frasers_rates.hpp"
#include "parameters.hpp"


class phasechange_event: public event {
public:
	phasechange_event(phase* , parstruct* ); // complete
	std::list<event*> modifyState(state* ); // complete
	ext_double calcEWT(state* ); // complete TODO make more efficient (delta)
	void updateLoad(ext_double , state* ); // complete TODO make more efficient
protected:
	parstruct* allPars;
};


class escmutation_event: public event {
public:
	escmutation_event(pathogen* );
	std::list<event*> modifyState(state* );
	ext_double calcEWT(state* );
	void updateLoad(ext_double , state* );
protected:
};


class revmutation_event: public event {
public:
	revmutation_event(pathogen* );
	std::list<event*> modifyState(state* );
	ext_double calcEWT(state* );
	void updateLoad(ext_double , state* );
protected:
};

#endif /* HOSTLEVEL_EVENTS_HPP_ */
