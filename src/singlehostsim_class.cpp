/*
 * singlehostsim_class.cpp
 *
 *  Created on: Nov 14, 2012
 *      Author: chris
 */

#include "singlehostsim_class.hpp"

singlehostsim::singlehostsim(parstruct* allParams) {
	this->allParams = allParams;
	NGMsize = allParams->PEPTIDES_N + 1;
	NGM = TNT::Array2D<double>(NGMsize,NGMsize,0.0);
	domEigenvecNGM = TNT::Array1D<double>(NGMsize,0.0);
	cumulativeDist = TNT::Array1D<double>(NGMsize,0.0);
	R0 = 0;
	sample.resize(allParams->SAMPLESIZE_LARGE);

	didWHEDsamples = false;
	didWHEDsamplesFromDist = false;
	didWHEDsamplesFromSamples = false;
	madeNGM = false;
	computedEigs = false;
	computedStatsitics = false;
}

singlehostsim::~singlehostsim() {
	std::vector< hostOffspringStruct >::iterator hs = sample.begin();
	for ( ; hs != sample.end(); hs++ ) {
		delete (hs->sampledHost);
	}
}


void singlehostsim::makeNGM() {
	std::cout << "# -> computing NGM...\n";
	int n = allParams->PEPTIDES_N;
	NGMsize = n+1;
	NGM = TNT::Array2D<double>(NGMsize,NGMsize,0.0);
	int NN = allParams->SAMPLESIZE;
	for ( int i = 0; i <= n; i++ ) {
		for ( int t = 0; t < NN; t++ ) {
			int f = i; int e = 0; /* will mutate at infection anyway... */
			pathogen* newPathogen = new pathogen(allParams,e,f); // deleted by the host!!
			host newHost(newPathogen,ext_double(0.0),allParams);
			/* do a mini-sellke simulation on one host */
			std::list<pathogen*> offspringList = getOffspring(&newHost);
			for ( std::list<pathogen*>::iterator v = offspringList.begin();
					v != offspringList.end(); v++ ) {
				if ( (*v) == NULL ) {
					std::cout << "WARNING: in singlehostsim::makeNGM: pointer to virus is NULL\n";
				}
				else {
					if ( (*v)->getE() + (*v)->getF() > allParams->PEPTIDES_N || (*v)->getE() + (*v)->getF() < 0) {
						std::cout << "WARNING: in singlehostsim::makeNGM: invalid virus\n";
					}
					else {
						NGM[(*v)->getE() + (*v)->getF()][i]++; /* dont use the transpose... */
					}
				}
			}
		}
	}
	madeNGM = true;
}

void singlehostsim::WHEDsamples() {
	std::cout << "# -> sampling host instances...\n";
	for ( int i = 0; i < allParams->SAMPLESIZE_LARGE; i++ ) {
		pathogen* newPathogen = new pathogen(allParams); // deleted by the host!!
		host* newHost = new host(newPathogen,ext_double(0.0),allParams);
		/* do a mini-sellke simulation on one host */
		std::list<pathogen*> offspringList = getOffspring(newHost);
		sample[i] = hostOffspringStruct(newHost,offspringList);
	}
	didWHEDsamples = true;
}


std::list<pathogen*> singlehostsim::getOffspring(host* newHost) {
	/* calcEWTofTransmission needs "absolute time" and "relative threshold" */
	ext_double timeInInfection = 0.0;
	std::list<pathogen*> offspringList;
	ext_double rate(1.0); // modify this rate if FLAT_TP == true
	if ( allParams->FLAT_TP == 1 ) {
		rate = ext_double(0.5)*(newHost->getTotalBeta()); // scale with 1/2
	}
	ext_double threshold = ext_double(RNG.Exponential(1.0)) * rate;
	timeInInfection += newHost->calcEWTofTransmission(timeInInfection,threshold);
	while ( timeInInfection < newHost->getTimeOfDeath() ) {
		offspringList.push_back(newHost->getPathogen(timeInInfection));
		threshold = ext_double(RNG.Exponential(1.0)) * rate;
		timeInInfection += newHost->calcEWTofTransmission(timeInInfection,threshold);
	}
	return offspringList;
}

