/*
 * filereader_class.hpp
 *
 *  Created on: Nov 7, 2012
 *      Author: chris
 */

#ifndef FILEREADER_CLASS_HPP_
#define FILEREADER_CLASS_HPP_

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include "ext_double_class.hpp"

#define COMMENT_DELIM '#'

typedef std::pair< std::pair<double,double>, int > range;

class filereader {
public:
	filereader(std::string );
	double getParameterDouble(std::string );
	int getParameterInt(std::string );
	std::string getParameterString(std::string );
	range getParameterRange(std::string );
	void writeNumbersToFile(std::string );
protected:
	std::map<std::string,double> doubleParameterMap;
	std::map<std::string,int> intParameterMap;
	std::map<std::string,range> rangeParameterMap;
	std::map<std::string,std::string> stringParameterMap;
	enum flags { invalidFlag=0, dFlag, iFlag, sFlag, rFlag };
};



#endif /* FILEREADER_CLASS_HPP_ */
