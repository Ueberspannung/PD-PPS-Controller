/* ************************************************************************
 * Project	: PD-PPS-Controller
 * Author	: Ueberspannung
 * Date		: 2022/12/19
 * 
 * Description:
 * This project is set up around Ryan Ma's PD-Micro 
 * 			https://github.com/ryan-ma
 * It allows you to controll a PD / PPS capable USB-C via a simple user 
 * interface to use it as a small adjustable power supply
 * It has been tested with different Power Supplies:
 * - UGREEN Nexode 2 Port 100W PD-Charger
 * 		Supported Profiles:
 * 			Fixed	 	5V / 9V / 12V / 15V 3A, 20V 5A
 * 			Augmented 	3.3-21V 5A 
 * 		Comments:
 * 			5A only with coded cable
 * 			no overcurrent shutdown when exceeding requested current
 * - Anker PowerPort I 30W PD
 * 		Supported Profiles:
 * 			Fixed 		5V 3A, 9V 3A, 15V 2A, 20V 1.5A
 * - iLepo USB C Fast Charger 65 W
 * 		Supported Profiles
 * 			Fixed		5V / 9V / 12V / 15V 3A, 20V 3.25A
 * 			Augmented	3.3V - 11V 5A
 * - INIU Power Bank 20000mAh, 22.5W
 * 		Supported Profiles
 * 			Fixed		5V 3A, 9V 2.22A, 12V 1.5A
 * 			Augmented	5.0V - 5.9V 3A
 * 						5.0V - 11V  2A
 * 		Issues
 * 		-	Powerbank has power save mode, when no significant current
 * 			is drawn, Powerbank disconnects and restarts in power some
 * 			kind of power svae mode, this causes the PD detection to fail
 * 			Tested with P_Micro_4.2_log: 
 * 			with load (20R):
 *			0006: FUSB302 ver ID:B_revA
 *			0118: USB attached CC1 vRd-3.0
 *			0172: RX Src_Cap id=1 raw=0x53A1
 *			0172:  obj0=0x2A01912C
 *			0172:  obj1=0x0002D0E9
 *			0172:  obj2=0x0003C096
 *			0172:  obj3=0xC076323C
 *			0172:  obj4=0xC0DC3228
 *			0172:    [0] 5.00V 3.00A
 *			0172:    [1] 9.00V 2.33A
 *			0172:    [2] 12.00V 1.50A
 *			0172:    [3] 5.00V-5.90V 3.00A PPS *
 *			0172:    [4] 5.00V-11.00V 2.00A PPS
 *			0176: TX Request id=0 raw=0x1082
 *			0176:  obj0=0x42022628
 *			0186: RX GoodCRC id=0 raw=0x0121
 *			0192: RX Accept id=2 raw=0x05A3
 *			0210: RX PS_RDY id=3 raw=0x07A6
 *			0212: PPS 5.50V 2.00A supply ready
 *			0214: Load SW ON
 *			5214: TX Request id=1 raw=0x1282
 *			5214:  obj0=0x42022628
 *			5222: RX GoodCRC id=1 raw=0x0321
 *			5228: RX Accept id=4 raw=0x09A3
 *			5246: RX PS_RDY id=5 raw=0x0BA6
 *			5246: PPS 5.50V 2.00A supply ready
 *			10248:TX Request id=2 raw=0x1482
 *			10248: obj0=0x42022628
 *			10256:RX GoodCRC id=2 raw=0x0521
 *			10262:RX Accept id=6 raw=0x0DA3
 *			10280:RX PS_RDY id=7 raw=0x0FA6
 *			10280:PPS 5.50V 2.00A supply ready
 *			...
 * 			without load:
 *			0006: FUSB302 ver ID:B_revA
 *			0118: USB attached CC1 vRd-3.0
 *			0172: RX Src_Cap id=1 raw=0x53A1
 *			0172:  obj0=0x2A01912C
 *			0172:  obj1=0x0002D0E9
 *			0172:  obj2=0x0003C096
 *			0172:  obj3=0xC076323C
 *			0172:  obj4=0xC0DC3228
 *			0172:    [0] 5.00V 3.00A
 *			0172:    [1] 9.00V 2.33A
 *			0172:    [2] 12.00V 1.50A
 *			0172:    [3] 5.00V-5.90V 3.00A PPS *
 *			0172:    [4] 5.00V-11.00V 2.00A PPS
 *			0176: TX Request id=0 raw=0x1082
 *			0176:  obj0=0x42022628
 *			0186: RX GoodCRC id=0 raw=0x0121
 *			0192: RX Accept id=2 raw=0x05A3
 *			0210: RX PS_RDY id=3 raw=0x07A6
 *			0212: PPS 5.50V 2.00A supply ready
 *			0214: Load SW ON
 *			5214: TX Request id=1 raw=0x1282
 *			5214:  obj0=0x42022628
 *			5222: RX GoodCRC id=1 raw=0x0321
 *			5228: RX Accept id=4 raw=0x09A3
 *			5248: RX PS_RDY id=5 raw=0x0BA6
 *			5248: PPS 5.50V 2.00A supply ready
 *			10250:TX Request id=2 raw=0x1482
 *			10250: obj0=0x42022628
 *			10258:RX GoodCRC id=2 raw=0x0521
 *			10264:RX Accept id=6 raw=0x0DA3
 *			10284:RX PS_RDY id=7 raw=0x0FA6
 *			10284:PPS 5.50V 2.00A supply ready
 *			
 *			0006: FUSB302 ver ID:B_revA
 *			0118: USB attached CC1 vRd-3.0
 *			0352: TX Get_Src_Cap id=0 raw=0x0087
 *			0704: TX Get_Src_Cap id=0 raw=0x0087
 *			1056: TX Get_Src_Cap id=0 raw=0x0087
 *			....			
 * 		-	the PD libriray selects automatically the most suitable profile
 * 			when selecting 5-11V augmented profile the device needs to be 
 * 			set to at least 9V fixed profile in order to select the 5-11V 
 * 			profile, otherwise the 5-5.9V profile will be selected
 * 
 * 
 * Some imorovements implemented with prototype #2 hardware
 * I observed a difference between set voltage and measured voltage
 * This is due to a not so accurate voltage stabilization in the power
 * supply and voltage loss over the USB cable wenn used with higher
 * currents.
 * Therefore a PI like regulator has been implemented. 
 * The P part ist directly derived from the requested voltage and the
 * I part is directly derived from the existing voltage difference
 * Allthoug the algorithm works quite well the response is limited by
 * the setup speed of the power supply. This is about 40ms with the 
 * UGREEN Nexode power supply
 * 
 * 
 * 
 * 
 * The Prototype #1 has been built using these items
 * 1. Ryan Ma's PD-Micro
 * 2. 256kBit EEProm Module
 * 3. ACS712-05 Current Sensor Modul
 * 4. 2004 HD44780 type LCD
 * 5. PCF8574(A) LCD Interface Modul
 * 6. two voltage dividers 1:5 (1,2k / 4,7k+100R)
 * 7. LM4040 2.5V Voltage reference with a 2.2k series resistor
 * 8. A voltage inverter (220R series resistor, 2x 1N4148, 2x 100uF)
 * 9. A LM334 current source with a 150-350R set resistor
 * 
 * SetUp:
 * 
 * the LCD interface comes with a 10k potentiometer, this has to be removed
 * and the cotrast pin has to be driven by the current source. To generate a
 * sufficant voltage drop even at 3,3V a negative voltage is needed. This is 
 * generated using a simple voltag inverter with 220R sieries ressitor and 
 * tow 100uF capacitors. The Current sorce set Resistors is a 150R resistor 
 * and a 200R potentiometer. this gives stabel contrast over the complete 
 * operating voltage range. The voltage inverter is driven by a PWM signal on
 * Digital Pin 9
 *
 * The voltage reference is connected to ADC Channel 4 (A0)
 * The current sensor is connected to ADC Channel 5 (A1)
 * The output voltag devider is connected to ADC Channel 6 (A2)
 * The Vbus voltage devider is connected to ADC Channel 7 (A3)
 *
 * The Switch / Encoder pins are connected dig.In 14-16
 * dig.In 14 (PB3) DT
 * dig.In 15 (PB1) CLK
 * dig.In 16 (PB2) SW
 * 
 * 
 * 
 * The Protoype #2 has a few imporvements and simplifications
 * It has been difficult to measure the output current due to three 
 * reasons
 * 
 * 1. I've been defraude by an chinese ebay seller. The ACS712 turned
 * out to be a relabled ACS 704
 * 
 * 2. Although i tested a real ACS712 it was very sensytive to VCC noise
 * and external magnetic fields
 * 
 * 3. reccomended operating voltage is 4.5 V to 5.5 V. This numbers 
 * should be taken seriously. The chip dos not operate reliable below 4.5 V. 
 * 
 * Therefore I decided to switch to a IN219 based current / voltage sensor
 * with a 0.15R shunt and 80mV input range
 * This step omits the need for a voltage reference and to measure to 5V rail.
 * There is no longer the need to use the internal adc.
 * 
 */
 
 /* *****************************************************************************
  * Revison History:
  * 1.0 Project start with ACS 712
  * 2.0 switch to INA219
  * 2.1 implemented Constant Voltage Regulator
  * 2.2 implemented Constant Current Regulator
  * 2.3 clean up regulator code
  * 3.0 port to Zero (SAMD21G18) platform
  * 3.1 combine SAM and AVR several fixes
  * 3.2 additinol CVCC modes utilizing power supply features
  */

#include <Wire.h>
#include "menu.h"

menu Menu;

void setup()
{
	Wire.begin();
	Wire.setClock(400000);
}

void loop() {
	Menu.process();
}