void singlehostsim::computeEigs() {
	std::cout << "# -> computing eigenspace of NGM...\n";

	/*
	std::string filename("data/ssdist.dat");
	std::ofstream filestream(filename.c_str());
	*/

	/* Uses the JAMA headers to compute the eigenvalues and eigenvectors
	 * of the NGM ( which is an object in the TNT namespace )
	 * Performance?? We only need the dominant eigenvector and value..
	 * Maybe the power method works better???
	 */

	/* NGM was constructed with makeNGM() */

	/* a matrix with (real) eigenvectors */
	TNT::Array2D<double> Vmatrix(NGMsize,NGMsize,0.0);
	/* a matrix with eigenvalues (block matrix) */
	TNT::Array1D<double> EigenvalsRealParts(NGMsize,0.0);
	TNT::Array1D<double> EigenvalsImagParts(NGMsize,0.0);

	/* make an JAMA::Eigenvalue object */
	JAMA::Eigenvalue<double> EigsObj(NGM);
	EigsObj.getV(Vmatrix); /* matrix of eigenvectors (sort of) */
	EigsObj.getRealEigenvalues(EigenvalsRealParts);
	EigsObj.getImagEigenvalues(EigenvalsImagParts);

	/* find the dominant eigenvalue */
	double* ep = EigenvalsRealParts;
	double* R0p = std::max_element(ep,ep+NGMsize);
	int indexOfDovEV = R0p - ep;
	R0 = *R0p / double(allParams->SAMPLESIZE);

	/* check if this was not the real part of some complex eigenvalue */
	if ( EigenvalsImagParts[indexOfDovEV] != 0.0 ) {
		double imPartR0 = EigenvalsImagParts[indexOfDovEV] / double(allParams->SAMPLESIZE);
		std::cout << "WARNING: selected eigenvalue is not real: " << R0 << " + i * " << imPartR0 << "\n";
		/* TODO: if this happens, what to do??? ( imag eigenvals ) */
	}

	/* get the dominant eigenvector */
	double scaleWith = 0.0;
	for ( int i = 0; i < NGMsize; i++ ) {
		 double count = Vmatrix[i][indexOfDovEV];
		 count = ( count < 0 ? -count : count );
		 domEigenvecNGM[i] = count;
		 scaleWith += count;
	}
	/* scale the eigenvector and get a CDF */
	if ( scaleWith != 0.0 ) {
		for ( int i = 0; i < NGMsize; i++ ) {
			domEigenvecNGM[i] /= scaleWith;
			cumulativeDist[i] = ( i > 0 ? cumulativeDist[i-1] : 0.0 ) + domEigenvecNGM[i];
		}
	}
	else {
		std::cout << "WARNING: dominant eigenvector is zero.\n";
	}

	/* testing...
	for ( int i = 0; i < NGMsize; i++ ) {
		filestream << domEigenvecNGM[i] << " ";
	}
	*/
	computedEigs = true;
}

