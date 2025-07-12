# The Serial Programming Interface

#### Table of contents
- [inteface parameters](#interface-parmeters)  
- [the catch with Arduino Windows and RS232 over USB](#the-catch-with-Arduino-Windows-and-RS232-over-USB)
  - [windows sucks](#windows-sucks)
  - [the delay function](#the-delay-function)
  - [the solution](#the-solution)
- [connecting to the mini](#connecting-to-the-mini)
- [command structure](#command-structure)
  - [request](#request)
  - [reply](#reply)
  - [naming conventions](#naming-conventions)
- [commands](#commands)
  - [system state](#system-state-)
  - [profile number](#number-of-profile-)
  - [output state](#requesting-the-output-state-)
  - [current calibration](#current-cailbration-c)
  - [erase settings](#erase-settings-e)
  - [find closest profile](#find-closest-fix-voltage-profile-f)
  - [avialable current and current limit](#avialable-current-and-current-limit-i)
  - [number profiles](#request-number-of-available-profiles-n)
  - [profile data](#request-profile-data-n)
  - [output switch](#operate-output-switch-o)
  - [controller operating mode](#controller-operating-mode-r)
  - [start up parameter](#start-up-parameter-s)
  - [profile type](#profile-type-t)
  - [voltage setting and available voltage](#voltage-setting-and-available-voltage-u)
  - [program version](#program-version-v)

## interface parameters
The serial communication parameters should be set to 115200 8N1. This is a USB 
serial device so any parameter should do.  

## the catch with Arduino Windows and RS232 over USB
When trying to identify if a serial connection is estaqblished or not I discoverd two
quite annoying flaws.
### windows sucks
according to the arduino documentation it should be as ease as a simple if statement
``` 
if (SerialUSB)
{	
    /*
        serial connection has been established
		do communication
	*/
}
else
{
    /*
	    no serial connection
		do other stuff
	*/
}
```
... and yes, it works, but not quite as expected.  
When opening a serial port on the windows machine the arduino detects the open port and
one can do their stuff.  
But when the port is closed on the windows machine it will stay open on the arduino.  
By the way trying this on a linux machine the port is detected open and closed on 
the arduino as expected.

### the delay function
when searching for USB CDC and arduino core libraries I discoverd that they added a 
delay in the SerialUSB call. Its in Serial_::operator bool() in CDC.cpp. Each call to 
SerialUSB takes 10 ms blocking. As the FUSB302 needs to be serviced every 10 ms this 
is a no go.

### the solution
most Terminal programs control the DTR line upon establishing a connection. A call to 
the SerialUSB.dtr() can be used even if there is no serial connection. So switching
between local and remote mode is done by using the dtr line.


## connecting to the mini
> [!IMPORTANT]  
> when connecting to the serial interface, it is important to raise the DTR line. This
> will indicate that there is a real connection and the module will display `REMOTE`.  
> Wenn disabling the DTR line the module will switch to local mode and display the
> menue icons instaed of `REMOTE`.

when a serial commection is establishe, the mini will try to identify a VT100 
terminal. This is doen using th `ESC[C` command. It will try 6 times. Just ignore 
these commands and flush the input buffer ~1s after the connection has been established.  
The mini is now in serial programming mode.  


## command structure
### request
1. 2 digiti ASCII command code  
	a. `?` as first character to request information  
	b. `!` as first character to change settings  
2. separator (SPACE)  
3. parmater  
    a. numerical parameter ('0'-'9' / '.')  
	b. text (ASCII)  
4. terminator (\r)  

### reply
1. single value (numerical or ASCII) or multiple values separated by (SPACE)
2. terminator (\r)

### naming conventions
- {a|b|c} Optionsfeld "a" "b" oder "c"
- [val] numerischer Wert ggf mit Vorzeichen und '.'
- [int] ganzzahliger Wert ggf mit Vorzeichen
- [uint] positiver, ganzzahliger Wert
- `<-` data transfer from computer to PD-PPS-Controller
- `->` data transfer from PD-PPS-Controller to computer

### commands

command | acces | description
:-----: | :---: | :----------
`!`     |   R   | [system state](#system-state-)
`#`     |  R/W  | [profile number](#number-of-profile-)
`?`     |  R/W  | [output state](#requesting-the-output-state-)
`C`     |  R/W  | [current calibration](#current-calibration-c)
`E`     |   W   | [erase settings](#erase-settings-e)
`F`     |   R   | [find closest profile](#find-closest-fix-voltage-profile-f)
`I`     |  R/W  | [avialable current and current limit](#avialable-current-and-current-limit-i)
`N`     |   R   | [number profiles](#request-number-of-available-profiles-n) and [profile data](#request-profile-data-n)
`O`     |  R/W  | [output switch](#operate-output-switch-o)
`R`     |  R/W  | [controller operating mode](#controller-operating-mode-r)
`S`     |  R/W  | [start up parameter](#start-up-parameter-s)
`T`     |   R   | [profile type](#profile-type-t)
`U`     |  R/W  | [voltage setting and available voltage](#voltage-setting-and-available-voltage-u)
`V`     |   R   | [program version](#program-version-v)

#### system state `!`
```    
<- ?!
-> {ready|busy [info]}
-> busy [info] 
        reading parameter [val]%
        erasing paramerer [val]%
        waiting for PD
        waiting for change
```  
- **`{ready|busy [info]}'**  
  - **`ready`** initialization completed, ready for commands
  - **`busy [info]`**  device is busy, see **`[info]`** for details
    - **`reading parameter [val]%`** initilization ongoing
    - **`erasing paramerer [val]%`** erase command ongoing
    - **`waiting for PD`** initial handshake with PD source
    - **`waiting for change`** profile / voltage change ongoing

#### number of profile `#`
to query the currently selected profile
```
<- ?#
-> ?# [INT] 
```
- **`[INT]`** number of current profile  
  if more than one augmented profile is availabe only the first will be reported for detials
  see [get profile data](#get-profile-data)

to select the desired profile
```
<- !# n
-> !# [INT] 
```
> [!NOTE]
> instead of the requested selected profile one might get a busy message, in this case the 
> command is still pending and needs to be finished by polling the currently selected profile 
> using `?#`


#### requesting the output state `?`
```
<- ??
-> ??{0|1|!} [val]V [val]V [val]A 
```
1. **`{0|1|!}`**  
   **`0`** outpunt off  
   **`1`** outpunt on  
   **`!`** constant current mode  
2. **`[val]V`** bus voltage at USB C input
3. **`[val]V`** output voltage
4. **`[val]A`** output current

#### requesting periodic output state messages
```
<- !?
<- !? [INT]ms 
-> ?? [INT]ms 
```
This command sets the intervall for periodic output state messages.  
The valid range is 100-65535 ms.  
Any value below 100 ms disables the function (this includes no value).  


#### current calibration `C`
requsting calibration data:
```
<- ?C 
-> ?C [val]A [val]A (interne Messwert, Referenzwert)
```
setting calibration data:
```
<- !C [val]A [val]A (interne Messwert, Referenzwert)
-> !C [val]A [val]A (interne Messwert, Referenzwert)
```
1. **`[val]A`**   
   internal current value
2. **`[val]A`**  
   external current value (measure with precise ampere meter)

#### erase settings `E`
```
	<- !E
	-> !E busy erasing parameter 0%
```
use [system state command `?`](#system-state-) or `?E` to query the progress. 
needs to be executed before calibration

#### find closest fix voltage profile `F`
use this command to find the closest fixed voltage profile which is less or euqal to the requested voltage.
```
<- ?F [val]V 
-> ?F [val]V
```
- in request : desired voltage
- in reply: closest voltage

example:  
   available profiles: 5,9,12,15,20V  
   requested voltage: 11.5 V  
   closest voltage: 9 V  
   
#### avialable current and current limit `I`
query the max. availe current (fix profile) or the programmed current limit (augmented profile)
```
<- ?I 
-> ?I [val]A
```

somtimes a manufacturer implements multiple augmented profiles with different voltage range and
different current rating.  
the PD-PPS-Controller reports treats this profiles as one. to get the auvailabele current at a 
certain voltage just at the voltage of interest to the request.
```
<- ?I [val]V	
-> ?I [val]A
```

to set a certain current limit in PPS mode add the desired current to the set current command.
```
<- !I [val]A 
-> !I [val]A
```
the returned current is the actually set value.  
> [!NOTE]  
> Although current limits less than 1 A can be set this is not defined. Current limiting is defined
> to -25 mA / +100 mA on load change to +/-500 mA. I could not safely verfy this limits.  
> I found that the current is limited to 110 % - 130 % of the nominal Value. an Overshoot to 200 % 
> causes the charger to reset

#### request number of available profiles `N`
```
<- ?N
-> ?N [INT] 
```
- `[INT]` number of available profiles, 1-7, 0 if no PD source has been detected 

#### request profile data `N`

```
<- ?N [n] (n: Nummer des Profils)
-> ?N #[n]: {FIX|AUG|BAT|VAR} [profile data]
```
- `FIX [profile data]`
   - `[val]V [val]A`  
     fix profile, fixed voltage and max. available current
- `AUG [profile data]`
   - `[val]V - [val]V [val]A`  
     augmente profile, min. and max. availabe voltage and max current
- `BAT [profile data]`
   - `[val]V - [val]V [val]W`  
     battery profile, max. voltage, max current and max. power
- `VAR [profile data]`
   - `[val]V - [val]V [val]A`  
     variable power supply profile, has been replace by augmented profile

#### operate output switch `O`
switch output
```
<- !O {0|1}
-> !O {0|1}
```
request output state
```
<- ?O
-> ?O {0|1}
```

#### controller operating mode `R`
> [!NOTE]  
> this command is only available in PPS mode  

set controller operating mode
```
<- !R {OFF|CV|CVCC|CVCC^}
-> !R {OFF|CV|CVCC|CVCC^}
```
request controler operating mode
```
<- ?R
-> ?R {OFF|CV|CVCC|CVCC^}
```
- `OFF` controller is of output relies entirely on PD source
- `CV` PD source is adjusted to match the output voltage to the set value, 
   current limiting is provided by PD source
- `CVCC` PD source isprogrammed to match output voltage and output current. 
  Current limit is still programmed to PD source. If there is a huge overcurrent
  event the PD source will reset. The controller SW is not fast enough to handle 
  such events
- `CVCC^` similair to `CVCC` but PS source current limit is set to max. This 
  prevents the PD source from resetting if the current is above the desired limit
  but below the max. current
  

#### start up parameter `S`
request start up parameter
```
	<- ?S
	-> ?S {OFF|SET|ON}
```
set start up parameter
```
	<- !S {OFF|SET|ON}
	-> !S {OFF|SET|ON}
```
- `OFF` set 5 V fix profile with output off
- `SET` restore last power settings with output off
- `ON` sam as `SET`, on succes enable output

#### profile type 'T'
request the currently active profile type
```
<- ?T 
-> ?T {N/A|FIX|PPS}
```
- `N/A` PD-PPS_Controller is sourced by a non PD source
- `FIX` fixed voltage profile is active
- `PPS` augmented profile is acitvv

#### voltage setting and available voltage `U`
request currently set voltage works with FIX and PPS mode
```
<- ?U	
-> ?U [val]V
```
set voltage in PPS mode
```
<- !U [val]V
-> !U [val]V	
``` 
get max. available voltage at a given current limit
```
<- ?U [val]A
-> ?U [val]V	
``` 

#### program version `V`
``` 
<- ?V
-> ?V [Ver] [Built]
```	

[home](../README.md)