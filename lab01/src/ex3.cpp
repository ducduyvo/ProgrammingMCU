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
#endif
#endif

#include <cr_section_macros.h>
#define MAXLEDS 7
#define TICKRATE_HZ1 (1000)

static const uint8_t diceport[] = { 0, 1, 0, 0, 0, 0, 1 };
static const uint8_t dicepins[] = { 0, 3, 16, 10, 9, 29, 9 };
#include <atomic>
static volatile std::atomic_int counter;
#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Handle interrupt from SysTick timer
 * @return Nothing
 */
void SysTick_Handler(void) {
	if (counter >= 7)
		counter = 0;
	counter ++;
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

// TODO: insert other include files here

// TODO: insert other definitions and declarations here
void allOn() {
	int idx;
	for (idx = 0; idx < MAXLEDS; idx++) {
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, diceport[idx], dicepins[idx]);
		Chip_GPIO_SetPinState(LPC_GPIO, diceport[idx], dicepins[idx], true);
	}
}

//Turn off all the LEDs
void allOff() {
	int idx;
	for (idx = 0; idx < MAXLEDS; idx++) {
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, diceport[idx], dicepins[idx]);
		Chip_GPIO_SetPinState(LPC_GPIO, diceport[idx], dicepins[idx], false);
	}
}
class Dice {
public:
	Dice();
//virtual ~Dice();
	void Show(int number);
private:
	/* you need to figure out yourself the private members that you need */
};

Dice::Dice() {
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 8,
			(IOCON_MODE_PULLUP | IOCON_DIGMODE_EN | IOCON_INV_EN));
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, 0, 8);
	Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 6,
			(IOCON_MODE_PULLUP | IOCON_DIGMODE_EN | IOCON_INV_EN));
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, 1, 6);
	allOff();
}
void Dice::Show(int number) {
	if (number == 0) {
		allOff();
	} else if (number == 1) {
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 10);	//PD4
		Chip_GPIO_SetPinState(LPC_GPIO, 0, 10, true);
	} else if (number == 2) {
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 0);	//PD7
		Chip_GPIO_SetPinState(LPC_GPIO, 0, 0, true);
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 9);	//PD3
		Chip_GPIO_SetPinState(LPC_GPIO, 0, 9, true);
	} else if (number == 3) {
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 10);	//PD4
		Chip_GPIO_SetPinState(LPC_GPIO, 0, 10, true);
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 0);	//PD7
		Chip_GPIO_SetPinState(LPC_GPIO, 0, 0, true);
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 9);	//PD3
		Chip_GPIO_SetPinState(LPC_GPIO, 0, 9, true);
	} else if (number == 4) {
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 0);	//PD7
		Chip_GPIO_SetPinState(LPC_GPIO, 0, 0, true);
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 9);	//PD1
		Chip_GPIO_SetPinState(LPC_GPIO, 1, 9, true);
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 9);	//PD3
		Chip_GPIO_SetPinState(LPC_GPIO, 0, 9, true);
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 16);	//PD5
		Chip_GPIO_SetPinState(LPC_GPIO, 0, 16, true);
	} else if (number == 5) {
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 0);	//PD7
		Chip_GPIO_SetPinState(LPC_GPIO, 0, 0, true);
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 9);	//PD1
		Chip_GPIO_SetPinState(LPC_GPIO, 1, 9, true);
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 9);	//PD3
		Chip_GPIO_SetPinState(LPC_GPIO, 0, 9, true);
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 16);	//PD5
		Chip_GPIO_SetPinState(LPC_GPIO, 0, 16, true);
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 10);	//PD4
		Chip_GPIO_SetPinState(LPC_GPIO, 0, 10, true);
	} else if (number == 6) {
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 0);	//PD7
		Chip_GPIO_SetPinState(LPC_GPIO, 0, 0, true);
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 9);	//PD1
		Chip_GPIO_SetPinState(LPC_GPIO, 1, 9, true);
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 9);	//PD3
		Chip_GPIO_SetPinState(LPC_GPIO, 0, 9, true);
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 16);	//PD5
		Chip_GPIO_SetPinState(LPC_GPIO, 0, 16, true);
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 3);	//PD6
		Chip_GPIO_SetPinState(LPC_GPIO, 1, 3, true);
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 29);	//PD2
		Chip_GPIO_SetPinState(LPC_GPIO, 0, 29, true);
	}
}

int main(void) {
	uint32_t sysTickRate;
	SystemCoreClockUpdate();

	Board_Init();

	Chip_Clock_SetSysTickClockDiv(1);
	sysTickRate = Chip_Clock_GetSysTickClockRate();
	SysTick_Config(sysTickRate / TICKRATE_HZ1);

	Dice dice;
	volatile int n;
	while (1) {
		if ((Chip_GPIO_GetPinState(LPC_GPIO, 0, 8))
				&& (Chip_GPIO_GetPinState(LPC_GPIO, 1, 6) == false)) {
			n = 0;
			allOn();
		} else if (Chip_GPIO_GetPinState(LPC_GPIO, 0, 8) == false) {
			allOff();
			if (Chip_GPIO_GetPinState(LPC_GPIO, 1, 6)) {
				n = counter;
			} else if (Chip_GPIO_GetPinState(LPC_GPIO, 1, 6) == false) {
				dice.Show(n);
			}
		}
	}

}
