# chargers


- [comment on USB type C chargers](#comment-on-USB-type-C-chargers)
    - [tested chargers](#tested-chargers)
    - [issues](#issues)
        - [power supply resets](#power-supply-resets)
        - [power bank resets after 10s](#power-bank-resets-after-10s)



### comment on USB type C chargers

My Charger supports 5 V / 9 V / 12 V / 15 V @ 3A and 20 V @ 5A fixed profiles and 3.3 V - 21 V @ 5 A augmented 
profile. Be careful many chargers marked with 100W (even Ugreen) will only support 65 W PPS (3.25 A) with 
limited voltage range. They may not supprot voltages below 3.3 V. Some will even have two PPS profiles with
differnet voltage / current ratings.

### tested chargers


- UGREEN Nexode 2 Port 100W PD-Charger  (Model CD254 #50827)
  Supported Profiles:  
    - Fixed	 	
	    - 5 V / 3 A, 9 V / 3 A, 12 V / 3 A, 15 V / 3 A, 20 V / 5A
    - Augmented 	
	    - 3.3 - 21 V / 5A 

- UGREEN Nexode 100W Desktop Charger (Model CD328 #90928)
  Supported Profiles:  
    - Fixed  
	    - 5 V / 3 A, 9 V / 3 A, 12 V / 3 A, 15 V / 3 A, 20 V / 5A
    - Augmented  	
	    - 3.3 - 21 V / 5A 
         
- Anker PowerPort I 30W PD  
  Supported Profiles:
  - Fixed 		
      - 5 V / 3 A, 9 V / 3 A, 15 V / 2 A, 20 V / 1.5 A

- iLepo USB C Fast Charger 65 W   
  Supported Profiles  
    - Fixed  
        - 5 V / 3 A, 9 V / 3 A, 12 V / 3A, 15 V / 3 A, 20 V / 3.25 A
    - Augmented  
        - 3.3V - 11V 5A

- INIU Power Bank 20000mAh, 22.5W   
  Supported Profiles
    - Fixed  
        - 5 V / 3 A, 9 V / 2.22 A, 12 V / 1.5 A
    - Augmented  
        -5.0 V - 5.9 V / 3 A
        -5.0 V - 11 V / 2 A

### issues

#### power supply resets
Some power suplly seem to reset the power when no current is drawn.  
My "Nexode 100W Desktop Charger" P/N 90928 resets after ~1 h without load. Other chargers don't.

#### power bank resets after 10s
My INIU Power Bank resets with no load within ~10s   

***log with load 20R***
```
0006: FUSB302 ver ID:B_revA
0118: USB attached CC1 vRd-3.0
0172: RX Src_Cap id=1 raw=0x53A1
0172:  obj0=0x2A01912C
0172:  obj1=0x0002D0E9
0172:  obj2=0x0003C096
0172:  obj3=0xC076323C
0172:  obj4=0xC0DC3228
0172:    [0] 5.00V 3.00A
0172:    [1] 9.00V 2.33A
0172:    [2] 12.00V 1.50A
0172:    [3] 5.00V-5.90V 3.00A PPS *
0172:    [4] 5.00V-11.00V 2.00A PPS
0176: TX Request id=0 raw=0x1082
0176:  obj0=0x42022628
0186: RX GoodCRC id=0 raw=0x0121
0192: RX Accept id=2 raw=0x05A3
0210: RX PS_RDY id=3 raw=0x07A6
0212: PPS 5.50V 2.00A supply ready
0214: Load SW ON
5214: TX Request id=1 raw=0x1282
5214:  obj0=0x42022628
5222: RX GoodCRC id=1 raw=0x0321
5228: RX Accept id=4 raw=0x09A3
5246: RX PS_RDY id=5 raw=0x0BA6
5246: PPS 5.50V 2.00A supply ready
10248:TX Request id=2 raw=0x1482
10248: obj0=0x42022628
10256:RX GoodCRC id=2 raw=0x0521
10262:RX Accept id=6 raw=0x0DA3
10280:RX PS_RDY id=7 raw=0x0FA6
10280:PPS 5.50V 2.00A supply ready
  ...
```

***log without load:***
```
0006: FUSB302 ver ID:B_revA
0118: USB attached CC1 vRd-3.0
0172: RX Src_Cap id=1 raw=0x53A1
0172:  obj0=0x2A01912C
0172:  obj1=0x0002D0E9
0172:  obj2=0x0003C096
0172:  obj3=0xC076323C
0172:  obj4=0xC0DC3228
0172:    [0] 5.00V 3.00A
0172:    [1] 9.00V 2.33A
0172:    [2] 12.00V 1.50A
0172:    [3] 5.00V-5.90V 3.00A PPS *
0172:    [4] 5.00V-11.00V 2.00A PPS
0176: TX Request id=0 raw=0x1082
0176:  obj0=0x42022628
0186: RX GoodCRC id=0 raw=0x0121
0192: RX Accept id=2 raw=0x05A3
0210: RX PS_RDY id=3 raw=0x07A6
0212: PPS 5.50V 2.00A supply ready
0214: Load SW ON
5214: TX Request id=1 raw=0x1282
5214:  obj0=0x42022628
5222: RX GoodCRC id=1 raw=0x0321
5228: RX Accept id=4 raw=0x09A3
5248: RX PS_RDY id=5 raw=0x0BA6
5248: PPS 5.50V 2.00A supply ready
10250:TX Request id=2 raw=0x1482
10250: obj0=0x42022628
10258:RX GoodCRC id=2 raw=0x0521
10264:RX Accept id=6 raw=0x0DA3
10284:RX PS_RDY id=7 raw=0x0FA6
10284:PPS 5.50V 2.00A supply ready

==> The Power bank resets and defaults to 5V only.

0006: FUSB302 ver ID:B_revA
0118: USB attached CC1 vRd-3.0
0352: TX Get_Src_Cap id=0 raw=0x0087
0704: TX Get_Src_Cap id=0 raw=0x0087
1056: TX Get_Src_Cap id=0 raw=0x0087
```

