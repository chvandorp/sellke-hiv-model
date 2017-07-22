/*
 * RngClass.hpp
 *
 *  Created on: Aug 4, 2014
 *      Author: chris
 */

#ifndef RNGCLASS_HPP_
#define RNGCLASS_HPP_

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <cmath>

#include "exceptions.hpp"

class RngClass {
public:
	RngClass();
	RngClass(unsigned long );
	void seed(unsigned long ); // seed with 32 bit integer
	virtual ~RngClass(); // destructor
// distributions
	unsigned long Integer();
	unsigned long Integer(unsigned long ); // modulo max
	double Uniform(); // uniform(0,1)
	double Uniform(double , double ); // Uniform(a,b)
	double Normal(double , double ); // mean and sd
	double SkewNormal(double , double , double ); // mu, sigma and alpha
	double Exponential(double );
	double Weibull(double , double ); // scale, shape
	double Erlang(double , unsigned ); // simple Erlang(rate,shape) (sum of exponentials)
	double Gamma(double , double ); // Gamma(scale,shape)
	double Beta(double , double ); // Beta(alpha,beta)
	double BetaMnSd(double , double ); // Beta, but with mean and sd as parameterization
	int Binomial(int , double ); // Binom(n,p)
	int PosBinomial(int , double); // Binomial, conditioned on > 0
	int Poisson(double ); // Poisson(lambda) with mean lambda
	int Hypergeometric(int , int , int ); // Hyper(n, K, N) (N = population size, K = successes, n = draws)
	void Shuffle(int* , int , int ); // a random sample (unique)
private:
	gsl_rng* gslRng;
	void init();
	// make copy constructor and assignment operator unavailable, they don't make sense
	RngClass(const RngClass & ); // copy constructor not defined
	void operator=(const RngClass & ); // assignment operator not defined
};

extern RngClass RNG;


#endif /* RNGCLASS_HPP_ */
