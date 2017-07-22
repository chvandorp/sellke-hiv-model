/*
 * phase_class.cpp
 *
 *  Created on: Oct 26, 2012
 *      Author: chris
 */

#include "phase_class.hpp"

/*********** methods for phase: public individual **************************/

phase::phase(phase::phase_type initialPhaseType) {
	currentPhaseType = initialPhaseType;

	// initialize the statistics
	timeEndAcutePhase = INFTY;
	timeEndAsymPhase = INFTY;
	timeEndAIDSPhase = INFTY;
}
phase::~phase() {
	//std::cout << "deleting phase\n"; // testing
}
void phase::changePhase(ext_double time) {
	switch ( currentPhaseType ) {
		case SUSC_PHASE:
			currentPhaseType = ACUTE_PHASE;
			std::cout << "WARNING: changing SUSC_PHASE into ACUTE_PHASE, but this has not "
					  << "been implemented yet.\n";
			break;
		case ACUTE_PHASE:
			currentPhaseType = ASYM_PHASE;
			timeEndAcutePhase = time;
			break;
		case ASYM_PHASE:
			currentPhaseType = AIDS_PHASE;
			timeEndAsymPhase = time;
			break;
		case AIDS_PHASE:
			currentPhaseType = REMOVED_PHASE;
			timeEndAIDSPhase = time;
			break;
		case REMOVED_PHASE:
			std::cout << "WARNING: attempt to increase the phase_type REMOVED_PHASE.\n";
			break;
		default:
			std::cout << "WARNING: unknown phase type during call of phase::changePhase.\n";
			break;
	} // switch ( currentPhaseType )
}

bool phase::isRemoved() { return currentPhaseType == REMOVED_PHASE; }
bool phase::isAsym() { return currentPhaseType == ASYM_PHASE; }

ext_double phase::calcDelta(pathogen* currentVirus, parstruct* allPars) {
	switch ( currentPhaseType ) {
	case ACUTE_PHASE:
		return ext_double(1/allPars->FRASERS_D1); // TODO: pre-calculate commonly used rate parameters
	case ASYM_PHASE:
		return ext_double(frasersDelta(currentVirus->calcSpVL(),allPars));
	case AIDS_PHASE:
		return ext_double(1/allPars->FRASERS_D3);
	default:
		return ext_double(0.0);
	}
}
phase::phase_type phase::getPhaseType() const { return currentPhaseType; }

ext_double phase::getLengthAsymPhase() const {
	return timeEndAsymPhase - timeEndAcutePhase;
}
