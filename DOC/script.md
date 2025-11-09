# Script Control Languge

#### Table of contents
- [features](#features)  
- [files](#files)  
  - [PRS file](parse-file)
  - [ERR file](error-file)

- [commands](#commands)  
  - [SET](#SET)  
  - [WAIT](#WAIT)  
  - [LOOP](#LOOP)  
  - [JUMP](#JUMP)  
  - [LABEL](#LABEL)  
  - [COMMENT](#COMMENT)  
  - [emty line](#empty-line)
  - [LOG](#LOG)  
  - [DISP](#DISP)  
  - [INFO](#INFO)  
  - [PASS](#PASS)  
  - [FAIL](#FAIL)  
  - [CLEAR](#CLEAR)  
  - [LOAD](#LOAD)  
  - [SAVE](#SAVE)  
- [examples](#examples)
  - [program with errors](#with-errors)
    - [program file](#program-file-with-error)
    - [error file](#error-file-with-error)
    - [parse file](#parse-file-with-error)
  - [porgram execution](#porgram-execution)
    - [parse file](#parse-file-without-error)
    - [log file](#log-file)



## features

Version 5.4 supports script control (on HW with SD Card slots).
with this script control one can  
control  
* the output state  
* the voltage setpoint  
* the current setpoint  
evaluate  
* the current output voltage  
* the current output current  
* the start mode (automatic or manuel)  
trigger  
* log file entries
* user messages

the scripting language supports (un)conditional jumps, (nested) loops and 10 general purpose unsigned 8 bit integers.
these integers can be saved to and loaded from the eeprom / config file.

when in AUTO ON mode the last program selected will be executed automatically on system start  


## files  
program files need to end on  **.PRG**. Only DOS 8.3 filenames are allowed.  
currently the system operates in verbose mode. This means an **.PRS** file will be generated
An **.ERR** file will be generated anyway, On succes this file will stay empty

### parse file
Te parse file consists of five sections:  
1. command and parameter decoding  
  for every parse program line the physical line number, the number of the corresponding program data element, the command and the parameters will be logged. Program data element and line number will be different becaus comments and empty lines will not produce an programm data element  
1. label checking  
  every JUMP command will be matched to its associated label line
1. loop checking pass 1  
  every loop header will be matched against its dedicated loop footer
1. loop checking pass 2  
  it will be checked if there are any unmatched loop commands
1. message list  
  a list of all defined logging and user messages will be created

### error file
Any errors during the parsing process will be reported in this file  
It contains the program line and the error message

possible error messages are:  

* line to long  
  the program line exceeds the buffer lenth (max. 80 chars)  
* unknown command  
  the identifier found in first place is not part of the supported commands  
* invalid label char  
  only "0" ... "9", "@", "A" ..."Z", "_", "a" ... "z" are allowed  
* label to long  
  a label is limited to 5 characters.   
* delay to long >60s  
   same delays are limited to 60000 ms  
* invalid delay  
   0 ms is not a vaild delay  
* previous expression pending  
  incompete expression in SET or JUMP statement  
* variable previosly used  
  varible previously used in current SET statement   
* multiple operators selected  
  all operators are single character operators "<", "=", ">", "+", "-"  
* no variable selected  
  operator used without variable  
* immedeate assignement only  
  some variables such as "O" support only direct assignement "="  
* invalid parameter sequence  
  variable selected in JUMP or SET does not exist  
* multiple conditions not allowed  
  jump statement does not support logic with multiple expressions  
* invalid operator  
  Output values do not support "equals" condion only "less than" or "greater than" are llowed  
* no operator selected  
  ther wos no operator detected bewtween a numerical value and an variable  
* out of message memory  
  program memory or message memory full  
* unexpected extra data  
  unexpected parameters, CLEAR with message, LOAD or SAVE with data  
* flag used without index  
  when using one of the 10 flags the use of the index digit is essential  
* closing loop without opening loop  
  there are more loop footers than headers  
* label has been previoulsly defined  
  a label has bben defined more than once, labels have to be unique  
* referenced label is undefined   
  the label reference by jump has not been defined
* opening loop without closing loop  
  there are more loop headers than footers
* expected data  
  message command without message

## commands

Notation for commands:  
- {a|b|c} options field, one of "a", "b" or "c"
- [val] numerical value with sign and decimals '.'
- [int] integral number, including sign, '+' is optional
- [uint] positive integral number 
- <...> optional parmeter

commands do not need to start on the first position of a line. Any number of whitespaces is allowed

there has to be only one command per line

System variables:  
- U output voltage  
  setpoint in SET command, measured output voltage in JUMP command  
- I output current  
  setpoint in SET command, measured output currend in JUMP command  
- O output switch state  
  modify in SET command, evaluate in jUMP command  
- R Reset Flag  
  for evaluation (JUMP) only,  
  0: program start was performed manually  
  1: program start was performed automatically on system start  
- F0 ... F9  
  10 8-bit flags read / write in JUMP / SET  

### SET
```
SET <U{=|+|-}[val]<V>> <I{=|+|-}[val]<A>> <O={0|1}> <Fn{=|+|-}[uint]> [val]<ms>  
```
- set / incrent / decrement voltage or current  
- set output switch   
- set / icnrement / decrement general purpose flags  
- delay after command of [val] ms  

general purpose flags:  
- [Fn] Flag n=0...9 Wertebereich 0..255  

operators
- '=' set operator, only positive values without sign  
- '+' incremant operator, only positive values without sign  
- '-' decremnt operator, only positive values without sign  


There has to be specified at least one of {U|I|O|Fn} 


### WAIT
```
WAIT [uint]<ms>
```
- pause for [uint] ms

### LOOP
```
LOOP <[uint]>
```
- loop header  
  loop cmd with parameter specifying the number of reptitions  
- loop footer
  loop comd without parameter

loop footer and headers will automatically be matched  
loops can be nested without limit  
loops can be exited with JUMP

### JUMP
```
JUMP <{ {U|I}{<|>}{val}<{V|A}> | {R|Fn|O}{<|=|>}{uint} }> [:LABEL]  
```

1. parameter  
   a) one conditon form {U|I|O|R|Fn} results in conditional jump  
   conditions on O or R need to be compared to 0 or 1
   b) [:LABEL] unconditional JUMP to :LABEL
1. parameter target LABEL.

### LABEL
```
:LABEL
```
JUMP target any sequence of up to 5 characters form [0 ... 9, @, A ... Z, _, a ... z]


### COMMENT

```
# comment text
```
single line command, comments at the end of a line are not allowed
any character sequece is allowed for comment text

# emty line
if ther is just a line spacing required, there no need for the comment command
an empty (or filled with whitespaces) line will do
 
### LOG
```
LOG <Text>
```

instead of automated logging, within a script the log command should be preferred.
this enables the user to controll the log file entries individually, depending on the current requirements

the log command could be used to
- either write o note to the logfile (command with parameter)
- or write the current data to the log file (voltage and current setpoint, output voltage and current readings, system temperature)  

### DISP
### INFO
### PASS
### FAIL
```
DISP <Text>
INFO <Text>
PASS <Text>
FAIL <Text>
```
display some characters.
the length of the text which can be displayed depends on the hardware

- in Terminal mode up to 50 characters can be displayed
- on the mini only 14 charactes can be displayed 
- on versions with 4x20 display the limit will be 18 characters (TBD)

depending on the hardware the text will be displayed in different styles

| hw | DISP | INFO | PASS | FAIL |
|----|:----:|:----:|:----:|:----:|
|terminal|white, low intensity|white, high intensity|green, high intensity|red, high intensity, blinking|
|mini|grey|white|green|red|
|4x20|text icon|i - icon|check icon|x - icon|


### CLEAR
```
CLEAR
```
clears the user message

### LOAD
```
LOAD
```
read all 10 user flags from eeprom / config file

### SAVE
```
SAVE
```
wrtite all 10 user flags to eeprom / config file

## examples

### with errors

#### program file-with error
```javascript {.line-numbers}
# test of current consumption

#start at 3.3V, limit current to 2,2A
set O=1 U=3.3V I=2.2A 1500ms

log
log increasing voltage
# increase voltage loop
:raise
	SET U+0.1V 100ms
	log
# stop test if voltage exceeds 14V
jump u>14.0V :err1
# continue if current limit not yet reached
jump i<0.7A :raise

log
log lowering voltage to match Iout=200mA
# decrease voltage loop
:lower
	set U=-0.1V 100ms
	log
# stop test if voltage falls below 3.6V
jump U<3.6V :err2
# continue if lower current limit not yet reached
jump I>0.2A :lower

# succes: 700mA reached below 14V and 200mA reached above 3.6V
log setpoint succesfully reached
pass Test O.K.
set O=0 7500
jump stop

# error message at upper limit
:err1
fail I<700mA!
log fail at upper testpoint
set o=0 7500
jump :stop

# error message at lower limit
:er2
fail I>200mA!
log fail at lower testpoint
set o=0 7500

# end of program
:stop
clear
```

#### error file with error

this produces on error file:
```
 21: multiple operators selected
 32: invalid parameter sequence
 24: referenced label is undefined
 32: referenced label is undefined
```

in line 21:
```
	set U=-0.1V 100ms
```
assignment with sign is not allowed. this should be an decrement such as
```
	set U-0.1V 100ms
```

in line 32
```
jump stop
```
labels need to be started by an ":"
```
jump :stop
```

in line 24
```
jump U<3.6V :err2
```
the target should be line 42 but the label has been mistyped
```
:er2
```
#### parse file with error

The vebose output produces a parse file:

```
 ***********************************************
  2672: log file opened LIMIT.LOG
  2672: error file opened LIMIT.ERR
  2672: prarser file opened LIMIT.PRS
  2672: Start load file LIMIT.PRG
  2680: line #1 : parsing COMMENT: test of current consumption
  2711: line #2  - / - 
  2737: line #3 : parsing COMMENT:start at 3.3V, limit current to 2,2A
  2767: line #4 : parsing SET, PDE=0, U=3300mV, I=2200mA, O=ON, Delay= 1500ms
  2798: line #5  - / - 
  2823: line #6 : parsing LOG, PDE=1, MessageID= 255 ::::
  2850: line #7 : parsing LOG, PDE=2, MessageID= 0 ::increasing voltage::
  2874: line #8 : parsing COMMENT: increase voltage loop
  2906: line #9 : parsing LABEL, PDE=3, LabelID= 379AEE2A
  2929: line #10 : parsing SET, PDE=4, U=U+100mV, Delay= 100ms
  2959: line #11 : parsing LOG, PDE=5, MessageID= 255 ::::
  2984: line #12 : parsing COMMENT: stop test if voltage exceeds 14V
  3015: line #13 : parsing JUMP, PDE=6 JUMP on U > 14000 mV LabelID= AB7DC1
  3040: line #14 : parsing COMMENT: continue if current limit not yet reached
  3074: line #15 : parsing JUMP, PDE=7 JUMP on I < 700 mA LabelID= 379AEE2A
  3099: line #16  - / - 
  3128: line #17 : parsing LOG, PDE=8, MessageID= 255 ::::
  3151: line #18 : parsing LOG, PDE=9, MessageID= 1 ::lowering voltage to match Iout=200mA::
  3176: line #19 : parsing COMMENT: decrease voltage loop
  3207: line #20 : parsing LABEL, PDE=10, LabelID= 31D3CAB7
  3232: line #21 : parsing SET, PDE=11, U=100mV, Delay= 100ms
  3289: line #22 : parsing LOG, PDE=12, MessageID= 255 ::::
  3317: line #23 : parsing COMMENT: stop test if voltage falls below 3.6V
  3349: line #24 : parsing JUMP, PDE=13 JUMP on U < 3600 mV LabelID= AB7DC2
  3376: line #25 : parsing COMMENT: continue if lower current limit not yet reached
  3406: line #26 : parsing JUMP, PDE=14 JUMP on I > 200 mA LabelID= 31D3CAB7
  3430: line #27  - / - 
  3455: line #28 : parsing COMMENT: succes: 700mA reached below 14V and 200mA reached above 3.6V
  3486: line #29 : parsing LOG, PDE=15, MessageID= 2 ::setpoint succesfully reached::
  3515: line #30 : parsing disp, PDE=16, MessageType=PASS, MessageID= 3 Test O.K.
  3540: line #31 : parsing SET, PDE=17, O=OFF, Delay= 7500ms
  3571: line #32 : parsing JUMP, PDE=18 JUMP unconditional LabelID= 0
  3613: line #33  - / - 
  3638: line #34 : parsing COMMENT: error message at upper limit
  3668: line #35 : parsing LABEL, PDE=19, LabelID= AB7DC1
  3693: line #36 : parsing disp, PDE=20, MessageType=FAIL, MessageID= 4 I<700mA!
  3716: line #37 : parsing LOG, PDE=21, MessageID= 5 ::fail at upper testpoint::
  3743: line #38 : parsing SET, PDE=22, O=OFF, Delay= 7500ms
  3775: line #39 : parsing JUMP, PDE=23 JUMP unconditional LabelID= E39D35
  3800: line #40  - / - 
  3824: line #41 : parsing COMMENT: error message at lower limit
  3855: line #42 : parsing LABEL, PDE=24, LabelID= 2ADC2
  3879: line #43 : parsing disp, PDE=25, MessageType=FAIL, MessageID= 6 I>200mA!
  3902: line #44 : parsing LOG, PDE=26, MessageID= 7 ::fail at lower testpoint::
  3928: line #45 : parsing SET, PDE=27, O=OFF, Delay= 7500ms
  3962: line #46  - / - 
  3986: line #47 : parsing COMMENT: end of program
  4017: line #48 : parsing LABEL, PDE=28, LabelID= E39D35
  4164: line #49 : parsing disp, PDE=29, MessageType=CLEAR, MessageID= 255 
  4180: End load file, 49 lines processed, 30 PDEs created
***********************************************
  4180: checking labels 
  4181: label in line 9, PDE=3, label id=379AEE2A
  4182: JUMP in line 13, PDE=6, label id=AB7DC1, Target: PDE=19, line 35
  4187: JUMP in line 15, PDE=7, label id=379AEE2A, Target: PDE=3, line 9
  4188: label in line 20, PDE=10, label id=31D3CAB7
  4212: JUMP in line 24, PDE=13, label id=AB7DC2, Target: -/-
  4216: JUMP in line 26, PDE=14, label id=31D3CAB7, Target: PDE=10, line 20
  4240: JUMP in line 32, PDE=18, label id=0, Target: -/-
  4245: label in line 35, PDE=19, label id=AB7DC1
  4245: JUMP in line 39, PDE=23, label id=E39D35, Target: PDE=28, line 48
  4246: label in line 42, PDE=24, label id=2ADC2
  4250: label in line 48, PDE=28, label id=E39D35
  4251: End checking labels 
***********************************************
  4251: matching loops 
  4252: End matching loops 
***********************************************
  4252: checking loops 
  4252: End checking loops 
***********************************************
  4252: message List 
  4252: Message #0, length=18 ::increasing voltage::
  4254: Message #1, length=36 ::lowering voltage to match Iout=200mA::
  4254: Message #2, length=28 ::setpoint succesfully reached::
  4260: Message #3, length=9 ::Test O.K.::
  4261: Message #4, length=8 ::I<700mA!::
  4262: Message #5, length=23 ::fail at upper testpoint::
  4264: Message #6, length=8 ::I>200mA!::
  4265: Message #7, length=23 ::fail at lower testpoint::
  4265: End message list 
***********************************************
  4270: done close file
```

### porgram execution

#### parse file without error
when the errors in the program file above are corrected this parse output is generated

```
***********************************************
  2669: log file opened LIMIT.LOG
  2669: error file opened LIMIT.ERR
  2669: prarser file opened LIMIT.PRS
  2669: Start load file LIMIT.PRG
  2678: line #1 : parsing COMMENT: test of current consumption
  2708: line #2  - / - 
  2733: line #3 : parsing COMMENT:start at 3.3V, limit current to 2,2A
  2763: line #4 : parsing SET, PDE=0, U=3300mV, I=2200mA, O=ON, Delay= 1500ms
  2795: line #5  - / - 
  2819: line #6 : parsing LOG, PDE=1, MessageID= 255 ::::
  2846: line #7 : parsing LOG, PDE=2, MessageID= 0 ::increasing voltage::
  2869: line #8 : parsing COMMENT: increase voltage loop
  2901: line #9 : parsing LABEL, PDE=3, LabelID= 379AEE2A
  2925: line #10 : parsing SET, PDE=4, U=U+100mV, Delay= 100ms
  2956: line #11 : parsing LOG, PDE=5, MessageID= 255 ::::
  2981: line #12 : parsing COMMENT: stop test if voltage exceeds 14V
  3012: line #13 : parsing JUMP, PDE=6 JUMP on U > 14000 mV LabelID= AB7DC1
  3036: line #14 : parsing COMMENT: continue if current limit not yet reached
  3071: line #15 : parsing JUMP, PDE=7 JUMP on I < 700 mA LabelID= 379AEE2A
  3095: line #16  - / - 
  3118: line #17 : parsing LOG, PDE=8, MessageID= 255 ::::
  3143: line #18 : parsing LOG, PDE=9, MessageID= 1 ::lowering voltage to match Iout=200mA::
  3167: line #19 : parsing COMMENT: decrease voltage loop
  3198: line #20 : parsing LABEL, PDE=10, LabelID= 31D3CAB7
  3223: line #21 : parsing SET, PDE=11, U=U-100mV, Delay= 100ms
  3253: line #22 : parsing LOG, PDE=12, MessageID= 255 ::::
  3281: line #23 : parsing COMMENT: stop test if voltage falls below 3.6V
  3312: line #24 : parsing JUMP, PDE=13 JUMP on U < 3600 mV LabelID= AB7DC2
  3341: line #25 : parsing COMMENT: continue if lower current limit not yet reached
  3375: line #26 : parsing JUMP, PDE=14 JUMP on I > 200 mA LabelID= 31D3CAB7
  3399: line #27  - / - 
  3424: line #28 : parsing COMMENT: succes: 700mA reached below 14V and 200mA reached above 3.6V
  3455: line #29 : parsing LOG, PDE=15, MessageID= 2 ::setpoint succesfully reached::
  3484: line #30 : parsing disp, PDE=16, MessageType=PASS, MessageID= 3 Test O.K.
  3508: line #31 : parsing SET, PDE=17, O=OFF, Delay= 7500ms
  3539: line #32 : parsing JUMP, PDE=18 JUMP unconditional LabelID= E39D35
  3563: line #33  - / - 
  3587: line #34 : parsing COMMENT: error message at upper limit
  3618: line #35 : parsing LABEL, PDE=19, LabelID= AB7DC1
  3643: line #36 : parsing disp, PDE=20, MessageType=FAIL, MessageID= 4 I<700mA!
  3667: line #37 : parsing LOG, PDE=21, MessageID= 5 ::fail at upper testpoint::
  3695: line #38 : parsing SET, PDE=22, O=OFF, Delay= 7500ms
  3726: line #39 : parsing JUMP, PDE=23 JUMP unconditional LabelID= E39D35
  3749: line #40  - / - 
  3774: line #41 : parsing COMMENT: error message at lower limit
  3805: line #42 : parsing LABEL, PDE=24, LabelID= AB7DC2
  3830: line #43 : parsing disp, PDE=25, MessageType=FAIL, MessageID= 6 I>200mA!
  3853: line #44 : parsing LOG, PDE=26, MessageID= 7 ::fail at lower testpoint::
  3882: line #45 : parsing SET, PDE=27, O=OFF, Delay= 7500ms
  3912: line #46  - / - 
  3936: line #47 : parsing COMMENT: end of program
  3967: line #48 : parsing LABEL, PDE=28, LabelID= E39D35
  3991: line #49 : parsing disp, PDE=29, MessageType=CLEAR, MessageID= 255 
  4009: End load file, 49 lines processed, 30 PDEs created
***********************************************
  4009: checking labels 
  4010: label in line 9, PDE=3, label id=379AEE2A
  4010: JUMP in line 13, PDE=6, label id=AB7DC1, Target: PDE=19, line 35
  4015: JUMP in line 15, PDE=7, label id=379AEE2A, Target: PDE=3, line 9
  4016: label in line 20, PDE=10, label id=31D3CAB7
  4017: JUMP in line 24, PDE=13, label id=AB7DC2, Target: PDE=24, line 42
  4018: JUMP in line 26, PDE=14, label id=31D3CAB7, Target: PDE=10, line 20
  4019: JUMP in line 32, PDE=18, label id=E39D35, Target: PDE=28, line 48
  4021: label in line 35, PDE=19, label id=AB7DC1
  4021: JUMP in line 39, PDE=23, label id=E39D35, Target: PDE=28, line 48
  4022: label in line 42, PDE=24, label id=AB7DC2
  4027: label in line 48, PDE=28, label id=E39D35
  4027: End checking labels 
***********************************************
  4027: matching loops 
  4029: End matching loops 
***********************************************
  4029: checking loops 
  4029: End checking loops 
***********************************************
  4029: message List 
  4029: Message #0, length=18 ::increasing voltage::
  4030: Message #1, length=36 ::lowering voltage to match Iout=200mA::
  4034: Message #2, length=28 ::setpoint succesfully reached::
  4034: Message #3, length=9 ::Test O.K.::
  4035: Message #4, length=8 ::I<700mA!::
  4037: Message #5, length=23 ::fail at upper testpoint::
  4037: Message #6, length=8 ::I>200mA!::
  4039: Message #7, length=23 ::fail at lower testpoint::
  4039: End message list 
***********************************************
  4044: done close file
```

#### log file 
the program has been started three times with ~19 Ohms, ~8 Ohms and no load

```
****************************************************
* t;line;data    (t)ype: d=data, m=message, e=error
m;line;message
e;line;message
d;line;---- time -----;-U/V-;-I/A-;Uout/V;Iout/A;deg C
m;    ; 0  0:00:34.509;"program started"
d;   6; 0  0:00:36.048; 3.30; 2.20; 3.298; 0.176; 30.2;
m;   7; 0  0:00:36.064;"increasing voltage"
d;  11; 0  0:00:36.179; 3.40; 2.20; 3.396; 0.181; 30.2;
d;  11; 0  0:00:36.300; 3.50; 2.20; 3.495; 0.186; 30.2;
d;  11; 0  0:00:36.418; 3.60; 2.20; 3.590; 0.191; 30.2;
d;  11; 0  0:00:36.536; 3.70; 2.20; 3.681; 0.196; 30.2;
d;  11; 0  0:00:36.719; 3.80; 2.20; 3.778; 0.201; 30.2;
d;  11; 0  0:00:36.839; 3.90; 2.20; 3.897; 0.207; 30.2;
d;  11; 0  0:00:36.956; 4.00; 2.20; 3.993; 0.213; 30.2;
d;  11; 0  0:00:37.076; 4.10; 2.20; 4.092; 0.217; 30.2;
d;  11; 0  0:00:37.238; 4.20; 2.20; 4.186; 0.223; 30.2;
d;  11; 0  0:00:37.368; 4.30; 2.20; 4.300; 0.229; 30.2;
d;  11; 0  0:00:37.483; 4.40; 2.20; 4.399; 0.235; 30.2;
d;  11; 0  0:00:37.654; 4.50; 2.20; 4.499; 0.239; 30.2;
d;  11; 0  0:00:37.769; 4.60; 2.20; 4.594; 0.244; 30.2;
d;  11; 0  0:00:37.895; 4.70; 2.20; 4.688; 0.250; 30.2;
d;  11; 0  0:00:38.012; 4.80; 2.20; 4.787; 0.255; 30.2;
d;  11; 0  0:00:38.197; 4.90; 2.20; 4.882; 0.260; 30.2;
d;  11; 0  0:00:38.313; 5.00; 2.20; 5.014; 0.268; 30.2;
d;  11; 0  0:00:38.430; 5.10; 2.20; 5.104; 0.272; 30.2;
d;  11; 0  0:00:38.548; 5.20; 2.20; 5.193; 0.277; 30.2;
d;  11; 0  0:00:38.667; 5.30; 2.20; 5.291; 0.282; 30.2;
d;  11; 0  0:00:38.799; 5.40; 2.20; 5.390; 0.288; 30.2;
d;  11; 0  0:00:38.918; 5.50; 2.20; 5.488; 0.292; 30.2;
d;  11; 0  0:00:39.032; 5.60; 2.20; 5.595; 0.297; 30.2;
d;  11; 0  0:00:39.148; 5.70; 2.20; 5.699; 0.303; 30.2;
d;  11; 0  0:00:39.268; 5.80; 2.20; 5.795; 0.309; 30.2;
d;  11; 0  0:00:39.432; 5.90; 2.20; 5.886; 0.314; 30.2;
d;  11; 0  0:00:39.548; 6.00; 2.20; 5.982; 0.319; 30.2;
d;  11; 0  0:00:39.665; 6.10; 2.20; 6.087; 0.325; 30.2;
d;  11; 0  0:00:39.782; 6.20; 2.20; 6.202; 0.331; 30.2;
d;  11; 0  0:00:39.899; 6.30; 2.20; 6.302; 0.335; 30.2;
d;  11; 0  0:00:40.036; 6.40; 2.20; 6.407; 0.341; 30.2;
d;  11; 0  0:00:40.151; 6.50; 2.20; 6.492; 0.346; 30.2;
d;  11; 0  0:00:40.269; 6.60; 2.20; 6.591; 0.351; 30.2;
d;  11; 0  0:00:40.385; 6.70; 2.20; 6.695; 0.356; 30.2;
d;  11; 0  0:00:40.506; 6.80; 2.20; 6.793; 0.362; 30.2;
d;  11; 0  0:00:40.635; 6.90; 2.20; 6.900; 0.367; 30.2;
d;  11; 0  0:00:40.756; 7.00; 2.20; 7.004; 0.373; 30.2;
d;  11; 0  0:00:40.871; 7.10; 2.20; 7.098; 0.379; 30.2;
d;  11; 0  0:00:41.038; 7.20; 2.20; 7.189; 0.383; 30.2;
d;  11; 0  0:00:41.158; 7.30; 2.20; 7.291; 0.389; 30.2;
d;  11; 0  0:00:41.277; 7.40; 2.20; 7.391; 0.394; 30.2;
d;  11; 0  0:00:41.395; 7.50; 2.20; 7.509; 0.400; 30.2;
d;  11; 0  0:00:41.561; 7.60; 2.20; 7.613; 0.405; 30.2;
d;  11; 0  0:00:41.679; 7.70; 2.20; 7.710; 0.411; 30.2;
d;  11; 0  0:00:41.795; 7.80; 2.20; 7.800; 0.416; 30.2;
d;  11; 0  0:00:41.914; 7.90; 2.20; 7.898; 0.422; 30.3;
d;  11; 0  0:00:42.035; 8.00; 2.20; 8.003; 0.427; 30.3;
d;  11; 0  0:00:42.159; 8.10; 2.20; 8.091; 0.431; 30.3;
d;  11; 0  0:00:42.281; 8.20; 2.20; 8.185; 0.436; 30.3;
d;  11; 0  0:00:42.398; 8.30; 2.20; 8.294; 0.442; 30.3;
d;  11; 0  0:00:42.548; 8.40; 2.20; 8.398; 0.448; 30.3;
d;  11; 0  0:00:42.666; 8.50; 2.20; 8.497; 0.453; 30.3;
d;  11; 0  0:00:42.784; 8.60; 2.20; 8.604; 0.459; 30.3;
d;  11; 0  0:00:42.903; 8.70; 2.20; 8.713; 0.464; 30.3;
d;  11; 0  0:00:43.066; 8.80; 2.20; 8.814; 0.470; 30.3;
d;  11; 0  0:00:43.184; 8.90; 2.20; 8.916; 0.475; 30.3;
d;  11; 0  0:00:43.302; 9.00; 2.20; 9.012; 0.481; 30.3;
d;  11; 0  0:00:43.468; 9.10; 2.20; 9.102; 0.485; 30.3;
d;  11; 0  0:00:43.586; 9.20; 2.20; 9.210; 0.491; 30.3;
d;  11; 0  0:00:43.705; 9.30; 2.20; 9.305; 0.496; 30.3;
d;  11; 0  0:00:43.827; 9.40; 2.20; 9.393; 0.501; 30.3;
d;  11; 0  0:00:43.984; 9.50; 2.20; 9.498; 0.506; 30.3;
d;  11; 0  0:00:44.104; 9.60; 2.20; 9.591; 0.511; 30.3;
d;  11; 0  0:00:44.220; 9.70; 2.20; 9.682; 0.516; 30.3;
d;  11; 0  0:00:44.388; 9.80; 2.20; 9.809; 0.523; 30.3;
d;  11; 0  0:00:44.505; 9.90; 2.20; 9.903; 0.528; 30.3;
d;  11; 0  0:00:44.647;10.00; 2.20; 9.992; 0.533; 30.3;
d;  11; 0  0:00:44.764;10.10; 2.20;10.095; 0.539; 30.3;
d;  11; 0  0:00:44.908;10.20; 2.20;10.197; 0.544; 30.3;
d;  11; 0  0:00:45.023;10.30; 2.20;10.283; 0.548; 30.3;
d;  11; 0  0:00:45.142;10.40; 2.20;10.410; 0.555; 30.3;
d;  11; 0  0:00:45.257;10.50; 2.20;10.506; 0.560; 30.3;
d;  11; 0  0:00:45.378;10.60; 2.20;10.592; 0.565; 30.3;
d;  11; 0  0:00:45.543;10.70; 2.20;10.694; 0.570; 30.3;
d;  11; 0  0:00:45.663;10.80; 2.20;10.795; 0.576; 30.3;
d;  11; 0  0:00:45.789;10.90; 2.20;10.886; 0.580; 30.3;
d;  11; 0  0:00:45.953;11.00; 2.20;11.000; 0.586; 30.3;
d;  11; 0  0:00:46.072;11.10; 2.20;11.104; 0.592; 30.3;
d;  11; 0  0:00:46.209;11.20; 2.20;11.201; 0.597; 30.3;
d;  11; 0  0:00:46.330;11.30; 2.20;11.297; 0.603; 30.3;
d;  11; 0  0:00:46.477;11.40; 2.20;11.401; 0.608; 30.3;
d;  11; 0  0:00:46.594;11.50; 2.20;11.500; 0.614; 30.3;
d;  11; 0  0:00:46.711;11.60; 2.20;11.590; 0.619; 30.3;
d;  11; 0  0:00:46.879;11.70; 2.20;11.694; 0.623; 30.3;
d;  11; 0  0:00:47.000;11.80; 2.20;11.790; 0.629; 30.3;
d;  11; 0  0:00:47.126;11.90; 2.20;11.916; 0.636; 30.3;
d;  11; 0  0:00:47.245;12.00; 2.20;12.004; 0.640; 30.3;
d;  11; 0  0:00:47.403;12.10; 2.20;12.095; 0.645; 30.3;
d;  11; 0  0:00:47.521;12.20; 2.20;12.190; 0.649; 30.3;
d;  11; 0  0:00:47.639;12.30; 2.20;12.292; 0.655; 30.4;
d;  11; 0  0:00:47.806;12.40; 2.20;12.389; 0.661; 30.4;
d;  11; 0  0:00:47.924;12.50; 2.20;12.506; 0.667; 30.4;
d;  11; 0  0:00:48.039;12.60; 2.20;12.607; 0.673; 30.4;
d;  11; 0  0:00:48.161;12.70; 2.20;12.708; 0.678; 30.4;
d;  11; 0  0:00:48.320;12.80; 2.20;12.797; 0.682; 30.4;
d;  11; 0  0:00:48.436;12.90; 2.20;12.898; 0.688; 30.4;
d;  11; 0  0:00:48.554;13.00; 2.20;13.002; 0.693; 30.4;
d;  11; 0  0:00:48.671;13.10; 2.20;13.085; 0.697; 30.4;
d;  11; 0  0:00:48.792;13.20; 2.20;13.181; 0.703; 30.4;
d;  17; 0  0:00:48.807;13.20; 2.20;13.184; 0.703; 30.4;
m;  18; 0  0:00:48.823;"lowering voltage to match Iout=200mA"
d;  22; 0  0:00:48.940;13.10; 2.20;13.088; 0.698; 30.4;
d;  22; 0  0:00:49.062;13.00; 2.20;13.000; 0.693; 30.5;
d;  22; 0  0:00:49.223;12.90; 2.20;12.895; 0.688; 30.5;
d;  22; 0  0:00:49.343;12.80; 2.20;12.797; 0.682; 30.5;
d;  22; 0  0:00:49.458;12.70; 2.20;12.711; 0.678; 30.5;
d;  22; 0  0:00:49.622;12.60; 2.20;12.609; 0.672; 30.5;
d;  22; 0  0:00:49.738;12.50; 2.20;12.506; 0.667; 30.5;
d;  22; 0  0:00:49.856;12.40; 2.20;12.386; 0.661; 30.5;
d;  22; 0  0:00:49.972;12.30; 2.20;12.294; 0.656; 30.5;
d;  22; 0  0:00:50.138;12.20; 2.20;12.188; 0.650; 30.5;
d;  22; 0  0:00:50.257;12.10; 2.20;12.100; 0.645; 30.5;
d;  22; 0  0:00:50.374;12.00; 2.20;12.004; 0.640; 30.5;
d;  22; 0  0:00:50.541;11.90; 2.20;11.897; 0.635; 30.6;
d;  22; 0  0:00:50.658;11.80; 2.20;11.812; 0.630; 30.6;
d;  22; 0  0:00:50.776;11.70; 2.20;11.713; 0.625; 30.6;
d;  22; 0  0:00:50.894;11.60; 2.20;11.608; 0.620; 30.6;
d;  22; 0  0:00:51.050;11.50; 2.20;11.510; 0.614; 30.6;
d;  22; 0  0:00:51.166;11.40; 2.20;11.404; 0.608; 30.6;
d;  22; 0  0:00:51.282;11.30; 2.20;11.297; 0.602; 30.6;
d;  22; 0  0:00:51.403;11.20; 2.20;11.203; 0.597; 30.6;
d;  22; 0  0:00:51.521;11.10; 2.20;11.088; 0.591; 30.6;
d;  22; 0  0:00:51.683;11.00; 2.20;10.984; 0.586; 30.6;
d;  22; 0  0:00:51.801;10.90; 2.20;10.882; 0.580; 30.6;
d;  22; 0  0:00:51.918;10.80; 2.20;10.798; 0.575; 30.6;
d;  22; 0  0:00:52.037;10.70; 2.20;10.691; 0.569; 30.6;
d;  22; 0  0:00:52.156;10.60; 2.20;10.594; 0.565; 30.6;
d;  22; 0  0:00:52.316;10.50; 2.20;10.507; 0.560; 30.6;
d;  22; 0  0:00:52.434;10.40; 2.20;10.405; 0.555; 30.6;
d;  22; 0  0:00:52.554;10.30; 2.20;10.309; 0.549; 30.6;
d;  22; 0  0:00:52.719;10.20; 2.20;10.192; 0.543; 30.6;
d;  22; 0  0:00:52.835;10.10; 2.20;10.098; 0.538; 30.6;
d;  22; 0  0:00:52.956;10.00; 2.20; 9.996; 0.533; 30.6;
d;  22; 0  0:00:53.074; 9.90; 2.20; 9.900; 0.528; 30.6;
d;  22; 0  0:00:53.235; 9.80; 2.20; 9.809; 0.522; 30.7;
d;  22; 0  0:00:53.351; 9.70; 2.20; 9.703; 0.517; 30.7;
d;  22; 0  0:00:53.468; 9.60; 2.20; 9.612; 0.512; 30.7;
d;  22; 0  0:00:53.585; 9.50; 2.20; 9.514; 0.507; 30.7;
d;  22; 0  0:00:53.709; 9.40; 2.20; 9.406; 0.502; 30.7;
d;  22; 0  0:00:53.867; 9.30; 2.20; 9.306; 0.496; 30.7;
d;  22; 0  0:00:53.988; 9.20; 2.20; 9.210; 0.491; 30.7;
d;  22; 0  0:00:54.103; 9.10; 2.20; 9.103; 0.485; 30.7;
d;  22; 0  0:00:54.266; 9.00; 2.20; 9.015; 0.481; 30.7;
d;  22; 0  0:00:54.403; 8.90; 2.20; 8.921; 0.475; 30.7;
d;  22; 0  0:00:54.570; 8.80; 2.20; 8.791; 0.469; 30.7;
d;  22; 0  0:00:54.687; 8.70; 2.20; 8.712; 0.465; 30.8;
d;  22; 0  0:00:54.805; 8.60; 2.20; 8.601; 0.459; 30.8;
d;  22; 0  0:00:54.925; 8.50; 2.20; 8.499; 0.453; 30.8;
d;  22; 0  0:00:55.090; 8.40; 2.20; 8.400; 0.447; 30.8;
d;  22; 0  0:00:55.209; 8.30; 2.20; 8.307; 0.443; 30.8;
d;  22; 0  0:00:55.326; 8.20; 2.20; 8.205; 0.437; 30.8;
d;  22; 0  0:00:55.494; 8.10; 2.20; 8.110; 0.432; 30.8;
d;  22; 0  0:00:55.610; 8.00; 2.20; 8.000; 0.426; 30.8;
d;  22; 0  0:00:55.728; 7.90; 2.20; 7.899; 0.422; 30.8;
d;  22; 0  0:00:55.848; 7.80; 2.20; 7.800; 0.416; 30.8;
d;  22; 0  0:00:56.006; 7.70; 2.20; 7.710; 0.410; 30.8;
d;  22; 0  0:00:56.128; 7.60; 2.20; 7.616; 0.406; 30.8;
d;  22; 0  0:00:56.245; 7.50; 2.20; 7.511; 0.400; 30.8;
d;  22; 0  0:00:56.410; 7.40; 2.20; 7.392; 0.394; 30.8;
d;  22; 0  0:00:56.527; 7.30; 2.20; 7.298; 0.388; 30.8;
d;  22; 0  0:00:56.643; 7.20; 2.20; 7.188; 0.383; 30.8;
d;  22; 0  0:00:56.762; 7.10; 2.20; 7.104; 0.379; 30.8;
d;  22; 0  0:00:56.923; 7.00; 2.20; 7.005; 0.373; 30.8;
d;  22; 0  0:00:57.040; 6.90; 2.20; 6.900; 0.368; 30.8;
d;  22; 0  0:00:57.156; 6.80; 2.20; 6.804; 0.363; 30.8;
d;  22; 0  0:00:57.324; 6.70; 2.20; 6.716; 0.358; 30.8;
d;  22; 0  0:00:57.441; 6.60; 2.20; 6.610; 0.352; 30.8;
d;  22; 0  0:00:57.558; 6.50; 2.20; 6.513; 0.348; 30.8;
d;  22; 0  0:00:57.678; 6.40; 2.20; 6.404; 0.342; 30.8;
d;  22; 0  0:00:57.841; 6.30; 2.20; 6.300; 0.336; 30.8;
d;  22; 0  0:00:57.960; 6.20; 2.20; 6.202; 0.331; 30.8;
d;  22; 0  0:00:58.077; 6.10; 2.20; 6.089; 0.325; 30.8;
d;  22; 0  0:00:58.249; 6.00; 2.20; 5.982; 0.319; 30.8;
d;  22; 0  0:00:58.368; 5.90; 2.20; 5.886; 0.313; 30.8;
d;  22; 0  0:00:58.537; 5.80; 2.20; 5.798; 0.309; 30.8;
d;  22; 0  0:00:58.654; 5.70; 2.20; 5.699; 0.304; 30.8;
d;  22; 0  0:00:58.772; 5.60; 2.20; 5.595; 0.298; 30.8;
d;  22; 0  0:00:58.887; 5.50; 2.20; 5.508; 0.294; 30.8;
d;  22; 0  0:00:59.051; 5.40; 2.20; 5.409; 0.289; 30.8;
d;  22; 0  0:00:59.167; 5.30; 2.20; 5.305; 0.282; 30.8;
d;  22; 0  0:00:59.284; 5.20; 2.20; 5.197; 0.277; 30.8;
d;  22; 0  0:00:59.399; 5.10; 2.20; 5.102; 0.271; 30.8;
d;  22; 0  0:00:59.519; 5.00; 2.20; 4.997; 0.266; 30.8;
d;  22; 0  0:00:59.686; 4.90; 2.20; 4.899; 0.261; 30.8;
d;  22; 0  0:00:59.805; 4.80; 2.20; 4.806; 0.256; 30.8;
d;  22; 0  0:00:59.921; 4.70; 2.20; 4.707; 0.251; 30.8;
d;  22; 0  0:01:00.086; 4.60; 2.20; 4.611; 0.246; 30.8;
d;  22; 0  0:01:00.203; 4.50; 2.20; 4.514; 0.241; 30.8;
d;  22; 0  0:01:00.334; 4.40; 2.20; 4.419; 0.235; 30.8;
d;  22; 0  0:01:00.450; 4.30; 2.20; 4.301; 0.229; 30.8;
d;  22; 0  0:01:00.597; 4.20; 2.20; 4.207; 0.224; 30.8;
d;  22; 0  0:01:00.714; 4.10; 2.20; 4.111; 0.219; 30.8;
d;  22; 0  0:01:00.834; 4.00; 2.20; 4.013; 0.214; 30.8;
d;  22; 0  0:01:00.969; 3.90; 2.20; 3.900; 0.208; 30.8;
d;  22; 0  0:01:01.088; 3.80; 2.20; 3.804; 0.202; 30.8;
d;  22; 0  0:01:01.207; 3.70; 2.20; 3.706; 0.197; 30.8;
m;  29; 0  0:01:01.225;"setpoint succesfully reached"
m;    ; 0  0:01:08.753;"program terminated"
****************************************************
* t;line;data    (t)ype: d=data, m=message, e=error
m;line;message
e;line;message
d;line;---- time -----;-U/V-;-I/A-;Uout/V;Iout/A;deg C
m;    ; 0  0:01:28.154;"program started"
d;   6; 0  0:01:29.689; 3.30; 2.20; 3.308; 0.401; 30.2;
m;   7; 0  0:01:29.705;"increasing voltage"
d;  11; 0  0:01:29.843; 3.40; 2.20; 3.401; 0.412; 30.2;
d;  11; 0  0:01:29.961; 3.50; 2.20; 3.498; 0.424; 30.2;
d;  11; 0  0:01:30.081; 3.60; 2.20; 3.593; 0.435; 30.2;
d;  11; 0  0:01:30.246; 3.70; 2.20; 3.693; 0.447; 30.2;
d;  11; 0  0:01:30.362; 3.80; 2.20; 3.786; 0.458; 30.2;
d;  11; 0  0:01:30.480; 3.90; 2.20; 3.884; 0.470; 30.2;
d;  11; 0  0:01:30.601; 4.00; 2.20; 3.997; 0.485; 30.3;
d;  11; 0  0:01:30.765; 4.10; 2.20; 4.093; 0.496; 30.3;
d;  11; 0  0:01:30.881; 4.20; 2.20; 4.189; 0.507; 30.3;
d;  11; 0  0:01:31.000; 4.30; 2.20; 4.285; 0.519; 30.3;
d;  11; 0  0:01:31.166; 4.40; 2.20; 4.399; 0.533; 30.3;
d;  11; 0  0:01:31.283; 4.50; 2.20; 4.496; 0.545; 30.3;
d;  11; 0  0:01:31.401; 4.60; 2.20; 4.592; 0.556; 30.3;
d;  11; 0  0:01:31.522; 4.70; 2.20; 4.687; 0.567; 30.3;
d;  11; 0  0:01:31.681; 4.80; 2.20; 4.782; 0.579; 30.3;
d;  11; 0  0:01:31.803; 4.90; 2.20; 4.896; 0.593; 30.3;
d;  11; 0  0:01:31.920; 5.00; 2.20; 4.994; 0.605; 30.3;
d;  11; 0  0:01:32.086; 5.10; 2.20; 5.097; 0.617; 30.2;
d;  11; 0  0:01:32.205; 5.20; 2.20; 5.205; 0.631; 30.2;
d;  11; 0  0:01:32.321; 5.30; 2.20; 5.297; 0.642; 30.2;
d;  11; 0  0:01:32.438; 5.40; 2.20; 5.405; 0.655; 30.2;
d;  11; 0  0:01:32.602; 5.50; 2.20; 5.488; 0.665; 30.2;
d;  11; 0  0:01:32.741; 5.60; 2.20; 5.622; 0.682; 30.2;
d;  11; 0  0:01:32.905; 5.70; 2.20; 5.689; 0.689; 30.2;
d;  11; 0  0:01:33.029; 5.80; 2.20; 5.787; 0.701; 30.2;
d;  17; 0  0:01:33.044; 5.80; 2.20; 5.787; 0.701; 30.2;
m;  18; 0  0:01:33.061;"lowering voltage to match Iout=200mA"
d;  22; 0  0:01:33.203; 5.70; 2.20; 5.688; 0.689; 30.2;
d;  22; 0  0:01:33.323; 5.60; 2.20; 5.590; 0.677; 30.2;
d;  22; 0  0:01:33.438; 5.50; 2.20; 5.484; 0.665; 30.2;
d;  22; 0  0:01:33.555; 5.40; 2.20; 5.401; 0.655; 30.3;
d;  22; 0  0:01:33.672; 5.30; 2.20; 5.297; 0.642; 30.3;
d;  22; 0  0:01:33.834; 5.20; 2.20; 5.200; 0.630; 30.3;
d;  22; 0  0:01:33.954; 5.10; 2.20; 5.112; 0.619; 30.3;
d;  22; 0  0:01:34.070; 5.00; 2.20; 5.011; 0.608; 30.3;
d;  22; 0  0:01:34.189; 4.90; 2.20; 4.917; 0.596; 30.3;
d;  22; 0  0:01:34.309; 4.80; 2.20; 4.803; 0.582; 30.3;
d;  22; 0  0:01:34.470; 4.70; 2.20; 4.707; 0.570; 30.3;
d;  22; 0  0:01:34.588; 4.60; 2.20; 4.611; 0.559; 30.3;
d;  22; 0  0:01:34.705; 4.50; 2.20; 4.514; 0.547; 30.3;
d;  22; 0  0:01:34.867; 4.40; 2.20; 4.397; 0.533; 30.3;
d;  22; 0  0:01:34.985; 4.30; 2.20; 4.303; 0.521; 30.3;
d;  22; 0  0:01:35.104; 4.20; 2.20; 4.205; 0.510; 30.3;
d;  22; 0  0:01:35.222; 4.10; 2.20; 4.114; 0.498; 30.3;
d;  22; 0  0:01:35.387; 4.00; 2.20; 4.001; 0.485; 30.3;
d;  22; 0  0:01:35.504; 3.90; 2.20; 3.901; 0.472; 30.3;
d;  22; 0  0:01:35.619; 3.80; 2.20; 3.804; 0.461; 30.3;
d;  22; 0  0:01:35.739; 3.70; 2.20; 3.709; 0.449; 30.3;
d;  22; 0  0:01:35.858; 3.60; 2.20; 3.617; 0.438; 30.3;
d;  22; 0  0:01:35.989; 3.50; 2.20; 3.502; 0.424; 30.3;
m;  44; 0  0:01:36.011;"fail at lower testpoint"
m;    ; 0  0:01:43.533;"program terminated"
****************************************************
* t;line;data    (t)ype: d=data, m=message, e=error
m;line;message
e;line;message
d;line;---- time -----;-U/V-;-I/A-;Uout/V;Iout/A;deg C
m;    ; 0  0:01:49.096;"program started"
d;   6; 0  0:01:50.627; 3.30; 2.20; 3.302; 0.000; 30.1;
m;   7; 0  0:01:50.643;"increasing voltage"
d;  11; 0  0:01:50.779; 3.40; 2.20; 3.410; 0.001; 30.1;
d;  11; 0  0:01:50.898; 3.50; 2.20; 3.513; 0.000; 30.1;
d;  11; 0  0:01:51.014; 3.60; 2.20; 3.599; 0.000; 30.1;
d;  11; 0  0:01:51.129; 3.70; 2.20; 3.697; 0.000; 30.1;
d;  11; 0  0:01:51.245; 3.80; 2.20; 3.802; 0.000; 30.1;
d;  11; 0  0:01:51.413; 3.90; 2.20; 3.898; 0.000; 30.1;
d;  11; 0  0:01:51.550; 4.00; 2.20; 3.999; 0.000; 30.1;
d;  11; 0  0:01:51.713; 4.10; 2.20; 4.095; 0.000; 30.1;
d;  11; 0  0:01:51.829; 4.20; 2.20; 4.200; 0.000; 30.1;
d;  11; 0  0:01:51.946; 4.30; 2.20; 4.296; 0.000; 30.1;
d;  11; 0  0:01:52.063; 4.40; 2.20; 4.402; 0.000; 30.1;
d;  11; 0  0:01:52.184; 4.50; 2.20; 4.490; 0.000; 30.1;
d;  11; 0  0:01:52.343; 4.60; 2.20; 4.610; 0.000; 30.1;
d;  11; 0  0:01:52.464; 4.70; 2.20; 4.699; 0.000; 30.1;
d;  11; 0  0:01:52.579; 4.80; 2.20; 4.797; 0.000; 30.1;
d;  11; 0  0:01:52.743; 4.90; 2.20; 4.893; 0.000; 30.1;
d;  11; 0  0:01:52.859; 5.00; 2.20; 4.995; 0.000; 30.1;
d;  11; 0  0:01:52.978; 5.10; 2.20; 5.088; 0.000; 30.1;
d;  11; 0  0:01:53.093; 5.20; 2.20; 5.193; 0.000; 30.1;
d;  11; 0  0:01:53.254; 5.30; 2.20; 5.286; 0.000; 30.1;
d;  11; 0  0:01:53.375; 5.40; 2.20; 5.403; 0.000; 30.1;
d;  11; 0  0:01:53.492; 5.50; 2.20; 5.507; 0.000; 30.1;
d;  11; 0  0:01:53.613; 5.60; 2.20; 5.595; 0.000; 30.1;
d;  11; 0  0:01:53.733; 5.70; 2.20; 5.691; 0.000; 30.1;
d;  11; 0  0:01:53.893; 5.80; 2.20; 5.801; 0.000; 30.1;
d;  11; 0  0:01:54.011; 5.90; 2.20; 5.887; 0.000; 30.1;
d;  11; 0  0:01:54.128; 6.00; 2.20; 5.985; 0.000; 30.1;
d;  11; 0  0:01:54.299; 6.10; 2.20; 6.092; 0.000; 30.1;
d;  11; 0  0:01:54.417; 6.20; 2.20; 6.190; 0.000; 30.1;
d;  11; 0  0:01:54.541; 6.30; 2.20; 6.305; 0.000; 30.1;
d;  11; 0  0:01:54.661; 6.40; 2.20; 6.410; 0.000; 30.1;
d;  11; 0  0:01:54.818; 6.50; 2.20; 6.510; 0.000; 30.1;
d;  11; 0  0:01:54.938; 6.60; 2.20; 6.601; 0.000; 30.1;
d;  11; 0  0:01:55.053; 6.70; 2.20; 6.701; 0.000; 30.1;
d;  11; 0  0:01:55.173; 6.80; 2.20; 6.804; 0.000; 30.1;
d;  11; 0  0:01:55.289; 6.90; 2.20; 6.892; 0.000; 30.1;
d;  11; 0  0:01:55.456; 7.00; 2.20; 6.989; 0.000; 30.1;
d;  11; 0  0:01:55.575; 7.10; 2.20; 7.092; 0.000; 30.1;
d;  11; 0  0:01:55.695; 7.20; 2.20; 7.183; 0.000; 30.1;
d;  11; 0  0:01:55.862; 7.30; 2.20; 7.304; 0.000; 30.1;
d;  11; 0  0:01:55.979; 7.40; 2.20; 7.403; 0.000; 30.1;
d;  11; 0  0:01:56.106; 7.50; 2.20; 7.504; 0.000; 30.1;
d;  11; 0  0:01:56.223; 7.60; 2.20; 7.602; 0.000; 30.1;
d;  11; 0  0:01:56.377; 7.70; 2.20; 7.709; 0.000; 30.1;
d;  11; 0  0:01:56.493; 7.80; 2.20; 7.806; 0.000; 30.1;
d;  11; 0  0:01:56.609; 7.90; 2.20; 7.894; 0.000; 30.1;
d;  11; 0  0:01:56.727; 8.00; 2.20; 8.000; 0.000; 30.1;
d;  11; 0  0:01:56.852; 8.10; 2.20; 8.097; 0.000; 30.1;
d;  11; 0  0:01:57.005; 8.20; 2.20; 8.190; 0.000; 30.1;
d;  11; 0  0:01:57.126; 8.30; 2.20; 8.294; 0.000; 30.1;
d;  11; 0  0:01:57.242; 8.40; 2.20; 8.392; 0.000; 30.1;
d;  11; 0  0:01:57.407; 8.50; 2.20; 8.484; 0.000; 30.1;
d;  11; 0  0:01:57.523; 8.60; 2.20; 8.608; 0.000; 30.1;
d;  11; 0  0:01:57.640; 8.70; 2.20; 8.707; 0.000; 30.1;
d;  11; 0  0:01:57.756; 8.80; 2.20; 8.801; 0.000; 30.1;
d;  11; 0  0:01:57.923; 8.90; 2.20; 8.895; 0.000; 30.1;
d;  11; 0  0:01:58.044; 9.00; 2.20; 9.004; 0.000; 30.2;
d;  11; 0  0:01:58.160; 9.10; 2.20; 9.091; 0.000; 30.2;
d;  11; 0  0:01:58.326; 9.20; 2.20; 9.190; 0.000; 30.2;
d;  11; 0  0:01:58.441; 9.30; 2.20; 9.295; 0.000; 30.2;
d;  11; 0  0:01:58.560; 9.40; 2.20; 9.396; 0.000; 30.2;
d;  11; 0  0:01:58.678; 9.50; 2.20; 9.500; 0.000; 30.2;
d;  11; 0  0:01:58.835; 9.60; 2.20; 9.610; 0.001; 30.2;
d;  11; 0  0:01:58.953; 9.70; 2.20; 9.703; 0.000; 30.2;
d;  11; 0  0:01:59.071; 9.80; 2.20; 9.793; 0.000; 30.2;
d;  11; 0  0:01:59.239; 9.90; 2.20; 9.903; 0.000; 30.2;
d;  11; 0  0:01:59.358;10.00; 2.20; 9.997; 0.000; 30.2;
d;  11; 0  0:01:59.490;10.10; 2.20;10.071; 0.000; 30.2;
d;  11; 0  0:01:59.614;10.20; 2.20;10.199; 0.000; 30.2;
d;  11; 0  0:01:59.760;10.30; 2.20;10.295; 0.000; 30.2;
d;  11; 0  0:01:59.878;10.40; 2.20;10.384; 0.000; 30.2;
d;  11; 0  0:01:59.993;10.50; 2.20;10.487; 0.000; 30.2;
d;  11; 0  0:02:00.112;10.60; 2.20;10.589; 0.000; 30.2;
d;  11; 0  0:02:00.230;10.70; 2.20;10.715; 0.000; 30.2;
d;  11; 0  0:02:00.389;10.80; 2.20;10.797; 0.000; 30.2;
d;  11; 0  0:02:00.508;10.90; 2.20;10.898; 0.000; 30.2;
d;  11; 0  0:02:00.625;11.00; 2.20;10.989; 0.000; 30.2;
d;  11; 0  0:02:00.790;11.10; 2.20;11.088; 0.000; 30.3;
d;  11; 0  0:02:00.906;11.20; 2.20;11.193; 0.000; 30.3;
d;  11; 0  0:02:01.026;11.30; 2.20;11.307; 0.000; 30.3;
d;  11; 0  0:02:01.144;11.40; 2.20;11.406; 0.000; 30.3;
d;  11; 0  0:02:01.310;11.50; 2.20;11.508; 0.000; 30.3;
d;  11; 0  0:02:01.426;11.60; 2.20;11.603; 0.000; 30.3;
d;  11; 0  0:02:01.547;11.70; 2.20;11.697; 0.000; 30.3;
d;  11; 0  0:02:01.712;11.80; 2.20;11.806; 0.000; 30.3;
d;  11; 0  0:02:01.831;11.90; 2.20;11.903; 0.000; 30.3;
d;  11; 0  0:02:01.947;12.00; 2.20;11.988; 0.000; 30.3;
d;  11; 0  0:02:02.068;12.10; 2.20;12.095; 0.000; 30.3;
d;  11; 0  0:02:02.229;12.20; 2.20;12.194; 0.001; 30.3;
d;  11; 0  0:02:02.349;12.30; 2.20;12.286; 0.000; 30.3;
d;  11; 0  0:02:02.464;12.40; 2.20;12.385; 0.000; 30.3;
d;  11; 0  0:02:02.633;12.50; 2.20;12.484; 0.000; 30.3;
d;  11; 0  0:02:02.755;12.60; 2.20;12.605; 0.000; 30.3;
d;  11; 0  0:02:02.872;12.70; 2.20;12.703; 0.000; 30.3;
d;  11; 0  0:02:02.989;12.80; 2.20;12.808; 0.000; 30.3;
d;  11; 0  0:02:03.150;12.90; 2.20;12.900; 0.000; 30.3;
d;  11; 0  0:02:03.266;13.00; 2.20;12.996; 0.000; 30.3;
d;  11; 0  0:02:03.383;13.10; 2.20;13.103; 0.000; 30.3;
d;  11; 0  0:02:03.502;13.20; 2.20;13.186; 0.000; 30.3;
d;  11; 0  0:02:03.622;13.30; 2.20;13.287; 0.000; 30.3;
d;  11; 0  0:02:03.781;13.40; 2.20;13.392; 0.000; 30.3;
d;  11; 0  0:02:03.904;13.50; 2.20;13.488; 0.000; 30.3;
d;  11; 0  0:02:04.022;13.60; 2.20;13.597; 0.000; 30.3;
d;  11; 0  0:02:04.186;13.70; 2.20;13.706; 0.000; 30.3;
d;  11; 0  0:02:04.306;13.80; 2.20;13.805; 0.000; 30.3;
d;  11; 0  0:02:04.425;13.90; 2.20;13.918; 0.000; 30.3;
d;  11; 0  0:02:04.575;14.00; 2.20;14.025; 0.000; 30.3;
d;  11; 0  0:02:04.739;14.10; 2.20;14.101; 0.000; 30.3;
m;  37; 0  0:02:04.763;"fail at upper testpoint"
m;    ; 0  0:02:12.282;"program terminated"
```

[home](../README.md)