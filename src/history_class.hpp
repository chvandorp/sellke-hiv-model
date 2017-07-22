/*
 * history_class.hpp
 *
 *  Created on: Oct 26, 2012
 *      Author: chris
 */

#ifndef HISTORY_CLASS_HPP_
#define HISTORY_CLASS_HPP_

#include "statesummary_class.hpp"

class history {
public:
	history();
	virtual ~history();
	virtual void push_back(statesummary* )=0; // act as if it is a list...
	//virtual statesummary* getStatesummary(ext_double )=0; // only used for host anyway
protected:
	// anything here??
};


#endif /* HISTORY_CLASS_HPP_ */
