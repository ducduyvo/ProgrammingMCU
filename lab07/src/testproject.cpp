#include "ITM_write.h"
#include "DigitalIoPin.h"
#include <atomic>
#include <string.h>
#include <stdio.h>
#include <ctime>
#include "SimpleMenu.h"
#include "IntegerEdit.h"
#include "printer.h"
#include <string>
#include "BarGraph.h"
#include "LiquidCrystal.h"
#if defined(__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"

#else
#include "board.h"

#endif
#endif

#define TICKRATE_HZ (1000)
#include <cr_section_macros.h>

static volatile int debounce;
static bool isPressed = false;

static volatile int counter;
static volatile int count = 0;
static SimpleMenu menu; /* this could also be allocated from the heap */
static Printer printer;
bool isChanged = false;
bool PropertyEdit::onState = false;
bool IntegerEdit::saveValue = false;
static volatile int a = 0;

IntegerEdit *temperature;
IntegerEdit *pressure;
IntegerEdit *humidity;

int list[3];

extern "C"
{
    void SysTick_Handler(void)
    {

        debounce--;
        count++;
        if (count >= 10000)
        {
            menu.event(MenuItem::back);
            count = 0;
        }
        if (counter > 0)
            counter--;
    }
}

extern "C"
{
    void PIN_INT0_IRQHandler(void)
    {
        if (debounce <= 0)
        {
            debounce = 100;
        }
        Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH0);
        count = 0;
        menu.event(MenuItem::up);
        printer.print("Pressed : %d \n", a++);
    }
    void PIN_INT1_IRQHandler(void)
    {
        if (debounce <= 0)
        {
            debounce = 100;
        }
        Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH1);
        count = 0;
        menu.event(MenuItem::ok);
        isPressed = true;
    }
    void PIN_INT2_IRQHandler(void)
    {
        if (debounce <= 0)
        {
            debounce = 100;
        }
        Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH2);
        count = 0;
        menu.event(MenuItem::down);
        isPressed = true;
    }
}

void Sleep(int ms)
{
    counter = ms;
    while (counter > 0)
    {
        __WFI();
    }
}

int main(void)
{

#if defined(__USE_LPCOPEN)
    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
    // Set up and initialize all required blocks and
    // functions related to the board hardware
    Board_Init();
    Chip_RIT_Init(LPC_RITIMER);

#endif
#endif
    /* Enable and setup SysTick Timer at a periodic rate */
    Chip_Clock_SetSysTickClockDiv(1);
    uint32_t sysTickRate = Chip_Clock_GetSysTickClockRate();
    SysTick_Config(SystemCoreClock / TICKRATE_HZ);

    // TODO: insert code here
    // NVIC_EnableIRQ(RITIMER_IRQn);

    DigitalIoPin sw1(0, 17);
    DigitalIoPin sw2(1, 11);
    DigitalIoPin sw3(1, 9);

    DigitalIoPin rs(0, 8, false, true, false);
    DigitalIoPin en(1, 6, false, true, false);
    DigitalIoPin d4(1, 8, false, true, false);
    DigitalIoPin d5(0, 5, false, true, false);
    DigitalIoPin d6(0, 6, false, true, false);
    DigitalIoPin d7(0, 7, false, true, false);

    // configure display geometry

    LiquidCrystal *lcd = new LiquidCrystal(&rs, &en, &d4, &d5, &d6, &d7);

    lcd->begin(16, 2);
    lcd->setCursor(0, 0);

    Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_PININT);
    Chip_SYSCTL_PeriphReset(RESET_PININT);

    //Configure pin interrupt 0 generate interrupt when it sees a falling edge on SW1
    Chip_INMUX_PinIntSel(0, 0, 17);
    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH0);
    Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH0);
    Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH0);

    //Configure pin interrupt 1 generate interrupt when it sees a falling edge on SW2
    Chip_INMUX_PinIntSel(1, 1, 11);
    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH1);
    Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH1);
    Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH1);

    //Configure pin interrupt 2 generate interrupt when it sees a falling edge on SW3
    Chip_INMUX_PinIntSel(2, 1, 9);
    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH2);
    Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH2);
    Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH2);

    //Enable pin interrupt 0,1,2 in NVIC
    NVIC_ClearPendingIRQ(PIN_INT0_IRQn);
    NVIC_EnableIRQ(PIN_INT0_IRQn);
    NVIC_ClearPendingIRQ(PIN_INT1_IRQn);
    NVIC_EnableIRQ(PIN_INT1_IRQn);
    NVIC_ClearPendingIRQ(PIN_INT2_IRQn);
    NVIC_EnableIRQ(PIN_INT2_IRQn);

    temperature = new IntegerEdit(lcd, std::string("Temperature"), 0, 100);
    pressure = new IntegerEdit(lcd, std::string("Pressure"), 3, 0, 100);
    humidity = new IntegerEdit(lcd, std::string("Humidity"), 7, 0, 50);
    menu.addItem(new MenuItem(temperature));
    menu.addItem(new MenuItem(pressure));
    menu.addItem(new MenuItem(humidity));
    temperature->setValue(21);
    pressure->setValue(89);
    humidity->setValue(42);

    menu.event(MenuItem::show); // display first menu item

    list[0] = temperature->getValue();
    list[1] = pressure->getValue();
    list[2] = humidity->getValue();

    auto printValue =
        []() -> void {
        if ((temperature->getValue() != list[0] || pressure->getValue() != list[1] || humidity->getValue() != list[2]))
        {

            printer.print("1.%s (%d - %d): %d \n",
                          temperature->getTitle().c_str(), temperature->getUpper(),
                          pressure->getLower(), temperature->getValue());
            printer.print("2.%s (%d - %d): %d \n", pressure->getTitle().c_str(),
                          pressure->getUpper(), pressure->getLower(),
                          pressure->getValue());
            printer.print("3.%s (%d - %d): %d \n", humidity->getTitle().c_str(),
                          humidity->getUpper(), humidity->getLower(),
                          humidity->getValue());
        }
        list[0] = temperature->getValue();
        list[1] = pressure->getValue();
        list[2] = humidity->getValue();
    };

    temperature->updatedValue = printValue;
    pressure->updatedValue = printValue;
    humidity->updatedValue = printValue;

    while (1)
    {

        Sleep(10);
    }

    delete temperature;
    delete pressure;
    delete humidity;
    delete[] lcd;

    return 0;
}
