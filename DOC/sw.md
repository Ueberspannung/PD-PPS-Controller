### the software

#### Table of contents
 - [AVR SW](#AVR-SW)
   - [build instructions](#AVR-build-instructions)
 - [first ARM prototype SW](#first-ARM-prototype-SW) 
   - [work in progress](#work-in-progress)
   - [build instructions](#first-ARM-prototyp-SW-build-instructions)
 - [unified sw](#unified sw)
   - [building the first ARM protype on unified SW](#building-the-first-ARM-protype-on-unified-SW)
   - [building the mini](#building-the-mini)
   - [modification of board definitions for the mini](#modification-of-board-definitions-for-the-mini)
     - [creating the mini board](#creating-the-mini-board)
	 - [description of the mini variant](description-of-the-mini-variant)
   - [flashing the mini bootloader](#flashing-the-mini-bootloader)
   - [building the mini application](#building-the-mini-application)
   
## AVR SW
the AVR SW is written with Arduino IDE. While developing the SW I found out a few things about the system:
- The booltloader on AT32U4 devices (such as Leonardo) takes up 4 kB of program space, that's 1/8 of the total
program space. 
- the AVR is not specified for 16 MHz @ 3.3 V, the clock has to be scaled down when operating at voltages below 5V
- the LCD backlight brightness drops sginificantly from 5 V to 3.3 VCC
- 32 kB is quite small for this project.

after initial succes a quickly ran into issues with FLASH and RAM. You might be able to run the AVR code but I 
recommend to reduce the functionality the free some memory.


### AVR build instructions
The software in the AVR folder is as is. There will be no further development in this branch. It kind of work but use 
at your own risk.
It only compiles without bootloader, you will need to add the boards.local.txt file from the config folder to
```
C:\Users\_user_\AppData\Local\Arduino15\packages\arduino\hardware\avr\1.8.6\
```
(at least on windows machines) and select ***Arduino Leonardo w/o bootloader*** from the available boards.

You will need [AVRDUDE](https://github.com/avrdudes/avrdude/) and an [USBasp](https://www.fischl.de/usbasp/)  
***Hint: be careful when purchasing one of the cheap clones. They often come with outdated SW and will not work. 
It is no problem to update those but you will need a working USBasp adaptor.***

programmer to flash the SW. For non cli users: [AVRDUESS](https://github.com/ZakKemble/AVRDUDESS) is a great gui for AVRDUDE.   
You might as well use an Arduino Uno as [USB to ISP bridge](https://docs.arduino.cc/built-in-examples/arduino-isp/ArduinoISP/).  


## first ARM prototype SW

The SW has been refactored and is now USB-PD2. 
in the first SW the control of the power supply was implemente in the menu class. This is been moved to its own controller class.  
A header for TODO reminders has been added
A power supply test class has been added to test profile changes.

#### work in progress
- a GUI will be added to controll the circuit when a VT100 Terminal is connected to micro USB
- a simple protocl will be added to control the device directly from SW when connected either to USB or HW UART


#### first ARM prototyp SW build instructions
In order to flash the SW for the first time you will need A JTAG Debug Interface such as 
- [Atmel ICE](https://www.microchip.com/en-us/development-tool/atatmel-ice)
- [Segger J-Link](https://www.segger.com/debug-trace-embedded-systems/)
- any other ARM 3.3 V JTAG / SWD debug probe which is supported by Atmel Studio or Arduino IDE.  
  many cheap ARM debug probes seem to emulate Segger J-Link.   
  you might find your luck with either the mythological greek female warrior or the Forty Thieves form One Thousand and One Nights. 
- maybe an Arduino Zero wich has a EDGB (not tested)

You can use Atmel Studio or the Arduino IDE to flash the Arduio Zero bootloader.  
You can use the Blink Demo as well. Select Sketch -> export binary. The IDE will create a file xxx.ino.with_bootloader.arduino_zero.bin.
Just flash the file. You might need to copy the fuse bits from an Arduino Zero.  
There are some samples in the Fuses folder. Some of the fuses are factory tuned and can not be overwritten.

Once the bootloader works you can flash SW using the SAM-BA bootloader. The SW provided by Microchip has a cli 
which is suitable for all SAM devices supporting SAM-BA and is therefore a bit tricky to use. A much more simple
tool is [Bossa from Shumatech](https://www.shumatech.com/web/products/bossa).

## unified sw
With the rising of the mini it was necessary to do some rework on the sw. The goal is to have one SW for all HW platforms.
The plattform is selected in the global config file.   
As mentioned earlier the AVR SW will not be supported. Only ARM (ATSAMD21G18) based boards will be supported.
Due to some HW use beyond the ARDUINO ZERO board some modifications to the board definitions and bootloader had to be made.
The unified SW uses subfolders for the HW layers and other componets.  

### building the first ARM protype on unified SW
The HW use is fully compatible to the ARDUINO Zero board. no modifications needed.
Just bild the blink example on the Zero board ord flash the Bootloader directly.  
uncomment the define  
```
#define PD_PPS_CONTROLLER
```
in the config.h file. Build for Zero Board, done


### building the mini
#### modification of board definitions for the mini
##### creating the mini board

locate the user specific arduino folder. typicalli its located at
```
c:\Users\_user_\AppData\Local\Arduino15\
```
on windows machines and
```
~home/_user_/-arduino15
```
navigate to
```
./packages/arduino/hardware/samd/1.8.14/
```
copy the boards.local.txt to this folder. If it alread exist just add the contents to 
your borads.local.txt but be careful not to create double entries.

navigate to 
```
variants
```
create a folder with the name
```
pd-pps-controller-mini
```
copy the contents of the
```
arduino_zero
```
folder to the new created one and copy replace the 
```
variant.c
variant.h
```
with those form the repository

when you start your IDE you can select the **PD-PPS Controller Mini (Native USB Port)**

##### description of the mini variant

variants.h:
 - the rx and tx led are not used but need to be disabled  
   PB03 is a button and PA27 is card detect
 - Serial EDBG interface is not required and disabled  
   later Serial is remapped to SerialUSB
 - we need two SPI ports
   - Port one (SPI = sercom4) is the SPI Header on the Arduino Zero, is maped Pins D10-13 as sercom1  
   - Port two (SPI1= sercom4) is used for SD Card as done in the mkrZero, PA12-15 and 27 need to modified in vartiants.c 
 - the extern UART Serial definion needs to be removed in order to be replaced with USB
 - the Serial has to be defined as SerialUSB to make Serial gloaballe availabe as USB Serial
 
variants.cpp:
 - PA14 (SS) is redefined as simple IO, no interrupts needed
 - PA15 (MISO) is redefined to sercom_alt
 - PA18,16,19 (D10-12) are redefined to use as SPI (SS, MOSI) MISO not used for display commuinication, used as io
 - PA17 redefined for SPI Display
 - PA12 moved from SPI Haeder to SD-Card
 - PB03 redefined as IO with interrupt (button)
 - PA27 redefined as IO without interrupt, input only (card detect)
 - PA13 assigned to sercom4 (SD-Card)
 - definitions of serial removed, now mapped to SerialUSB
 

#### flashing the bootloader
you will need to use the modified bootloader 
```
samd21_sam_ba_pd_pps_controller_mini
```
flash it as described in [first ARM prototyp SW build instructions](#first-ARM-prototyp-SW-build-instructions)

two files have been modified / added:
board_definitions.h:
 - added include of board_definitions_pd_pps_controller_mini.h for BOARD_ID_pd_pps_controller_mini)

board_definitions_pd_pps_controller_mini.h derived from borad_definitions_arduino_zero:
 - added SAM_BA_USBCDC_ONLY to prevent bootloader from switching to serial mode, as the orignial serial pins are used for different tasks
 - changed product string to "PD-PPS Mini"
 - changed LED Port and pin to red led
 - removed definitions for Rx/Tx LED (pins are used for button ond card detect)
 
 
#### building the mini application
after the preparations have been done the board should be visible as serial port
select the Mini and the new serial port. open the ino file and upload  
done  





[home](../README.md)