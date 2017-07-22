/*
 * withinhost_state_class.cpp
 *
 *  Created on: Oct 26, 2012
 *      Author: chris
 */

#include "withinhost_state_class.hpp"

/******** methods for withinhost_state *********************/

withinhost_state::withinhost_state(pathogen* foundingVirus, phase* initialPhase, parstruct* allPars) {
	currentVirus = foundingVirus;
	currentPhase = initialPhase;

	this->allPars = allPars;

	// statistics
	spVLonsetAsymPhase = -(INFTY);
	virusEndAcutePhase = NULL;
	infectingVirus = new pathogen(*currentVirus);
	infectingVirus->clearEvents();
}

withinhost_state::~withinhost_state() {
	/*
	std::cout << "deleting withinhost_state "
				<< "infecting virus was: " << infectingVirus << "\n"
				<< "virus end acute phase was: " << virusEndAcutePhase << "\n"
				<< "current virus was: " << currentVirus << "\n";
	*/
	delete currentVirus;
	delete currentPhase;
	delete virusEndAcutePhase;
	delete infectingVirus;
}

ext_double withinhost_state::getDelta() {
	return currentPhase->calcDelta(currentVirus,allPars);
}

ext_double withinhost_state::getLambdaEsc() {
	return currentVirus->calcLambdaEsc(currentPhase);
}

ext_double withinhost_state::getLambdaRev() {
	return currentVirus->calcLambdaRev(currentPhase);
}

ext_double withinhost_state::getLambda() {
	return currentVirus->calcLambda(currentPhase);
}

ext_double withinhost_state::getBeta() {
	return currentVirus->calcBeta(currentPhase);
}

ext_double withinhost_state::getVL() {
	return currentVirus->calcSpVL();
}

ext_double withinhost_state::getLengthAsymPhase() {
	return currentPhase->getLengthAsymPhase();
}

void withinhost_state::assignSpVLonsetAsymPhase() {
	if ( currentPhase->isAsym() ) {
		spVLonsetAsymPhase = getVL();
	}
	else {
		std::cout << "WARNING: tried to set spVLonsetAsymPhase while not in ASYM_PHASE.\n";
		/* this does not catch all errors: namely if in the middle of the ASYM_PHASE
		 * TODO: make fool proof withinhost_state::assignSpVLonsetAsymPhase()
		 */
	}
}

void withinhost_state::assignVirusEndAcutePhase() {
	virusEndAcutePhase = new pathogen(*currentVirus);
	virusEndAcutePhase->clearEvents();
}

pathogen* withinhost_state::getVirusEndAcutePhase() {
	return virusEndAcutePhase;
}

pathogen* withinhost_state::getInfectingVirus() {
	return infectingVirus;
}


ext_double withinhost_state::getSpVLonsetAsymPhase() {
	return spVLonsetAsymPhase;
}

statesummary* withinhost_state::makeStatesummary() {
	//std::cout << "withinhost_state::makeStatesummary()\n";

	ext_double beta = getBeta();
	/* the VL variable that is passed to the statesummary is used
	 * for calculating the cumulative virus load over time. This is
	 * then averaged, which gives the set-point virus load.
	 * Therefore, only write positive VL's to the summary
	 * if in ASYM_PHASE.
	 */
	ext_double VL = ( currentPhase->isAsym() ? getVL() : -(INFTY) );

	pathogen* copyOfVirus = new pathogen(*currentVirus);
	copyOfVirus->clearEvents(); // otherwise virus will point at deleted events...
	statesummary* newSummary = new withinhost_statesummary(time,
			beta, VL, copyOfVirus);
	return newSummary;
}

