/*
 * statesummary_class.hpp
 *
 *  Created on: Oct 23, 2012
 *      Author: chris
 */

#ifndef STATESUMMARY_CLASS_HPP_
#define STATESUMMARY_CLASS_HPP_

#include <iostream> // for testing

#include "ext_double_class.hpp"

class statesummary {
public:
	statesummary();
	virtual ~statesummary();
	ext_double getTime(); // complete
protected:
	ext_double time;
};





#endif /* STATESUMMARY_CLASS_HPP_ */
