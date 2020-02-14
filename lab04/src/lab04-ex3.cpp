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

static Printer printer;

/* I2CM transfer record */
static I2CM_XFER_T i2cmXferRec;
/* I2C clock is set to 1.8MHz */
#define I2C_CLK_DIVIDER         (40)
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

void handleEvent();
void show(uint8_t leds);

#define getTemp(temp) ( \
		(int8_t) temp \
)
#define LOW_TEMP 26

enum state {
	DEFAULT, LUNCH, NIGHT
};
static state currentState { DEFAULT };
static int substate { 0 };

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
	DigitalIoPin sw1(0, 17, true, true, true);
	DigitalIoPin sw3(1, 9, true, true, true);

	DigitalIoPin ledGreen { 0, 3, false, false, true };
	DigitalIoPin ledBlue { 1, 1, false, false, true };

	uint32_t time { 0 };

	bool night = false;
	bool lunch = false;

	while (true) {
		if (sw1.read()) {
			while (sw1.read())
				;
			if (!lunch) {
				night = !night;
				//ledBlue = night;
			}
			if (night) {
				currentState = NIGHT;
			} else {
				currentState = DEFAULT;
			}
			substate = 0;
		}

		if (sw3.read()) {
			while (sw3.read())
				;
			if (!night) {
				lunch = !lunch;
				//ledGreen = lunch;
			}
			if (lunch) {
				currentState = LUNCH;
			} else {
				currentState = DEFAULT;
			}
			substate = 0;
		}

		if (counter - time > 2000) {
			time = counter;
			handleEvent();
		}
	}
}

void handleEvent() {
	uint8_t lunch[] { 0, 4, 1, 5 };
	uint8_t def[] { 0, 4, 6, 7 };

	int8_t temp;
	uint8_t tempRaw;
	uint8_t tempAddress { 0x00 };

	++substate;
	if (substate == 4) {
		substate = 0;
	}

	switch (currentState) {
	case DEFAULT:
		show(def[substate]);
		if (substate == 3) {
			SetupXferRecAndExecute(I2C_TEMP_ADDR_7BIT, &tempAddress, 1,
					&tempRaw, 1);
			temp = getTemp(tempRaw);
			printer.print("Temp: %d", temp);
			if (temp < LOW_TEMP) {
				substate = 0;
			}
		}
		break;
	case LUNCH:
		show(lunch[substate]);
		if (substate == 3) {
			SetupXferRecAndExecute(I2C_TEMP_ADDR_7BIT, &tempAddress, 1,
					&tempRaw, 1);
			temp = getTemp(tempRaw);
			printer.print("Temp: %d", temp);
			if (temp < LOW_TEMP) {
				substate = 0;
			}
		}
		break;
	case NIGHT:
		show(7);
		break;
	}
}

void show(uint8_t leds) {
//	DigitalIoPin jacks { 0, 24, false, false, false };
//	DigitalIoPin bar { 1, 0, false, false, false };
//	DigitalIoPin diner { 0, 27, false, false, false };

	DigitalIoPin jacks { 0, 3, false, true, true };
	DigitalIoPin bar { 1, 1, false, true, true };
	DigitalIoPin diner(0, 25, false, true, true);

	if (leds == 7) {
		diner.toggle();
	}

}

