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
#define TICKRATE_HZ1 (1000)
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

// TODO: insert other include files here

// TODO: insert other definitions and declarations here

int main(void) {


	uint32_t sysTickRate;
	SystemCoreClockUpdate();

	Board_Init();

	Chip_Clock_SetSysTickClockDiv(1);
	sysTickRate = Chip_Clock_GetSysTickClockRate();
	SysTick_Config(sysTickRate / TICKRATE_HZ1);

	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 17,
			(IOCON_MODE_PULLUP | IOCON_DIGMODE_EN | IOCON_INV_EN));
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, 0, 17);

	while (1) {
		if (Chip_GPIO_GetPinState(LPC_GPIO, 0, 17)) {
			//Red
			Board_LED_Set(0, true);
			Sleep(1000);
			Board_LED_Set(0, false);

			//Green
			Board_LED_Set(1, true);
			Sleep(1000);
			Board_LED_Set(1, false);

			//Blue
			Board_LED_Set(2, true);
			Sleep(1000);
			Board_LED_Set(2, false);
		}
	}

}
