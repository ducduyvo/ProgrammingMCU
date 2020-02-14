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
#include <string>
#include "LiquidCrystal.h"
#include "RealTimeClock.h"

#endif
#endif

#include <cr_section_macros.h>
#define TICKRATE_HZ1 (1000) // 1000 ticks per second

static Printer printer;
void delayMicroseconds(unsigned int us);
static volatile int counter;
static volatile int cursor;

extern "C" {
void SysTick_Handler(void) {
	if (counter > 0)
		counter--;
}
}

extern "C" {
void PIN_INT0_IRQHandler(void) {
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH0);
	cursor++;
}
}

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
	Chip_RIT_Init(LPC_RITIMER); // initialize RIT (enable clocking etc.)
#endif
#endif
	Chip_Clock_SetSysTickClockDiv(1);
	uint32_t sysTickRate = Chip_Clock_GetSysTickClockRate();
	SysTick_Config(sysTickRate / TICKRATE_HZ1);
	//Chip_RIT_Init(LPC_RITIMER);
	//Chip_GPIO_Init(LPC_GPIO);

	DigitalIoPin rs(0, 8, false, true, false);
	DigitalIoPin en(1, 6, false, true, false);
	DigitalIoPin d4(1, 8, false, true, false);
	DigitalIoPin d5(0, 5, false, true, false);
	DigitalIoPin d6(0, 6, false, true, false);
	DigitalIoPin d7(0, 7, false, true, false);


	LiquidCrystal lcd(&rs, &en, &d4, &d5, &d6, &d7);
	// configure display geometry
	lcd.begin(16, 2);

	Chip_PININT_Init(LPC_GPIO_PIN_INT);
	DigitalIoPin sw1(0, 17, true, true, false);

	//Configure and enable pin interrupt in the pin interrupt hardware
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_PININT);
	Chip_SYSCTL_PeriphReset(RESET_PININT);

	//Configure pin interrupt 0 generate interrupt when it sees a falling edge on SW1
	Chip_INMUX_PinIntSel(0, 0, 17);
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH0);
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH0);
	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH0);

	//Enable pin interrupt 0 in NVIC
	NVIC_ClearPendingIRQ(PIN_INT0_IRQn);
	NVIC_EnableIRQ(PIN_INT0_IRQn);
	//NVIC_SetPriority(PIN_INT0_IRQn, PININTCH0);

	lcd.setCursor(0, 0);
	lcd.print("Button was pressed:");
	int prev = 0;
	int diff = 0;

	while (1) {

		//later = cursor;
		int current = cursor;

		if (current!=prev) {
			diff = current - prev;
			prev += diff;
			printer.print("Button was pressed: %d\n", diff);
//			char buffer[20];
//			lcd.setCursor(0, 1);
//			snprintf(buffer,20,"%d", later - prev);
//			lcd.print(buffer);
		}
		Sleep(100);


	}
//	DigitalIoPin A0(0,8,false,false,false);
//	A0.write(true);
//	delayMicroseconds(1);
//	A0.write(false);

}
