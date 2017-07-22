/*
 * pathogen_class.hpp
 *
 *  Created on: Oct 26, 2012
 *      Author: chris
 */

#ifndef PATHOGEN_CLASS_HPP_
#define PATHOGEN_CLASS_HPP_

#include "individual_class.hpp"
#include "ext_double_class.hpp"
#include "RngClass.hpp" // for mutation at infection
class phase;
#include "phase_class.hpp"
#include "frasers_rates.hpp"
#include "parameters.hpp"

class pathogen: public individual {
public:
	pathogen(parstruct* );
	pathogen(parstruct* , int , int ); /* specify the number of mutations */
	pathogen(parstruct* , int , int , int ); /* specify also extra deleterious mutations */
	pathogen(const pathogen & ); // copy constructor (copies the genome)
	const pathogen & operator=(const pathogen & ); // assignment operator
	~pathogen();
	int getE() const;
	int getF() const;
	int getD() const; // the number of extra deleterious mutations
	int getK() const;
	int getNumberOfMutations() const;
	double calcSpVL() const; // complete!
	ext_double calcLambdaEsc(phase* ); // complete!
	ext_double calcLambdaRev(phase* ); // complete!
	ext_double calcLambda(phase* );
	ext_double calcBeta(phase* );
	void doEscMutation();
	void doRevMutation();
	void doMutationAtInfection(int );
	void print(std::ostream & ) const;
protected:
	/* a pointer to a parameter struct with all the parameters */
	parstruct* pars; // ALWAYS PASS THIS!! (people will point at this object)
	int e, f, d;
	int k;
};

std::ostream & operator<<(std::ostream & , const pathogen* );



#endif /* PATHOGEN_CLASS_HPP_ */
