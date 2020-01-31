//#ifndef IO_H_
//#define IO_H_
//
//void Sleep(int ms);
//
//class DigitalIoPin {
//    public:
//        DigitalIoPin(int port, int pin, bool input = true, bool pullup = true, bool invert = false, const char *name_ = "");
//        /* virtual ~DigitalIoPin(); */
//        bool read();
//        void write(bool on);
//        void toggle();
//        int measure_press();
//        const char *get_name() { return name; }
//        const char *get_state() { return ((read()) ? ("ON") : ("OFF")); }
//    private:
//        int port;
//        int pin;
//        bool input;
//        bool pullup;
//        bool invert;
//        const char *name;
//};
//
//#endif
