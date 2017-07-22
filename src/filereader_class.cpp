/*
 * filereader_class.cpp
 *
 *  Created on: Nov 7, 2012
 *      Author: chris
 */

#include "filereader_class.hpp"

filereader::filereader(std::string fileName) {
	/* set the flag map */
	std::map<std::string,flags> flagMap;
	flagMap["-d"] = dFlag;
	flagMap["-i"] = iFlag;
	flagMap["-s"] = sFlag;
	flagMap["-r"] = rFlag;

	std::ifstream fileStream;
	fileStream.open(fileName.c_str());
	if ( fileStream ) {
		/* a char* to write to */
		char line[256];
		/* loop through the file */
		while ( fileStream.good() ) {
			/* get the name of the parameter */
			fileStream.getline(line,256,'\n');
			std::string lineString(line);
			//std::cout << "filereader reads line: " << lineString << std::endl;
			/* test if the parameter has a valid name
			 * i.e. not a comment or an empty line
			 */
			if ( !lineString.empty() ) {
				if ( (lineString.c_str())[0] != COMMENT_DELIM ) {
					/* put the lineString into an istringstream */
					std::istringstream lineStream(lineString);

					/* get the name of the parameter*/
					char name[256];
					lineStream.getline(name,256,'\t');
					std::string paramname(name);
					//std::cout << "filereader reads name: " << paramname << std::endl;


					/* get the value of the parameter */
					/* but first get the flag to decide on the type */
					char flag[256];
					do {
						lineStream.getline(flag,256,'\t');
					} while ( std::string(flag).empty() );
					//std::cout << "flag = " << std::string(flag) << ".\n";
					switch ( flagMap[std::string(flag)] ) {
						case dFlag: {
							double paramvalue(0.0);
							char value[256];
							lineStream.getline(value,256,'\n');
							std::istringstream(std::string(value)) >> paramvalue;
							doubleParameterMap[paramname] = paramvalue;
							break;
						}
						case iFlag: {
							int paramvalue(0);
							char value[256];
							lineStream.getline(value,256,'\n');
							std::istringstream(std::string(value)) >> paramvalue;
							intParameterMap[paramname] = paramvalue;
							break;
						}
						case sFlag: {
							char value[256];
							lineStream.getline(value,256,'\n');
							stringParameterMap[paramname] = std::string(value);
							break;
						}
						case rFlag: {
							double paramvalue1(0); double paramvalue2(0); int paramvalue3(1);
							char value1[256]; char value2[256]; char value3[256];
							lineStream.getline(value1,256,','); /* minimum value */
							std::istringstream(std::string(value1)) >> paramvalue1;
							lineStream.getline(value2,256,';'); /* maximum value */
							std::istringstream(std::string(value2)) >> paramvalue2;
							lineStream.getline(value3,256,'\n'); /* number of points */
							std::istringstream(std::string(value3)) >> paramvalue3;
							/* make the range object */
							range paramvalue;
							paramvalue.first.first = paramvalue1;
							paramvalue.first.second = paramvalue2;
							paramvalue.second = paramvalue3;
							/* put the paramvalue in the map */
							rangeParameterMap[paramname] = paramvalue;
							break;
						}
						case invalidFlag: {
							std::cout << "WARNING: parameter file contains invalid flag: '"
									  << std::string(flag) << "'\n";
							break;
						}
					}

					//std::cout.precision(10);
					//std::cout << "filereader reads value: " << paramvalue << std::endl;

					/* put name and value in the map */
				}
			}
		}
		/* close the file */
		fileStream.close();
	}
	else { /* the file does not exist or is not readable */
		std::cout << "WARNING: can't open input file.\n";
	}
}

double filereader::getParameterDouble(std::string parname) {
	std::map<std::string,double>::iterator pairIt = doubleParameterMap.find(parname);
	if ( pairIt == doubleParameterMap.end() ) {
		std::cout << "WARNING: requested parameter name '"
				  << parname << "' was not in file.\n";
		return 0.0;
	}
	else return doubleParameterMap[parname];
}

int filereader::getParameterInt(std::string parname) {
	std::map<std::string,int>::iterator pairIt = intParameterMap.find(parname);
	if ( pairIt == intParameterMap.end() ) {
		std::cout << "WARNING: requested parameter name '"
				  << parname << "' was not in file.\n";
		return 0;
	}
	else return intParameterMap[parname];
}

std::string filereader::getParameterString(std::string parname) {
	std::map<std::string,std::string>::iterator pairIt = stringParameterMap.find(parname);
	if ( pairIt == stringParameterMap.end() ) {
		std::cout << "WARNING: requested parameter name '"
				  << parname << "' was not in file.\n";
		return "";
	}
	else return stringParameterMap[parname];
}

range filereader::getParameterRange(std::string parname) {
	std::map<std::string,range>::iterator pairIt = rangeParameterMap.find(parname);
	if ( pairIt == rangeParameterMap.end() ) {
		std::cout << "WARNING: requested parameter name '"
				  << parname << "' was not in file.\n";
		range dummyrange;
		dummyrange.first.first = 0.0;
		dummyrange.first.second = 0.0;
		dummyrange.second = 1;
		return dummyrange;
	}
	else return rangeParameterMap[parname];
}


void filereader::writeNumbersToFile(std::string filename) {
	std::ofstream filestream(filename.c_str());
	if ( filestream.good() ) {
		filestream << "<parameters ";
		for ( std::map<std::string,double>::iterator i = doubleParameterMap.begin();
				i != doubleParameterMap.end(); i++ ) {
			filestream << i->first << "='" << i->second << "' ";
		}
		for ( std::map<std::string,int>::iterator i = intParameterMap.begin();
				i != intParameterMap.end(); i++ ) {
			filestream << i->first << "='" << i->second << "' ";
		}
		filestream << "/>\n";
		filestream.close();
	}
	else {
		std::cout << "WARNING: could not open output file to write parameters.\n";
	}
}


