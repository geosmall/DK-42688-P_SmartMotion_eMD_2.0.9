#include "SPI.h"

#ifdef __cplusplus
extern "C" {
#endif

uint64_t inv_timer_get_counter(unsigned timer_num)
{
	return micros();
}

void inv_delay_us(uint32_t us)
{
	delayMicroseconds(us);
}

#ifdef __cplusplus
}
#endif