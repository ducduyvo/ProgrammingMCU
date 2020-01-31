#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#include "ITM_write.h"
#include "DigitalIoPin.h"
#endif
#endif

#include <cr_section_macros.h>
#include <cstdio>

//class DigitalIoPin {
//public:
//	DigitalIoPin(int port, int pin, bool input = true, bool pullup = true,
//			bool invert = false);
//	/* virtual ~DigitalIoPin(); */
//	bool read();
//	void write(bool value);
//private:
//	int port;
//	int pin;
//	bool input;
//	bool pullup;
//	bool invert;
//};
//
//DigitalIoPin::DigitalIoPin(int port_, int pin_, bool input_, bool pullup_,
//		bool invert_) :
//		port(port_), pin(pin_), input(input_), pullup(pullup_), invert(invert_) {
//	Chip_IOCON_PinMuxSet(LPC_IOCON, port, pin,
//			(IOCON_DIGMODE_EN | ((pullup) ? (IOCON_MODE_PULLUP) : (0))
//					| ((invert) ? (IOCON_INV_EN) : (0))));
//	if (input) {
//		Chip_GPIO_SetPinDIRInput(LPC_GPIO, port_, pin_);
//	} else {
//		Chip_GPIO_SetPinDIROutput(LPC_GPIO, port_, pin_);
//	}
//}
//
//bool DigitalIoPin::read() {
//	return Chip_GPIO_GetPinState(LPC_GPIO, port, pin);
//}
class Menu {
public:
	Menu();
	void print_menu(char *buffer, int l1, int l2, int l3, int led);
	void turn(int l1, int l2, int l3);
	void increase();
	void decrease();
private:
	int cursor = 0;
};

Menu::Menu() {
	ITM_init();
}
Menu::increase(){
	cursor++;
	if(cursor > 2){
		cursor =0;
	}
}
Menu::decrease(){
	cursor--;
	if(cursor < 1){
		cursor = 2;
	}
}
void Menu::print_menu(char *buffer, int l1, int l2, int l3, int led) {
	snprintf(buffer, 256,
			"Select led:\nRed\t\t%s%s\nGreen\t%s%s\nBlue\t%s%s\n\n",
			(l1 == 0 ? "OFF" : "ON"), (led == 1 ? " <--" : ""),
			(l2 == 0 ? "OFF" : "ON"), (led == 2 ? " <--" : ""),
			(l3 == 0 ? "OFF" : "ON"), (led == 3 ? " <--" : ""));
	ITM_write(buffer);
}
void Menu::turn(int l1, int l2, int l3) {
	if (l1 == 1) {
		Board_LED_Set(0, true);
	} else {
		Board_LED_Set(0, false);
	}

	if (l2 == 1) {
		Board_LED_Set(1, true);
	} else {
		Board_LED_Set(1, false);
	}

	if (l3 == 1) {
		Board_LED_Set(2, true);
	} else {
		Board_LED_Set(2, false);
	}
}

int main(void) {

	SystemCoreClockUpdate();
	Board_Init();

	DigitalIoPin sw1(0, 17, true, true, true);
	DigitalIoPin sw2(1, 11, true, true, true);
	DigitalIoPin sw3(1, 9, true, true, true);

	Menu menu;

	int n = 0;
	int led = 2;
	int l1 = 0;
	int l2 = 0;
	int l3 = 0;
	char buffer[256];

	//ITM_write(buffer);
	menu.print_menu(buffer, l1, l2, l3, led);
	while (true) {
		if (sw1.read() && n == 0) {
			n = 1;					//Receive that sw1 is pressed
			if (led == 1) {
				led = 3;
			} else {
				led--;
			}
		} else if (sw2.read() && n == 0) {
			n = 2;					//Receive that sw2 is pressed
			if (led == 1) {
				l1 = (l1 == 0) ? 1 : 0; //Set light for red
			} else if (led == 2) {
				l2 = (l2 == 0) ? 1 : 0; //Set light for Green
			} else if (led == 3) {
				l3 = (l3 == 0) ? 1 : 0; //Set light for Blue
			}
		} else if (sw3.read() && n == 0) {
			n = 3;					////Receive that sw3 is pressed
			if (led == 3) {
				led = 1;
			} else {
				led++;
			}
		} else if (!sw1.read() && !sw2.read() && !sw3.read()) {
			if (n == 1) {
				menu.print_menu(buffer, l1, l2, l3, led);
			} else if (n == 2) {
				menu.print_menu(buffer, l1, l2, l3, led);
				menu.turn(l1, l2, l3);

			} else if (n == 3) {
				menu.print_menu(buffer, l1, l2, l3, led);
			}
			n = 0;
		}

	}
}
