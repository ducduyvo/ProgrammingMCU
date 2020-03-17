#include "ITM_write.h"
#include "DigitalIoPin.h"
#include <atomic>
#include <string.h>
#include <stdio.h>
#include "SimpleMenu.h"
#include "IntegerEdit.h"
#include "printer.h"
#include <string>
#include "BarGraph.h"
#include "LiquidCrystal.h"
#include "DecimalEdit.h"
#if defined(__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"

#else
#include "board.h"

#endif
#endif

#define TICKRATE_HZ (1000)
#include <cr_section_macros.h>

//Function for turn led
void check_Led();      // Check time for LED
void set_Led(int idx); // Set colour for BOARD LED

static volatile int sleepTime;
static volatile int counter = 0;
static int LEDS = 0;
static SimpleMenu menu; /* this could also be allocated from the heap */
static Printer printer;

DecimalEdit *time;
DecimalEdit *blank;
IntegerEdit *light;

// BOARD LED configuration
DigitalIoPin *ledRed = new DigitalIoPin(0, 25, false, true, true);
DigitalIoPin *ledGreen = new DigitalIoPin(0, 3, false, true, true);
DigitalIoPin *ledBlue = new DigitalIoPin(1, 1, false, true, true);
DigitalIoPin *ledList[3] = {ledRed, ledGreen, ledBlue};
DigitalIoPin *ledPtr = nullptr;

// Variable to control the Board LED
static int lightOn = 0;
static int lightOff = 0;
static int preOn = 0;
static int preOff = 0;
static int lightCounter = 0;
static int preLed = 0;
static int onLed = 0;
static bool isOn = true;

extern "C"
{
    void SysTick_Handler(void)
    {
        if (ledPtr != nullptr)
        {
            if (isOn)
            {
                if (lightCounter < lightOn)
                {
                    ledPtr->write(true);
                    lightCounter++;
                }
                // else if (lightOn == 0)
                // {
                //     for (int i = 0; i < 3; i++)
                //     {
                //         ledList[i]->write(false);
                //     }
                // }
                else
                {
                    for (int i = 0; i < 3; i++)
                    {
                        ledList[i]->write(false);
                    }
                    lightCounter = 0;
                    isOn = false;
                }
            }
            else
            {
                if (lightCounter < lightOff)
                {
                    lightCounter++;
                }
                else
                {
                    lightCounter = 0;
                    isOn = true;
                    //ledPtr->write(true);
                }
            }
        }
        else
        {
            for (int i = 0; i < 3; i++)
            {
                ledList[i]->write(false);
            }
        }

        counter++;
        if (counter >= 10000)
        {
            menu.event(MenuItem::back);
            counter = 0;
        }
        if (sleepTime > 0)
            sleepTime--;
    }
}
extern "C"
{

    void PIN_INT0_IRQHandler(void)
    {
        Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH0);
        counter = 0;
        menu.event(MenuItem::up);
    }
    void PIN_INT1_IRQHandler(void)
    {
        Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH1);
        counter = 0;
        menu.event(MenuItem::ok);
        check_Led();
    }
    void PIN_INT2_IRQHandler(void)
    {
        Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH2);
        counter = 0;
        menu.event(MenuItem::down);
    }
}
void Sleep(int ms)
{
    sleepTime = ms;
    while (sleepTime > 0)
    {
        __WFI();
    }
}
bool PropertyEdit::onState = false;
bool IntegerEdit::saveValue = false;

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

    time = new DecimalEdit(lcd, std::string("Time"), 200, 0, 2000);
    blank = new DecimalEdit(lcd, std::string("Blank (10*s)"), 0.1, 0, 1);
    light = new IntegerEdit(lcd, std::string("Light"), 1, 0, 3);

    menu.addItem(new MenuItem(time));
    menu.addItem(new MenuItem(blank));
    menu.addItem(new MenuItem(light));

    time->setValue(50.0);
    blank->setValue(0.2);
    light->setValue(3);

    // preOn = time->getValue() * 10;
    // preOff = blank->getValue() * 10000;
    // preLed = light->getValue();

    // lightOn = time->getValue() * 10;
    // lightOff = blank->getValue() * 10000;
    onLed = light->getValue();

    //Turn off all Board Led
    ledRed->write(false);
    ledGreen->write(false);
    ledBlue->write(false);

    menu.event(MenuItem::show); // display first menu item
    check_Led();
    while (1)
    {

        Sleep(10);
    }
    delete ledRed;
    delete ledGreen;
    delete ledBlue;
    delete time;
    delete blank;
    delete light;

    return 0;
}
void set_Led(int idx)
{
    if (idx > 2 || idx < 0)
        ledPtr = nullptr;
    ledPtr = ledList[idx];
    for (int i = 0; i < 3; i++)
    {
        if (i != idx)
            ledList[i]->write(false);
    }
}
void check_Led()
{
    // ledRed->write(false);
    // ledGreen->write(false);
    // ledBlue->write(false);
    //printer.print("%d     %d\n", lightOn, lightOff);

    //Check changed ontime value
    // if (preOn != time->getValue())
    // {
    //     lightOn = time->getValue() * 10;
    //     preOn = lightOn;
    // }
    // //Check changed offtime value
    // if (preOff != (blank->getValue() * 10000))
    // {
    //     lightOff = blank->getValue() * 10000;
    //     preOff = lightOff;
    // }

    // Check board Led

    if (PropertyEdit::onState)
    {
        if (light->getValue() == 0)
        {
            ledPtr = nullptr;
        }
        else
        {
            set_Led(light->getValue() - 1);
            lightOn = time->getValue();
            lightOff = blank->getValue() * 1000;
        }
        printer.print("Time = %.2f, Blank = %.2f, Led = %d\n",
                      time->getValue(), blank->getValue(),
                      light->getValue());
        PropertyEdit::onState = false;
    }
}
