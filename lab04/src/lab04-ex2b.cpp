#include "board.h"
#include "ITM_write.h"
#include "DigitalIoPin.h"
#include <atomic>
#include <stdio.h>
#include "printer.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
/* Own functions */
uint8_t writeByte(uint8_t reg, uint8_t data);
uint8_t readByte(uint8_t reg);
bool unlockSensor();
bool lockSensor();

static Printer printer;

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

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Initializes pin muxing for I2C interface - note that SystemInit() may
 already setup your pin muxing at system startup */

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
static void ReadTemperatureI2CM() {
	uint8_t config = readByte(CONFIG_REG);

	/* Test for valid operation */
	if (i2cmXferRec.status == I2CM_STATUS_OK) {
		printer.print("The device is ready.\n");
	} else {
		printer.print("Opps, the device is not ready\n");
	}

	/* test if the device is ready */
	if ((config & 0x40) != 0) {
		printer.print("The data is  ready\n");
	} else {
		printer.print("The data is not ready\n");
	}

	uint8_t temperature = readByte(TEMP_REG);
	printer.print("The temperature now is: %d °C\n", (int8_t) temperature);
	printer.print("----------------------------------\n");

}
#endif

/*****************************************************************************
 * Public functions
 ****************************************************************************/

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
	printer.print("In normal mode\n");
	/* Test for valid operation */
	if (i2cmXferRec.status != I2CM_STATUS_OK) {
		printer.print("Error\n");
		return false;
	}

	// in standby mode so put to normal mode
	if ((config & (1 << 7)) != 0) {
		printer.print("The command mode before: %d\n", config);
		config = writeByte(CONFIG_REG, (config & 0));
		//config = writeByte(CONFIG_REG, 64);
		Sleep(250);
		printer.print("The command mode value after: %d\n", config);
		printer.print("Sensor is set to the normal mode\n");
		printer.print("----------------------------------\n");
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
		printer.print("The command mode before: %d\n", config);
		config = writeByte(CONFIG_REG, ((config & 0) | (1 << 7)));
		//config = writeByte(CONFIG_REG, 128);
		printer.print("The command mode after: %d\n", config);
		printer.print(
				"Sensor is set to standby mode, press SW3 to set to normal mode\n");
		printer.print("----------------------------------\n");

	} else {
		printer.print("Sensor was already in sleep mode\n");
		printer.print("----------------------------------\n");
	}

	return true;
}

int main(void) {
	/* Generic Initialization */
	SystemCoreClockUpdate();
	Board_Init();

	/* Clear activity LED */
	Board_LED_Set(0, false);

	/* Setup I2C pin muxing */
	Init_I2C_PinMux();

	/* Allocate I2C handle, setup I2C rate, and initialize I2C
	 clocking */
	setupI2CMaster();

	/* Disable the interrupt for the I2C */
	NVIC_DisableIRQ(I2C0_IRQn);

	/* Enable SysTick Timer */
	SysTick_Config(SystemCoreClock / TICKRATE_HZ);
	bool isLock = 0;

	DigitalIoPin sw3(1, 9, true, true, true);

	while (1) {
		while (!sw3.read()) {
			if (counter == 0 && !isLock) {
				// Put sensor to standby mode
				lockSensor();
				Sleep(10);
				isLock = true;
			}
		}

		if (sw3.read()) {

			if (unlockSensor()) {
				ReadTemperatureI2CM();
			} else {
				printer.print("Wasn't able to unlock the sensor,"
						"cannot read the temperature\n");
			}
			while (sw3.read())
				; // wait until switch is released
			counter = 5000;
			isLock = false;
		}
	}

	/* Code never reaches here. Only used to satisfy standard main() */
	return 0;
}
