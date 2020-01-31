/*
 * MorseCodeSender.cpp
 *
 *  Created on: 26 Jan 2020
 *      Author: DucVoo
 */
#include "MorseCodeSender.h"
#include <ctype.h>
#include <stdio.h>
#include "board.h"
#include <cstdio>
#include "math.h"
#include "LpcUart.h"

void Sleep(int ms);

//LpcPinMap none = { -1, -1 }; // unused pin has negative values in it
//LpcPinMap txpin = { 0, 18 }; // transmit pin that goes to debugger's UART->USB converter
//LpcPinMap rxpin = { 0, 13 }; // receive pin that goes to debugger's UART->USB converter
//LpcUartConfig cfg = { LPC_USART0, 115200, UART_CFG_DATALEN_8
//		| UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1, false, txpin, rxpin, none,
//		none };
//LpcUart dbgu(cfg);

MorseSender::MorseSender(DigitalIoPin *led1, DigitalIoPin *decoder1) {
//	led = new DigitalIoPin(*led1);
//	decoder = new DigitalIoPin(*decoder1);
	led = led1;
	decoder = decoder1;

}

MorseSender::~MorseSender() {
//	delete[] led;
//	delete[] decoder;
}
//void MorseSender::sendCode(const char *text) {
//	int index = 0;
//	//int dot = 10000 / wpm;
//	int result[8];
//	bool word_before = false;
//
//	while (text[index + 1] != '\0') {
//		char c = toupper(text[index]);
//		if (isspace(c)) {
//			delay(7, dot);
//			index++;
//			continue;
//			word_before = true;
//
//		}
//
//		else if (index > 0 && !word_before)
//			delay(2, dot);
//		else
//			word_before = false;
//		unsigned int k = 0;
//		while (ITU_morse[k].symbol != '0') {
//			if (c == ITU_morse[k].symbol) {
//				for (unsigned int n = 0; n < 7; n++) {
//					result[n] = ITU_morse[k].code[n];
//					if (result[n] == 1 && n != 0) {
//						delay(1, dot);
//					} else if (result[n] == 3 && n != 0) {
//						delay(3, dot);
//					} else if (result[n] == 1 && n == 0) {
//						Board_UARTPutChar(' ');
//						delay(4, dot);
//					} else if (result[n] == 3 && n == 0) {
//						Board_UARTPutChar(' ');
//						delay(5, dot);
//					}
//				}
//				break;
//			} else {
//				k++;
//			}
//		}
//		if (ITU_morse[k].symbol == '0') {
//			delay(5, dot);
//			delay(1, dot);
//			delay(1, dot);
//			delay(3, dot);
//		}
//		index++;
//	}
//
//}

//void MorseSender::delay(int number, int dotlength) {
//	if (number == 1) {
//		Sleep(dotlength * 1);
//		Board_UARTPutChar('.');
//		this->decoder->write(true);
//		this->led->write(false);
//		Sleep(dotlength * 1);
//		this->decoder->write(false);
//		this->led->write(true);
//	} else if (number == 3) {
//		Sleep(dotlength * 1);
//		Board_UARTPutChar('_');
//		this->decoder->write(true);
//		this->led->write(false);
//		Sleep(dotlength * 3);
//		this->decoder->write(false);
//		this->led->write(true);
//	} else if (number == 4) {
//		Board_UARTPutChar('.');
//		this->decoder->write(true);
//		this->led->write(false);
//		Sleep(dotlength * 1);
//		this->decoder->write(false);
//		this->led->write(true);
//	} else if (number == 5) {
//		Board_UARTPutChar('_');
//		this->decoder->write(true);
//		this->led->write(false);
//		Sleep(dotlength * 3);
//		this->decoder->write(false);
//		this->led->write(true);
//	} else if (number == 7) {
//		this->decoder->write(false);
//		this->led->write(true);
//		Sleep(dotlength * 1);
//		Board_UARTPutChar('a');
//		Sleep(dotlength * 1);
//		Board_UARTPutChar('a');
//		Sleep(dotlength * 1);
//		Board_UARTPutChar('a');
//		Sleep(dotlength * 1);
//		Board_UARTPutChar('a');
//		Sleep(dotlength * 1);
//		Board_UARTPutChar('a');
//		Sleep(dotlength * 1);
//		Board_UARTPutChar('a');
//		Sleep(dotlength * 1);
//		Board_UARTPutChar('a');
//
//	} else if (number == 2) {
//		this->decoder->write(false);
//		this->led->write(true);
//		Sleep(dotlength * 1);
//		Board_UARTPutChar('|');
//		Sleep(dotlength * 1);
//		Board_UARTPutChar('|');
//		Sleep(dotlength * 1);
//		Board_UARTPutChar('|');
//
//	}
////	this->led->write(true);
////	this->decoder->write(false);
//
//}
//bool MorseSender::command(const char *text) {
//	//set command
//	if (text[0] == 's' && text[1] == 'e' && text[2] == 't') {
//		char setting[30];
//		snprintf(setting, 30, "\n\rwpm: %d\n\rdot length: %d\n\r", this->wpm,
//				this->dot);
//		for (int i = 0; i < 30; i++) {
//			if (setting[i] == '\0') {
//				break;
//			}
//			Board_UARTPutChar(setting[i]);
//		}
//		return false;
//	}
//	//wpm command
//	if (text[0] == 'w' && text[1] == 'p' && text[2] == 'm') {
//
//		return false;
//	}
//	//send command
//	if (text[0] == 's' && text[1] == 'e' && text[2] == 'n' && text[3] == 'd') {
//		return true;
//	}
//	return false;
//}

