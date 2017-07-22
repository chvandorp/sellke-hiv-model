/*
 * frasers_rates.hpp
 *
 *  Created on: Jul 27, 2012
 *      Author: chris
 */

#ifndef FRASERS_RATES_HPP_
#define FRASERS_RATES_HPP_

#define DAYS_IN_YEAR 365.25

#include <cmath>
#include "parameters.hpp"

double frasersBeta(double , parstruct* ); // beta(log v)
double frasersDelta(double , parstruct* ); // 1/D(log v)

#endif /* FRASERS_RATES_HPP_ */
