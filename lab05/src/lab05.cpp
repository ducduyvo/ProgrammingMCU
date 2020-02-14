/*
 ===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
 ===============================================================================
 */

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#include "ITM_write.h"
#include "DigitalIoPin.h"
#include <atomic>
#include <string.h>
#include <stdio.h>
#include "printer.h"
#include "LiquidCrystal.h"
#endif
#endif

#include <cr_section_macros.h>
#define TICKRATE_HZ1 (1000)
#define PADDING -6

static Printer printer;

// TODO: insert other include files here
void delayMicroseconds(unsigned int us);
static volatile int counter;

//#ifdef __cplusplus
//extern "C" {
//#endif
///**
// * @brief Handle interrupt from SysTick timer
// * @return Nothing
// */
//void SysTick_Handler(void) {
//	if (pointer != NULL)
//		pointer->tick();
//	if (counter > 0)
//		counter--;
//}
//#ifdef __cplusplus
//}
//#endif
//
//void Sleep(int ms) {
//	counter = ms;
//	while (counter > 0) {
//		__WFI();
//	}
//}
// TODO: insert other definitions and declarations here

char* display(int l1, int l2, int l3) {
	char buffer[128];
	char *result;
	snprintf(buffer, 128, "%*s%*s%*s\n"
			"%*s%*s%*s\n", -10, "B1", -10, "B2", -10, "B3", -10,
			(l1 == 0 ? "UP" : "DOWN"),

			-10, (l2 == 0 ? "UP" : "DOWN"), -10, (l3 == 0 ? "UP" : "DOWN"));
	strcpy(result, buffer);
	return result;
}
int main(void) {

#if defined (__USE_LPCOPEN)
// Read clock settings and update SystemCoreClock variable
	SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
// Set up and initialize all required blocks and
// functions related to the board hardware
	Board_Init();
	Chip_RIT_Init(LPC_RITIMER); // initialize RIT (enable clocking etc.)
#endif
#endif
//	Chip_Clock_SetSysTickClockDiv(1);
//	uint32_t sysTickRate = Chip_Clock_GetSysTickClockRate();
//	SysTick_Config(sysTickRate / TICKRATE_HZ1);
//	Chip_RIT_Init(LPC_RITIMER);

	DigitalIoPin rs(0, 8, false, true, false);
	DigitalIoPin en(1, 6, false, true, false);
	DigitalIoPin d4(1, 8, false, true, false);
	DigitalIoPin d5(0, 5, false, true, false);
	DigitalIoPin d6(0, 6, false, true, false);
	DigitalIoPin d7(0, 7, false, true, false);

	DigitalIoPin sw1(0, 17, true, true, true);
	DigitalIoPin sw2(1, 11, true, true, true);
	DigitalIoPin sw3(1, 9, true, true, true);

	LiquidCrystal lcd(&rs, &en, &d4, &d5, &d6, &d7);
	// configure display geometry
	lcd.begin(16, 2);
	// set the cursor to column 0, line 1
	// (note: line 1 is the second row, since counting begins with 0):
	lcd.setCursor(0, 0);
	// Print a message to the LCD.
	//lcd.write('A');
	lcd.print("Sw1  Sw2  Sw3");
	bool l1 = 0;
	bool l2 = 0;
	bool l3 = 0;
	while (true) {
		l1 = sw1.read();
		l2 = sw2.read();
		l3 = sw3.read();
		lcd.setCursor(0, 1);
		lcd.print((l1 == 0 ? "UP   " : "DOWN "));
		lcd.print((l2 == 0 ? "UP   " : "DOWN "));
		lcd.print((l3 == 0 ? "UP   " : "DOWN "));
		delayMicroseconds(250);


	}
}
