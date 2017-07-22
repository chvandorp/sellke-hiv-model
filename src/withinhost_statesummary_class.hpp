/*
 * withinhost_statesummary_class.hpp
 *
 *  Created on: Oct 26, 2012
 *      Author: chris
 */

#ifndef WITHINHOST_STATESUMMARY_CLASS_HPP_
#define WITHINHOST_STATESUMMARY_CLASS_HPP_

#include <list>
#include "statesummary_class.hpp"
#include "ext_double_class.hpp"
#include "pathogen_class.hpp"

class withinhost_statesummary: public statesummary {
public:
	/* getTime is implemented in the superclass */
	withinhost_statesummary(ext_double , ext_double , ext_double , pathogen* );
	~withinhost_statesummary(); // complete
	ext_double getBeta(); // complete
	ext_double getCumulativeBeta(); // complete
	void assignCumulativeBeta(ext_double ); // complete
	ext_double getVL();
	ext_double getCumulativeExpVL();
	void assignCumulativeExpVL(ext_double );
	pathogen* getPathogen(); // complete

	void print(std::ostream &) const;
protected:
	ext_double beta;
	ext_double cumulativeBeta;
	ext_double VL;
	ext_double cumulativeExpVL;
	pathogen* virus;
};


// TODO: check if the operator<< can be defined on the top level (means that we need a virtual print function)
// TODO: operator<< for individual etc. (and not host)
std::ostream & operator<<(std::ostream & , const withinhost_statesummary* );


#endif /* WITHINHOST_STATESUMMARY_CLASS_HPP_ */
