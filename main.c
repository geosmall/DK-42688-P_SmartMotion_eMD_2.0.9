#include "example-raw-data-registers.h"

/* InvenSense utils */
#include "Invn/EmbUtils/Message.h"
#include "Invn/EmbUtils/ErrorHelper.h"
#include "Invn/EmbUtils/RingBuffer.h"

/* board driver */
#include "common.h"
// #include "uart.h"
// #include "uart_mngr.h"
// #include "delay.h"
#include "gpio.h"
// #include "timer.h"
// #include "rtc_timer.h"

#include "system-interface.h"

/* std */
// #include <stdio.h>

/* --------------------------------------------------------------------------------------
 *  Example configuration
 * -------------------------------------------------------------------------------------- */

/*
 * Select UART port on which INV_MSG() will be printed.
 */
#define LOG_UART_ID INV_UART_SENSOR_CTRL

/* 
 * Set of timers used throughout standalone applications 
 */
// #define TIMEBASE_TIMER INV_TIMER1
// #define DELAY_TIMER    INV_TIMER2
#define TIMEBASE_TIMER
#define DELAY_TIMER

/* 
 * Select communication link between SmartMotion and ICM426xx 
 */
#define SERIF_TYPE ICM426XX_UI_SPI4
//#define SERIF_TYPE ICM426XX_UI_I2C

/* 
 * Define msg level 
 */
#define MSG_LEVEL INV_MSG_LEVEL_DEBUG

/*
 * Set power mode flag
 * Set this flag to run example in low-noise mode.
 * Reset this flag to run example in low-power mode.
 * Note : low-noise mode is not available with sensor data frequencies less than 12.5Hz.
 */
#define IS_LOW_NOISE_MODE 1

/* 
 * Set this to 0 if you want to test timestamping mechanism without CLKIN 32k capability.
 * Please set a hardware bridge between PA17 (from MCU) and CLKIN pins (to ICM).
 * Warning: This option is not available for all ICM426XX. Please check the datasheet.
 */
#define USE_CLK_IN 0

/* --------------------------------------------------------------------------------------
 *  Global variables
 * -------------------------------------------------------------------------------------- */

/* 
 * Buffer to keep track of the timestamp when icm426xx data ready interrupt fires.
 * The buffer can contain up to 64 items in order to store one timestamp for each packet in FIFO.
 */
RINGBUFFER(timestamp_buffer, 64, uint64_t);

/* --------------------------------------------------------------------------------------
 *  Static variables
 * -------------------------------------------------------------------------------------- */

/* Flag set from icm426xx device irq handler */
static volatile int irq_from_device;

/* --------------------------------------------------------------------------------------
 *  Forward declaration
 * -------------------------------------------------------------------------------------- */

static void SetupMCUHardware(struct inv_icm426xx_serif *icm_serif);
static void ext_interrupt_cb(void *context, unsigned int int_num);
static void check_rc(int rc, const char *msg_context);
void        msg_printer(int level, const char *str, va_list ap);

int main(void)
{
	int rc = 0;
	struct inv_icm426xx_serif icm426xx_serif;

	/* Initialize MCU hardware */
	SetupMCUHardware(&icm426xx_serif);

	/* Initialize Icm426xx */
	rc = SetupInvDevice(&icm426xx_serif);
	
}


/* --------------------------------------------------------------------------------------
 *  Functions definitions
 * -------------------------------------------------------------------------------------- */

/*
 * This function initializes MCU on which this software is running.
 * It configures:
 *   - a UART link used to print some messages
 *   - interrupt priority group and GPIO so that MCU can receive interrupts from ICM426xx
 *   - a microsecond timer requested by Icm426xx driver to compute some delay
 *   - a microsecond timer used to get some timestamps
 *   - a serial link to communicate from MCU to Icm426xx
 */
static void SetupMCUHardware(struct inv_icm426xx_serif *icm_serif)
{

}

/*
 * Icm426xx interrupt handler.
 * Function is executed when an Icm426xx interrupt rises on MCU.
 * This function get a timestamp and store it in the timestamp buffer.
 * Note that this function is executed in an interrupt handler and thus no protection
 * are implemented for shared variable timestamp_buffer.
 */
static void ext_interrupt_cb(void *context, unsigned int int_num)
{
	(void)context;

#if USE_CLK_IN
	/* 
	 * Read timestamp from the RTC derived from SLCK, clocking CLKIN
	 */
	uint64_t timestamp = rtc_timer_get_time_us();
#else
	/* 
	 * Read timestamp from the timer dedicated to timestamping 
	 */
	uint64_t timestamp = inv_timer_get_counter(TIMEBASE_TIMER);
#endif

	if (int_num == INV_GPIO_INT1 && !RINGBUFFER_FULL(&timestamp_buffer)) {
		RINGBUFFER_PUSH(&timestamp_buffer, &timestamp);
	}

	irq_from_device |= TO_MASK(int_num);
}

/*
 * Helper function to check RC value and block programm exectution
 */
static void check_rc(int rc, const char *msg_context)
{
	if (rc < 0) {
		INV_MSG(INV_MSG_LEVEL_ERROR, "%s: error %d (%s)\r\n", msg_context, rc, inv_error_str(rc));
		while (1)
			;
	}
}


/* --------------------------------------------------------------------------------------
 *  Extern functions definition
 * -------------------------------------------------------------------------------------- */

/*
 * Icm426xx driver needs to get time in us. Let's give its implementation here.
 */
uint64_t inv_icm426xx_get_time_us(void)
{
#if USE_CLK_IN
	return rtc_timer_get_time_us();
#else
	return inv_timer_get_counter(TIMEBASE_TIMER);
#endif
}

/*
 * Clock calibration module needs to disable IRQ. Thus inv_helper_disable_irq is
 * defined as extern symbol in clock calibration module. Let's give its implementation
 * here.
 */
void inv_helper_disable_irq(void)
{
	inv_disable_irq();
}

/*
 * Clock calibration module needs to enable IRQ. Thus inv_helper_enable_irq is
 * defined as extern symbol in clock calibration module. Let's give its implementation
 * here.
 */
void inv_helper_enable_irq(void)
{
	inv_enable_irq();
}

/*
 * Icm426xx driver needs a sleep feature from external device. Thus inv_icm426xx_sleep_us
 * is defined as extern symbol in driver. Let's give its implementation here.
 */
void inv_icm426xx_sleep_us(uint32_t us)
{
	inv_delay_us(us);
}
