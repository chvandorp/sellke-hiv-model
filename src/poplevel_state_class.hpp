/*
 * poplevel_state_class.hpp
 *
 *  Created on: Oct 25, 2012
 *      Author: chris
 */

#ifndef POPLEVEL_STATE_CLASS_HPP_
#define POPLEVEL_STATE_CLASS_HPP_

#include <vector>
#include "state_class.hpp"
#include "poplevel_statesummary_class.hpp"
#include "host_class.hpp"
#include "pathogen_class.hpp"
#include "parameters.hpp"
#include "RngClass.hpp"

class poplevel_state: public state {
public:
	poplevel_state(parstruct* );
	~poplevel_state();
	statesummary* makeStatesummary();
	void addInfected();
	void addSusceptible();
	void removeInfected();
	void removeSusceptible();
	ext_double getFractionSusceptibles() const;
	ext_double getDensitySusceptibles() const;
	ext_double getNumberSusceptibles() const;
	ext_double getPopulationSize() const;
	ext_double getNumberInfecteds() const;
	ext_double getBirthRate() const;
	ext_double getDeathRate() const;
	void addCouple(host* , host* );
protected:
	int S; // number of susceptibles
	int I; // total number of infecteds
	int S0; // population size in infection-free case

	host* currentTransmitter;
	host* currentReceiver;

	parstruct* allParams;
	ext_double birthRate;
	ext_double deathRate;
};




#endif /* POPLEVEL_STATE_CLASS_HPP_ */


/* TODO: note.
 * The population_state must somehow know the present virus loads
 * (and other statistics). In order to do this, I can save for
 * every event (maybe only the infection events) a set point virus
 * load (both sender and receiver for heritability :-)
 * Saving this for EVERY event takes a lot of time and memory,
 * but the transmission_event (infection_event??) class can be
 * given a static counter (\mod N) and only makes state::saveThisStateBool
 * true for counter \equiv 0 (\mod N)
 * this means that I have to process the data to get means (how??)
 *
 * among the given statistics, there can also be time of death
 * so that a set point virus load can participate to the mean for a
 * known length of time.
 */