void singlehostsim::WHEDsamplesFromDist() {
	std::cout << "# -> sampling from equilibrium distribution...\n";

	/* find the first index for which the CDF is not zero. */
	int firstIndex = NGMsize;
	for ( int i = 0; i < NGMsize; i++ ) {
		if ( cumulativeDist[i] != 0.0 ) {
			firstIndex = i;
			break;
		}
	}
	if ( firstIndex == NGMsize ) {
		std::cout << "WARNING: ill-defined CDF. aborting 'singlehostsim::WHEDsamplesFromDist()'.\n";
		return;
	}

	for ( int i = 0; i < allParams->SAMPLESIZE_LARGE; i++ ) {

		/* get a random virus from the stable distribution */

		double U = RNG.Uniform(0.0,1.0); /* use CDF to get inverse... */
		int sampleIndex = NGMsize-1;
		for ( int j = firstIndex; j < NGMsize; j++ ) {
			if ( U <= cumulativeDist[j] ) {
				sampleIndex = j;
				break;
			}
		}

		/* define a virus with the desired number of mutations */
		int e = 0; int f = sampleIndex;

		pathogen* newPathogen = new pathogen(allParams,e,f); // deleted by the host!!
		host* newHost = new host(newPathogen,ext_double(0.0),allParams);
		/* do a mini-sellke simulation on one host */
		std::list<pathogen*> offspringList = getOffspring(newHost);
		sample[i] = hostOffspringStruct(newHost,offspringList);
	}
	didWHEDsamplesFromDist = true;
}

void singlehostsim::WHEDsamplesFromSamples() {
	/* get total number of offspring
	 * and put then into a vector (for random access)
	 */
	std::vector< hostOffspringStruct >::iterator It = sample.begin();
	int NN = allParams->SAMPLESIZE_LARGE;

	int totalCount = 0;
	for ( ; It != sample.end(); It++ ) {
		totalCount += (It->offspringList).size();
	}
	while ( totalCount == 0 ) {
		std::cout 	<< "WARNING: in singlehostsim::WHEDsamplesFromSamples: "
					<< "previous sample produced no new pathogen. Re-sampling...\n";
		for ( int i = 0; i < NN; i++ ) {
			sample[i].offspringList = getOffspring(sample[i].sampledHost);
			totalCount += (sample[i].offspringList).size();
		}
	}
	std::vector<pathogen*> offSpringVec(totalCount,NULL);
	std::vector<pathogen*>::iterator Jt = offSpringVec.begin();
	for ( It = sample.begin(); It != sample.end(); It++ ) {
		std::list<pathogen*>::iterator Lit = (It->offspringList).begin();
		for ( ; Lit != (It->offspringList).end(); Lit++ ) {
			*Jt = *Lit;
			Jt++;
		}
	}

	/* make a new sample */
	std::vector<hostOffspringStruct> newSample(NN);

	/* draw random integers between 0 and totalCount-1 and get the corresponding virus */
	for ( int i = 0; i < NN; i++ ) {
		int r = RNG.Integer(NN);
		pathogen* transmittedVirus = new pathogen(*(offSpringVec[r]));
		transmittedVirus->clearEvents();
		// TODO make a copy constuctor for pathogen (or individual) that clears the event list
		host* newHost = new host(transmittedVirus,ext_double(0.0),allParams);
		std::list<pathogen*> offspringList = getOffspring(newHost);

		newSample[i] = hostOffspringStruct(newHost,offspringList);
	}
	/* clear the old sample and replace it with the new one */
	for ( int i = 0; i < NN; i++ ) {
		delete sample[i].sampledHost;
		sample[i].sampledHost = newSample[i].sampledHost;
		sample[i].offspringList = newSample[i].offspringList;
	}
	didWHEDsamplesFromSamples = true;
}







