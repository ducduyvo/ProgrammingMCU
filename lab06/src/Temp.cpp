/*
 * Temp.cpp
 *
 *  Created on: 11 Feb 2020
 *      Author: DucVoo
 */
#include "Temp.h"
#include "Printer.h"
static Printer printer;
void Sleep(int ms);

void setupI2CMaster() {
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

uint8_t readByte(uint8_t reg) {
	uint8_t config;
	/* Read LM75 temperature sensor */
	SetupXferRecAndExecute(
	I2C_TEMP_ADDR_7BIT, &reg, 1, &config, 1);
	return config;
}

uint8_t writeByte(uint8_t reg, uint8_t data) {
	uint8_t dataArray[2] = { reg, data };
	uint8_t config;
	SetupXferRecAndExecute(
	I2C_TEMP_ADDR_7BIT, dataArray, 2, &config, 1);
	return config;
}
bool unlockSensor() {
	uint8_t config = readByte(CONFIG_REG);
	//printer.print("In normal mode\n");
	/* Test for valid operation */
	if (i2cmXferRec.status != I2CM_STATUS_OK) {
		printer.print("Error\n");
		return false;
	}

	// in standby mode so put to normal mode
	if ((config & (1 << 7)) != 0) {
		//printer.print("The command mode before: %d\n", config);
		config = writeByte(CONFIG_REG, (config & 0));
		//config = writeByte(CONFIG_REG, 64);
		Sleep(250);
		// printer.print("The command mode value after: %d\n", config);
		// printer.print("Sensor is set to the normal mode\n");
		// printer.print("----------------------------------\n");
	}

	return !(config & (1 << 7));
}

bool lockSensor() {
	uint8_t config = readByte(CONFIG_REG);
	if (i2cmXferRec.status != I2CM_STATUS_OK) {
		printer.print("Error\n");
		return false;
	}

	// in normal mode so we have put it to standby mode
	if ((config & (1 << 7)) == 0) {
		//printer.print("The command mode before: %d\n", config);
		config = writeByte(CONFIG_REG, ((config & 0) | (1 << 7)));
		//config = writeByte(CONFIG_REG, 128);
		// printer.print("The command mode after: %d\n", config);
		// printer.print(
		// 		"Sensor is set to standby mode, press SW3 to set to normal mode\n");
		// printer.print("----------------------------------\n");

	} else {
		// printer.print("Sensor was already in sleep mode\n");
		// printer.print("----------------------------------\n");
	}

	return true;
}
