# logging live data

#### Table of contents
- [features](#features)  
- [log file](#log-file)  


## features

on systems with sd card an automated logging can be enabled when an sd card is present.

Logging intervals of 250ms, 500ms, 1s, 2.5s, 5s, 10s, 15s, 20s, and 30s are available.
the file name is an 8 digit numerical value and automatically incremented whenever a logging session is closed.

when enabled the logging will start immedeately after system start.



## log file

the content of a logfile looks like this
```
---- time -----;mod;-U/V-;-I/V-;Ubus/V;Uout/V;Iout/A;deg C;ms
 0  0:11:30.645;UI^; 5.50; 1.00; 5.418; 0.000; 0.001; 29.5; 0
 0  0:11:35.000;UI^; 5.50; 1.00; 5.418; 0.000; 0.001; 29.5;12
 0  0:11:40.031;UI^; 5.50; 1.00; 5.419; 0.000; 0.001; 29.5;13
 0  0:11:45.000;UI^; 5.50; 1.00; 5.415; 0.000; 0.001; 29.5;13
 0  0:11:50.000;UI^; 5.50; 1.00; 5.418; 0.000; 0.000; 29.5;14
 0  0:11:55.000;UI^; 5.50; 1.00; 5.420; 0.000; 0.001; 29.5;13
 0  0:12:00.000;UI^; 5.50; 1.00; 5.420; 0.001; 0.000; 29.5;13
 0  0:12:05.000;UI^; 5.50; 1.00; 5.418; 0.000; 0.001; 29.5;14
 0  0:12:10.000;UI^; 5.50; 1.00; 5.419; 0.000; 0.000; 29.5;17
 0  0:12:15.000;UI^; 5.50; 1.00; 5.462; 5.486; 0.292; 29.5;14
 0  0:12:20.000;UI^; 5.50; 1.00; 5.461; 5.486; 0.292; 29.6;12
 0  0:12:25.000;UI^; 5.50; 1.00; 5.464; 5.488; 0.292; 29.6;15
 0  0:12:30.000;UI^; 5.50; 1.00; 5.458; 5.488; 0.292; 29.6;12
 0  0:12:35.000;UI^; 5.50; 1.00; 5.460; 5.488; 0.292; 29.7;15
 0  0:12:40.019;UI^; 5.50; 1.00; 5.464; 5.484; 0.292; 29.7;12
 0  0:12:45.000;UI^; 5.50; 1.00; 5.460; 5.486; 0.291; 29.8;15
 0  0:12:50.000;UI^; 5.50; 1.00; 5.464; 5.489; 0.293; 29.8;17
 0  0:12:55.000;UI^; 5.50; 1.00; 5.463; 5.488; 0.292; 29.8;14
 0  0:13:00.000;UI^; 5.50; 1.00; 5.460; 5.486; 0.292; 29.8;14
 0  0:13:05.000;UI^; 5.50; 1.00; 5.464; 5.489; 0.292; 29.8;13
 0  0:13:10.000;UI^; 5.50; 1.00; 5.463; 5.488; 0.292; 29.8;14
```
it shows the uptime, the operation mode, the set voltage and curren, the bus voltage, the output voltage and current the internal temperature and a information about the duration of the logging cycle.
This program uses the arduino library for SD and FAT acces. There are usign blocking access and will be quite slow. Writing 64 bytes to the SD card will cost up to 20ms.
I have not found a SD Card library for arduino platform hich make use of DMA and interrupts. If I missed such a lib drop me a note.


[home](../README.md)