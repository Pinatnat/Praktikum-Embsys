/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include <stdio.h>
#include <ioport.h>
#include <board.h>


int lamp0 = 0;
int lamp1 = 0;
int lamp2 = 0;
int counter = 0;
uint16_t result = 0;
uint16_t result2 = 0;
static char strbuf[201];

static void kedip(void){
	if(lamp1==0) {
		gpio_set_pin_high(LED1_GPIO);
		lamp1 = 1;
		} else {
		gpio_set_pin_low(LED1_GPIO);
		lamp1 = 0;
	}
}

static void kedip2(void){
	if(lamp2==0) {
		gpio_set_pin_high(LED0_GPIO);
		lamp2 = 1;
		} else {
		gpio_set_pin_low(LED0_GPIO);
		lamp2 = 0;
	}
}

static void kedip3(void){
	if(lamp0==0) {
		gpio_set_pin_high(LED2_GPIO);
		ioport_set_pin_level(J1_PIN0, 1);
		lamp0 = 1;
		} else {
		gpio_set_pin_low(LED2_GPIO);
		ioport_set_pin_level(J1_PIN0, 0);
		lamp0 = 0;
	}
}

static void tulis(void){
		counter = counter + 1;
		snprintf(strbuf, sizeof(strbuf), "Counter: %d times  ", counter);
		gfx_mono_draw_string("Praktikum 3 - interrupt", 0, 0, &sysfont);
		gfx_mono_draw_string("Nadya A. - 1906398566", 0, 8, &sysfont);
		gfx_mono_draw_string(strbuf, 0, 16, &sysfont);
}

void setup_timer0(void){
	
	tc_enable(&TCD0);
	tc_set_overflow_interrupt_callback(&TCD0,kedip);
	tc_set_wgm(&TCD0, TC_WG_NORMAL);
	tc_write_period(&TCD0, 2000);
	tc_set_overflow_interrupt_level(&TCD0, TC_INT_LVL_LO);
	tc_write_clock_source(&TCD0, TC_CLKSEL_DIV1024_gc);
	
	cpu_irq_enable();
}

void setup_timer1(void){
	
	tc_enable(&TCD1);
	tc_set_overflow_interrupt_callback(&TCD1,kedip2);
	tc_set_wgm(&TCD1, TC_WG_NORMAL);
	tc_write_period(&TCD1, 1000);
	tc_set_overflow_interrupt_level(&TCD1, TC_INT_LVL_LO);
	tc_write_clock_source(&TCD1, TC_CLKSEL_DIV1024_gc);
	
	cpu_irq_enable();
}

void setup_timer3(void){
	
	tc_enable(&TCE0);
	tc_set_overflow_interrupt_callback(&TCE0,kedip3);
	tc_set_wgm(&TCE0, TC_WG_NORMAL);
	tc_write_period(&TCE0, 500);
	tc_set_overflow_interrupt_level(&TCE0, TC_INT_LVL_LO);
	tc_write_clock_source(&TCE0, TC_CLKSEL_DIV1024_gc);
	
	cpu_irq_enable();
}

void setup_timer4(void){
	
	tc_enable(&TCE1);
	tc_set_overflow_interrupt_callback(&TCE1,tulis);
	tc_set_wgm(&TCE1, TC_WG_NORMAL);
	tc_write_period(&TCE1, 1000);
	tc_set_overflow_interrupt_level(&TCE1, TC_INT_LVL_LO);
	tc_write_clock_source(&TCE1, TC_CLKSEL_DIV1024_gc);
	
	cpu_irq_enable();
}
int main (void)
{
	/* Insert system clock initialization code here (sysclk_init()). */
	//init board
	board_init();
	
	sysclk_init();
	pmic_init();
		
	//init lcd
	gfx_mono_init();
	
	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_RTC);
	while (RTC32.SYNCCTRL & RTC32_SYNCBUSY_bm) {
		// Wait for RTC32 sysclk to become stable
	}
		
			
	//set background lcd on
	gpio_set_pin_high(LCD_BACKLIGHT_ENABLE_PIN);
	ioport_set_pin_dir(J1_PIN0, IOPORT_DIR_OUTPUT);
	
	setup_timer0();
	setup_timer1();
	setup_timer3();
	setup_timer4();
	
	
	
	// Workaround for known issue: Enable RTC32 sysclk

	while(1){
	
	}
	

	/* Insert application code here, after the board has been initialized. */
}