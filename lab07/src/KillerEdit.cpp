/*
 * KillerEdit.cpp
 *
 *  Created on: 27 Feb 2020
 *      Author: DucVoo
 */

#include "KillerEdit.h"

KillerEdit::KillerEdit()
{
}
void KillerEdit::KillerForSystem()
{
	LPC_WWDT->FEED = 0xAA;
	LPC_WWDT->FEED = 0xAA;

}
