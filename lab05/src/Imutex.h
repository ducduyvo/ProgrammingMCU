/*
 * Imutex.h
 *
 *  Created on: 6 Feb 2020
 *      Author: DucVoo
 */

#ifndef IMUTEX_H_
#define IMUTEX_H_

class Imutex {
public:
	Imutex();
	~Imutex();
	void lock();
	void unlock();
private:
	bool enable;
};

#endif /* IMUTEX_H_ */
