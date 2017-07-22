/*
 * singlehostsim_class.hpp
 *
 *  Created on: Nov 14, 2012
 *      Author: chris
 */

#ifndef SINGLEHOSTSIM_CLASS_HPP_
#define SINGLEHOSTSIM_CLASS_HPP_

#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <ostream>
#include "host_class.hpp"
#include "parameters.hpp"
#include "frasers_rates.hpp" /* for setting the parameters of Hannekes functions */
#include "jama125/jama_eig.h" /* matrices and eigenspaces */
#include "RngClass.hpp"


struct hostOffspringStruct {
	hostOffspringStruct();
	hostOffspringStruct(host* , std::list<pathogen*> );
	host* sampledHost;
	std::list<pathogen*> offspringList;
	int counter; /* for counting the cumulative number of offspring for sampling */
};

struct mapWrapper { /* just a map, but with a print function so that it can be put into an ostream */
	std::map< std::string, ext_double > map;
	void print(std::ostream & ) const;
};

std::ostream & operator<<(std::ostream & , const mapWrapper & );


// TODO: singlehostsim -> put part of GEN method here (the convergence...)
class singlehostsim {
public:
	singlehostsim(parstruct* );
	~singlehostsim();
	void makeNGM(); /* make a NGM */
	void WHEDsamples(); /* from a single virus */
	void WHEDsamplesFromDist(); /* from the distribution calculated with NGM */
	void WHEDsamplesFromSamples(); /* from a previous sample */
	void computeEigs(); /* compute eigenspace of NGM */
	void print(std::ostream & ) const;
	void printNGM(std::ostream & ) const;
	void printTCs(std::ostream & ) const; /* for heritability analysis in R */
	void computeStatistics(); /* do statistics on the sample */
	mapWrapper getStatistics() const; /* return a map with statistics */
protected:
	std::list<pathogen*> getOffspring(host* ); /* do a mini-sellke simulation with one host */
	parstruct* allParams;
	int NGMsize;
	TNT::Array2D<double> NGM;
	TNT::Array1D<double> domEigenvecNGM;
	TNT::Array1D<double> cumulativeDist;
	double R0;
	std::vector< hostOffspringStruct > sample;
	mapWrapper statistics;

	bool didWHEDsamples;
	bool didWHEDsamplesFromDist;
	bool didWHEDsamplesFromSamples;
	bool madeNGM;
	bool computedEigs;
	bool computedStatsitics;
};

std::ostream & operator<<(std::ostream & , const singlehostsim & );


#endif /* SINGLEHOSTSIM_CLASS_HPP_ */
