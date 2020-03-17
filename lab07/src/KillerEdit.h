/*
 * KillerEdit.h
 *
 *  Created on: 27 Feb 2020
 *      Author: DucVoo
 */

#ifndef KILLEREDIT_H_
#define KILLEREDIT_H_

#if defined(__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"

#else
#include "board.h"

#endif
#endif
#include "SimpleMenu.h"

class KillerEdit {
public:
	KillerEdit();
	void KillerForSystem();
};

#endif /* KILLEREDIT_H_ */
