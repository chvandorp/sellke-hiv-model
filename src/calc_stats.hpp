/*
 * calc_stats.hpp
 *
 *  Created on: Jul 4, 2013
 *      Author: chris
 */

#ifndef CALC_STATS_HPP_
#define CALC_STATS_HPP_

#include <list>
#include <utility>
#include "ext_double_class.hpp"

class triple {
public:
	triple(ext_double, ext_double, ext_double);
	ext_double fst;
	ext_double snd;
	ext_double trd;
};

struct fstLeqT {
	fstLeqT(const ext_double & );
	bool operator()(const triple & );
	ext_double t;
};


class correlation {
public:
	correlation(const std::list< triple > & );
	correlation(const std::list< ext_double > & );
	ext_double get_slope() const;
	ext_double get_R2() const;
	ext_double get_intercept() const;
	ext_double get_N() const;
	ext_double get_mean() const;
	ext_double get_var() const;
	ext_double get_median() const;
	ext_double get_top_percentile() const;
	ext_double get_bottom_percentile() const;
private:
	void calcPercentiles(); // median and percentiles
	std::list<ext_double> percentileList; // for calculating the percentiles and median

	ext_double slope;
	ext_double intercept;
	ext_double R2;
	ext_double N;
	ext_double mean;
	ext_double var;
	ext_double median;
	ext_double top_percentile;
	ext_double bottom_percentile;
};

#endif /* CALC_STATS_HPP_ */
