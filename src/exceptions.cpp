/*
 * exceptions.cpp
 *
 *  Created on: Apr 23, 2014
 *      Author: chris
 */

#include "exceptions.hpp"


/* C++ expects a definition of the constructor of class ex_info
 */
ex_info::~ex_info() throw() { /* empty */ }
/* we also make a standard function that adds info to exception strings
 */
const std::string ex_info::makeMessage(const std::string & errMsg) const {
	return errMsg + (info.empty() ? "" : ": " + info);
}

/*********** definition of exception messages ************/

ex_generalError::ex_generalError(const std::string & info) throw() {
	this->info = info;
}
const char* ex_generalError::what() const throw() {
	return makeMessage("unknown error").c_str();
}

ex_commandLineError::ex_commandLineError(const std::string & info) throw() {
	this->info = info;
}
const char* ex_commandLineError::what() const throw() {
	return makeMessage("command line error").c_str();
}

ex_parseError::ex_parseError(const std::string & info) throw() {
	this->info = info;
}
const char* ex_parseError::what() const throw() {
	return makeMessage("parse error").c_str();
}

ex_rngError::ex_rngError(const std::string & info) throw() {
	this->info = info;
}
const char* ex_rngError::what() const throw() {
	return makeMessage("rng error").c_str();
}

ex_mathError::ex_mathError(const std::string & info) throw() {
	this->info = info;
}
const char* ex_mathError::what() const throw() {
	return makeMessage("rng error").c_str();
}
