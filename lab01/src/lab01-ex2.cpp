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
class DigitalIoPin {
public:
	DigitalIoPin(int port, int pin, bool input, bool pullup, bool invert);
	//virtual ~DigitalIoPin();
	bool read();
	//void write(bool value);
private:
	// add these as needed
	int m_pin;
	int m_port;
	bool m_input;
};

DigitalIoPin::DigitalIoPin(int port, int pin, bool input, bool pullup,
		bool invert) {
	m_port = port;
	m_pin = pin;
	m_input = input;

	if (m_input) {
		Chip_GPIO_SetPinDIRInput(LPC_GPIO, m_port, m_pin);
		Chip_IOCON_PinMuxSet(LPC_IOCON, m_port, m_pin,
				((pullup ? IOCON_MODE_PULLUP : 0) | IOCON_DIGMODE_EN
						| (invert ? IOCON_INV_EN : 0)));

	} else {
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, m_port, m_pin);
		Chip_IOCON_PinMuxSet(LPC_IOCON, m_port, m_pin,
				(IOCON_MODE_INACT | IOCON_DIGMODE_EN));

	}

//	if (m_input) {
//		Chip_GPIO_SetPinDIRInput(LPC_GPIO, m_port, m_pin);
//
//	} else {
//		Chip_GPIO_SetPinDIROutput(LPC_GPIO, m_port, m_pin);
//	}
//	if (input && pullup && invert) {
//		Chip_IOCON_PinMuxSet(LPC_IOCON, m_port, m_pin, (IOCON_MODE_PULLUP |
//		IOCON_DIGMODE_EN | IOCON_INV_EN));
//	} else if (input && pullup && !invert) {
//		Chip_IOCON_PinMuxSet(LPC_IOCON, m_port, m_pin, (IOCON_MODE_PULLUP |
//		IOCON_DIGMODE_EN));
//	} else if (input && !pullup && invert) {
//		Chip_IOCON_PinMuxSet(LPC_IOCON, m_port, m_pin, (IOCON_MODE_PULLDOWN |
//		IOCON_DIGMODE_EN | IOCON_INV_EN));
//	} else if (input && !pullup && !invert) {
//		Chip_IOCON_PinMuxSet(LPC_IOCON, m_port, m_pin, (IOCON_MODE_PULLDOWN |
//		IOCON_DIGMODE_EN));
//	} else if (!input) {
//		Chip_IOCON_PinMuxSet(LPC_IOCON, m_port, m_pin,
//				(IOCON_MODE_INACT | IOCON_DIGMODE_EN));
//	}

}
bool DigitalIoPin::read() {
	return Chip_GPIO_GetPinState(LPC_GPIO, m_port, m_pin);
}

void lighting(DigitalIoPin &sw, int &timer, int led){
	if(sw.read() && timer <=0){
		Board_LED_Set(led,true);
		timer = 100;
	}
	else if(sw.read() && timer == 100){ }
	else if(!sw.read() && timer > 0){
		timer--;
	}
	else if(timer <= 0){
		Board_LED_Set(led,false);
	}
}

int main(void) {

	uint32_t sysTickRate;
	SystemCoreClockUpdate();

	Board_Init();

	Chip_Clock_SetSysTickClockDiv(1);
	sysTickRate = Chip_Clock_GetSysTickClockRate();
	SysTick_Config(sysTickRate / TICKRATE_HZ1);


	DigitalIoPin sw1(0, 17, true, true, true);
	DigitalIoPin sw2(1, 11, true, true, true);
	DigitalIoPin sw3(1, 9, true, true, true);

	int t1 = 0, t2 =0, t3 = 0;
	while (1) {
//		if (sw1.read()) {
//			Board_LED_Set(0, true);
//			while (sw1.read()) {
//			}
//			Sleep(1000);
//			Board_LED_Set(0, false);
//		} else if (!sw1.read()) {
//			Board_LED_Set(0, false);
//		}
//		if (sw2.read()) {
//			Board_LED_Set(1, true);
//			Sleep(1000);
//			Board_LED_Set(1, false);
//		} else if (!sw2.read()) {
//			Board_LED_Set(1, false);
//		}
//		if (sw3.read()) {
//			Board_LED_Set(2, true);
//			Sleep(1000);
//			Board_LED_Set(2, false);
//		} else if (!sw3.read()) {
//			Board_LED_Set(2, false);
//		}
		lighting(sw1,t1,0);
		lighting(sw2,t2,1);
		lighting(sw3,t3,2);

		Sleep(10);
	}

}
