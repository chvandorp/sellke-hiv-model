/*
 * exceptions.hpp
 *
 *  Created on: Apr 23, 2014
 *      Author: chris
 */

#ifndef EXCEPTIONS_HPP_
#define EXCEPTIONS_HPP_

#include <exception>
#include <string>
#include <vector>

/* C++ comes with a standard class for exceptions (class exception)
 * we define below a couple of children with useful names
 * and these classes can also be initiated with an extra message
 * stored in the member std::string ex_info::info
 */

/* add an extra std::string info line to the std::exception class.
 * Inherit this class in the other (grand-)children...
 */
class ex_info : public std::exception {
public:
	virtual const char* what() const throw()=0;
	virtual ~ex_info() throw();
	const std::string makeMessage(const std::string & ) const;
protected:
	std::string info;
};

class ex_generalError : public ex_info {
public:
	ex_generalError(const std::string & info="") throw();
	virtual const char* what() const throw();
};

class ex_parseError : public ex_info {
public:
	ex_parseError(const std::string & info="") throw();
	virtual const char* what() const throw();
};

class ex_commandLineError : public ex_info {
public:
	ex_commandLineError(const std::string & info="") throw();
	virtual const char* what() const throw();
};

class ex_rngError : public ex_info {
public:
	ex_rngError(const std::string & info="") throw();
	virtual const char* what() const throw();
};

class ex_mathError : public ex_info {
public:
	ex_mathError(const std::string & info="") throw();
	virtual const char* what() const throw();
};


#endif /* EXCEPTIONS_HPP_ */
