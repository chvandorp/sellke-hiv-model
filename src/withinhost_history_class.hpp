/*
 * withinhost_history_class.hpp
 *
 *  Created on: Oct 26, 2012
 *      Author: chris
 */

#ifndef WITHINHOST_HISTORY_CLASS_HPP_
#define WITHINHOST_HISTORY_CLASS_HPP_

#include <list>
#include <cmath>
#include "history_class.hpp"
#include "statesummary_class.hpp"
#include "withinhost_statesummary_class.hpp"

typedef ext_double (withinhost_statesummary::*ss_memfun) (void);

class withinhost_history: public history {
public:
	withinhost_history(); // complete
	~withinhost_history(); // complete
	void push_back(statesummary* ); // complete
	withinhost_statesummary* getStatesummary(ext_double , ss_memfun ); // complete
	withinhost_statesummary* getFinalStatesummary(); // complete

	void print(std::ostream & os) const;
private:
	std::list<withinhost_statesummary*> summaryList;
	std::list<withinhost_statesummary*>::iterator cyclicIterator; // for awesome speed!!!
};

std::ostream & operator<<(std::ostream & , const withinhost_history* );


#endif /* WITHINHOST_HISTORY_CLASS_HPP_ */
