/*
 * frasers_rates.cpp
 *
 *  Created on: Jul 27, 2012
 *      Author: chris
 */

#include"frasers_rates.hpp"

double frasersBeta(double logV, parstruct* pars) { // beta(log v)
	double power1 = pow(10.0,logV*pars->FRASERS_BETAK);
	static double power2 = pow(pars->FRASERS_BETA50,pars->FRASERS_BETAK);
	double beta = 0;
	if ( power1 == power1 ) // test if not a NaN
		beta = pars->FRASERS_BETAMAX * power1 / (power1 + power2);
	else
		beta = pars->FRASERS_BETAMAX;
	return beta;
}

double frasersDelta(double logV, parstruct* pars) { // 1/D(log v)
	double power1 = pow(10.0,logV*pars->FRASERS_DK);
	static double power2 = pow(pars->FRASERS_D50,pars->FRASERS_DK);
	double delta = 0;
	if ( power1 == power1 ) //test if not a NaN
		delta = ( power1 + power2 ) / ( pars->FRASERS_DMAX * power2 + pars->FRASERS_DMIN * power1 );
	else
		delta = pars->FRASERS_DMIN;
	return delta;
}
