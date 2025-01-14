#include "uvos_brd.h"

using namespace uvos;

UVOSboard hw;

#ifdef __cplusplus
extern "C" {
#endif



uint64_t inv_timer_get_counter(unsigned timer_num)
{
	return System::GetUs();
}

void inv_delay_us(uint32_t us)
{
	System::DelayUs(us);
}

#ifdef __cplusplus
}
#endif