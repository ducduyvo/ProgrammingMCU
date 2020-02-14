/*
 * RealTimeClock.h
 *
 *  Created on: 6 Feb 2020
 *      Author: DucVoo
 */

#ifndef REALTIMECLOCK_H_
#define REALTIMECLOCK_H_
#include "Imutex.h"
#include <mutex>
void delayMicroseconds(unsigned int us);


class RealTimeClock {
	public:
		RealTimeClock(int ticksPerSecond);
		RealTimeClock(int ticksPerSecond, tm *now);
		int GetTick();
		void Tick();
		RealTimeClock(int ticksPerSecond, int h, int m, int s);
		void gettime(tm *now);
	private:
		volatile int tick = 0;
		volatile int hour;
		volatile int min;
		volatile int sec;
		int tps;
		Imutex guard;
};


#endif /* REALTIMECLOCK_H_ */
