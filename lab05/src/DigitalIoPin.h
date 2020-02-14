/*
 * DigitalIoPin.h
 *
 *  Created on: 22 Jan 2020
 *      Author: DucVoo
 */

#ifndef DIGITALIOPIN_H_
#define DIGITALIOPIN_H_

class DigitalIoPin {
    public:
        DigitalIoPin(int port, int pin, bool input = true, bool pullup = true, bool invert = false);
        bool read();
        void write(bool value);
        void toggle();
    private:
        int port;
        int pin;
        bool input;
        bool pullup;
        bool invert;
};




#endif /* DIGITALIOPIN_H_ */
