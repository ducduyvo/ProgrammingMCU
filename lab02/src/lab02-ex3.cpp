#include "ITM_write.h"
#include <stdio.h>
#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */
#include "stdlib.h"
#include "DigitalIoPin.h"

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#define size 50
#define speed 72000000
#define TICKRATE_HZ1 (1000)	/* 1000 ticks per second */
//#include <atomic>
//static volatile std::atomic_int counter;
//
//void Sleep(int ms) {
//	counter = ms;
//	while (counter > 0) {
//		__WFI();
//	}
//}
int compare(const void * a, const void * b) {
	return (*(int*) a - *(int*) b);
}
class Printer {
public:
	Printer();
	void print(const char *format, ...);
};

Printer::Printer() {
	ITM_init();
}

void Printer::print(const char *format, ...) {
	char buffer[256];
	int cx;
	va_list argptr;
	va_start(argptr, format);
	cx = vsnprintf(buffer, 256, format, argptr);
	va_end(argptr);
	ITM_write(buffer);
}

int main(void) {
	Board_Init();
	uint32_t sysTickRate;
	Chip_Clock_SetSysTickClockDiv(1);
	sysTickRate = Chip_Clock_GetSysTickClockRate();
	SysTick_Config(sysTickRate / TICKRATE_HZ1);
	ITM_init();
	char buf[256];


	DigitalIoPin sw1(0, 17, true, true, true);
	DigitalIoPin sw2(1, 11, true, true, true);
	DigitalIoPin sw3(1, 9, true, true, true);



	CoreDebug->DEMCR |= 1 << 24;
	DWT->CTRL |= 1;


	Printer printer;
	int arr[size] = { 0 };
	int idx = 0;
	while (idx < size) {
		if (sw1.read()||sw2.read() || sw3.read()) {
			int value1 = DWT->CYCCNT;
			Sleep(1);
			int value2 = DWT->CYCCNT;
			arr[idx] = value2 - value1;
			printer.print("diff: %d\n", arr[idx]*1000000/speed);
			while (sw1.read()||sw2.read() || sw3.read())
				;
			Sleep(50);
			idx++;
		}
	}
//	for (int i = 0; i < 50; i++) {
//		for (int j = 0; j < (50 - i - 1); j++) {
//			if (arr[j] > arr[j + 1]) {
//				int temp = arr[j];
//				arr[j] = arr[j + 1];
//				arr[j + 1] = temp;
//			}
//		}
//	}
	qsort(arr, size, sizeof(int), compare);
	float sum = 0;

	for (int i = 0; i < size; i++) {
		snprintf(buf, 256, "[%d]: %d\n", i + 1, arr[i]);
		sum += arr[i];
		ITM_write(buf);
	}
	float ave = sum /size;
	snprintf(buf, 256,
			"The average sleep time in counter cycles: %f\nAnd in microseconds: %f\n",
			ave, ave*1000000/speed );
	ITM_write(buf);

}
