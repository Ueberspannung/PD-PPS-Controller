### the software

#### Table of contents
 - [AVR SW](#AVR-SW)
   - [build instructions](#AVR-build-instructions)
 - [first ARM prototype SW](#first-ARM-prototype-SW) 
   - [work in progress](#work-in-progress)
   - [build instructions](#first-ARM-prototyp-SW-build-instructions)
 - [unified sw](#unified sw)
   - [building the first ARM protype on unified SW]#(#building-the-first-ARM-protype-on-unified-SW)
   - [building the mini](#building-the-mini)
   - [modification of board definitions](#modification-of-board-definitions)
 
   
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
As mentioned earlier the AVR SW wull not be supported. Only ARM (ATSAMD21G18) based boards will be supported.
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
#### modification of board definitions
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


#### flashing the bootloader
you will need to use the modified bootloader 
```
samd21_sam_ba_pd_pps_controller_mini
```
flash it as described in [first ARM prototyp SW build instructions](#first-ARM-prototyp-SW-build-instructions)
this fixes three issues:
 - there is no buildin led
 - standard bootloader is dual serial and usb,  
   the serrial pins are in use for different tasks, this causes tghe bootloader to get stuck in serial mode and it will not be acccessible
 - RX and TX LEDs are not present, pins used for different tasks
 
#### building the mini application
after the preparations have been done the board should be visible as serial port
select the Mini and the new serial port. open the ino file and upload  
done  





[home](../README.md)