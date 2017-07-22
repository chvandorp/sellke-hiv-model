/*
 * host_class.cpp
 *
 *  Created on: Oct 25, 2012
 *      Author: chris
 */

#include"host_class.hpp"

host::host(pathogen* foundingVirus, ext_double timeOfInfection, parstruct* allPars) {
	// set the time of infection (Absolute!!!!)
	this->timeOfInfection = timeOfInfection;
	this->allPars = allPars;

	spVLonsetAsymPhase = -(INFTY);
	spVLtimeAverage = -(INFTY);
	totalBeta = ext_double(0.0);

	// choose a host phenotype
	k = chooseImmuneResp();

	// choose the initial pathogen phenotype
	foundingVirus->doMutationAtInfection(k);

	// initialize the state
	phase* initialPhase = new phase(phase::ACUTE_PHASE);
	currentState = new withinhost_state(foundingVirus, initialPhase, allPars);

	// initiate the eventList
	event* firstPhaseChangeEvent = new phasechange_event(initialPhase, allPars);
	event* firstEscMutationEvent = new escmutation_event(foundingVirus);
	event* firstRevMutationEvent = new revmutation_event(foundingVirus);
	eventList.push_back(firstPhaseChangeEvent);
	eventList.push_back(firstEscMutationEvent);
	eventList.push_back(firstRevMutationEvent);

	// tell individuals about their events
	initialPhase->addEvent(firstPhaseChangeEvent);
	foundingVirus->addEvent(firstEscMutationEvent);
	foundingVirus->addEvent(firstRevMutationEvent);

	// init the history
	oldStates = new withinhost_history;

	// the host is only binded to events, so we must set
	deleteByEventBool = true;
	// to avoid memory leaks

	// run within-host simulation!
	run();

	// set time of death (relative!!!)
	timeOfDeath = (((withinhost_history*) oldStates)->getFinalStatesummary())->getTime();
	// calculate the mean set-point virus load (log10 scale)
	getSpVLonsetAsymPhase(); // TODO turned off for now: expensive function. Improve this
	getSpVLtimeAverage(); // uses memory of currentState->phase and cumulative VLs in oldStates
	getTotalBeta();
	pointerToVirusEndAcutePhase = ((withinhost_state*) currentState)->getVirusEndAcutePhase();
	pointerToInfectingVirus = ((withinhost_state*) currentState)->getInfectingVirus();
}

host::~host() {
	// std::cout << "a host dies\n";
	// TODO: do something smart here...
}

pathogen* host::getPathogen(ext_double absoluteTime) {
	// calculate the relative time
	ext_double relativeTime = absoluteTime - timeOfInfection;
	// correct for possible errors (TODO: warning)
	if ( relativeTime <= ext_double(0.0) ) relativeTime = ext_double(0.0);
	// get the right state summary
	withinhost_statesummary* statsum = ((withinhost_history*) oldStates)->
			getStatesummary(relativeTime, &withinhost_statesummary::getTime);
	if ( statsum ) return statsum->getPathogen();
	else return NULL;
}


ext_double host::calcEWTofTransmission(ext_double absoluteTime, ext_double relativeThreshold) {
	// calculate the relative time
	//std::cout << "time of infection: " << timeOfInfection << std::endl;
	ext_double relativeTime = absoluteTime - timeOfInfection;
	// correct for possible errors (TODO: warning)
	if ( relativeTime < ext_double(0.0) ) {
		std::cout << "WARNING: negative relative time\n";
		relativeTime = ext_double(0.0);
	}
	// get the right state summary (where we start)
	withinhost_statesummary* statsum0 = ((withinhost_history*) oldStates)->
			getStatesummary(relativeTime, &withinhost_statesummary::getTime);
	if ( statsum0 == NULL ) {
		std::cout << "WARNING: in host::calcEWTofTransmission: getStateSummary returned NULL (1)\n";
		return INFTY;
	}
	// correct the threshold for the starting time
	ext_double absoluteThreshold = relativeThreshold + statsum0->getCumulativeBeta() +
			statsum0->getBeta() * (relativeTime - statsum0->getTime());
	// now find the right summary where the threshold is reached
	withinhost_statesummary* statsum1 = ((withinhost_history*) oldStates)->
			getStatesummary(absoluteThreshold, &withinhost_statesummary::getCumulativeBeta);
	if ( statsum1 == NULL ) {
		std::cout << "WARNING: in host::calcEWTofTransmission: getStateSummary returned NULL (2)\n";
		return INFTY;
	}
	// now find the right time point...
	ext_double beta = statsum1->getBeta();
	if ( beta < ext_double(0.0) ) {
		std::cout << "WARNING: in host::calcEWTofTransmission: negative beta " << beta << "\n";
		return INFTY; // this will almost never happen
	}
	else {
		return ( statsum1->getTime() - relativeTime +
				(absoluteThreshold - statsum1->getCumulativeBeta()) / beta );
	}
}


