/*
 * calc_stats.cpp
 *
 *  Created on: Jul 4, 2013
 *      Author: chris
 */

#include "calc_stats.hpp"


triple::triple(ext_double fst, ext_double snd, ext_double trd) : fst(fst), snd(snd), trd(trd) {
	// empty!!!
}

fstLeqT::fstLeqT(const ext_double & t) : t(t) {
	//empty!
}

bool fstLeqT::operator()(const triple & x) {
	return ( x.fst < t );
}

/* constructor for the heritability list */
correlation::correlation(const std::list<triple> & heritlist) :
		slope(0.0), intercept(0.0), R2(0.0), N((double)heritlist.size()),
		mean(0.0), var(0.0), median(0.0), top_percentile(0.0), bottom_percentile(0.0)
{
	if ( heritlist.size() > 2 ) {
		ext_double Ex(0.0); ext_double Ey(0.0);
		ext_double Exx(0.0); ext_double Eyy(0.0);
		ext_double Exy(0.0);

		ext_double SSerr(0.0); ext_double SStot(0.0);

		for ( std::list<triple>::const_iterator it = heritlist.begin(); it != heritlist.end(); it++ ) {
			Ex += it->snd; Ey += it->trd;
			Exx += it->snd * it->snd; Eyy += it->trd * it->trd;
			Exy += it->snd * it->trd;

			/* add spVLs to the percentileList */
			percentileList.push_back(it->snd); percentileList.push_back(it->trd);
		}
		Ex /= N; Ey /= N; Exx /= N; Eyy /= N; Exy /= N;

		SStot = Eyy - Ey * Ey;
		// the linear model for virus load y = slope * x + intercept + NORMAL
		slope = (Exy - Ex * Ey) / (Exx - Ex * Ex);
		intercept = Ey - slope * Ex;
		for ( std::list<triple>::const_iterator it = heritlist.begin(); it != heritlist.end(); it++ ) {
			ext_double err = it->trd - (slope * it->snd + intercept);
			SSerr += err * err;
		}
		SSerr /= N;
		R2 = ext_double(1.0) - SSerr/SStot;

		mean = ext_double(0.5) * (Ex + Ey); // more smooth result
		var = ext_double(0.5) * (Exx + Eyy) - ext_double(0.25)*(Ex + Ey)*(Ex + Ey); // more smooth result
	}
	else {
		slope = NANUM; R2 = NANUM; intercept = NANUM; N = NANUM; mean = NANUM; var = NANUM;
	}
	calcPercentiles();
}

ext_double correlation::get_slope() const { return slope; }
ext_double correlation::get_R2() const { return R2; }
ext_double correlation::get_intercept() const { return intercept; }
ext_double correlation::get_N() const { return N; }
ext_double correlation::get_mean() const { return mean; }
ext_double correlation::get_var() const { return var; }
ext_double correlation::get_median() const { return median; }
ext_double correlation::get_top_percentile() const { return top_percentile; }
ext_double correlation::get_bottom_percentile() const { return bottom_percentile; }


void correlation::calcPercentiles() {
	if ( !percentileList.empty() ) {
		percentileList.sort();
		int n = percentileList.size(); int i = 0;
		std::list<ext_double>::iterator it = percentileList.begin();
		int locationBottom = int(n * 0.025);
		int locationTop = int(n * 0.975);
		int locationMedian = int(n * 0.5);
		for ( ; i < locationBottom; i++ ) it++;
		bottom_percentile = *it;
		for ( ; i < locationMedian; i++ ) it++;
		median = *it;
		for ( ; i < locationTop; i++ ) it++;
		top_percentile = *it;
	}
	else {
		median = NANUM; top_percentile = NANUM; bottom_percentile = NANUM;
	}
}

