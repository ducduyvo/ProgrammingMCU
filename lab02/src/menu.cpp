//#include "io.h"
//#include "printer.h"
//#include "menu.h"
//
//
//Menu::Menu(const DigitalIoPin &sw1, const DigitalIoPin &sw2, const DigitalIoPin &sw3,
//        const DigitalIoPin &led1, const DigitalIoPin &led2, const DigitalIoPin &led3)
//{
//    sws[0] = new DigitalIoPin(sw1);
//    sws[1] = new DigitalIoPin(sw2);
//    sws[2] = new DigitalIoPin(sw3);
//    leds[0] = new DigitalIoPin(led1);
//    leds[1] = new DigitalIoPin(led2);
//    leds[2] = new DigitalIoPin(led3);
//    printer = new Printer();
//}
//Menu::~Menu(){
//	delete  sws[0];
//	delete  sws[1];
//	delete  sws[2];
//	delete  leds[0];
//	delete  leds[1];
//	delete  leds[2];
//	delete  printer;
//
//
//}
//
//void Menu::print_menu() {
//    printer->print("Select led:\n");
//    printer->print("%*s%*s", PADDING, leds[0]->get_name(), 3, leds[0]->get_state());
//    if (cursor_pos == RED) printer->print_cursor(); // Red
//    printer->print("\n");
//
//    printer->print("%*s%*s", PADDING, leds[1]->get_name(), 3, leds[1]->get_state());
//    if (cursor_pos == GREEN) printer->print_cursor(); // Green
//    printer->print("\n");
//
//    printer->print("%*s%*s", PADDING, leds[2]->get_name(), 3, leds[2]->get_state());
//    if (cursor_pos == BLUE) printer->print_cursor(); // Blue
//    printer->print("\n\n");
//}
//
//void Menu::increment_cursor() {
//    ++cursor_pos;
//    if (cursor_pos >= MAXLEDS) cursor_pos = 0;
//}
//
//void Menu::decrement_cursor() {
//    --cursor_pos;
//    if (cursor_pos < 0) cursor_pos = MAXLEDS - 1;
//}
//
//
//void Menu::print_cursor() {
//    printer->print("<--");
//}
//
//void Menu::check_input() {
//    if (sws[0]->read()) {
//        increment_cursor();
//        print_menu();
//        while (sws[0]->read());
//    }
//
//    if (sws[1]->read()) {
//        leds[cursor_pos]->toggle();
//        print_menu();
//        while (sws[1]->read());
//    }
//
//    if (sws[2]->read()) {
//        decrement_cursor();
//        print_menu();
//        while (sws[2]->read());
//    }
//
//}
