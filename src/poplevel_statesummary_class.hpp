/*
 * poplevel_statesummary_class.hpp
 *
 *  Created on: Oct 31, 2012
 *      Author: chris
 */

#ifndef POPLEVEL_STATESUMMARY_CLASS_HPP_
#define POPLEVEL_STATESUMMARY_CLASS_HPP_

#include "statesummary_class.hpp"
#include "ext_double_class.hpp"
#include "pathogen_class.hpp"
#include "calc_stats.hpp" // for reading the contents of a correlation object
#include <iostream>
#include <map>
#include <string>

class poplevel_statesummary : public statesummary {
public:
	poplevel_statesummary(ext_double , int , int , // time, prevalence, popsize
			ext_double , ext_double ,
			ext_double , ext_double ,
			pathogen* , pathogen* );
	poplevel_statesummary(ext_double , int , int );
	~poplevel_statesummary();
	void writeToFile(std::ostream & );
	std::pair<ext_double,ext_double> getSpVLpair();
	std::pair<ext_double,ext_double> getNoMutspair();
	void addRunningStats(const correlation & , const correlation & , ext_double );
	void initStatsMap();
protected:
	int prevalence, popsize;
	ext_double spVLtrans, spVLrec;
	ext_double kTrans, kRec;
	ext_double numberOfMutationsTrans, numberOfMutationsRec;
	ext_double dRec, dTrans;
	pathogen* virusRec; pathogen* virusTrans;

	/* running stats (added by history before write to file)
	 * TODO: put all the stats in a map...
	 */

	std::map< std::string, ext_double > statsMap;
};



#endif /* POPLEVEL_STATESUMMARY_CLASS_HPP_ */
