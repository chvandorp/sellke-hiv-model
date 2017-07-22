/*
 * individual_class.hpp
 *
 *  Created on: Oct 23, 2012
 *      Author: chris
 */

#ifndef INDIVIDUAL_CLASS_HPP_
#define INDIVIDUAL_CLASS_HPP_


class event;
#include "event_class.hpp"
#include <list>

class individual {
public:
	individual();
	virtual ~individual();
	void disableMyEvents(); // complete! (TODO: maybe put in protected and call upon destruction)
	void addEvent(event* ); // complete!
	void clearEvents(); // complete!
	bool detachEvent(event* ); // complete
	bool deleteByEvent(); // complete
protected:
	std::list<event*> myEvents;
	bool deleteByEventBool; // standard false
};


#endif /* INDIVIDUAL_CLASS_HPP_ */
