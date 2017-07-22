/*
 * poplevel_history_class.hpp
 *
 *  Created on: Oct 31, 2012
 *      Author: chris
 */

#ifndef POPLEVEL_HISTORY_CLASS_HPP_
#define POPLEVEL_HISTORY_CLASS_HPP_

#define CALC_HERIT_INTERVAL 1.0 // 1 year TODO: move to parameter file (?)

#include <string>
#include <ctime>
#include <stdio.h> // for ETA

#include "history_class.hpp"
#include "poplevel_statesummary_class.hpp"
#include "parameters.hpp"
#include "calc_stats.hpp" // for calculating heritability and the triple class

class poplevel_history: public history {
public:
	poplevel_history(parstruct* );
	~poplevel_history();
	void push_back(statesummary* );
protected:
	std::ofstream fileLink;
	ext_double nextSampleTime;
	ext_double sample_constant;
	int next_echo_time;
	int maxtime;
	bool echo_bool;
	time_t start_clock_time;
	std::list< triple > heritlist; // (t,V_trans,V_rec)
	std::list< triple > heritlistNoMuts; // (t,e+f_trans,e+f_rec)
};

#endif /* POPLEVEL_HISTORY_CLASS_HPP_ */
