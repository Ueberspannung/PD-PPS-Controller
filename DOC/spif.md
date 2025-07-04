# The Serial Programming Interface

#### Table of contents
- [inteface parameters](#interface-parmeters)  
- [connecting to the mini](#connecting-to-the-mini)
- [command structure](#command-structure)
    - [request](#request)
	- [reply](#reply)

## interface parameters
The serial communication parameters should be set to 115200 8N1. This is a USB 
serial device so any parameter should do.  


## connecting to the mini
when a serial commection is establishe, the mini will try to identify a VT100 
terminal. This is doen using th ESC[C command. It will try 6 times. Just ignore 
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

### naming conventions:
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
`#`     |  R/W  | [profile number](#number-of-profile)
`?`     |   R   | [output state](#requesting-the-output-state)
`C`     |  R/W  | [current calibration](#current-calbration)


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
-> [INT] 
```
- **`[INT]`** number of current profile  
  if more than one augmented profile is availabe only the first will be reported for detials see [get profile data](#get-profile-data)

to select the desired profile
```
<- !# n
-> [INT] 
```

#### requesting the output state
```
<- ??
-> {0|1|!} [val]V [val]V [val]A 
```
1. **`{0|1|!}`**  
   **`0`** outpunt off  
   **`1`** outpunt on  
   **`!`** constant current mode  
2. **`[val]V`** bus voltage at USB C input
3. **`[val]V`** output voltage
4. **`[val]A`** output current

#### current calbration
requsting calibration data:
```
<- ?C 
-> [val]A [val]A (interne Messwert, Referenzwert)
```
setting calibration data:
```
<- !C [val]A [val]A (interne Messwert, Referenzwert)
-> [val]A [val]A (interne Messwert, Referenzwert)
```
1. **`[val]A`**   
   internal current value
2. **`[val]A`**  
   external current value (measure with precise ampere meter)


- Parameter loeschen
	<- !E
	-> busy erasing parameter 0%

- nächstes Fix Profil finden welches zu einer geforderten Spannung passt
	Bsp: 11.5V gefordert, verfügbarer Profile 5,9,12,15,20V -> 9V
	<- ?F [val]V 
	-> [val]V

- max. Strom setzen / anfragen / suchen 
	<- ?I 			( aktuell eingestellten Strom anfordern )
	<- ?I [val]V	( PPS: max. verfügbaren Strom zu einer Spannung anfordern)
	<- !I [val]A 	( PPS: Strom einstellen )
	-> [val]A

- Anzahl der möglichen Profile anfordern
	<- ?N
- Daten eines spezifischen Profils anfordern
	-> [INT] (0-7, 0:keine PD fähigkeit)
	<- ?N n (n: Nummer des Profils)
	-> FIX [val]V [val]A (fix Profil)
	-> AUG [val]V - [val]V [val]A (Augmented Profil)
	-> BAT [val]V - [val]V [val]W (Battery Profil)
	-> VAR [val]V - [val]V [val]A (Variable Profil, replace by AUG)

- Ausgang schalten / Stellzustand anfordern
	<- !O {0|1}
	<- ?O
	-> {0|1}

- Regler Betriebsar einstellen / anfordern
	<- !R {OFF|CV|CVCC|CVCC^}
	<- ?R
	-> {OFF|CV|CVCC|CVCC^}

- Startup Parameter abrufen / setzen
	<- ?S
	<- !S {OFF|SET|ON}
	-> {OFF|SET|ON}

- aktuellen Profiltyp (FIX/PPS) anfordern
	<- ?T 
	-> {N/A|FIX|PPS}

- Spannung (FIX) auswählen / anfordern
	<- !U [val]V	(Spannung auswählen)
	<- ?U [val]A 	(PPS:  max. verfügbare Sannung bei [val]A)
	<- ?U			(eingestellte Spannug anfordern FIX/PPS)
	-> [val]V		(Tatsächlich gewählte / verfügbare Spannug)

- Version
	<- ?V
	-> [Ver] [Built]
	