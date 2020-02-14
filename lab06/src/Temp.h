/*
 * Temp.h
 *
 *  Created on: 11 Feb 2020
 *      Author: DucVoo
 */

#ifndef TEMP_H_
#define TEMP_H_
#include "board.h"


/* I2CM transfer record */
static I2CM_XFER_T i2cmXferRec;
/* I2C clock is set to 50 KHz */
#define I2C_CLK_DIVIDER         (1440)
/* 100KHz I2C bit-rate */
#define I2C_BITRATE         (100000)
/* Standard I2C mode */
#define I2C_MODE    (0)

#if defined(BOARD_NXP_LPCXPRESSO_1549)
/** 7-bit I2C addresses of Temperature Sensor */
#define I2C_TEMP_ADDR_7BIT  (0x48) // 1001 000
#endif

#if defined(BOARD_KEIL_MCB1500)
/** 7-bit I2C address of STMPE811 Touch Screen Controller */
#define I2C_STMPE811_ADDR_7BIT 0x41
#endif

/* SysTick rate in Hz */
#define TICKRATE_HZ         (1000)

#define TEMP_REG (0)
#define CONFIG_REG (1)
static void Init_I2C_PinMux(void) {
#if defined(BOARD_KEIL_MCB1500)||defined(BOARD_NXP_LPCXPRESSO_1549)
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 22, IOCON_DIGMODE_EN | I2C_MODE);
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 23, IOCON_DIGMODE_EN | I2C_MODE);
	Chip_SWM_EnableFixedPin(SWM_FIXED_I2C0_SCL);
	Chip_SWM_EnableFixedPin(SWM_FIXED_I2C0_SDA);
#else
#endif
}

/* Setup I2C handle and parameters */
 void setupI2CMaster();

static void SetupXferRecAndExecute(uint8_t devAddr, uint8_t *txBuffPtr,
		uint16_t txSize, uint8_t *rxBuffPtr, uint16_t rxSize);
uint8_t readByte(uint8_t reg);
uint8_t writeByte(uint8_t reg, uint8_t data);
bool unlockSensor();
bool lockSensor();


#endif /* TEMP_H_ */
