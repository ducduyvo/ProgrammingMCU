#include <stdio.h>
#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */
#include "../ITM_write.h"
#include "menu.h"
#include "printer.h"
#include "io.h"

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#define TICKRATE_HZ1 (1000)	/* 1000 ticks per second */

int main(void) {
    Board_Init();
    uint32_t sysTickRate;
    Chip_Clock_SetSysTickClockDiv(1);
    sysTickRate = Chip_Clock_GetSysTickClockRate();
    SysTick_Config(sysTickRate / TICKRATE_HZ1);

    DigitalIoPin sw1(0, 17, true, true, true, "SW1");
    DigitalIoPin sw2(1, 11, true, true, true, "SW2");
    DigitalIoPin sw3(1, 9, true, true, true, "SW3");

    DigitalIoPin led1(0, 25, false, true, true, "Red");
    DigitalIoPin led2(0, 3, false, true, true, "Green");
    DigitalIoPin led3(1, 1, false, true, true, "Blue");

     Printer printer;
    /* printer.print_menu(sw1, sw2, sw3, led1, led2, led3); */

    Menu menu(sw1, sw2, sw3, led1, led2, led3);
    while(true) {
        menu.print_menu();
        menu.check_input();
    }

    return 0;
}
