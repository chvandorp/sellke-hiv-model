/*
 * state_class.hpp
 *
 *  Created on: Oct 23, 2012
 *      Author: chris
 */

#ifndef STATE_CLASS_HPP_
#define STATE_CLASS_HPP_

class statesummary;
#include "statesummary_class.hpp"
#include "individual_class.hpp"

class state {
public:
	state();
	virtual ~state();
	virtual statesummary* makeStatesummary()=0;
	bool isFinalState(); // complete
	bool saveThisState(); // complete
	void makeFinalState(); // complete
	void increaseTimeWith(ext_double ); // complete
	ext_double getTime(); // complete
	void rememberMe(); // set the saveThisStateBool to true
	void forgetMe(); // set the saveThisStateBool to false
protected:
	bool finalStateBool;
	bool saveThisStateBool;
	ext_double time;
};



#endif /* STATE_CLASS_HPP_ */
