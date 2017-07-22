/*
 * epidemic_class.hpp
 *
 *  Created on: Oct 25, 2012
 *      Author: chris
 */

#ifndef EPIDEMIC_CLASS_HPP_
#define EPIDEMIC_CLASS_HPP_

#include "sellke_class.hpp"
#include "poplevel_state_class.hpp"
#include "poplevel_history_class.hpp"
#include "poplevel_events.hpp"
#include "host_class.hpp"
#include "parameters.hpp"
#include "frasers_rates.hpp" /* to set Hannekes functions */


#define INITIAL_NUMBER_OF_HOSTS 10 // the initial number of hosts (if > 1 then small prob of "minor epidemic")

class epidemic: public sellke {
public:
	epidemic(parstruct* );
	~epidemic();
protected:
	/* I probably don't need any extra members...
	 * maybe a buffer for tau-leaping....???
	 */
};


#endif /* EPIDEMIC_CLASS_HPP_ */
