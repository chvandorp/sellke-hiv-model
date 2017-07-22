/*
 * pathogen_class.cpp
 *
 *  Created on: Oct 26, 2012
 *      Author: chris
 */

/*********** methods for pathogen: public individual ***********************/

#include "pathogen_class.hpp"

pathogen::pathogen(parstruct* allParams) {
	pars = allParams;
	k = allParams->MEAN_K;
	e = allParams->INITIAL_E;
	f = allParams->INITIAL_F;
	d = allParams->INITIAL_D;
}

pathogen::pathogen(parstruct* allParams, int e, int f) {
	pars = allParams;
	k = allParams->MEAN_K;
	this->e = e;
	this->f = f;
	d = 0;
}

pathogen::pathogen(parstruct* allParams, int e, int f, int d) {
	pars = allParams;
	k = allParams->MEAN_K;
	this->e = e;
	this->f = f;
	this->d = d;
}

pathogen::pathogen(const pathogen & virus) {
	pars = virus.pars;
	k = virus.k;
	e = virus.e;
	f = virus.f;
	d = virus.d;
}

const pathogen & pathogen::operator=(const pathogen & virus) {
	if ( &virus != this ) {
		pars = virus.pars;
		k = virus.k;
		e = virus.e;
		f = virus.f;
		d = virus.d;
	}
	return *this;
}

pathogen::~pathogen() {
	//std::cout << "deleting pathogen\n"; // testing
}

int pathogen::getE() const { return e; }
int pathogen::getF() const { return f; }
int pathogen::getD() const { return d; }
int pathogen::getK() const { return k; }
int pathogen::getNumberOfMutations() const { return getE() + getF(); } // TODO: weird name... ignores d

double pathogen::calcSpVL() const {
	if ( pars != NULL ) {
		double returnval = pars->VLMAX - (pars->SIGMA)*double(k-e) - (pars->PHI)*double(e+f+d);
		return returnval;
	}
	else {
		std::cout << "WARNING! (in pathogen::calcSpVL) pathogen::pars is a pointer to NULL\n";
		return 0.0;
	}
}

ext_double pathogen::calcLambdaEsc(phase* ph) {
	double lambda = pars->PESC; // the number of sites is handled by the constructor of mutation_event
	switch ( ph->getPhaseType() ) {
	case phase::ACUTE_PHASE: {
		return ext_double(lambda * pars->LAMBDA1);
	}
	case phase::ASYM_PHASE: {
		return ext_double(lambda * pars->LAMBDA2);
	}
	case phase::AIDS_PHASE: {
		return ext_double(lambda * pars->LAMBDA3);
	}
	default:
		return ext_double(0.0);
	}
}

ext_double pathogen::calcLambdaRev(phase* ph) {
	double lambda = pars->PREV; // the number of sites is handled by the constructor of mutation_event
	switch ( ph->getPhaseType() ) {
	case phase::ACUTE_PHASE: {
		return ext_double(lambda * pars->LAMBDA1);
	}
	case phase::ASYM_PHASE: {
		return ext_double(lambda * pars->LAMBDA2);
	}
	case phase::AIDS_PHASE: {
		return ext_double(lambda * pars->LAMBDA3);
	}
	default:
		return ext_double(0.0);
	}
}

ext_double pathogen::calcLambda(phase* ph) {
	switch ( ph->getPhaseType() ) {
		case phase::ACUTE_PHASE: {
			double lambda = pars->LAMBDA1;
			return ext_double(lambda);
		}
		case phase::ASYM_PHASE: {
			double lambda = pars->LAMBDA2;
			return ext_double(lambda);
		}
		case phase::AIDS_PHASE: {
			double lambda = pars->LAMBDA3;
			return ext_double(lambda);
		}
		default: {
			std::cout << "WARNING (in pathogen::calcLambda): unknown phase.\n";
			return ext_double(0.0);
		}
	}
}

ext_double pathogen::calcBeta(phase* ph) {
	switch ( ph->getPhaseType() ) {
	case phase::ACUTE_PHASE:
		return ext_double(pars->FRASERS_BETA1);
	case phase::ASYM_PHASE:
		return ext_double(frasersBeta(calcSpVL(),pars));
	case phase::AIDS_PHASE:
		return ext_double(pars->FRASERS_BETA3);
	default:
		return ext_double(0.0);
	}
}

void pathogen::doEscMutation() {
	if ( e < k ) e++;
}

void pathogen::doRevMutation() { // undo a deleterious mutation
	if ( f+d > 0 ) {
		if ( pars->NONEPITOPE_M == 0 ) { // the 'classical' case
			f--;
		}
		else { // a reversion can be in the epitope region (f--) or in the non-epitope region (d--)
			if ( d == 0 ) { f--; return; }
			if ( f == 0 ) { d--; return; }

			// else: choose a mutation according to chance
			double ratio = double(f)/double(f+d);
			double p = RNG.Uniform();
			if ( p < ratio ) f--;
			else d--;
		}
	}
}

void pathogen::doMutationAtInfection(int k) { // TODO maybe add extra features here...
	this->k = k;
	int numberOfMutations = e + f;
	e = RNG.Hypergeometric(k,numberOfMutations,pars->PEPTIDES_N);
	f = numberOfMutations - e;
}

void pathogen::print(std::ostream & os) const {
	os	<< "<virus"
		<< " e='" << e
		<< "' f='" << f
		<< "' d='" << d
		<< "' SpVL='" << calcSpVL()
		<< "' ImResp='" << k-e
		<< "' /> ";
}


std::ostream & operator<<(std::ostream & os, const pathogen* vir) {
	vir->print(os);
	return os;
}
