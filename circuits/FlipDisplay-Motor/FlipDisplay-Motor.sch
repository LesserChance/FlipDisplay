EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	1050 850  1150 850 
$Comp
L power:GND #PWR?
U 1 1 61A412C1
P 1500 850
F 0 "#PWR?" H 1500 600 50  0001 C CNN
F 1 "GND" H 1505 677 50  0000 C CNN
F 2 "" H 1500 850 50  0001 C CNN
F 3 "" H 1500 850 50  0001 C CNN
	1    1500 850 
	1    0    0    -1  
$EndComp
Wire Wire Line
	1500 850  1450 850 
$Comp
L power:+12V #PWR?
U 1 1 61A3FA54
P 1050 850
F 0 "#PWR?" H 1050 700 50  0001 C CNN
F 1 "+12V" H 1065 1023 50  0000 C CNN
F 2 "" H 1050 850 50  0001 C CNN
F 3 "" H 1050 850 50  0001 C CNN
	1    1050 850 
	1    0    0    -1  
$EndComp
$Comp
L Device:CP1 C1
U 1 1 61A3E426
P 1300 850
F 0 "C1" V 1552 850 50  0000 C CNN
F 1 "100uF" V 1461 850 50  0000 C CNN
F 2 "Capacitor_SMD:C_Elec_6.3x7.7" H 1300 850 50  0001 C CNN
F 3 "~" H 1300 850 50  0001 C CNN
F 4 "C2836443" V 1300 850 50  0001 C CNN "Cnum"
	1    1300 850 
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1350 1400 1250 1400
Text Label 2700 800  0    50   ~ 0
EN
Wire Wire Line
	2850 800  2700 800 
$Comp
L Connector:Conn_01x03_Male J1
U 1 1 61A324CC
P 3050 900
F 0 "J1" H 3022 832 50  0000 R CNN
F 1 "ARDUINO" H 3022 923 50  0000 R CNN
F 2 "Connector_JST:JST_XH_B3B-XH-A_1x03_P2.50mm_Vertical" H 3050 900 50  0001 C CNN
F 3 "~" H 3050 900 50  0001 C CNN
	1    3050 900 
	-1   0    0    1   
$EndComp
Wire Wire Line
	2700 1500 2850 1500
Wire Wire Line
	2700 1600 2850 1600
Wire Wire Line
	2700 1700 2850 1700
Wire Wire Line
	2700 1800 2850 1800
Wire Wire Line
	2700 900  2850 900 
Wire Wire Line
	2700 1000 2850 1000
Text Label 2700 2500 0    50   ~ 0
GND
Text Label 2700 2400 0    50   ~ 0
VMOT
Text Label 2700 2300 0    50   ~ 0
VDD
Wire Wire Line
	2850 2500 2700 2500
Wire Wire Line
	2850 2400 2700 2400
Wire Wire Line
	2850 2300 2700 2300
$Comp
L Connector:Conn_01x03_Male J7
U 1 1 619EB1B2
P 3050 2400
F 0 "J7" H 3022 2332 50  0000 R CNN
F 1 "OUT" H 3022 2423 50  0000 R CNN
F 2 "Connector_JST:JST_XH_B3B-XH-A_1x03_P2.50mm_Vertical" H 3050 2400 50  0001 C CNN
F 3 "~" H 3050 2400 50  0001 C CNN
	1    3050 2400
	-1   0    0    1   
$EndComp
Text Label 2700 2150 0    50   ~ 0
GND
Text Label 2700 2050 0    50   ~ 0
VMOT
Text Label 2700 1950 0    50   ~ 0
VDD
Wire Wire Line
	2850 2150 2700 2150
Wire Wire Line
	2850 2050 2700 2050
Wire Wire Line
	2850 1950 2700 1950
$Comp
L Connector:Conn_01x03_Male J6
U 1 1 619E6DF8
P 3050 2050
F 0 "J6" H 3022 1982 50  0000 R CNN
F 1 "IN" H 3022 2073 50  0000 R CNN
F 2 "Connector_JST:JST_XH_B3B-XH-A_1x03_P2.50mm_Vertical" H 3050 2050 50  0001 C CNN
F 3 "~" H 3050 2050 50  0001 C CNN
	1    3050 2050
	-1   0    0    1   
$EndComp
Wire Wire Line
	2700 1250 2850 1250
Wire Wire Line
	2700 1150 2850 1150
Text Label 2700 1350 0    50   ~ 0
GND
Text Label 2700 1250 0    50   ~ 0
VDD
Text Label 2700 1150 0    50   ~ 0
LOOP
Wire Wire Line
	2850 1350 2700 1350
$Comp
L Connector:Conn_01x03_Male J2
U 1 1 619E24E6
P 3050 1250
F 0 "J2" H 3022 1182 50  0000 R CNN
F 1 "LOOP SENSOR" H 3022 1273 50  0000 R CNN
F 2 "Connector_JST:JST_XH_B3B-XH-A_1x03_P2.50mm_Vertical" H 3050 1250 50  0001 C CNN
F 3 "~" H 3050 1250 50  0001 C CNN
	1    3050 1250
	-1   0    0    1   
$EndComp
Wire Wire Line
	1250 2000 1350 2000
