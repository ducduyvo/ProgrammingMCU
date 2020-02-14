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
#include <ctime>
#include "printer.h"
#include "LiquidCrystal.h"
#include "RealTimeClock.h"

#endif
#endif

#include <cr_section_macros.h>
#define TICKRATE_HZ1 (10000) // 10000 ticks per second
#define PADDING -6

static Printer printer;

// TODO: insert other include files here
void delayMicroseconds(unsigned int us);
static volatile int counter;
static RealTimeClock *pointer;

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Handle interrupt from SysTick timer
 * @return Nothing
 */
void SysTick_Handler(void) {
	if (pointer != NULL)
		pointer->Tick(); // 10000 times per tick
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
// TODO: insert other definitions and declarations here


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
	Chip_Clock_SetSysTickClockDiv(1);
	uint32_t sysTickRate = Chip_Clock_GetSysTickClockRate();
	SysTick_Config(sysTickRate / TICKRATE_HZ1);
	//Chip_RIT_Init(LPC_RITIMER);

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
	char buffer[32];

	time_t t = time(0);
	struct tm * now = localtime(&t);
	int num[3];


	printer.print(__TIME__);

	sscanf(__TIME__, "%i:%i:%i", &num[0],&num[1], &num[2]);



	static RealTimeClock clockk(10000,num[0],num[1],num[2]);
	pointer = &clockk;

	while (1) {
		lcd.setCursor(0, 0);

		clockk.gettime(now);
		sniprintf(buffer, 32, "%s%d:%s%d:%s%d", (now->tm_hour < 10 ? " " : ""),
				now->tm_hour, (now->tm_min < 10 ? "0" : ""), now->tm_min,
				(now->tm_sec < 10 ? "0" : ""), now->tm_sec);
		lcd.print(buffer);


	}

}

