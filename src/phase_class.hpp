/*
 * phase_class.hpp
 *
 *  Created on: Oct 26, 2012
 *      Author: chris
 */

#ifndef PHASE_CLASS_HPP_
#define PHASE_CLASS_HPP_

#include "individual_class.hpp"
class pathogen;
#include "pathogen_class.hpp"
#include "parameters.hpp"

class phase: public individual {
public:
	enum phase_type {
		SUSC_PHASE,
		ACUTE_PHASE,
		ASYM_PHASE,
		AIDS_PHASE,
		REMOVED_PHASE
	};
	phase(phase_type ); // complete!
	~phase(); // testing...
	/* the phasechange_event calls phase::changePhase(ext_double )
	 * and passes the time of the event. The phase remembers the time of
	 * the phase changes so that phase::getLengthAsymPhase() can return
	 * the length of the asymptomatic phase.
	 */
	void changePhase(ext_double ); // complete!
	bool isRemoved(); // complete!
	bool isAsym(); // returns true when in asymptomatic phase.
	phase_type getPhaseType() const; // complete!
	ext_double calcDelta(pathogen* , parstruct* ); // complete!
	ext_double getLengthAsymPhase() const; // complete
private:
	phase_type currentPhaseType;

	/* the following members are used for gathering statistics
	 * The event can only update the state. These statistics
	 * will be set at a certain time point and remembered for
	 * future use
	 */
	ext_double timeEndAcutePhase;
	ext_double timeEndAsymPhase;
	ext_double timeEndAIDSPhase;
};


#endif /* PHASE_CLASS_HPP_ */
