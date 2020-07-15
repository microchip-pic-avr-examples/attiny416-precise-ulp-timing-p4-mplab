#include "atmel_start.h"
#include "main.h"
#include <ccp.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define F_XOSC32K 32768UL
#define DESIRED_RTC_TICK_S 3 // Desired duration of an RTC tick in seconds
// 3 seconds is 3*32768 = 98304 cycles of the XOSC32K, so
// we can measure the RTC tick duration with about
// (1/98304) or 10 ppm resolution
#define RTC_PER_VALUE (DESIRED_RTC_TICK_S * 1024 - 1) // RTC will run on an (approximate) 1024 Hz clock,
// so this will give a tick roughly every 3 seconds
#define N_TICKS_BETWEEN_MEAS 300 // Number of ticks between measurements
// With 3 seconds per tick, this is 3*300 = 900 seconds or 15 minutes
#define N_TICKS_FOR_MEAS 2 // Number of ticks to use for a measurement

#define BLED1_bm (1 << 4) // Least significant button (B1) and LED (LED1)
#define BLED1PORT PORTA   // are connected to PA4

volatile uint32_t n_31us               = 0;
volatile uint32_t secs                 = 0;
volatile uint32_t mins                 = 0;
volatile int      now_in_meas_mode     = 0;
volatile int      cnt_in_meas_mode     = 0;
volatile int      cnt_not_in_meas_mode = 0;
volatile uint32_t meas_value           = (F_XOSC32K * DESIRED_RTC_TICK_S); // Start with estimated measurement

ISR(RTC_CNT_vect)
{
	/* Overflow interrupt flag has to be cleared manually */
	RTC.INTFLAGS = RTC_OVF_bm;

	BLED1PORT.OUTTGL = BLED1_bm; // Toggle LED to show when we are in handler
	if (!now_in_meas_mode) {
		cnt_in_meas_mode = 0;
		cnt_not_in_meas_mode++;
	} else {
		cnt_in_meas_mode++;
		cnt_not_in_meas_mode = 0;
	}

	if (cnt_in_meas_mode == N_TICKS_FOR_MEAS) {
		meas_value = (TCB0.CCMP) + 1; // Get count that was saved by TCB, add 1 since a value of 0 means 1 clock cycle
		meas_value
		    = meas_value
		      + 65536; // The TCB0 will overflow once during a 3-second duration, so we add 2^16 to get the true count
	}
	n_31us           = n_31us + meas_value; // Update the (1/32768) second counter by adding the measured value
	secs             = n_31us >> 15;        // Determine the integer number of seconds
	mins             = secs / 60;
	BLED1PORT.OUTTGL = BLED1_bm; // Toggle LED again to show that we are getting out of handler

	if (mins & 0x01) // Toggle the LED for odd and even minutes
		BLED1PORT.OUTCLR = BLED1_bm;
	else
		BLED1PORT.OUTSET = BLED1_bm;
}

void switch_main_clk_to_ulp32k()
{
	//_PROTECTED_WRITE(CLKCTRL.MCLKCTRLA,CLKCTRL_CLKSEL_OSCULP32K_gc); // Select ULP
	ccp_write_io((void *)&(CLKCTRL.MCLKCTRLA), CLKCTRL_CLKSEL_OSCULP32K_gc /* 32KHz oscillator */);
	while (CLKCTRL.MCLKSTATUS & (1 << CLKCTRL_SOSC_bp)) {
		; // Wait for System Oscillator changing bit to go low
	}
}

void switch_main_clk_to_xosc32k()
{
	//_PROTECTED_WRITE(CLKCTRL.MCLKCTRLA,CLKCTRL_CLKSEL_XOSC32K_gc); // Select XOSC
	ccp_write_io((void *)&(CLKCTRL.MCLKCTRLA), CLKCTRL_CLKSEL_XOSC32K_gc /* 32KHz XOSC */);
	while (CLKCTRL.MCLKSTATUS & (1 << CLKCTRL_SOSC_bp)) {
		; // Wait for System Oscillator changing bit to go low
	}
}

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();

	while (1) {

		// Switch the main clk to XOSC32K so we can use the TCB to make an accurate
		// measurement of the duration of two consecutive RTC ticks
		switch_main_clk_to_xosc32k();

		// At this point the RTC is running from OSC32KULP divided by 32,
		// while CPU and everything else is running from XOSC32K
		now_in_meas_mode = 1; // Inform ISR that we are in measurement mode

		// Wait for ISR to make measurement
		while (cnt_in_meas_mode < N_TICKS_FOR_MEAS)
			; // Wait for ISR to make measurement

		now_in_meas_mode = 0;        // Inform ISR that we are out of measurement mode
		switch_main_clk_to_ulp32k(); // Switch main clock back to ULP32K
		while ((cnt_not_in_meas_mode < N_TICKS_BETWEEN_MEAS)) {
			// Sleep has already been enabled as part of initialization
			sleep_cpu(); // Sleep while not in measurement mode
		}
	}
	return 0;
}
