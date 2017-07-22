/*
 * host_class.hpp
 *
 *  Created on: Oct 25, 2012
 *      Author: chris
 */

#ifndef HOST_CLASS_HPP_
#define HOST_CLASS_HPP_

#include "individual_class.hpp"
#include "sellke_class.hpp"
#include "ext_double_class.hpp"
#include "RngClass.hpp" // for choosing k
#include "pathogen_class.hpp"
#include "phase_class.hpp"
#include "withinhost_state_class.hpp"
#include "withinhost_history_class.hpp"
#include "hostlevel_events.hpp"
#include "frasers_rates.hpp" // for choosing k (contains definitions)
#include "parameters.hpp" // a struct containing all parameters


class host: public individual, public sellke {
public:
	host(pathogen* , ext_double , parstruct*);
	~host();
	/* get the EWT of the next transmission event to a given
	 * start time (global) (first argument) and a required increase
	 * of load (second argument)
	 */
	ext_double calcEWTofTransmission(ext_double , ext_double); // complete
	/* get the increase in load (for a virgin population) for
	 * transmission events if we start at (global time) (first argument)
	 * and integrate over an interval of length (second argument)
	 */
	ext_double calcIntegralBeta(ext_double , ext_double); // complete

	pathogen* getPathogen(ext_double ); // complete (get the pathogen at absolute time t)
	ext_double getTimeOfDeath(); // complete (relative)
	ext_double getTimeOfInfection(); // complete (absolute)
	ext_double getSpVLonsetAsymPhase(); // complete
	pathogen* getVirusEndAcutePhase(); // complete (NB: DOES NOT MAKE A HARD COPY)
	pathogen* getInfectingVirus(); // (NB: does not make a hard copy)
	ext_double getSpVLtimeAverage();
	ext_double getTotalBeta();
	int getK();
	void print(std::ostream & os) const;
protected:
	int chooseImmuneResp();
	ext_double getVL(ext_double ); // get the virus load at a certain point in time...

	ext_double timeOfInfection; // absolute!!
	ext_double timeOfDeath; // relative!!

	ext_double spVLonsetAsymPhase;
	ext_double spVLtimeAverage;
	ext_double totalBeta;
	pathogen* pointerToInfectingVirus;
	pathogen* pointerToVirusEndAcutePhase;

	int k; // maximal number of immune responses

	parstruct* allPars;
};

std::ostream & operator<<(std::ostream & , const host* );



#endif /* HOST_CLASS_HPP_ */
