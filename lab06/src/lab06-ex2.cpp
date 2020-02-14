/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#if defined(__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#include "ITM_write.h"
#include "DigitalIoPin.h"
#include <atomic>
#include "Temp.h"
#include <string.h>
#include <stdio.h>
#include <ctime>
#include "printer.h"
#include <string>
#include "BarGraph.h"
#include "LiquidCrystal.h"
#endif
#endif

#define TICKRATE_HZ (1000)
#define ADC_MAX (4095)
#define ADC_MIN (0)
#define TEMP_MAX (150)
#define TEMP_MIN (-65)

#include <cr_section_macros.h>

// TODO: insert other include files here

// TODO: insert other definitions and declarations here
static volatile int counter;
static volatile unsigned int blinktime;
static volatile unsigned int frequency;
static volatile int diff;
static int rate(100);
static volatile int count = 0;

DigitalIoPin ledRed(0, 25, false, true, true);
DigitalIoPin ledGreen(0, 3, false, true, true);
DigitalIoPin ledBlue(1, 1, false, true, true);

extern "C"
{
    void SysTick_Handler(void)
    {
        if (count < blinktime)
        {
            count++;
        }
        else
        {
            count = 0;

            ledGreen.write(diff == 0);

            if (diff < 0)
            {
                ledBlue.toggle();
                ledRed.write(false);
            }
            else if (diff > 0)
            {

                ledRed.toggle();

                ledBlue.write(false);
            }
            else
            {
                ledBlue.write(false);
                ledRed.write(false);
            }
        }

        if (counter > 0)
            counter--;
    }

    void Sleep(int ms)
    {
        counter = ms;
        while (counter > 0)
        {
            __WFI();
        }
    }
    static Printer printer;
    /* Start ADC calibration */
    void ADC_StartCalibration(LPC_ADC_T *pADC)
    {
        // clock divider is the lowest 8 bits of the control register
        /* Setup ADC for about 500KHz (per UM) */
        uint32_t ctl = (Chip_Clock_GetSystemClockRate() / 500000) - 1;
        /* Set calibration mode */
        ctl |= ADC_CR_CALMODEBIT;
        pADC->CTRL = ctl;
        /* Calibration is only complete when ADC_CR_CALMODEBIT bit has cleared */
        while (pADC->CTRL & ADC_CR_CALMODEBIT)
        {
        };
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
        Chip_RIT_Init(LPC_RITIMER); // initialize RIT (enable clocking etc.)

#endif
#endif

        Init_I2C_PinMux();
        setupI2CMaster();
        NVIC_DisableIRQ(I2C0_IRQn);

        // TODO: insert code here
        /* Setup ADC for 12-bit mode and normal power */
        Chip_ADC_Init(LPC_ADC0, 0);
        /* Setup for ADC clock rate */
        Chip_ADC_SetClockRate(LPC_ADC0, 500000);
        /* For ADC0, sequencer A will be used without threshold events.
 It will be triggered manually, convert CH8 and CH10 in the sequence */
        Chip_ADC_SetupSequencer(LPC_ADC0, ADC_SEQA_IDX, (ADC_SEQ_CTRL_CHANSEL(8) | ADC_SEQ_CTRL_CHANSEL(10) | ADC_SEQ_CTRL_MODE_EOS));
        // fix this and check if this is needed
        Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 0, (IOCON_MODE_INACT | IOCON_ADMODE_EN));
        Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 0, (IOCON_MODE_INACT | IOCON_ADMODE_EN));
        /* For ADC0, select analog input pin for channel 0 on ADC0 */
        Chip_ADC_SetADC0Input(LPC_ADC0, 0);
        /* Use higher voltage trim for both ADC */
        Chip_ADC_SetTrim(LPC_ADC0, ADC_TRIM_VRANGE_HIGHV);
        /* Assign ADC0_8 to PIO1_0 via SWM (fixed pin) and ADC0_10 to PIO0_0 */
        Chip_SWM_EnableFixedPin(SWM_FIXED_ADC0_8);
        Chip_SWM_EnableFixedPin(SWM_FIXED_ADC0_10);
        /* Need to do a calibration after initialization and trim */
        //while (!(Chip_ADC_IsCalibrationDone(LPC_ADC0))); // The NXP library function violates their
        //own access rules given in data sheet so we can't use it
        ADC_StartCalibration(LPC_ADC0);
        /* Set maximum clock rate for ADC */
        /* Our CPU clock rate is 72 MHz and ADC clock needs to be 50 MHz or less
* so the divider must be at least two. The real divider used is the value below + 1
*/
        Chip_ADC_SetDivider(LPC_ADC0, 1);
        /* Chip_ADC_SetClockRate set the divider but due to rounding error it sets the divider too low
* which results in a clock rate that is out of allowed range
*/
        //Chip_ADC_SetClockRate(LPC_ADC0, 500000); // does not work with 72 MHz clock when we want
        // maximum frequency
        /* Clear all pending interrupts and status flags */
        Chip_ADC_ClearFlags(LPC_ADC0, Chip_ADC_GetFlags(LPC_ADC0));
        /* Enable sequence A completion interrupts for ADC0 */
        Chip_ADC_EnableInt(LPC_ADC0, ADC_INTEN_SEQA_ENABLE);
        /* We don't enable the corresponding interrupt in NVIC so the flag is set but no interrupt is triggered */
        /* Enable sequencer */
        Chip_ADC_EnableSequencer(LPC_ADC0, ADC_SEQA_IDX);
        /* Configure systick timer */
        SysTick_Config(Chip_Clock_GetSysTickClockRate() / TICKRATE_HZ);

        // Setup temperature sensor for I2C transmitting

        uint32_t a0;
        uint32_t d0;
        uint32_t a10;
        //uint32_t temp;
        char str[80];

        DigitalIoPin rs(0, 8, false, true, false);
        DigitalIoPin en(1, 6, false, true, false);
        DigitalIoPin d4(1, 8, false, true, false);
        DigitalIoPin d5(0, 5, false, true, false);
        DigitalIoPin d6(0, 6, false, true, false);
        DigitalIoPin d7(0, 7, false, true, false);

        DigitalIoPin sw1(0, 17, true, true, true);
        DigitalIoPin sw2(1, 11, true, true, true);
        DigitalIoPin sw3(1, 9, true, true, true);

        LiquidCrystal lcd(&rs, &en, &d4, &d5, &d6, &d7);
        // configure display geometry
        lcd.begin(16, 2);

        BarGraph Trimgraph(&lcd, 50, false, 5, 0,true);
        BarGraph Tempgraph(&lcd, 50, false, 5, 1,true);

        while (1)
        {
            Chip_ADC_StartSequencer(LPC_ADC0, ADC_SEQA_IDX);
            // poll sequence complete flag
            while (!(Chip_ADC_GetFlags(LPC_ADC0) & ADC_FLAGS_SEQA_INT_MASK))
                ;
            // clear the flags
            Chip_ADC_ClearFlags(LPC_ADC0, Chip_ADC_GetFlags(LPC_ADC0));
            // get data from ADC channels
            a0 = Chip_ADC_GetDataReg(LPC_ADC0, 8); // raw value
            d0 = ADC_DR_RESULT(a0);                // ADC result with status bits masked to zero and
            //shifted to start from zero
            int8_t trimmer = ((int)d0 / (((float)(ADC_MAX - ADC_MIN) / (float)(TEMP_MAX - TEMP_MIN)))) + TEMP_MIN;
            int8_t temp = readByte(TEMP_REG);
            diff = (trimmer - temp);

            blinktime = 5000 / std::abs(trimmer - temp);
            char value1[80];
            char value2[80];

            //sprintf(value2, "%s%lu", (temp > 999 ? "" : (temp > 99 ? " " : (temp > 9 ? "  " : "   "))), temp);
            sprintf(value2,"%4d",temp);
            lcd.print(value2, 0, 1);
            Tempgraph.draw(temp * 50 / 30);

            sprintf(value1, "%s%lu", (d0 > 999 ? "" : (d0 > 99 ? " " : (d0 > 9 ? "  " : "   "))), d0);
            lcd.print(value1, 0,0);
            Trimgraph.draw(d0 / (ADC_MAX / 50));

            sprintf(str, "frequency = %d temp = %d a0 = %08lX, d0 = %lu, scaled value = %d\n", blinktime, temp, a0, d0, trimmer);
            //Board_UARTPutSTR(str);
            printer.print(str);
            Sleep(100);
        }
        return 0;
    }
}
