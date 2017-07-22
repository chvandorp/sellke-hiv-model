/*
 * withinhost_state_class.hpp
 *
 *  Created on: Oct 26, 2012
 *      Author: chris
 */

#ifndef WITHINHOST_STATE_CLASS_HPP_
#define WITHINHOST_STATE_CLASS_HPP_

#include "state_class.hpp"
#include "withinhost_statesummary_class.hpp"
#include "ext_double_class.hpp"
class pathogen;
#include "pathogen_class.hpp"
class phase;
#include "phase_class.hpp"

class withinhost_state: public state {
public:
	withinhost_state(pathogen* , phase* , parstruct* ); // complete!
	~withinhost_state(); // deletes currentVirus, virusEndAcutePhase and currentPhase
	statesummary* makeStatesummary();
	ext_double getBeta();
	ext_double getVL();
	ext_double getDelta();
	ext_double getLambdaEsc();
	ext_double getLambdaRev();
	ext_double getLambda();

	/* methods that are used for statistics
	 * ONLY CALL THESE FUNCTIONS AFTER host::run()
	 */
	ext_double getLengthAsymPhase(); // calls currentPhase->getLengthAsymPhase()
	void assignSpVLonsetAsymPhase();
	void assignVirusEndAcutePhase();
	ext_double getSpVLonsetAsymPhase();
	pathogen* getVirusEndAcutePhase(); // DOES NOT MAKE A HARD COPY!!!
	pathogen* getInfectingVirus(); // DOES NOT MAKE A HARD COPY!!!
protected:
	pathogen* currentVirus;
	phase* currentPhase;

	parstruct* allPars;

	/* only for statistics.. not really the place to keep this
	 * but this will be a lot faster...
	 */
	ext_double spVLonsetAsymPhase;
	pathogen* virusEndAcutePhase;
	pathogen* infectingVirus;
};



#endif /* WITHINHOST_STATE_CLASS_HPP_ */
