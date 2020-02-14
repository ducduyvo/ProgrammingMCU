/*
 * DigitalIoPin.cpp
 *
 *  Created on: 22 Jan 2020
 *      Author: DucVoo
 */
#include"DigitalIoPin.h"
#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

DigitalIoPin::DigitalIoPin(int port_, int pin_, bool input_, bool pullup_,
		bool invert_) :
		port(port_), pin(pin_), input(input_), pullup(pullup_), invert(invert_) {
	Chip_IOCON_PinMuxSet(LPC_IOCON, port, pin,
			(IOCON_DIGMODE_EN | ((pullup) ? (IOCON_MODE_PULLUP) : (0))
					| ((invert) ? (IOCON_INV_EN) : (0))));
	if (input) {
		Chip_GPIO_SetPinDIRInput(LPC_GPIO, port_, pin_);
	} else {
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, port_, pin_);
	}
}

bool DigitalIoPin::read() {
	return Chip_GPIO_GetPinState(LPC_GPIO, port, pin);
}
void DigitalIoPin::toggle() {
	 Chip_GPIO_SetPinToggle(LPC_GPIO, port, pin);
}
void DigitalIoPin::write(bool value) {
	if(value){
		Chip_GPIO_SetPinState(LPC_GPIO, port, pin, !invert);
	}
	else{
		Chip_GPIO_SetPinState(LPC_GPIO, port, pin, invert);
	}
}
