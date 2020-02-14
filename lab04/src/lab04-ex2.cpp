/*
 ===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
 ===============================================================================
 */

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#include "ITM_write.h"
#include "DigitalIoPin.h"
#include <atomic>
#include <stdio.h>
#include "printer.h"
#endif
#endif

#include <cr_section_macros.h>

// TODO: insert other include files here
static volatile std::atomic_int counter;
#ifdef __cplusplus
extern "C" {
#endif

void SysTick_Handler(void) {
	if (counter > 0)
		counter--;
}
#ifdef __cplusplus
}
#endif
void Sleep(int ms) {
	counter = ms;
	while (counter > 0) {
		__WFI();
	}
}
// TODO: insert other definitions and declarations here

static I2CM_XFER_T i2cmXferRec;
/* I2C clock is set to 1.8MHz */
#define I2C_CLK_DIVIDER         (40)
/* 100KHz I2C bit-rate */
#define I2C_BITRATE         (100000)
/* Standard I2C mode */
#define I2C_MODE    (0)

#if defined(BOARD_NXP_LPCXPRESSO_1549)
/** 7-bit I2C addresses of Temperature Sensor */
#define I2C_TEMP_ADDR_7BIT  (0x48)
#endif

#if defined(BOARD_KEIL_MCB1500)
/** 7-bit I2C address of STMPE811 Touch Screen Controller */
#define I2C_STMPE811_ADDR_7BIT 0x41
#endif

/* SysTick rate in Hz */
#define TICKRATE_HZ1         (1000)

/* Current state for LED control via I2C cases */

static void Init_I2C_PinMux(void) {
#if defined(BOARD_KEIL_MCB1500)||defined(BOARD_NXP_LPCXPRESSO_1549)
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 22, IOCON_DIGMODE_EN | I2C_MODE);
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 23, IOCON_DIGMODE_EN | I2C_MODE);
	Chip_SWM_EnableFixedPin(SWM_FIXED_I2C0_SCL);
	Chip_SWM_EnableFixedPin(SWM_FIXED_I2C0_SDA);
#else
#error "No I2C Pin Muxing defined for this example"
#endif
}

/* Setup I2C handle and parameters */
static void setupI2CMaster() {
	/* Enable I2C clock and reset I2C peripheral - the boot ROM does not
	 do this */
	Chip_I2C_Init(LPC_I2C0);

	/* Setup clock rate for I2C */
	Chip_I2C_SetClockDiv(LPC_I2C0, I2C_CLK_DIVIDER);

	/* Setup I2CM transfer rate */
	Chip_I2CM_SetBusSpeed(LPC_I2C0, I2C_BITRATE);

	/* Enable Master Mode */
	Chip_I2CM_Enable(LPC_I2C0);
}

static void SetupXferRecAndExecute(uint8_t devAddr, uint8_t *txBuffPtr,
		uint16_t txSize, uint8_t *rxBuffPtr, uint16_t rxSize) {
	/* Setup I2C transfer record */
	i2cmXferRec.slaveAddr = devAddr;
	i2cmXferRec.status = 0;
	i2cmXferRec.txSz = txSize;
	i2cmXferRec.rxSz = rxSize;
	i2cmXferRec.txBuff = txBuffPtr;
	i2cmXferRec.rxBuff = rxBuffPtr;

	Chip_I2CM_XferBlocking(LPC_I2C0, &i2cmXferRec);
}

/* Master I2CM receive in polling mode */
#if defined(BOARD_NXP_LPCXPRESSO_1549)
/* Function to read LM75 I2C temperature sensor and output result */
static void WriteConfig() {
	Printer printer;
	uint8_t temperature;
	uint8_t config;
	uint8_t tempAddress = 0;
	uint8_t confAddress[2] = { 1, 128 };

	/* Read LM75 temperature sensor */
	SetupXferRecAndExecute(

	/* The LM75 I2C bus address */
	I2C_TEMP_ADDR_7BIT,

	/* Transmit one byte, the config register address */
	confAddress, 2,

	/* Receive back one byte, the contents of the config register */
	&config, 1);

	printer.print("The standby mode %d\n", config);
}
static void WriteNormal() {
	Printer printer;
	uint8_t temperature;
	uint8_t config;
	uint8_t tempAddress = 0;
	uint8_t confAddress[2] = { 1, 0 };

	SetupXferRecAndExecute( I2C_TEMP_ADDR_7BIT, &confAddress[0], 1, &config, 1);
	printer.print("The normal mode %d\n", config);
	if ((config & 0x80) != 0) {
		SetupXferRecAndExecute(I2C_TEMP_ADDR_7BIT, &tempAddress, 1, &config, 1);

		printer.print("The normal mode \n");
	}
}
static void ReadTemperatureI2CM() {
	Printer printer;
	uint8_t temperature;
	uint8_t config;
	uint8_t tempAddress = 0;
	uint8_t confAddress[2] = {1,1};

	/* Read LM75 temperature sensor */
	SetupXferRecAndExecute(

	/* The LM75 I2C bus address */
	I2C_TEMP_ADDR_7BIT,

	/* Transmit one byte, the config register address */
	confAddress, 2,

	/* Receive back one byte, the contents of the config register */
	&config, 1);
	printer.print("hello %d\n", config);

	/* Test for valid operation */
	if (i2cmXferRec.status == I2CM_STATUS_OK) {
		/* test if the device is ready */
		if (((config & 0x40) != 0)) {
			/* Read LM75 temperature sensor */

			SetupXferRecAndExecute(
			/* The LM75 I2C bus address */
			I2C_TEMP_ADDR_7BIT,
			/* Transmit one byte, the temperature register address */
			&tempAddress, 1,
			/* Receive back one byte, the contents of the temperature register */
			&temperature, 1);
			printer.print("Temperature: %d °C\n", (int8_t) temperature);

		}

		else {
			printer.print("Error, the device is not ready\n");
		}
	} else {
		printer.print("Error\n");
	}
}
#endif

int main(void) {

	/* Generic Initialization */
	SystemCoreClockUpdate();
	Board_Init();
	Printer printer;

	/* Setup I2C pin muxing */
	Init_I2C_PinMux();

	/* Allocate I2C handle, setup I2C rate, and initialize I2C
	 clocking */
	setupI2CMaster();

	/* Disable the interrupt for the I2C */
	NVIC_DisableIRQ(I2C0_IRQn);

	uint32_t sysTickRate;
	Chip_Clock_SetSysTickClockDiv(1);
	sysTickRate = Chip_Clock_GetSysTickClockRate();
	SysTick_Config(sysTickRate / TICKRATE_HZ1);

	DigitalIoPin sw3(1, 9, true, true, true);
	while (1) {
		if (!sw3.read() && counter == 0) {
			Sleep(250);
			WriteConfig();
		} else if (sw3.read()) {
			if (sw3.read()) {
				while (sw3.read())
					;
			}
			WriteNormal();
			Sleep(250);
			ReadTemperatureI2CM();
			Sleep(5);
			counter = 4750;
		}

	}

}
