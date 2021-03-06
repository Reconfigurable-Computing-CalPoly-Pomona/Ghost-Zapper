#include "ctimer_core.h"

ConstTimerCore::ConstTimerCore(uint32_t core_base_addr){
	base_addr = core_base_addr;
}

ConstTimerCore::~ConstTimerCore(){}

uint32_t ConstTimerCore::read(){
	return io_read(base_addr, 0);
}