void MorseSender::sendCode(const char *text) { //Send code function
	int index = 0;
	bool word_before = false;
	while (text[index + 1] != '\0') {
		char c = toupper(text[index]);

		if (isspace(c)) {
			toggle_off(7, dot);
			index++;
			word_before = true;
			continue;
		} else if (index > 0 && !word_before)
			toggle_off(3, dot);
		else
			word_before = false;
		unsigned int k = 0;
//		if (isalpha(c)) {
//			k = c - 65;
//			changing(k);
//		} else if (isdigit(c)) {
//			if (c == '0')
//				k = c - 23;
//			else
//				k = c - 13;
//			changing(k);
//
//		} else {
//			changing(23);
//
//		}
		while (ITU_morse[k].symbol != 0) {
			if (c == ITU_morse[k].symbol) {
				changing(k);
				break;
			}

			else {
				k++;
			}
		}
		if (ITU_morse[k].symbol == 0) {
			changing(23);
		}
		index++;
	}
	//toggle_off(0, dot);

}
void sendCode(const std::string text){
	sendCode(text.c_str());
}

void MorseSender::toggle_on(int number, int dotlength) {
	if (number == 1) {
		Board_UARTPutChar('.');
//		dbgu.write('.');
	} else if (number == 3) {
//		dbgu.write('_');
		Board_UARTPutChar('_');
	}
	this->decoder->write(true);
	this->led->write(false);
	Sleep(number * dotlength);
}
void MorseSender::toggle_off(int number, int dotlength) {
	this->decoder->write(false);
	this->led->write(true);
	Sleep(number * dotlength);
	if (number == 7) {
		Board_UARTPutChar('a');
		Board_UARTPutChar('a');
		Board_UARTPutChar('a');
		Board_UARTPutChar('a');
		Board_UARTPutChar('a');
		Board_UARTPutChar('a');
		Board_UARTPutChar('a');
//		dbgu.write('aaaaaaa');

	}
	if (number == 3) {
//		dbgu.write('|||');
		Board_UARTPutChar('|');
		Board_UARTPutChar('|');
		Board_UARTPutChar('|');
	}
}
void MorseSender::changing(int k) {
	int result[8];
	for (unsigned int n = 0; ITU_morse[k].code[n] != 0; n++) {
		result[n] = ITU_morse[k].code[n];
		if (n > 0) {
			toggle_off(1, dot);
		}
		toggle_on(result[n], dot);
		this->decoder->write(false);
		this->led->write(true);
	}
}
int MorseSender::get_wpm() {
	return wpm;
}
int MorseSender::get_dot() {
	return dot;
}
void MorseSender::set_wpm(int value) {
	wpm = value;
	dot = 1000/wpm;
}

