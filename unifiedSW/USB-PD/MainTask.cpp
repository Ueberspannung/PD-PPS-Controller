#include "MainTask.h"
#include "src/memory/sd_detect.h"


mainTask_c::mainTask_c(HardwareSerial & port,	uint16_t RxTimeout_ms) : VT100_c(port,RxTimeout_ms)
{
	init();
}
#ifdef HAS_USB_SERIAL
mainTask_c::mainTask_c(Serial_ &  port,	uint16_t RxTimeout_ms) : VT100_c(port,RxTimeout_ms)
{
	init();
}
#endif

void mainTask_c::init()
{
	Terminal.init(this,&Controller,&Parameter,&Log, &Program);
	SerialIF.init(this,&Controller,&Parameter,&Log, &Program);
	Log.init(&Controller);
	state=state_wait_for_parameter;
	process_state=process_end;
}

void mainTask_c::SerialOpen(void)
{	// Serial API start
	SerialIF.begin();
	Menu.setRemote(true);
}	// Serial API start

void mainTask_c::SerialProcess(void)
{	// Serial API process
	if (SerialIF.process())
        Menu.forceUpdate();
}	// Serial API process

void mainTask_c::SerialClose(void)
{	// Serial API end
	SerialIF.end();
	Menu.setRemote(false);
}	// Serial API end

void mainTask_c::TerminalOpen(void)
{	// TerminalOpen
	Terminal.begin();
	Menu.setRemote(true);
}	// TerminalOpen

void mainTask_c::TerminalProcess(void)
{	// TerminalProcess
	if (Terminal.process())
        Menu.forceUpdate();
}	// TerminalProcess

void mainTask_c::TerminalClose(void)
{	// TerminalClose
	Terminal.end();
	Menu.setRemote(false);
}	// TerminalClose

void mainTask_c::process(void)
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
				process_state=process_log;
				break;
			case process_log:
				Log.process();
				process_state=process_prog;
				break;
			case process_prog:
				Program.process();
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
			if (!Parameter.isBusy() && !Log.isBusy() && !Program.isBusy())
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

					if (*Parameter.getProgramName())
						Program.setProgramFileName(Parameter.getProgramName());

				} // AutoSet
				else if (!Parameter.getExtendedMenu())
				{ 	// standard menu and not autoset -> try PPS
					// try 5V 3A (=15W smallest PD-supply i can think of)
					Controller.set_operating_mode(controller_c::CONTROLLER_MODE_CVCCmax);
					Controller.set_power(5000,3000);
				}	// standard menu and not autoset -> try PPS

				if (Log.hasSD())
				{	// SD Card avaulable
					Log.setInterval(Parameter.getLogInterval()); 
				}	// SD Card avaulable 

				state=state_wait_for_power;
			}
			break;
		case state_wait_for_power:
			if (Controller.is_power_ready() && !Program.isBusy())
			{
				if (startup_ok)
				{
					if ((*Parameter.getProgramName()==NUL) || !Program.hasSD()) // no program => switch on Output
						Controller.enable_output(Parameter.getAutoOn());
					else if (Program.is_loaded() && !Program.has_errors())	// program o.k. => start 
						Program.startProgram(Parameter.getAutoOn(),true);	
				}
				state=state_up;
				SerialIF.pauseAutoSend(false);
			}
			break;
		case state_up:
			if (Controller.is_bus_power_change())
			{	// bus_power_change
				/* 
				 * bus voltage change has been detected:
				 * hystereses: < 1 V ok -> fail
				 *             > 4 V fail -> ok
				 */
				Menu.forceUpdate();
				Terminal.begin();
				SerialIF.pauseAutoSend(true);
				Controller.enable_output(false);
				state=state_wait_for_controller;
			}	// bus_power_cahnge
			if (sd_detect.inserted()) NVIC_SystemReset();
			
			break;
		default:
			state=state_wait_for_parameter;
			break;
	}
}	// process
