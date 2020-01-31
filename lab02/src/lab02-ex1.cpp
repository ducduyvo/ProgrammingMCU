#include "ITM_write.h"
#include <stdio.h>
#include <stdarg.h>/* va_list, va_start, va_arg, va_end */
#include "DigitalIoPin.h"

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#define TICKRATE_HZ1 (1000)	/* 1000 ticks per second */

//#include <atomic>
//static volatile std::atomic_int counter;
//#ifdef __cplusplus
//extern "C" {
//#endif
//    /**
//     * @brief
//     Handle interrupt from SysTick timer
//     * @return
//     Nothing
//     */
//    void SysTick_Handler(void) {
//        if (counter > 0)
//            counter--;
//    }
//#ifdef __cplusplus
//}
//#endif
//void Sleep(int ms) {
//    counter = ms;
//    while (counter > 0) {
//        __WFI();
//    }
//}

//class DigitalIoPin {
//    public:
//        DigitalIoPin(int port, int pin, bool input = true, bool pullup = true, bool invert = false);
//        bool read();
//        void write(bool value);
//        int measure_press();
//    private:
//        int port;
//        int pin;
//        bool input;
//        bool pullup;
//        bool invert;
//};
//
//DigitalIoPin::DigitalIoPin(int port_, int pin_, bool input_, bool pullup_, bool invert_) :
//    port(port_),
//    pin(pin_),
//    input(input_),
//    pullup(pullup_),
//    invert(invert_)
//{
//    Chip_IOCON_PinMuxSet(LPC_IOCON, port, pin, (IOCON_DIGMODE_EN  |
//                ((pullup) ? (IOCON_MODE_PULLUP) : (0)) |
//                ((invert) ? (IOCON_INV_EN) : (0))));
//    if (input) {
//        Chip_GPIO_SetPinDIRInput(LPC_GPIO, port_, pin_);
//    }
//    else {
//        Chip_GPIO_SetPinDIROutput(LPC_GPIO, port_, pin_);
//    }
//}
//
//int DigitalIoPin::measure_press() {
//    int time = 0;
//    while(read()) {
//        Sleep(10);
//        time += 10;
//    }
//    return time;
//}
//
//bool DigitalIoPin::read() {
//    return Chip_GPIO_GetPinState(LPC_GPIO, port, pin);
//}

class Printer {
    public:
        Printer();
        void print(char* buf, int buf_size,const char *format,int time);
};


Printer::Printer() {
    ITM_init();
}

void Printer::print(char* buf, int buf_size, const char *format, int time) {

    snprintf(buf, buf_size, format, time);
    ITM_write(buf);
}

int main(void) {
    Board_Init();
    uint32_t sysTickRate;
    Chip_Clock_SetSysTickClockDiv(1);
    sysTickRate = Chip_Clock_GetSysTickClockRate();
    SysTick_Config(sysTickRate / TICKRATE_HZ1);

    DigitalIoPin sw1(0, 17, true, true, true);
    DigitalIoPin sw2(1, 11, true, true, true);
    DigitalIoPin sw3(1, 9, true, true, true);

    Printer printer;
    char buffer[256];

    printer.print(buffer,256,"***Start the number %d counting game *** \n", 1);
    while(1) {
        if (sw1.read()) {
            printer.print(buffer,256,"SW1 pressed for %d ms\n", sw1.measure_press());
        }

        else if (sw2.read()) {
            printer.print(buffer,256,"SW2 pressed for %d ms\n", sw2.measure_press());
        }

        else if (sw3.read()) {
            printer.print(buffer,256,"SW3 pressed for %d ms\n", sw3.measure_press());
        }


    }

    return 0;
}
