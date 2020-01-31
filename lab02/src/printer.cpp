//#include "printer.h"
//#include "ITM_write.h"
//#include "io.h"
//#include <stdio.h>
//#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */
//
//#define MAXLEDS 3
//#define RED 0
//#define GREEN 1
//#define BLUE 2
//
//Printer::Printer() {
//    ITM_init();
//}
//
//void Printer::print(const char *format, ...) {
//    char buffer [256];
//    int cx;
//    va_list argptr;
//    va_start(argptr, format);
//    cx = vsnprintf (buffer, 256, format, argptr);
//    va_end(argptr);
//    ITM_write(buffer);
//}
//
//void Printer::print_cursor() {
//    print("<--");
//}