void singlehostsim::print(std::ostream & os) const {
	std::cout << "# -> writing to file...\n";

	os 	<< "<stable_dist_sample "
		<< "VLMAX='" << allParams->VLMAX << "' "
		<< "LAMBDA1='" << allParams->LAMBDA1 << "' "
		<< "LAMBDA2='" << allParams->LAMBDA2 << "' "
		<< "LAMBDA3='" << allParams->LAMBDA3 << "' "
		<< "PREV='" << allParams->PREV << "' "
		<< "PESC='" << allParams->PESC << "' "
		<< "SIGMA='" << allParams->SIGMA << "' "
		<< "PHI='" << allParams->PHI << "' "
		<< "PEPTIDES_N='" << allParams->PEPTIDES_N << "' "
		<< "NONEPITOPE_M='" << allParams->NONEPITOPE_M << "' "
		<< "MEAN_K='" << allParams->MEAN_K << "' "
		<< "STANDARD_DEV_K='" << allParams->STANDARD_DEV_K << "' "
		<< "R0='" << R0 << "' >\n";

	std::vector< hostOffspringStruct >::const_iterator hs = sample.begin();
	for ( ; hs != sample.end(); hs++ ) {
		os << "<single_host_simulation >\n";
		os << hs->sampledHost << "\n";
		os << "<transmitted_virus number='" <<  (hs->offspringList).size() << "' > ";
		for ( std::list<pathogen*>::const_iterator v = (hs->offspringList).begin();
				v != (hs->offspringList).end(); v++ ) {
			os 	<< (*v) << " ";
		}
		os << "</transmitted_virus >\n";
		os << "</single_host_simulation >\n";
	}
	os << "</stable_dist_sample > \n";
}

void singlehostsim::printNGM(std::ostream & os) const {
	for ( int i = 0; i < NGMsize; i++ ) {
		for ( int j = 0; j < NGMsize; j++ ) {
			os << NGM[i][j] << " ";
		}
		os << "\n";
	}
}

void singlehostsim::printTCs(std::ostream & os) const {
	std::vector<hostOffspringStruct>::const_iterator hoIt = sample.begin();
	for ( ; hoIt != sample.end(); hoIt++ ) {
		for ( std::list<pathogen*>::const_iterator vIt = hoIt->offspringList.begin();
				vIt != hoIt->offspringList.end();
				vIt++ ) {
			pathogen* transmittedVirus = new pathogen(**vIt);
			transmittedVirus->clearEvents();
			host secondaryCase(transmittedVirus,ext_double(0.0),allParams);

			os 	<< secondaryCase.getSpVLtimeAverage() << " "
				<< secondaryCase.getSpVLonsetAsymPhase() << " "
				<< secondaryCase.getK() << " "
				<< secondaryCase.getInfectingVirus()->getNumberOfMutations() << " "
				<< hoIt->sampledHost->getSpVLtimeAverage() << " "
				<< hoIt->sampledHost->getSpVLonsetAsymPhase() << " "
				<< hoIt->sampledHost->getK() << " "
				<< hoIt->sampledHost->getInfectingVirus()->getNumberOfMutations() << "\n";
		}
	}
}


