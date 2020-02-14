/*
 * RealTimeClock.cpp
 *
 *  Created on: 6 Feb 2020
 *      Author: DucVoo
 */

#include "RealTimeClock.h"
#include "LiquidCrystal.h"

#include <mutex>
#include "Imutex.h"
#ifndef REALTIMECLOCK_H
#define REALTIMECLOCK_H

void RealTimeClock::Tick() {

	tick++;
	if(tick >= tps){
		sec++;
		tick = 0;
	}
	if (sec >= 60) {
		sec = 0;
		min++;
	}
	if (min >= 60) {
		min = 0;
		hour++;
	}
	if (hour >= 24)
		hour = 0;
}

int RealTimeClock::GetTick() {
	return tick;
}

RealTimeClock::RealTimeClock(int ticksPerSecond) {
	hour = 23;
	min = 58;
	sec = 35;
	tps = ticksPerSecond;
}

RealTimeClock::RealTimeClock(int ticksPerSecond, tm *now) {
	now->tm_hour = hour;
	now->tm_min = min;
	now->tm_sec = sec;
	tps = ticksPerSecond;
}
RealTimeClock::RealTimeClock(int ticksPerSecond, int h, int m, int s) {
	hour = h;
	min = m;
	sec = s;
	tps = ticksPerSecond;
}

// Get current time
void RealTimeClock::gettime(tm *now) {
	std::lock_guard<Imutex> lock(guard);
	now->tm_hour = hour;
	now->tm_min = min;
	now->tm_sec = sec;

	// do same for seconds, years, months
	// you may not do any kind of processing or printing here
	// all processing must be done outside the critical section (outside this function)
	// Interrupts are enabled/restored at the end by lock_guard destructor
}
#endif
