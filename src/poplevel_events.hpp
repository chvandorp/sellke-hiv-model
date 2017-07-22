/*
 * poplevel_events.hpp
 *
 *  Created on: Oct 26, 2012
 *      Author: chris
 */

#ifndef POPLEVEL_EVENTS_HPP_
#define POPLEVEL_EVENTS_HPP_

#include <iostream>
#include "event_class.hpp"
#include "state_class.hpp"
#include "ext_double_class.hpp"
#include "host_class.hpp"
#include "poplevel_state_class.hpp"
#include "parameters.hpp"

#define DENSITY_DEPENDENT_TRANSMISSION


class transmission_event: public event {
public:
	transmission_event(host* , parstruct* );
	std::list<event*> modifyState(state* );
	ext_double calcEWT(state* );
	void updateLoad(ext_double , state* );
private:
	parstruct* allPars;
};


class death_event: public event {
public:
	death_event(host* , parstruct* );
	std::list<event*> modifyState(state* );
	ext_double calcEWT(state* );
	void updateLoad(ext_double , state* );
private:
};

class birth_event: public event {
public:
	birth_event();
	std::list<event*> modifyState(state* );
	ext_double calcEWT(state* );
	void updateLoad(ext_double , state* );
private:
};

class natural_death_event: public event {
public:
	natural_death_event();
	std::list<event*> modifyState(state* );
	ext_double calcEWT(state* );
	void updateLoad(ext_double , state* );
private:
};

#endif /* POPLEVEL_EVENTS_HPP_ */
