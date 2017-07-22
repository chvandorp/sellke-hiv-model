/*
 * event_class.hpp
 *
 *  Created on: Oct 23, 2012
 *      Author: chris
 */

#ifndef EVENT_CLASS_HPP_
#define EVENT_CLASS_HPP_

#include <iostream> // testing...
#include <list>
#include "ext_double_class.hpp"

class state;
#include "state_class.hpp"
class individual;
#include "individual_class.hpp"

class event {
public:
	event();
	event(individual* );
	virtual ~event();
	virtual std::list<event*> modifyState(state* )=0;
	virtual ext_double calcEWT(state* )=0;
	virtual void updateLoad(ext_double , state* )=0;
	ext_double getEWT();
	void disable();
	bool isEnabled();
protected:
	individual* subject;
	ext_double load;
	ext_double threshold;
	ext_double EWTcache;
	bool enabled;
};

bool compareEvents(event* , event* ); // complete



#endif /* EVENT_CLASS_HPP_ */
