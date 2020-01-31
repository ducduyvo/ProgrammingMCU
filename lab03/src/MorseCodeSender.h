/*
 * MorseCode.h
 *
 *  Created on: 26 Jan 2020
 *      Author: DucVoo
 */

#ifndef MORSECODESENDER_H_
#define MORSECODESENDER_H_
#include "DigitalIoPin.h"
#include <string>

struct MorseCode {
	char symbol;
	unsigned char code[7];
};
const int DOT = 1;
const int DASH = 3;
const MorseCode ITU_morse[] = { { 'A', { DOT, DASH } }, // A
		{ 'B', { DASH, DOT, DOT, DOT } }, // B
		{ 'C', { DASH, DOT, DASH, DOT } }, // C
		{ 'D', { DASH, DOT, DOT } }, // D
		{ 'E', { DOT } }, // E
		{ 'F', { DOT, DOT, DASH, DOT } }, // F
		{ 'G', { DASH, DASH, DOT } }, // G
		{ 'H', { DOT, DOT, DOT, DOT } }, // H
		{ 'I', { DOT, DOT } }, // I
		{ 'J', { DOT, DASH, DASH, DASH } }, // J
		{ 'K', { DASH, DOT, DASH } }, // K
		{ 'L', { DOT, DASH, DOT, DOT } }, // L
		{ 'M', { DASH, DASH } }, // M
		{ 'N', { DASH, DOT } }, // N
		{ 'O', { DASH, DASH, DASH } }, // O
		{ 'P', { DOT, DASH, DASH, DOT } }, // P
		{ 'Q', { DASH, DASH, DOT, DASH } }, // Q
		{ 'R', { DOT, DASH, DOT } }, // R
		{ 'S', { DOT, DOT, DOT } }, // S
		{ 'T', { DASH } }, // T
		{ 'U', { DOT, DOT, DASH } }, // U
		{ 'V', { DOT, DOT, DOT, DASH } }, // V
		{ 'W', { DOT, DASH, DASH } }, // W
		{ 'X', { DASH, DOT, DOT, DASH } }, // X
		{ 'Y', { DASH, DOT, DASH, DASH } }, // Y
		{ 'Z', { DASH, DASH, DOT, DOT } }, // Z
		{ '1', { DOT, DASH, DASH, DASH, DASH } }, // 1
		{ '2', { DOT, DOT, DASH, DASH, DASH } }, // 2
		{ '3', { DOT, DOT, DOT, DASH, DASH } }, // 3
		{ '4', { DOT, DOT, DOT, DOT, DASH } }, // 4
		{ '5', { DOT, DOT, DOT, DOT, DOT } }, // 5
		{ '6', { DASH, DOT, DOT, DOT, DOT } }, // 6
		{ '7', { DASH, DASH, DOT, DOT, DOT } }, // 7
		{ '8', { DASH, DASH, DASH, DOT, DOT } }, // 8
		{ '9', { DASH, DASH, DASH, DASH, DOT } }, // 9
		{ '0', { DASH, DASH, DASH, DASH, DASH } }, // 0
		{ 0, { 0 } } // terminating entry - Do not remove!
};

class MorseSender {
private:
	int wpm =30;
	int dot= 1000/wpm;
	DigitalIoPin *decoder;
	DigitalIoPin *led;
public:
	MorseSender(DigitalIoPin *led1, DigitalIoPin *decoder1);
	virtual ~MorseSender();
	void sendCode(const char *text);
	void sendCode(const std::string text);
	bool command(const char *text);
	void delay(int number, int dotlength);
	void toggle_on(int number, int dotlength);
	void toggle_off(int number, int dotlength);
	void changing(int k);
	int get_wpm();
	int get_dot();
	void set_wpm(int value);
};

#endif /* MORSECODESENDER_H_ */