void singlehostsim::computeStatistics() {
	std::cout << "# -> computing statistics...\n";

	/* put the parameters in the map */
	statistics.map["VLMAX"] = allParams->VLMAX;
	statistics.map["LAMBDA1"] = allParams->LAMBDA1;
    statistics.map["LAMBDA2"] = allParams->LAMBDA2;
    statistics.map["LAMBDA3"] = allParams->LAMBDA3;
    statistics.map["PREV"] = allParams->PREV;
    statistics.map["PESC"] = allParams->PESC;
    statistics.map["SIGMA"] = allParams->SIGMA;
    statistics.map["PHI"] = allParams->PHI;
    statistics.map["PEPTIDES_N"] = allParams->PEPTIDES_N;
    statistics.map["NONEPITOPE_M"] = allParams->NONEPITOPE_M;

    if ( !madeNGM ) { // in the case of the sample from sample method
    	for ( std::vector< hostOffspringStruct >::iterator i = sample.begin(); i !=sample.end(); i++ ) {
    		R0 += (double) (i->offspringList).size();
    	}
    	R0 /= (double) allParams->SAMPLESIZE_LARGE;
    	statistics.map["R0"] = R0;
    }
    else {
    	statistics.map["R0"] = R0;
    }




    /* TODO: use the objects from "clac_stats.hpp" to produce the needed statistics!
     * Include 95% CIs and medians for the number of mutations (e2-e1 and f2-f1) after the
     * acute phase to improve the comparison with reality
     */



	/* compute some statistics for the sample */
	/* moments of spVL */
	ext_double spVLtavM1 = 0.0; /* mean */
	ext_double spVLtavM2 = 0.0; /* snd moment */
	ext_double spVLtavM3 = 0.0; /* trd moment */

	ext_double spVLminM1 = 0.0; /* mean */
	ext_double spVLminM2 = 0.0; /* snd moment */
	ext_double spVLminM3 = 0.0; /* trd moment */

	ext_double eInfectM1 = 0.0;
	ext_double fInfectM1 = 0.0;
	ext_double efInfectDOT = 0.0;
	ext_double eInfectM2 = 0.0;
	ext_double fInfectM2 = 0.0;

	ext_double eAcuteM1 = 0.0;
	ext_double fAcuteM1 = 0.0;
	ext_double efAcuteDOT = 0.0;
	ext_double eAcuteM2 = 0.0;
	ext_double fAcuteM2 = 0.0;

	/* for the number of escapes in the acute phase (eAcute - eInfect),
	 * we really need a range (i.e. 95% CI) and a median.
	 */

	ext_double escAcutePhaseMedian = NANUM;
	ext_double escAcutePhaseTopPercentile = NANUM;
	ext_double escAcutePhaseBottomPercentile = NANUM;

	/* try to explain the heritability with some ratio statistics */

	ext_double fractionRemainingImRespM1 = 0.0; // (k-e2)/(k-e1)
	ext_double fractionRemainingDeleteriousM1 = 0.0; // f2/f1
	ext_double fractionRemainingImRespM2 = 0.0; // (k-e2)/(k-e1) // snd moment
	ext_double fractionRemainingDeleteriousM2 = 0.0; // f2/f1
	ext_double fractionRemainingImRespM3 = 0.0; // (k-e2)/(k-e1) // trd moment
	ext_double fractionRemainingDeleteriousM3 = 0.0; // f2/f1



	/* make a list of spVL pairs for heritability */
	std::list< std::pair<ext_double,ext_double> > heritList;

	/* make a list of escapes in acute phase for median and range */
	std::list< ext_double > percentileListEscAcutePhase;


	/* covariance etc are also needed for heritability
	 * note that we need moments for transmitters,
	 * some may be part or multiple couples // TODO: check this!!!!
	 */
	ext_double spVLtavDOT_fst_snd = 0; // inner product
	ext_double spVLtavM1_fst = 0.0; /* mean spVL of primary cases */
	ext_double spVLtavM2_fst = 0.0; /* snd moment of primary cases */
	ext_double spVLtavM1_snd = 0.0; /* mean spVL of secondary cases */
	ext_double spVLtavM2_snd = 0.0; /* snd moment of secondary cases */
	ext_double a = 0; /* slope */
	ext_double b = 0; /* intercept */
	ext_double SStot = 0; /* total variation */
	ext_double SSerr = 0; /* total error */
	ext_double R2 = 0.0;

	/* and a list of infecting virus pairs (e+f = m only) for "genotypic" heritability */
	std::list< std::pair<ext_double,ext_double> > heritList_m;

	ext_double mDOT_fst_snd = 0; // inner product
	ext_double mM1_fst = 0.0; /* mean spVL of primary cases */
	ext_double mM2_fst = 0.0; /* snd moment of primary cases */
	ext_double mM1_snd = 0.0; /* mean spVL of secondary cases */
	ext_double mM2_snd = 0.0; /* snd moment of secondary cases */
	ext_double a_m = 0; /* slope */
	ext_double b_m = 0; /* intercept */
	ext_double SStot_m = 0; /* total variation */
	ext_double SSerr_m = 0; /* total error */
	ext_double R2_m = 0.0;




	/* prepare */
	std::vector<hostOffspringStruct>::iterator hoIt = sample.begin();
	for ( ; hoIt != sample.end(); hoIt++ ) {
		host* hostPointer = hoIt->sampledHost;
		/* spVL time average */
		ext_double spVLtav = hostPointer->getSpVLtimeAverage(); // also used for heritability below...
		spVLtavM1 += spVLtav;
		spVLtavM2 += spVLtav * spVLtav;
		spVLtavM3 += spVLtav * spVLtav * spVLtav;

		/* spVL onset asym phase */

		ext_double spVLmin = hostPointer->getSpVLonsetAsymPhase();
		spVLminM1 += spVLmin;
		spVLminM2 += spVLmin * spVLmin;
		spVLminM3 += spVLmin * spVLmin * spVLmin;

		/* number of escapes and reversions at the end of the acute phase */
		double e_inf = (double) hostPointer->getInfectingVirus()->getE();
		double f_inf = (double) hostPointer->getInfectingVirus()->getF();
		eInfectM1 += e_inf;
		fInfectM1 += f_inf;
		eInfectM2 += e_inf * e_inf;
		fInfectM2 += f_inf * f_inf;
		efInfectDOT += f_inf * e_inf;

		double e_acu = (double) hostPointer->getVirusEndAcutePhase()->getE();
		double f_acu = (double) hostPointer->getVirusEndAcutePhase()->getF();
		eAcuteM1 += e_acu;
		fAcuteM1 += f_acu;
		eAcuteM2 += e_acu * e_acu;
		fAcuteM2 += f_acu * f_acu;
		efAcuteDOT += f_acu * e_acu;

		double k = (double) hostPointer->getK();

		ext_double fracIR = (e_inf == k ? 0 : (k - e_acu) / (k - e_inf));
		ext_double fracDel = (f_inf == 0 ? 0 : f_acu / f_inf);

		/* fill the percentileList for the number of escapes in the acute phase */
		percentileListEscAcutePhase.push_back(ext_double(e_acu - e_inf));

		fractionRemainingImRespM1 += fracIR; // (k-e2)/(k-e1)
		fractionRemainingDeleteriousM1 += fracDel; // f2/f1
		fractionRemainingImRespM2 += fracIR * fracIR; // (k-e2)/(k-e1) // snd moment
		fractionRemainingDeleteriousM2 += fracDel * fracDel; // f2/f1
		fractionRemainingImRespM3 += fracIR * fracIR * fracIR; // (k-e2)/(k-e1) // trd moment
		fractionRemainingDeleteriousM3 += fracDel * fracDel * fracDel; // f2/f1

		/* heritability */
		/* for every host, infect new hosts with their transmitted strains
		 * then get the spVLtav from the secondary cases and put them in the list
		 */
		for ( std::list<pathogen*>::iterator vIt = hoIt->offspringList.begin();
				vIt != hoIt->offspringList.end();
				vIt++ ) {
			pathogen* transmittedVirus = new pathogen(**vIt);
			transmittedVirus->clearEvents();
			host secondaryCase(transmittedVirus,ext_double(0.0),allParams);

			std::pair<ext_double,ext_double> spVLpair(spVLtav,secondaryCase.getSpVLtimeAverage());
			heritList.push_back(spVLpair);
			spVLtavDOT_fst_snd += spVLpair.first * spVLpair.second;
			spVLtavM1_snd += spVLpair.second;
			spVLtavM2_snd += spVLpair.second * spVLpair.second;
			spVLtavM1_fst += spVLpair.first;
			spVLtavM2_fst += spVLpair.first * spVLpair.first;

			/* heritability of e+f */

			/* get mutations of transmitted strains */
			double e_trans = (double) (*vIt)->getE();
			double f_trans = (double) (*vIt)->getF();

			std::pair<ext_double,ext_double> mPair(e_inf+f_inf,e_trans+f_trans);
			heritList_m.push_back(mPair);
			mDOT_fst_snd += mPair.first * mPair.second;
			mM1_snd += mPair.second;
			mM2_snd += mPair.second * mPair.second;
			mM1_fst += mPair.first;
			mM2_fst += mPair.first * mPair.first;
		}
	}
	/* divide to finish */
	ext_double samplesize = ext_double((double) allParams->SAMPLESIZE_LARGE);
	spVLtavM1 /= samplesize;
	spVLtavM2 /= samplesize;
	spVLtavM3 /= samplesize;

	spVLminM1 /= samplesize;
	spVLminM2 /= samplesize;
	spVLminM3 /= samplesize;

	eInfectM1 /= samplesize;
	fInfectM1 /= samplesize;
	eAcuteM1 /= samplesize;
	fAcuteM1 /= samplesize;

	eInfectM2 /= samplesize;
	fInfectM2 /= samplesize;
	eAcuteM2 /= samplesize;
	fAcuteM2 /= samplesize;

	efInfectDOT /= samplesize;
	efAcuteDOT /= samplesize;

	fractionRemainingImRespM1 /= samplesize;
	fractionRemainingDeleteriousM1 /= samplesize;
	fractionRemainingImRespM2 /= samplesize;
	fractionRemainingDeleteriousM2 /= samplesize;
	fractionRemainingImRespM3 /= samplesize;
	fractionRemainingDeleteriousM3 /= samplesize;


	/* heritability needs an extra loop. samplesize is now different, length of heritlist*/
	ext_double samplesize_herit = ext_double((double) heritList.size());
	spVLtavM1_fst /= samplesize_herit;
	spVLtavM2_fst /= samplesize_herit;
	spVLtavM1_snd /= samplesize_herit;
	spVLtavM2_snd /= samplesize_herit;
	spVLtavDOT_fst_snd /= samplesize_herit;

	SStot = spVLtavM2_snd - spVLtavM1_snd * spVLtavM1_snd;
	// the linear model for virus load Vrec = a * Vtrans + b + NORMAL
	a = (spVLtavDOT_fst_snd - spVLtavM1_fst * spVLtavM1_snd) / (spVLtavM2_fst - spVLtavM1_fst * spVLtavM1_fst);
	b = spVLtavM1_snd - a * spVLtavM1_fst;
	for ( std::list< std::pair<ext_double,ext_double> >::iterator tc = heritList.begin();
			tc != heritList.end();
			tc++ ) {
		ext_double err = tc->second - (a * tc->first + b);
		SSerr += err * err;
	}
	SSerr /= samplesize_herit;
	R2 = ext_double(1.0) - SSerr/SStot;


	/* heritability of m = e+f */

	mM1_fst /= samplesize_herit;
	mM2_fst /= samplesize_herit;
	mM1_snd /= samplesize_herit;
	mM2_snd /= samplesize_herit;
	mDOT_fst_snd /= samplesize_herit;

	SStot_m = mM2_snd - mM1_snd * mM1_snd;
	// the linear model for virus load Vrec = a * Vtrans + b + NORMAL
	a_m = (mDOT_fst_snd - mM1_fst * mM1_snd) / (mM2_fst - mM1_fst * mM1_fst);
	b_m = mM1_snd - a_m * mM1_fst;
	for ( std::list< std::pair<ext_double,ext_double> >::iterator tc = heritList_m.begin();
			tc != heritList_m.end();
			tc++ ) {
		ext_double err = tc->second - (a_m * tc->first + b_m);
		SSerr_m += err * err;
	}
	SSerr_m /= samplesize_herit;
	R2_m = ext_double(1.0) - SSerr_m/SStot_m;

	/* calculate the percentiles for the number of escapes in the acute phase */

	if ( !percentileListEscAcutePhase.empty() ) {
		percentileListEscAcutePhase.sort();
		int nn = percentileListEscAcutePhase.size(); int ii = 0;
		std::list<ext_double>::iterator it = percentileListEscAcutePhase.begin();
		int locationBottom = int(nn * 0.025);
		int locationTop = int(nn * 0.975);
		int locationMedian = int(nn * 0.5);
		for ( ; ii < locationBottom; ii++ ) it++;
		escAcutePhaseBottomPercentile = *it;
		for ( ; ii < locationMedian; ii++ ) it++;
		escAcutePhaseMedian = *it;
		for ( ; ii < locationTop; ii++ ) it++;
		escAcutePhaseTopPercentile = *it;
	}

	/* put statistics in a map object */

	statistics.map["spVLtavM1"] = spVLtavM1;
	statistics.map["spVLtavM2"] = spVLtavM2;
	statistics.map["spVLtavM3"] = spVLtavM3;

	statistics.map["spVLminM1"] = spVLminM1;
	statistics.map["spVLminM2"] = spVLminM2;
	statistics.map["spVLminM3"] = spVLminM3;

	statistics.map["eInfectM1"] = eInfectM1;
	statistics.map["fInfectM1"] = fInfectM1;
    statistics.map["eAcuteM1"] = eAcuteM1;
    statistics.map["fAcuteM1"] = fAcuteM1;

	statistics.map["eInfectM2"] = eInfectM2;
	statistics.map["fInfectM2"] = fInfectM2;
    statistics.map["eAcuteM2"] = eAcuteM2;
    statistics.map["fAcuteM2"] = fAcuteM2;

    statistics.map["efInfectDOT"] = efInfectDOT; // needed for sd number of mutations
    statistics.map["efAcuteDOT"] = efAcuteDOT; // needed for sd number of mutations

    /* fractions below relate to end of acute phase */

    statistics.map["fractionRemainingImRespM1"] = fractionRemainingImRespM1;
    statistics.map["fractionRemainingDeleteriousM1"] = fractionRemainingDeleteriousM1;
    statistics.map["fractionRemainingImRespM2"] = fractionRemainingImRespM2;
    statistics.map["fractionRemainingDeleteriousM2"] = fractionRemainingDeleteriousM2;
    statistics.map["fractionRemainingImRespM3"] = fractionRemainingImRespM3;
    statistics.map["fractionRemainingDeleteriousM3"] = fractionRemainingDeleteriousM3;

    statistics.map["escAcutePhaseMedian"] = escAcutePhaseMedian;
    statistics.map["escAcutePhaseTopPercentile"] = escAcutePhaseTopPercentile;
    statistics.map["escAcutePhaseBottomPercentile"] = escAcutePhaseBottomPercentile;


    statistics.map["R2"] = R2; // R^2
    statistics.map["rho"] = a; // slope
    statistics.map["R2_m"] = R2_m; // R^2 for number of mutations e+f
    statistics.map["rho_m"] = a_m; // slope for number of mutations e+f
}

mapWrapper singlehostsim::getStatistics() const {
	return statistics;
}



std::ostream & operator<<(std::ostream & os, const singlehostsim & sim) {
	sim.print(os);
	return os;
}

/**************************************************************************/
/********************* methods for hostOffspringStruct ********************/
/**************************************************************************/

hostOffspringStruct::hostOffspringStruct(host* sampledHost, std::list<pathogen*> offspringList) :
		sampledHost(sampledHost), offspringList(offspringList) { counter = 0; }

hostOffspringStruct::hostOffspringStruct() {
	sampledHost = NULL;
	counter = 0;
}

/**************************************************************************/
/********************* methods for mapWrapper *****************************/
/**************************************************************************/

void mapWrapper::print(std::ostream & os) const {
	os << "<stable_dist_sample ";
	std::map<std::string, ext_double>::const_iterator it = map.begin();
	for ( ; it != map.end(); it++ ) {
		os << it->first << "='" << it->second << "' ";
	}
	os << ">" << " </stable_dist_sample >";
}


std::ostream & operator<<(std::ostream & os, const mapWrapper & mw) {
	mw.print(os);
	return os;
}





