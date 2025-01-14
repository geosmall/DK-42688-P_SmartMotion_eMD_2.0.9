#include "uvos_brd.h"
#include "inv_main.h"

using namespace uvos;

UVOSboard hw;

int main(void)
{
    // Initialize the UVOS board hardware
    hw.Init();

    inv_main();
}

/* --------------------------------------------------------------------------------------
 *  Extern functions definition
 * -------------------------------------------------------------------------------------- */

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