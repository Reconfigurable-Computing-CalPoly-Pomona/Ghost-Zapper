/*
 * ctimer_core.h
 *
 *  Created on: Dec 3, 2019
 *      Author: Dominic
 */

#ifndef SRC_CTIMER_CORE_H_
#define SRC_CTIMER_CORE_H_

#include "chu_init.h"
#include "chu_io_rw.h"

class ConstTimerCore {
public:

	ConstTimerCore(uint32_t core_base_addr);

	~ConstTimerCore();

	uint32_t read(); //reads value from counter

private:
   uint32_t base_addr;
};



#endif /* SRC_CTIMER_CORE_H_ */
