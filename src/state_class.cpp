/*
 * state_class.cpp
 *
 *  Created on: Oct 23, 2012
 *      Author: chris
 */

#include "state_class.hpp"

/******** general methods for state class ******************/

state::state() {
	finalStateBool = false;
	saveThisStateBool = true;
	time = ext_double(0.0);
}
state::~state() {};

bool state::isFinalState() { return finalStateBool; }
bool state::saveThisState() { return saveThisStateBool; }
void state::increaseTimeWith(ext_double WT) { time += WT; }
void state::makeFinalState() { finalStateBool = true; }
ext_double state::getTime() { return time; }
void state::rememberMe() { saveThisStateBool = true; }
void state::forgetMe() { saveThisStateBool = false; }
