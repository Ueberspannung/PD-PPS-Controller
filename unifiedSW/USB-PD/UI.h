#ifndef __UI__
#define __UI__

#include "config.h"
#include "src/VT100/VT100.h"
#include "src/UI/Terminal.h"
#include "src/UI/menu_lcd.h"
#include "src/UI/menu_mini.h"
#include "src/UI/SerialIF.h"
#include "controller.h"
#include "parameter.h"

class UI_c:public VT100_c
{
	public:

		UI_c(HardwareSerial & port,	uint16_t RxTimeout_ms=RX_TIMEOUT);
		#ifdef HAS_USB_SERIAL
		UI_c(Serial_ & port,	uint16_t RxTimeout_ms=RX_TIMEOUT);
		#endif

		void TerminalOpen(void);
		void TerminalProcess(void);
		void TerminalClose(void);
		void SerialOpen(void);
		void SerialProcess(void);
		void SerialClose(void);

		void process(void);

	private:
		typedef enum:uint8_t {	state_wait_for_parameter,
								state_wait_for_controller,
								state_wait_for_power,
								state_up } startup_et;

		typedef enum:uint8_t {	process_param,
								process_menu,
								process_serial,
								process_end } process_state_et;

		static const uint16_t RX_TIMEOUT=100;

		controller_c 	Controller;
		parameter 		Parameter;
		#if defined(PD_PPS_CONTROLLER)
			menu_lcd_c		Menu{&Controller,&Parameter};
        #elif defined (PD_PPS_CONTROLLER_MINI)
            menu_mini_c     Menu{&Controller,&Parameter};
		#endif
		Terminal_c		Terminal;
		SerialIF_c		SerialIF;

		startup_et		state;
		process_state_et process_state;
		bool			startup_ok;

		void init(void);
};

#endif // __UI__
