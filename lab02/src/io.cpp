//#include "io.h"
//
//#if defined (__USE_LPCOPEN)
//#if defined(NO_BOARD_LIB)
//#include "chip.h"
//#else
//#include "board.h"
//#endif
//#endif
//
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
//
//DigitalIoPin::DigitalIoPin(int port_, int pin_, bool input_, bool pullup_, bool invert_, const char *name_) :
//    port(port_),
//    pin(pin_),
//    input(input_),
//    pullup(pullup_),
//    invert(invert_),
//    name(name_)
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
//void DigitalIoPin::write(bool on) {
//    if (on) {
//        Chip_GPIO_SetPinState(LPC_GPIO, port, pin, !invert);
//    }
//    else {
//        Chip_GPIO_SetPinState(LPC_GPIO, port, pin, invert);
//    }
//}
//
//bool DigitalIoPin::read() {
//    return Chip_GPIO_GetPinState(LPC_GPIO, port, pin);
//}
//
//void DigitalIoPin::toggle() {
//    Chip_GPIO_SetPinToggle(LPC_GPIO, port, pin);
//}