Text Label 2700 1000 0    50   ~ 0
LOOP
Text Label 2700 900  0    50   ~ 0
ST
Wire Wire Line
	1250 1900 1250 1800
Wire Wire Line
	2050 1500 2050 2100
Wire Wire Line
	1850 1500 2050 1500
Wire Wire Line
	1850 2100 2050 2100
Wire Wire Line
	1850 2000 2350 2000
$Comp
L power:+3.3V #PWR02
U 1 1 619D0F09
P 2350 1150
F 0 "#PWR02" H 2350 1000 50  0001 C CNN
F 1 "+3.3V" H 2365 1323 50  0000 C CNN
F 2 "" H 2350 1150 50  0001 C CNN
F 3 "" H 2350 1150 50  0001 C CNN
	1    2350 1150
	1    0    0    -1  
$EndComp
Wire Wire Line
	2100 1400 2100 1150
Wire Wire Line
	1850 1400 2100 1400
$Comp
L power:+12V #PWR01
U 1 1 619D00D5
P 2100 1150
F 0 "#PWR01" H 2100 1000 50  0001 C CNN
F 1 "+12V" H 2115 1323 50  0000 C CNN
F 2 "" H 2100 1150 50  0001 C CNN
F 3 "" H 2100 1150 50  0001 C CNN
	1    2100 1150
	1    0    0    -1  
$EndComp
Text Label 2700 1600 0    50   ~ 0
1B
Text Label 2700 1500 0    50   ~ 0
1A
Text Label 2700 1700 0    50   ~ 0
2A
Text Label 2700 1800 0    50   ~ 0
2B
$Comp
L Connector:Conn_01x04_Male J4
U 1 1 619CB09A
P 3050 1700
F 0 "J4" H 3022 1582 50  0000 R CNN
F 1 "MOTOR" H 3022 1673 50  0000 R CNN
F 2 "Connector_JST:JST_XH_B4B-XH-A_1x04_P2.50mm_Vertical" H 3050 1700 50  0001 C CNN
F 3 "~" H 3050 1700 50  0001 C CNN
	1    3050 1700
	-1   0    0    1   
$EndComp
Text Label 1850 2100 0    50   ~ 0
GND
Text Label 1850 2000 0    50   ~ 0
VDD
Text Label 1850 1900 0    50   ~ 0
1B
Text Label 1850 1800 0    50   ~ 0
1A
Text Label 1850 1700 0    50   ~ 0
2A
Text Label 1850 1600 0    50   ~ 0
2B
Text Label 1850 1500 0    50   ~ 0
GND
Text Label 1850 1400 0    50   ~ 0
VMOT
$Comp
L Connector:Conn_01x08_Female J5
U 1 1 619C9E9C
P 1650 1800
F 0 "J5" H 1600 1300 50  0000 L CNN
F 1 "Conn_01x08_Female" H 1350 1250 50  0001 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 1650 1800 50  0001 C CNN
F 3 "~" H 1650 1800 50  0001 C CNN
	1    1650 1800
	-1   0    0    1   
$EndComp
Text Label 1250 2100 0    50   ~ 0
DIR
Text Label 1250 2000 0    50   ~ 0
ST
Text Label 1250 1900 0    50   ~ 0
SL
Text Label 1250 1800 0    50   ~ 0
RS
Text Label 1250 1700 0    50   ~ 0
MS3
Text Label 1250 1600 0    50   ~ 0
MS2
Text Label 1250 1500 0    50   ~ 0
MS1
Text Label 1250 1400 0    50   ~ 0
EN
Wire Wire Line
	1350 1900 1250 1900
Wire Wire Line
	1350 1800 1250 1800
$Comp
L Connector:Conn_01x08_Female J3
U 1 1 619C2B15
P 1550 1700
F 0 "J3" H 1500 2100 50  0000 L CNN
F 1 "DRIVER" H 1450 2200 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 1550 1700 50  0001 C CNN
F 3 "~" H 1550 1700 50  0001 C CNN
	1    1550 1700
	1    0    0    -1  
$EndComp
Connection ~ 2050 2100
$Comp
L power:GND #PWR03
U 1 1 619D2EAC
P 900 2350
F 0 "#PWR03" H 900 2100 50  0001 C CNN
F 1 "GND" H 905 2177 50  0000 C CNN
F 2 "" H 900 2350 50  0001 C CNN
F 3 "" H 900 2350 50  0001 C CNN
	1    900  2350
	1    0    0    -1  
$EndComp
Wire Wire Line
	900  2250 900  2350
Wire Wire Line
	2050 2100 2050 2250
Wire Wire Line
	2050 2250 900  2250
Connection ~ 900  2250
Wire Wire Line
	900  2100 900  2250
Wire Wire Line
	900  2100 1350 2100
Connection ~ 900  2100
Wire Wire Line
	900  1700 900  2100
Wire Wire Line
	900  1700 1350 1700
Connection ~ 900  1700
Wire Wire Line
	900  1600 900  1700
Wire Wire Line
	900  1600 1350 1600
Wire Wire Line
	2350 1150 2350 2000
Wire Wire Line
	900  1600 900  1500
Wire Wire Line
	900  1500 1350 1500
Connection ~ 900  1600
$EndSCHEMATC
