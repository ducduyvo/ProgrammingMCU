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
#include "SleepEdit.h"
#include "KillerEdit.h"
#if defined(__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"

#else
#include "board.h"

#endif
#endif

#define TICKRATE_HZ (1000)
#include <cr_section_macros.h>

static volatile int counter;
static volatile int count = 0;
static SimpleMenu menu; /* this could also be allocated from the heap */
static Printer printer;
bool isChanged = false;
bool PropertyEdit::onState = false;
bool IntegerEdit::saveValue = false;

IntegerEdit *temperature;
IntegerEdit *pressure;
IntegerEdit *humidity;

SleepEdit SleepSystem;
KillerEdit KillerSystem;

int list[3];

extern "C"
{
	void SysTick_Handler(void)
	{
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
		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH0);
		count = 0;
		menu.event(MenuItem::up);
	}
	void PIN_INT1_IRQHandler(void)
	{
		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH1);
		count = 0;
		menu.event(MenuItem::ok);
	}
	void PIN_INT2_IRQHandler(void)
	{
		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH2);
		count = 0;
		menu.event(MenuItem::down);
	}
}
void Sleep(int ms)
{
	counter = ms;
	while (counter > 0)
	{
		LPC_PWRD_API->power_mode_configure(PMU_DEEP_SLEEP, ~PMU_PD_WDOSC);
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

	auto printValue = []() -> void {
		// if ((IntegerEdit::saveValue))
		// {
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

			list[0] = temperature->getValue();
			list[1] = pressure->getValue();
			list[2] = humidity->getValue();
			printer.print("Sleep mode.\n");
			SleepSystem.SleepForSystem();
		}
		else
		{
			printer.print("Watchdog mode.\n");
			KillerSystem.KillerForSystem();
		}
		// IntegerEdit::saveValue = false;
	};

	temperature->updatedValue = printValue;
	pressure->updatedValue = printValue;
	humidity->updatedValue = printValue;

	// Set leds off
	Board_LED_Set(0, false);
	Board_LED_Set(1, false);
	Board_LED_Set(2, false);

	// Check the status flag of watchdog to see if it had been resetted
	uint32_t status = Chip_WWDT_GetStatus(LPC_WWDT);
	printer.print(" flag: %d", status);
	if (status == 0)
		Board_LED_Set(1, true); // status flag ok
	else
		Board_LED_Set(0, true); // reset caused by watchdog

	//Watchdog timer

	uint32_t wdtFreq;
	/* Enable the WDT oscillator */
	Chip_SYSCTL_PowerUp(SYSCTL_POWERDOWN_WDTOSC_PD);

	/* The WDT divides the input frequency into it by 4 */
	wdtFreq = Chip_Clock_GetWDTOSCRate() / 4;

	/* Initialize WWDT (also enables WWDT clock) */
	Chip_WWDT_Init(LPC_WWDT);

	/* Set watchdog feed time constant to approximately seconds
	 Set watchdog warning time to 512 ticks after feed time constant
	 Set watchdog window time to 3s */
	Chip_WWDT_SetTimeOut(LPC_WWDT, wdtFreq * 1);
	Chip_WWDT_SetWarning(LPC_WWDT, 512);
	Chip_WWDT_SetWindow(LPC_WWDT, wdtFreq * 3);

	/*Configure WWDT to reset on timeout */
	Chip_WWDT_SetOption(LPC_WWDT, WWDT_WDMOD_WDRESET);

	/* Clear watchdog warning and timeout interrupts */
	Chip_WWDT_ClearStatusFlag(LPC_WWDT, WWDT_WDMOD_WDTOF | WWDT_WDMOD_WDINT);

	// /* Power everything up except for ADC, USB and temp sensor on wake up from
	//    deep sleep. */
	// Chip_SYSCTL_SetWakeup(~(SYSCTL_SLPWAKE_IRCOUT_PD | SYSCTL_SLPWAKE_IRC_PD |
	//                         SYSCTL_SLPWAKE_FLASH_PD | SYSCTL_SLPWAKE_SYSOSC_PD |
	//                         SYSCTL_SLPWAKE_SYSPLL_PD | SYSCTL_SLPWAKE_WDTOSC_PD));
	// /* Allow WDT to wake from deep sleep. */
	// Chip_SYSCTL_EnableERP0PeriphWakeup(SYSCTL_ERP0_WAKEUP_WDTINT);

	// /* Clear and enable watchdog interrupt */
	// NVIC_ClearPendingIRQ(WWDT_IRQn);
	// NVIC_EnableIRQ(WWDT_IRQn);
	/* Start watchdog */
	Chip_WWDT_Start(LPC_WWDT);

	while (1)
	{
		Chip_WWDT_Feed(LPC_WWDT);
	}

	delete temperature;
	delete pressure;
	delete humidity;
	delete[] lcd;

	return 0;
}
