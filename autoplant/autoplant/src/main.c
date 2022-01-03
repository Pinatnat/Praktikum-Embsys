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
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

/*
This code is based on implementation from Embedded system labs

*/
#include <asf.h>
#include <stdio.h>
#include <string.h>
#include <ioport.h>
#include <board.h>
#include <adc_sensors/adc_sensors.h>

#define USART_SERIAL_EXAMPLE             &USARTC0
#define USART_SERIAL_EXAMPLE_BAUDRATE    9600
#define USART_SERIAL_CHAR_LENGTH         USART_CHSIZE_8BIT_gc
#define USART_SERIAL_PARITY              USART_PMODE_DISABLED_gc
#define USART_SERIAL_STOP_BIT            false

static char reads[100];
static char strbuf[128];

/*
Based from lab 4 USART
*/
void setUpSerial()
{
	// Baud rate selection
	// BSEL = (2000000 / (2^0 * 16*9600) -1 = 12.0208... ~ 12 -> BSCALE = 0
	// FBAUD = ( (2000000)/(2^0*16(12+1)) = 9615.384 -> mendekati lah ya
	
	USARTC0_BAUDCTRLB = 0; //memastikan BSCALE = 0
	USARTC0_BAUDCTRLA = 0x0C; // 12
	
	//USARTC0_BAUDCTRLB = 0; //Just to be sure that BSCALE is 0
	//USARTC0_BAUDCTRLA = 0xCF; // 207
	
	//Disable interrupts, just for safety
	USARTC0_CTRLA = 0;
	//8 data bits, no parity and 1 stop bit
	USARTC0_CTRLC = USART_CHSIZE_8BIT_gc;
	
	//Enable receive and transmit
	USARTC0_CTRLB = USART_TXEN_bm | USART_RXEN_bm;
}

void sendChar(char c) {
	while( !(USARTC0_STATUS & USART_DREIF_bm) ); //Wait until DATA buffer is empty
	USARTC0_DATA = c;
}

void sendString(char *text) {
	while(*text) {
		sendChar(*text++);
		//usart_putchar(USART_SERIAL_EXAMPLE, *text++);
		delay_ms(1);
	}
}

char receiveChar() {
	while (!(USARTC0_STATUS & USART_RXCIF_bm)); //Wait until receive finish
	return USARTC0_DATA;
}

void receiveString() {
	int i = 0;
	while(1){
		char inp = receiveChar();
		//char inp = usart_getchar(USART_SERIAL_EXAMPLE);
		if(inp=='\n') break;
		else reads[i++] = inp;
		delay_ms(1);
	}
	reads[i] = '\0';
}

int main (void) {
	// Insert system clock initialization code here (sysclk_init()).

	board_init();
	sysclk_init();
	gfx_mono_init();
	adc_sensors_init();
	
	// Inisialisasi interrupt vector
	pmic_init();
	cpu_irq_enable();
	
	gpio_set_pin_high(LCD_BACKLIGHT_ENABLE_PIN);
	ioport_set_pin_dir(J1_PIN0, IOPORT_DIR_OUTPUT);
	
	PORTC_OUTSET = PIN3_bm; // PC3 as TX
	PORTC_DIRSET = PIN3_bm; //TX pin as output
	
	PORTC_OUTCLR = PIN2_bm; //PC2 as RX
	PORTC_DIRCLR = PIN2_bm; //RX pin as input
	
	setUpSerial();
	
	static usart_rs232_options_t USART_SERIAL_OPTIONS = {
		.baudrate = USART_SERIAL_EXAMPLE_BAUDRATE,
		.charlength = USART_SERIAL_CHAR_LENGTH,
		.paritytype = USART_SERIAL_PARITY,
		.stopbits = USART_SERIAL_STOP_BIT
	};
	
	usart_init_rs232(USART_SERIAL_EXAMPLE, &USART_SERIAL_OPTIONS);
	
	gfx_mono_draw_string("loading ...", 0, 0, &sysfont);
	gfx_mono_draw_string(".       ",0, 8, &sysfont);
	delay_ms(500);
	gfx_mono_draw_string("..      ",0, 8, &sysfont);
	delay_ms(500);
	gfx_mono_draw_string("...     ",0, 8, &sysfont);
	delay_s(2);
	
	//toggle flag 1=low, 0=high
	int flagLed0=1;
	
	// Variable untuk sampling light sensor
	uint32_t intensity = 0;
	uint8_t iterations = 0;
	#define LIGHTSENSOR_NUM_SAMPLES 20
	sendString("1");

	while (1) {
		
		receiveString();
		gfx_mono_draw_string("Automatic Plant Care", 0, 0, &sysfont);
		
		// check code sent from arduino
		if (strcmp("1",reads)==0){
			//toogle buzzer only low water
			ioport_set_pin_level(J1_PIN0, 1); //set high
			gfx_mono_draw_string("                 ",0, 8, &sysfont);
			gfx_mono_draw_string("> low water",0, 8, &sysfont);
		}
		else if (strcmp("2",reads)==0){
			//toogle buzzer & print hot
			ioport_set_pin_level(J1_PIN0, 1); //set high
			gfx_mono_draw_string("                 ",0, 8, &sysfont);
			gfx_mono_draw_string("> low water & hot",0, 8, &sysfont);
		}
		else if (strcmp("3",reads)==0) {
			//off buzzer
			ioport_set_pin_level(J1_PIN0, 0); //set low
			gfx_mono_draw_string("                 ",0, 8, &sysfont);
			gfx_mono_draw_string("> hot",0, 8, &sysfont);
		}
		else if (strcmp("0",reads)==0) {
			//off buzzer
			ioport_set_pin_level(J1_PIN0, 0); //set low
			gfx_mono_draw_string("                 ",0, 8, &sysfont);
			gfx_mono_draw_string("> good",0, 8, &sysfont);
		}
		
		// code used from lab 2
		lightsensor_measure();									// Mengambil data dari pengukuran intensitas oleh light sensor
		while(!lightsensor_data_is_ready());					// Menunggu data sampai siap untuk ditampilkan
		intensity += lightsensor_get_raw_value();				// Mengambil hasil olah data dalam raw ADC value

		// Dikarenakan hasil yang diperoleh merupakan data raw diperlukan sampling agar mendapatkan hasil yang baik
		if(iterations++ >= LIGHTSENSOR_NUM_SAMPLES) {
			iterations = 0;
			intensity /= LIGHTSENSOR_NUM_SAMPLES;
			
			if (intensity < 60){
				flagLed0 = 0;
				gfx_mono_draw_string("low light     ",0, 16, &sysfont);
			}
			else {
				flagLed0 = 1;
				gfx_mono_draw_string("lighting good ",0, 16, &sysfont);
			}
			
			ioport_set_pin_level(LED0_GPIO,flagLed0);
			intensity = 0;
		}
		
		delay_ms(500);
		
		
	}
	
}
