/*
 ===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
 ===============================================================================
 */
#include <stdio.h>
#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */
#include "stdlib.h"
#include "DigitalIoPin.h"
#include <cctype>
#include "MorseCodeSender.h"
#include "LpcUart.h"
#include <string.h>
#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>
#define TICKRATE_HZ1 (1000)

// TODO: insert other include files here

// TODO: insert other definitions and declarations here

#include <atomic>
static volatile std::atomic_int counter;
#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief
 Handle interrupt from SysTick timer
 * @return
 Nothing
 */
void SysTick_Handler(void) {
	if (counter > 0)
		counter--;
}
#ifdef __cplusplus
}
#endif
void Sleep(int ms) {
	counter = ms;
	while (counter > 0) {
		__WFI();
	}
}
int main(void) {

#if defined (__USE_LPCOPEN)
	// Read clock settings and update SystemCoreClock variable
	SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
	// Set up and initialize all required blocks and
	// functions related to the board hardware
	Board_Init();
	// Set the LED to the state of "On"
#endif
#endif
	uint32_t sysTickRate;
	Chip_Clock_SetSysTickClockDiv(1);
	sysTickRate = Chip_Clock_GetSysTickClockRate();
	SysTick_Config(sysTickRate / TICKRATE_HZ1);

	DigitalIoPin led(0, 25, false, true, false);
	DigitalIoPin decoder(0, 8, false, true, false);
	MorseSender morse(&led, &decoder);

	LpcPinMap none = { -1, -1 }; // unused pin has negative values in it
	LpcPinMap txpin = { 0, 18 }; // transmit pin that goes to debugger's UART->USB converter
	LpcPinMap rxpin = { 0, 13 }; // receive pin that goes to debugger's UART->USB converter
	LpcUartConfig cfg = { LPC_USART0, 115200, UART_CFG_DATALEN_8
			| UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1, false, txpin, rxpin,
			none, none };
	LpcUart dbgu(cfg);

//	int wordNumber = 0;
//	int c;
//	char text[83];
//
//	while(1) {
//
//			c = Board_UARTGetChar();
//			if((c != EOF)){
//				Board_UARTPutChar(c);
//				text[wordNumber] = c;
//				if((c == '\r')|(c=='\n')|(wordNumber >= 82)){
//						Board_UARTPutChar('\n');
//						text[wordNumber+2] = '\0';
//						morse.sendCode(text);
//				}
//				wordNumber++;
//
//			}
//	    }



	dbgu.write("Type Command:\r\n");

	char text[81];
	char c;
	char *result;
	int index = 0;

	while (1) {
		if (dbgu.read(c) > 0) {
			if (c == '\n')
				dbgu.write('\r'); // precede linefeed with carriage return
			dbgu.write(c);
			if (c == '\r')
				dbgu.write('\n'); // send line feed after carriage return

			if ((c == '\n') | (c == '\r') | (index >= 80)) {

				// change the word per minute
				if ((result = strstr(text, "wpm ")) != nullptr) {
					int number = 0;
					result[index] = '\0';
					if (sscanf(result, "wpm %d", &number) == 1) {
						if (number > 0) {
							dbgu.write("\r\n");
							char wpm[16];
							snprintf(wpm, 16, "%d", number);
							dbgu.write("The wpm changed to ");
							dbgu.write(wpm);
							morse.set_wpm(number);
							dbgu.write("\r\n");
							dbgu.write("\r\n");

						}
					}
				}
				// print the setting of wpm
				else if ((result = strstr(text, "set")) != nullptr) {
					char setting[30];
					snprintf(setting, 30, "\r\nwpm: %d\n\rdot length: %d\r\n",
							morse.get_wpm(), morse.get_dot());
					for (int i = 0; i < 30; i++) {
						if (setting[i] == '\0') {
							break;
						}
						dbgu.write(setting[i]);

					}
					dbgu.write("\r\n");

				}
				// send the morse code
				else if ((result = strstr(text, "send ")) != nullptr) {
					text[index + 1] = '\0';
					morse.sendCode(text + 5);
					dbgu.write("\r\n\r\n");

				}
				// stop program
				else if ((result = strstr(text, "stop")) != nullptr) {
					break;
				}
				index = 0;
				dbgu.write("Type Command:\r\n");

			} else {
				text[index] = c;
				++index;
			}
		}
	}

}

