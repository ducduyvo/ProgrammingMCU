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

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>

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

	DigitalIoPin sw1(0, 17, true, true, true);
	DigitalIoPin sw2(1, 11, true, true, true);
	DigitalIoPin sw3(1, 9, true, true, true);
	DigitalIoPin led(0, 25, false, true, true);

	Board_UARTPutSTR("\r\nPress any button to get upper case....\r\n");
	Board_UARTPutChar('\r');
	Board_UARTPutChar('\n');
	int c;
	while (1) { // echo back what we receive
		if (sw1.read() || sw2.read() || sw3.read()) {
			led.toggle();

			while (sw1.read() || sw2.read() || sw3.read())
				;
		}
		c = Board_UARTGetChar();
		if (c != EOF) {
			if (c == '\n')
				Board_UARTPutChar('\r'); // precede linefeed with carriage return
			else if (led.read()) {
				Board_UARTPutChar(toupper(c));
			} else if (!led.read())
				Board_UARTPutChar(c);
		}
		if (c == '\r')
			Board_UARTPutChar('\n'); // send line feed after carriage return
	}
//		} else if ((sw1.read() || sw2.read() || sw3.read()) && Board_LED_Test(0) ) {
//			Board_LED_Set(0, false);
//
//			c = Board_UARTGetChar();
//			if (c != EOF) {
//				if (c == '\n')
//					Board_UARTPutChar('\r'); // precede linefeed with carriage return
//				Board_UARTPutChar(tolower(c));
//				if (c == '\r')
//					Board_UARTPutChar('\n'); // send line feed after carriage return
//			}
//		}
//	}
	return 0;
}
