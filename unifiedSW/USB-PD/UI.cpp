#include "UI.h"



UI_c::UI_c(HardwareSerial & port,	uint16_t RxTimeout_ms) : VT100_c(port,RxTimeout_ms)
{
	init();
}
#ifdef HAS_USB_SERIAL
UI_c::UI_c(Serial_ &  port,	uint16_t RxTimeout_ms) : VT100_c(port,RxTimeout_ms)
{
	init();
}
#endif

void UI_c::init()
{
	Terminal.init(this,&Controller,&Parameter);
	SerialIF.init(this,&Controller,&Parameter);
	state=state_wait_for_parameter;
	process_state=process_end;
}

void UI_c::SerialOpen(void)
{	// Serial API start
	SerialIF.begin();
	Menu.setRemote(true);
}	// Serial API start

void UI_c::SerialProcess(void)
{	// Serial API process
	if (SerialIF.process())
        Menu.forceUpdate();
}	// Serial API process

void UI_c::SerialClose(void)
{	// Serial API end
	SerialIF.end();
	Menu.setRemote(false);
}	// Serial API end

void UI_c::TerminalOpen(void)
{	// TerminalOpen
	Terminal.begin();
	Menu.setRemote(true);
}	// TerminalOpen

void UI_c::TerminalProcess(void)
{	// TerminalProcess
	if (Terminal.process())
        Menu.forceUpdate();
}	// TerminalProcess

void UI_c::TerminalClose(void)
{	// TerminalClose
	Terminal.end();
	Menu.setRemote(false);
}	// TerminalClose

void UI_c::process(void)
{	// process
	Controller.process();
	
	if (!Controller.is_busy())
	{	// controller is not busy
		// process other tasks
		switch (process_state)
		{	// process_state
			case process_param:
				Parameter.process();
				process_state=process_menu;
				break;
			case process_menu:
				Menu.process();
				process_state=process_serial;
				break;
			case process_serial:
				VT100_c::process();	
				process_state=process_param;
				break;
			default:
				process_state=process_param;
				break;
		}	// process_state
	}	// process other tasks
	
	switch (state)
	{
		case state_wait_for_parameter:
			if (!Parameter.isBusy())
			{
				state=state_wait_for_controller;
			}
			break;
		case state_wait_for_controller:
			if (Controller.is_up())
			{
				uint16_t Cal,Ref;
				uint16_t u;
				uint16_t i=5000;

				startup_ok=true;
				Parameter.getCalI(&Cal,&Ref);
				Controller.set_output_current_calibration(Cal,Ref);


				if (Parameter.getAutoSet())
				{	// AutoSet
					// generate dummy profile change
					Controller.set_operating_mode(Parameter.getCVCC());

					u=Parameter.getVoltage_mV();
					i=Parameter.getCurrent_mA();
					if (Parameter.getModePPS() && 				// reqeuested profile is APDO
						(Controller.get_max_voltage_mV(0)>0))	// max voltage is >0 if APDO is available
					{	// pps
						if (u>Controller.get_max_voltage_mV(0))
							u=Controller.get_max_voltage_mV(0);
						if (u<Controller.get_min_voltage_mV())
							u=Controller.get_min_voltage_mV();
						if (i>Controller.get_max_current_mA(u))
							i=Controller.get_max_current_mA(u);
						startup_ok=	startup_ok &&
									(u==Parameter.getVoltage_mV()) &&
									(i==Parameter.getCurrent_mA());
						Controller.set_power(u,i);
					}	// pps
					else
					{	// fix
						u=Controller.get_fix_voltage_mV(u);
						startup_ok=	startup_ok &&
									(u==Parameter.getVoltage_mV());

						Controller.set_voltage(u);
					}	// fix
				} // AutoSet

				state=state_wait_for_power;
			}
			break;
		case state_wait_for_power:
			if (Controller.is_power_ready())
			{
				if (startup_ok)
					Controller.enable_output(Parameter.getAutoOn());
				state=state_up;
			}
			break;
		case state_up:
			break;
		default:
			state=state_wait_for_parameter;
	}
}	// process