ext_double host::calcIntegralBeta(ext_double globalTime, ext_double lengthTimeInterval) {
	ext_double localTime0 = globalTime - timeOfInfection;
	ext_double localTime1 = localTime0 + lengthTimeInterval;

	withinhost_statesummary* statsum0 = ((withinhost_history*) oldStates)->
			getStatesummary(localTime0,&withinhost_statesummary::getTime);
	withinhost_statesummary* statsum1 = ((withinhost_history*) oldStates)->
				getStatesummary(localTime1,&withinhost_statesummary::getTime);
	if ( statsum0 == NULL || statsum1 == NULL ) {
		std::cout << "WARNING: in host::calcIntegralBeta: getStatesummary returned NULL\n";
		return ext_double(0.0);
	}
	else {
		ext_double cBeta0 = statsum0->getCumulativeBeta()
				+ statsum0->getBeta() * ( localTime0 - statsum0->getTime() );
		ext_double cBeta1 = statsum1->getCumulativeBeta()
						+ statsum1->getBeta() * ( localTime1 - statsum1->getTime() );
		return ( cBeta1 - cBeta0 );
	}
}

ext_double host::getTimeOfDeath() { return timeOfDeath; }
ext_double host::getTimeOfInfection() { return timeOfInfection; }

ext_double host::getSpVLonsetAsymPhase() {
	if ( spVLonsetAsymPhase == -(INFTY) ) {
		spVLonsetAsymPhase = ((withinhost_state*) currentState)->getSpVLonsetAsymPhase();
	}
	return spVLonsetAsymPhase;
}

pathogen* host::getVirusEndAcutePhase() { return pointerToVirusEndAcutePhase; }
pathogen* host::getInfectingVirus() { return pointerToInfectingVirus; }


ext_double host::getVL(ext_double localTime) {
	pathogen* oldVirus = getPathogen(localTime + timeOfInfection);
	if ( oldVirus !=  NULL ) {
		return ext_double(oldVirus->calcSpVL());
	}
	else {
		std::cout << "WARNING: no virus present at the time of VL request\n";
		return -(INFTY); // log(0)
	}
}

ext_double host::getSpVLtimeAverage() {
	if ( spVLtimeAverage == -(INFTY) ) {
		withinhost_statesummary* finStatsum = ((withinhost_history*) oldStates)->
				getFinalStatesummary();
		if ( finStatsum == NULL ) {
			std::cout << "WARING: getFinalStateSummary returned NULL\n";
			return -(INFTY);
		}
		spVLtimeAverage = log10( finStatsum->getCumulativeExpVL()
				/ ((withinhost_state*) currentState)->getLengthAsymPhase() );
	}
	return spVLtimeAverage;
}

ext_double host::getTotalBeta() {
	if ( totalBeta == ext_double(0.0) ) {
		withinhost_statesummary* finStatsum = ((withinhost_history*) oldStates)->
				getFinalStatesummary();
		if ( finStatsum == NULL ) {
			std::cout << "WARING: getFinalStateSummary returned NULL\n";
			return ext_double(0.0);
		}
		totalBeta = finStatsum->getCumulativeBeta();
	}
	return totalBeta;
}




int host::chooseImmuneResp() {
	int imresp = 0;
	if ( allPars->STANDARD_DEV_K <= 0 ) {
		imresp =  allPars->MEAN_K;
	}
	else {
		int kSloppy = allPars->MEAN_K;
		do {
			kSloppy = (int) ( RNG.Normal(allPars->MEAN_K,allPars->STANDARD_DEV_K) + 0.5);
		} while (kSloppy < 0 || kSloppy > allPars->PEPTIDES_N);
		imresp =  kSloppy;
	}
	return imresp;
}

int host::getK() {
	return k;
} // int host::chooseImmuneResp()

void host::print(std::ostream & os) const {
	os 	<< "<whed_simulation "
			<< "TOI='" << timeOfInfection << "' "
			<< "TOD='" << timeOfDeath << "' "
			<< "spVLmin='" << spVLonsetAsymPhase << "' "
			<< "spVLtav='" << spVLtimeAverage << "' "
			<< "k='" << k << "' "
		<< ">\n";
	os << ((withinhost_history*) oldStates);
	os << "</whed_simulation >";
}

std::ostream & operator<<(std::ostream & os, const host* wiske) {
	wiske->print(os);
	return os;
}



