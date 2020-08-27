EESchema Schematic File Version 4
LIBS:DomoticShutterPanel-cache
EELAYER 29 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Roller Shutter Controller Panel"
Date "2020-04-24"
Rev ""
Comp ""
Comment1 "Radio Approach"
Comment2 "NOT FINISHED"
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L CustomLibraries_Switch:SW_Push SW1
U 1 1 5E9CB25B
P 5050 2700
F 0 "SW1" V 5100 2850 50  0000 L CNN
F 1 "SW_Push" V 5200 2750 50  0000 L CNN
F 2 "Custom_libraries:SW_PUSH_6mm" H 5050 2900 50  0001 C CNN
F 3 "~" H 5050 2900 50  0001 C CNN
	1    5050 2700
	0    1    1    0   
$EndComp
$Comp
L CustomLibraries_Switch:SW_Push SW2
U 1 1 5E9CB5A7
P 5900 2700
F 0 "SW2" V 5950 2850 50  0000 L CNN
F 1 "SW_Push" V 6050 2800 50  0000 L CNN
F 2 "Custom_libraries:SW_PUSH_6mm" H 5900 2900 50  0001 C CNN
F 3 "~" H 5900 2900 50  0001 C CNN
	1    5900 2700
	0    1    1    0   
$EndComp
$Comp
L CustomLibraries_Switch:SW_Push SW4
U 1 1 5E9CBCA4
P 7600 2700
F 0 "SW4" V 7650 2850 50  0000 L CNN
F 1 "SW_Push" V 7750 2800 50  0000 L CNN
F 2 "Custom_libraries:SW_PUSH_6mm" H 7600 2900 50  0001 C CNN
F 3 "~" H 7600 2900 50  0001 C CNN
	1    7600 2700
	0    1    1    0   
$EndComp
$Comp
L CustomLibraries_Switch:SW_Push SW3
U 1 1 5E9CC698
P 6750 2700
F 0 "SW3" V 6800 2850 50  0000 L CNN
F 1 "SW_Push" V 6900 2800 50  0000 L CNN
F 2 "Custom_libraries:SW_PUSH_6mm" H 6750 2900 50  0001 C CNN
F 3 "~" H 6750 2900 50  0001 C CNN
	1    6750 2700
	0    1    1    0   
$EndComp
$Comp
L CustomLibraries_Device:CP C3
U 1 1 5E9CCFC0
P 6450 2700
F 0 "C3" H 6550 2650 50  0000 L CNN
F 1 "1uF" H 6550 2550 50  0000 L CNN
F 2 "Capacitor_THT:C_Radial_D5.0mm_H5.0mm_P2.00mm" H 6488 2550 50  0001 C CNN
F 3 "~" H 6450 2700 50  0001 C CNN
	1    6450 2700
	1    0    0    -1  
$EndComp
$Comp
L CustomLibraries_Device:CP C4
U 1 1 5E9D1B46
P 7300 2700
F 0 "C4" H 7400 2650 50  0000 L CNN
F 1 "1uF" H 7400 2550 50  0000 L CNN
F 2 "Capacitor_THT:C_Radial_D5.0mm_H5.0mm_P2.00mm" H 7338 2550 50  0001 C CNN
F 3 "~" H 7300 2700 50  0001 C CNN
	1    7300 2700
	1    0    0    -1  
$EndComp
$Comp
L CustomLibraries_Device:CP C2
U 1 1 5E9D37C0
P 5600 2700
F 0 "C2" H 5700 2650 50  0000 L CNN
F 1 "1uF" H 5700 2550 50  0000 L CNN
F 2 "Capacitor_THT:C_Radial_D5.0mm_H5.0mm_P2.00mm" H 5638 2550 50  0001 C CNN
F 3 "~" H 5600 2700 50  0001 C CNN
	1    5600 2700
	1    0    0    -1  
$EndComp
$Comp
L CustomLibraries_power:GND #PWR013
U 1 1 5E9D458F
P 6300 3200
F 0 "#PWR013" H 6300 2950 50  0001 C CNN
F 1 "GND" H 6305 3027 50  0000 C CNN
F 2 "" H 6300 3200 50  0001 C CNN
F 3 "" H 6300 3200 50  0001 C CNN
	1    6300 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	6750 2900 6750 3050
Wire Wire Line
	7600 2900 7600 3050
Connection ~ 6750 3050
Wire Wire Line
	5900 3050 5900 2900
Connection ~ 5900 3050
Wire Wire Line
	5050 3050 5050 2900
Wire Wire Line
	5050 2350 5050 2500
Wire Wire Line
	5900 2350 5900 2500
Wire Wire Line
	6750 2350 6750 2500
Wire Wire Line
	7600 2350 7600 2500
$Comp
L CustomLibraries_Device:R R1
U 1 1 5E9E8880
P 5050 2200
F 0 "R1" H 5120 2246 50  0000 L CNN
F 1 "15k" H 5120 2155 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P1.90mm_Vertical" V 4980 2200 50  0001 C CNN
F 3 "~" H 5050 2200 50  0001 C CNN
	1    5050 2200
	1    0    0    -1  
$EndComp
Connection ~ 5050 2350
$Comp
L CustomLibraries_Device:R R2
U 1 1 5E9E9134
P 5900 2200
F 0 "R2" H 5970 2246 50  0000 L CNN
F 1 "15k" H 5970 2155 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P1.90mm_Vertical" V 5830 2200 50  0001 C CNN
F 3 "~" H 5900 2200 50  0001 C CNN
	1    5900 2200
	1    0    0    -1  
$EndComp
Connection ~ 5900 2350
$Comp
L CustomLibraries_Device:R R3
U 1 1 5E9E963F
P 6750 2200
F 0 "R3" H 6820 2246 50  0000 L CNN
F 1 "15k" H 6820 2155 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P1.90mm_Vertical" V 6680 2200 50  0001 C CNN
F 3 "~" H 6750 2200 50  0001 C CNN
	1    6750 2200
	1    0    0    -1  
$EndComp
Connection ~ 6750 2350
$Comp
L CustomLibraries_Device:R R4
U 1 1 5E9E9CE9
P 7600 2200
F 0 "R4" H 7670 2246 50  0000 L CNN
F 1 "15k" H 7670 2155 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P1.90mm_Vertical" V 7530 2200 50  0001 C CNN
F 3 "~" H 7600 2200 50  0001 C CNN
	1    7600 2200
	1    0    0    -1  
$EndComp
Connection ~ 7600 2350
Wire Wire Line
	6750 3050 7300 3050
$Comp
L CustomLibraries_power:+3.3V #PWR012
U 1 1 5E9EBD42
P 6300 1850
F 0 "#PWR012" H 6300 1700 50  0001 C CNN
F 1 "+3.3V" H 6315 2023 50  0000 C CNN
F 2 "" H 6300 1850 50  0001 C CNN
F 3 "" H 6300 1850 50  0001 C CNN
	1    6300 1850
	1    0    0    -1  
$EndComp
Wire Wire Line
	5050 1950 5050 2050
Wire Wire Line
	5900 2050 5900 1950
Connection ~ 5900 1950
Wire Wire Line
	5900 1950 5050 1950
Wire Wire Line
	6750 1950 6750 2050
Wire Wire Line
	6750 1950 7600 1950
Wire Wire Line
	7600 1950 7600 2050
Connection ~ 6750 1950
Wire Notes Line
	4600 1600 4600 3450
Wire Wire Line
	4750 2850 4750 3050
Wire Wire Line
	4750 3050 5050 3050
Connection ~ 5050 3050
Wire Wire Line
	4750 2550 4750 2350
Wire Wire Line
	4750 2350 5050 2350
$Comp
L CustomLibraries_Device:CP C1
U 1 1 5E9D2266
P 4750 2700
F 0 "C1" H 4850 2650 50  0000 L CNN
F 1 "1uF" H 4850 2550 50  0000 L CNN
F 2 "Capacitor_THT:C_Radial_D5.0mm_H5.0mm_P2.00mm" H 4788 2550 50  0001 C CNN
F 3 "~" H 4750 2700 50  0001 C CNN
	1    4750 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	5600 2350 5600 2550
Wire Wire Line
	5600 2350 5900 2350
Wire Wire Line
	5600 2850 5600 3050
Connection ~ 5600 3050
Wire Wire Line
	5600 3050 5050 3050
Wire Wire Line
	5600 3050 5900 3050
Wire Wire Line
	6450 2550 6450 2350
Wire Wire Line
	6450 2850 6450 3050
Connection ~ 6450 3050
Wire Wire Line
	7300 2850 7300 3050
Connection ~ 7300 3050
Wire Wire Line
	7300 2550 7300 2350
Wire Wire Line
	7300 2350 7600 2350
Wire Wire Line
	6450 2350 6750 2350
Wire Wire Line
	6450 3050 6750 3050
Wire Wire Line
	7300 3050 7600 3050
Text Notes 4700 1750 0    60   ~ 0
Button setup
Wire Notes Line
	4600 1800 5350 1800
Wire Notes Line
	5350 1800 5350 1600
Wire Wire Line
	5900 3050 6300 3050
Wire Wire Line
	6300 3200 6300 3050
Connection ~ 6300 3050
Wire Wire Line
	6300 3050 6450 3050
Wire Wire Line
	5900 1950 6300 1950
Wire Wire Line
	6300 1850 6300 1950
Connection ~ 6300 1950
Wire Wire Line
	6300 1950 6750 1950
Wire Wire Line
	1300 2400 1300 2750
Wire Wire Line
	1900 2950 1900 3050
$Comp
L CustomLibraries_connectors:Conn_01x03 J9
U 1 1 5EACF56F
P 2100 2850
F 0 "J9" H 2050 2650 50  0000 L CNN
F 1 "Conn_01x03" H 1850 2550 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 2100 2850 50  0001 C CNN
F 3 "~" H 2100 2850 50  0001 C CNN
	1    2100 2850
	1    0    0    -1  
$EndComp
Wire Notes Line
	1000 3500 2550 3500
Wire Notes Line
	2550 2050 1000 2050
Text Notes 1050 2200 0    60   ~ 0
Lamp Relay
Wire Notes Line
	1000 2050 1000 3500
Wire Notes Line
	2550 2050 2550 3500
$Comp
L CustomLibraries_connectors:Conn_01x02 J1
U 1 1 5EA2AF4A
P 750 1100
F 0 "J1" H 668 1317 50  0000 C CNN
F 1 "Conn_01x02" H 668 1226 50  0000 C CNN
F 2 "Custom_libraries:PinSocket_1x02_P2.54mm_Vertical" H 750 1100 50  0001 C CNN
F 3 "~" H 750 1100 50  0001 C CNN
	1    750  1100
	-1   0    0    -1  
$EndComp
Wire Notes Line
	1550 700  1550 1500
Text Notes 1600 900  0    50   ~ 0
3.3V DC-DC Step-Down \nPower Module
Wire Notes Line
	1550 1500 2750 1500
Wire Notes Line
	2750 1500 2750 700 
Wire Notes Line
	2750 700  1550 700 
$Comp
L CustomLibraries_power:GND #PWR02
U 1 1 5EA41F46
P 1300 1350
F 0 "#PWR02" H 1300 1100 50  0001 C CNN
F 1 "GND" H 1305 1177 50  0000 C CNN
F 2 "" H 1300 1350 50  0001 C CNN
F 3 "" H 1300 1350 50  0001 C CNN
	1    1300 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	1300 900  1300 1100
Connection ~ 1300 1100
Wire Wire Line
	1300 1200 1300 1350
Connection ~ 1300 1200
Wire Wire Line
	2950 900  2950 1100
$Comp
L CustomLibraries_power:GND #PWR06
U 1 1 5EA488A9
P 2950 1350
F 0 "#PWR06" H 2950 1100 50  0001 C CNN
F 1 "GND" H 2955 1177 50  0000 C CNN
F 2 "" H 2950 1350 50  0001 C CNN
F 3 "" H 2950 1350 50  0001 C CNN
	1    2950 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	2950 1200 2950 1350
$Comp
L CustomLibraries_connectors:Conn_02x02_Odd_Even J2
U 1 1 5EA533B3
P 2100 1100
F 0 "J2" H 2150 900 50  0000 C CNN
F 1 "3.3V DC-DC Step-Down Power Module" H 2150 800 31  0000 C CNN
F 2 "Custom_libraries:DCDC_3v3_PowerModule" H 2100 1100 50  0001 C CNN
F 3 "~" H 2100 1100 50  0001 C CNN
	1    2100 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	1300 1100 1900 1100
Wire Wire Line
	1300 1200 1900 1200
Wire Wire Line
	2400 1100 2950 1100
Wire Wire Line
	2400 1200 2950 1200
Text Notes 8750 2200 0    50   ~ 0
LCD 16x2 I2C
$Comp
L CustomLibraries_connectors:Conn_01x04 J3
U 1 1 5EA32128
P 9350 2500
F 0 "J3" H 9430 2492 50  0000 L CNN
F 1 "Conn_01x04" H 9430 2401 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 9350 2500 50  0001 C CNN
F 3 "~" H 9350 2500 50  0001 C CNN
	1    9350 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	9150 2400 9050 2400
Wire Wire Line
	9150 2500 8900 2500
Wire Wire Line
	9150 2600 8900 2600
Wire Wire Line
	9150 2700 8900 2700
$Comp
L CustomLibraries_power:GND #PWR04
U 1 1 5EA49EAB
P 9050 2850
F 0 "#PWR04" H 9050 2600 50  0001 C CNN
F 1 "GND" H 9055 2677 50  0000 C CNN
F 2 "" H 9050 2850 50  0001 C CNN
F 3 "" H 9050 2850 50  0001 C CNN
	1    9050 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	8900 2500 8900 2450
Text Label 8950 2700 0    39   ~ 0
I2C_SCL
Wire Notes Line
	8700 2100 9900 2100
Wire Notes Line
	9900 3100 8700 3100
Wire Notes Line
	9900 2100 9900 3100
Wire Notes Line
	8700 2100 8700 3100
$Comp
L CustomLibraries_SRD-05VDC-SL-C:SRD-05VDC-SL-C K1
U 1 1 5EACF577
P 1600 2850
F 0 "K1" H 1600 3217 50  0000 C CNN
F 1 "SRD-05VDC-SL-C" H 1600 3126 50  0000 C CNN
F 2 "Custom_libraries:RELAY_SRD-05VDC-SL-C" H 1600 2850 50  0001 L BNN
F 3 "" H 1600 2850 50  0001 L BNN
F 4 "IPC-7251" H 1600 2850 50  0001 L BNN "Campo4"
	1    1600 2850
	1    0    0    -1  
$EndComp
NoConn ~ 1300 3050
NoConn ~ 1300 2400
$Comp
L CustomLibraries_power:+5V #PWR0101
U 1 1 5EB0B1CD
P 8900 2450
F 0 "#PWR0101" H 8900 2300 50  0001 C CNN
F 1 "+5V" H 8915 2623 50  0000 C CNN
F 2 "" H 8900 2450 50  0001 C CNN
F 3 "" H 8900 2450 50  0001 C CNN
	1    8900 2450
	1    0    0    -1  
$EndComp
Wire Wire Line
	9050 2400 9050 2850
$Comp
L CustomLibraries_power:+5V #PWR0102
U 1 1 5EB0FA34
P 1300 900
F 0 "#PWR0102" H 1300 750 50  0001 C CNN
F 1 "+5V" H 1315 1073 50  0000 C CNN
F 2 "" H 1300 900 50  0001 C CNN
F 3 "" H 1300 900 50  0001 C CNN
	1    1300 900 
	1    0    0    -1  
$EndComp
Wire Wire Line
	950  1200 1300 1200
Wire Wire Line
	950  1100 1300 1100
Wire Notes Line
	4200 3900 10100 3900
Wire Wire Line
	8100 5000 8850 5000
Connection ~ 9300 5000
Wire Wire Line
	9300 5050 9300 5000
Wire Wire Line
	9300 5000 9300 4900
Wire Wire Line
	9500 5000 9300 5000
Connection ~ 9300 5400
Wire Wire Line
	9300 5450 9300 5400
Wire Wire Line
	9300 5400 9300 5350
Wire Wire Line
	9900 5400 9300 5400
Wire Wire Line
	9900 5000 9900 5400
Wire Wire Line
	5100 5500 5100 5550
Wire Wire Line
	5050 5500 5100 5500
Wire Wire Line
	6050 5800 6050 5500
Wire Wire Line
	5100 5400 5100 5300
$Comp
L CustomLibraries_Device:R R5
U 1 1 5EB2F921
P 5100 5150
F 0 "R5" H 5250 5100 50  0000 C CNN
F 1 "10k" H 5250 5200 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P1.90mm_Vertical" V 5030 5150 50  0001 C CNN
F 3 "~" H 5100 5150 50  0001 C CNN
	1    5100 5150
	-1   0    0    1   
$EndComp
$Comp
L CustomLibraries_connectors:Conn_01x02 J4
U 1 1 5EB23EC5
P 4850 5400
F 0 "J4" H 5150 5400 50  0000 C CNN
F 1 "BOOT OPTION" H 5200 5300 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 4850 5400 50  0001 C CNN
F 3 "~" H 4850 5400 50  0001 C CNN
	1    4850 5400
	-1   0    0    -1  
$EndComp
$Comp
L CustomLibraries_power:+3V3 #PWR03
U 1 1 5EB09053
P 5100 4900
F 0 "#PWR03" H 5100 4750 50  0001 C CNN
F 1 "+3V3" V 5100 5150 50  0000 C CNN
F 2 "" H 5100 4900 50  0001 C CNN
F 3 "" H 5100 4900 50  0001 C CNN
	1    5100 4900
	1    0    0    -1  
$EndComp
$Comp
L CustomLibraries_power:GND #PWR01
U 1 1 5EAF6562
P 5100 5550
F 0 "#PWR01" H 5100 5300 50  0001 C CNN
F 1 "GND" H 5105 5377 50  0000 C CNN
F 2 "" H 5100 5550 50  0001 C CNN
F 3 "" H 5100 5550 50  0001 C CNN
	1    5100 5550
	1    0    0    -1  
$EndComp
$Comp
L CustomLibraries_Device:R R8
U 1 1 5EAF50ED
P 5750 5600
F 0 "R8" H 5550 5650 50  0000 L CNN
F 1 "10k" H 5550 5550 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P1.90mm_Vertical" V 5680 5600 50  0001 C CNN
F 3 "~" H 5750 5600 50  0001 C CNN
	1    5750 5600
	1    0    0    -1  
$EndComp
$Comp
L CustomLibraries_Switch:SW_Push SW5
U 1 1 5EABBA7B
P 9700 5000
F 0 "SW5" H 9850 5250 50  0000 L CNN
F 1 "SW_Push" H 9750 5150 50  0000 L CNN
F 2 "Custom_libraries:SW_PUSH_6mm" H 9700 5200 50  0001 C CNN
F 3 "~" H 9700 5200 50  0001 C CNN
	1    9700 5000
	1    0    0    -1  
$EndComp
Wire Wire Line
	8400 4700 8100 4700
Wire Wire Line
	8400 5200 8400 4700
Wire Wire Line
	8400 5200 8450 5200
Connection ~ 8400 5200
Wire Wire Line
	8400 5700 8400 5200
$Comp
L CustomLibraries_power:GND #PWR017
U 1 1 5EAAC6E5
P 8400 5700
F 0 "#PWR017" H 8400 5450 50  0001 C CNN
F 1 "GND" H 8405 5527 50  0000 C CNN
F 2 "" H 8400 5700 50  0001 C CNN
F 3 "" H 8400 5700 50  0001 C CNN
	1    8400 5700
	1    0    0    -1  
$EndComp
Wire Wire Line
	8100 5400 8450 5400
Wire Wire Line
	8100 5300 8450 5300
Wire Wire Line
	8100 5200 8400 5200
$Comp
L CustomLibraries_connectors:Conn_01x03 J5
U 1 1 5EA8DAF9
P 8650 5300
F 0 "J5" H 8730 5342 50  0000 L CNN
F 1 "UART COM" H 8730 5251 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 8650 5300 50  0001 C CNN
F 3 "~" H 8650 5300 50  0001 C CNN
	1    8650 5300
	1    0    0    -1  
$EndComp
NoConn ~ 7150 5900
$Comp
L CustomLibraries_power:GND #PWR019
U 1 1 5EA771AA
P 9300 5450
F 0 "#PWR019" H 9300 5200 50  0001 C CNN
F 1 "GND" H 9305 5277 50  0000 C CNN
F 2 "" H 9300 5450 50  0001 C CNN
F 3 "" H 9300 5450 50  0001 C CNN
	1    9300 5450
	1    0    0    -1  
$EndComp
$Comp
L CustomLibraries_power:+3V3 #PWR018
U 1 1 5EA6D326
P 9300 4600
F 0 "#PWR018" H 9300 4450 50  0001 C CNN
F 1 "+3V3" H 9315 4773 50  0000 C CNN
F 2 "" H 9300 4600 50  0001 C CNN
F 3 "" H 9300 4600 50  0001 C CNN
	1    9300 4600
	1    0    0    -1  
$EndComp
$Comp
L CustomLibraries_Device:C C6
U 1 1 5EA6419A
P 9300 5200
F 0 "C6" H 9185 5154 50  0000 R CNN
F 1 "0.1uF" H 9185 5245 50  0000 R CNN
F 2 "Capacitor_THT:C_Radial_D5.0mm_H5.0mm_P2.00mm" H 9338 5050 50  0001 C CNN
F 3 "~" H 9300 5200 50  0001 C CNN
	1    9300 5200
	-1   0    0    1   
$EndComp
$Comp
L CustomLibraries_Device:R R9
U 1 1 5EA62F4D
P 9300 4750
F 0 "R9" H 9100 4800 50  0000 L CNN
F 1 "10k" H 9100 4700 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P1.90mm_Vertical" V 9230 4750 50  0001 C CNN
F 3 "~" H 9300 4750 50  0001 C CNN
	1    9300 4750
	1    0    0    -1  
$EndComp
NoConn ~ 8100 4900
NoConn ~ 8100 4800
Wire Wire Line
	6050 5500 6150 5500
$Comp
L CustomLibraries_power:GND #PWR016
U 1 1 5EA52912
P 5900 5900
F 0 "#PWR016" H 5900 5650 50  0001 C CNN
F 1 "GND" H 5905 5727 50  0000 C CNN
F 2 "" H 5900 5900 50  0001 C CNN
F 3 "" H 5900 5900 50  0001 C CNN
	1    5900 5900
	1    0    0    -1  
$EndComp
$Comp
L CustomLibraries_power:GND #PWR014
U 1 1 5EA42901
P 5450 4800
F 0 "#PWR014" H 5450 4550 50  0001 C CNN
F 1 "GND" V 5350 4650 50  0000 C CNN
F 2 "" H 5450 4800 50  0001 C CNN
F 3 "" H 5450 4800 50  0001 C CNN
	1    5450 4800
	0    1    1    0   
$EndComp
Wire Wire Line
	6150 4800 5750 4800
Wire Wire Line
	5750 4700 5750 4800
$Comp
L CustomLibraries_Device:C C5
U 1 1 5EA39A31
P 5600 4800
F 0 "C5" V 5450 4850 50  0000 R CNN
F 1 "0.1uF" V 5350 4900 50  0000 R CNN
F 2 "Capacitor_THT:C_Radial_D5.0mm_H5.0mm_P2.00mm" H 5638 4650 50  0001 C CNN
F 3 "~" H 5600 4800 50  0001 C CNN
	1    5600 4800
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5750 4400 5750 4350
Wire Wire Line
	6000 4350 6000 4300
Connection ~ 6000 4350
Wire Wire Line
	5750 4350 6000 4350
$Comp
L CustomLibraries_Device:R R7
U 1 1 5EA34E10
P 5750 4550
F 0 "R7" H 5820 4596 50  0000 L CNN
F 1 "10k" H 5820 4505 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P1.90mm_Vertical" V 5680 4550 50  0001 C CNN
F 3 "~" H 5750 4550 50  0001 C CNN
	1    5750 4550
	1    0    0    -1  
$EndComp
Wire Wire Line
	6000 4700 6000 4350
Wire Wire Line
	6150 4700 6000 4700
$Comp
L CustomLibraries_power:+3V3 #PWR015
U 1 1 5EA3091F
P 6000 4300
F 0 "#PWR015" H 6000 4150 50  0001 C CNN
F 1 "+3V3" H 6015 4473 50  0000 C CNN
F 2 "" H 6000 4300 50  0001 C CNN
F 3 "" H 6000 4300 50  0001 C CNN
	1    6000 4300
	1    0    0    -1  
$EndComp
Wire Notes Line
	4200 3900 4200 6400
Wire Notes Line
	10100 3900 10100 6400
$Comp
L ESP8266:ESP-13-WROOM-02 U1
U 1 1 5EA1DF21
P 7150 5000
F 0 "U1" H 7125 5765 50  0000 C CNN
F 1 "ESP-13-WROOM-02" H 7125 5674 50  0000 C CNN
F 2 "Custom_libraries:ESP-13-WROOM-02" H 7050 5000 50  0001 C CNN
F 3 "http://l0l.org.uk/2014/12/esp8266-modules-hardware-guide-gotta-catch-em-all/" H 7050 5000 50  0001 C CNN
	1    7150 5000
	1    0    0    -1  
$EndComp
Text Notes 5150 4050 0    60   ~ 0
ESP-13e
Wire Notes Line
	3950 6850 3950 3900
NoConn ~ 2100 5650
NoConn ~ 2100 5850
NoConn ~ 2100 5950
NoConn ~ 3600 5950
NoConn ~ 3600 5850
NoConn ~ 3600 5750
NoConn ~ 2650 4450
Wire Wire Line
	3150 4400 3150 4600
Wire Wire Line
	3050 4400 3050 4600
Wire Wire Line
	2950 4450 2950 4600
Wire Wire Line
	2850 4450 2850 4600
Wire Wire Line
	2750 4450 2750 4600
Wire Wire Line
	2650 4450 2650 4600
NoConn ~ 3150 6650
NoConn ~ 3050 6650
Wire Wire Line
	3150 6650 3150 6600
Wire Wire Line
	3050 6650 3050 6600
Wire Wire Line
	2350 6250 2350 5350
$Comp
L CustomLibraries_power:GND #PWR07
U 1 1 5E9BAC72
P 2350 6250
F 0 "#PWR07" H 2350 6000 50  0001 C CNN
F 1 "GND" H 2355 6077 50  0000 C CNN
F 2 "" H 2350 6250 50  0001 C CNN
F 3 "" H 2350 6250 50  0001 C CNN
	1    2350 6250
	1    0    0    -1  
$EndComp
Wire Wire Line
	3550 4950 3550 5350
$Comp
L CustomLibraries_power:GND #PWR010
U 1 1 5EBF56D4
P 3450 6250
F 0 "#PWR010" H 3450 6000 50  0001 C CNN
F 1 "GND" H 3455 6077 50  0000 C CNN
F 2 "" H 3450 6250 50  0001 C CNN
F 3 "" H 3450 6250 50  0001 C CNN
	1    3450 6250
	1    0    0    -1  
$EndComp
NoConn ~ 3450 4950
NoConn ~ 3600 5650
NoConn ~ 3600 5550
NoConn ~ 3600 5450
Connection ~ 3150 4400
Wire Wire Line
	3300 4400 3300 4450
Wire Wire Line
	3150 4400 3300 4400
Wire Wire Line
	3050 4400 3150 4400
Text Notes 2750 5050 0    60   ~ 0
1
Wire Wire Line
	3350 6150 3600 6150
Wire Wire Line
	3600 6050 3350 6050
Wire Wire Line
	3350 5950 3600 5950
Wire Wire Line
	3600 5850 3350 5850
Wire Wire Line
	3350 5750 3600 5750
Wire Wire Line
	3600 5650 3350 5650
Wire Wire Line
	3350 5550 3600 5550
Wire Wire Line
	3600 5450 3350 5450
Wire Wire Line
	3350 5250 3600 5250
Wire Wire Line
	3350 5350 3550 5350
Wire Wire Line
	3450 5150 3450 6250
Wire Wire Line
	3350 5150 3450 5150
Wire Wire Line
	3450 5050 3350 5050
Wire Wire Line
	3450 4950 3450 5050
Wire Wire Line
	2450 6150 2100 6150
Wire Wire Line
	2450 6050 2100 6050
Wire Wire Line
	2100 5950 2450 5950
Wire Wire Line
	2450 5850 2100 5850
Wire Wire Line
	2100 5750 2450 5750
Wire Wire Line
	2450 5650 2100 5650
Wire Wire Line
	2450 5550 2100 5550
Wire Wire Line
	2100 5450 2450 5450
Wire Wire Line
	2450 5250 2100 5250
Wire Wire Line
	2100 5150 2450 5150
Wire Wire Line
	2450 5050 2100 5050
Wire Wire Line
	2450 5350 2350 5350
$Comp
L CustomLibraries_power:GND #PWR09
U 1 1 56D7539A
P 3300 4450
F 0 "#PWR09" H 3300 4200 50  0001 C CNN
F 1 "GND" H 3450 4400 50  0000 C CNN
F 2 "" H 3300 4450 50  0000 C CNN
F 3 "" H 3300 4450 50  0000 C CNN
	1    3300 4450
	1    0    0    -1  
$EndComp
Text Label 2750 4450 1    60   ~ 0
1(Tx)
Text Label 3150 6650 3    60   ~ 0
A6
Text Label 3050 6650 3    60   ~ 0
A7
Wire Notes Line
	1875 4100 3050 4100
Text Label 3600 5450 0    60   ~ 0
A3
Text Label 3600 5550 0    60   ~ 0
A2
Text Label 3600 5650 0    60   ~ 0
A1
Text Label 3600 5750 0    60   ~ 0
A0
Text Label 3600 5850 0    60   ~ 0
13
Text Label 3600 5950 0    60   ~ 0
12
Text Label 3600 6050 0    60   ~ 0
11
Text Label 3600 6150 0    60   ~ 0
10
Text Label 2100 6150 0    60   ~ 0
9
Text Label 2100 6050 0    60   ~ 0
8
Text Label 2100 5950 0    60   ~ 0
7
Text Label 2100 5850 0    60   ~ 0
6
Text Label 2100 5750 0    60   ~ 0
5
Text Label 2100 5650 0    60   ~ 0
4
Text Label 2100 5550 0    60   ~ 0
3
Text Label 2100 5450 0    60   ~ 0
2
Text Label 3600 5250 0    60   ~ 0
Reset
Text Label 2100 5250 0    60   ~ 0
Reset
Text Label 2100 5150 0    60   ~ 0
0(Rx)
Text Label 2100 5050 0    60   ~ 0
1(Tx)
Text Label 2850 4450 1    60   ~ 0
0(Rx)
Text Label 2650 4450 1    60   ~ 0
DTR
Text Label 3450 4950 1    60   ~ 0
RAW
Wire Notes Line
	8150 1600 4600 1600
Wire Notes Line
	8150 3450 8150 1600
Wire Notes Line
	4600 3450 8150 3450
$Comp
L CustomLibraries_connectors:Arduino_MiniPro U2
U 1 1 5EA809AC
P 2900 5550
F 0 "U2" H 2900 4950 50  0000 C CNN
F 1 "Arduino_MiniPro" H 2900 4850 50  0000 C CNN
F 2 "Custom_libraries:Socket_Strip_Arduino_MiniPro" H 2650 5550 50  0001 C CNN
F 3 "~" H 2650 5550 50  0001 C CNN
	1    2900 5550
	1    0    0    -1  
$EndComp
Text Notes 1900 4025 0    60   ~ 0
Shield Arduino Pro Mini \n
Wire Notes Line
	1850 6850 1850 3900
Wire Notes Line
	1850 3900 3950 3900
Wire Notes Line
	3950 6850 1850 6850
Wire Notes Line
	4200 6400 10100 6400
Wire Notes Line
	4100 2250 2800 2250
Wire Notes Line
	4100 3450 4100 2250
Wire Notes Line
	2800 3450 4100 3450
Wire Notes Line
	2800 2250 2800 3450
Text Notes 2850 2400 0    60   ~ 0
Radio modules
$Comp
L CustomLibraries_connectors:Conn_01x03 J7
U 1 1 5EA332C1
P 3450 2750
F 0 "J7" H 3530 2792 50  0000 L CNN
F 1 "Conn_01x03" H 3530 2701 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 3450 2750 50  0001 C CNN
F 3 "~" H 3450 2750 50  0001 C CNN
	1    3450 2750
	1    0    0    -1  
$EndComp
$Comp
L CustomLibraries_connectors:Conn_01x04 J6
U 1 1 5EA3215E
P 3450 3100
F 0 "J6" H 3530 3092 50  0000 L CNN
F 1 "Conn_01x04" H 3530 3001 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 3450 3100 50  0001 C CNN
F 3 "~" H 3450 3100 50  0001 C CNN
	1    3450 3100
	1    0    0    -1  
$EndComp
$Comp
L CustomLibraries_power:+3V3 #PWR05
U 1 1 5ECC7A24
P 2950 900
F 0 "#PWR05" H 2950 750 50  0001 C CNN
F 1 "+3V3" H 2965 1073 50  0000 C CNN
F 2 "" H 2950 900 50  0001 C CNN
F 3 "" H 2950 900 50  0001 C CNN
	1    2950 900 
	1    0    0    -1  
$EndComp
$Comp
L CustomLibraries_power:+3V3 #PWR08
U 1 1 5ECC88CE
P 2950 4450
F 0 "#PWR08" H 2950 4300 50  0001 C CNN
F 1 "+3V3" H 2965 4623 50  0000 C CNN
F 2 "" H 2950 4450 50  0001 C CNN
F 3 "" H 2950 4450 50  0001 C CNN
	1    2950 4450
	1    0    0    -1  
$EndComp
$Comp
L CustomLibraries_power:+3V3 #PWR011
U 1 1 5ECC932A
P 3550 4950
F 0 "#PWR011" H 3550 4800 50  0001 C CNN
F 1 "+3V3" H 3565 5123 50  0000 C CNN
F 2 "" H 3550 4950 50  0001 C CNN
F 3 "" H 3550 4950 50  0001 C CNN
	1    3550 4950
	1    0    0    -1  
$EndComp
Connection ~ 5750 4800
Connection ~ 5100 5400
Wire Wire Line
	5100 5400 5050 5400
Wire Wire Line
	5100 5400 6150 5400
Wire Wire Line
	5100 4900 5100 5000
Text Label 5900 5300 0    39   ~ 0
I2C_SDA
Text Label 5900 4900 0    39   ~ 0
I2C_SCL
Wire Wire Line
	6150 4900 5900 4900
Wire Wire Line
	6150 5300 5900 5300
Wire Wire Line
	5750 5450 5750 5200
Wire Wire Line
	5750 5200 6150 5200
Wire Wire Line
	5750 5750 5750 5800
Wire Wire Line
	5750 5800 5900 5800
Wire Wire Line
	5900 5900 5900 5800
Connection ~ 5900 5800
Wire Wire Line
	5900 5800 6050 5800
Text Label 8950 2600 0    39   ~ 0
I2C_SDA
Wire Wire Line
	2650 6600 2650 6700
Text Label 2750 6600 3    60   ~ 0
A4
Wire Wire Line
	2750 6600 2750 6800
Text Label 2650 6700 1    60   ~ 0
A5
Wire Wire Line
	2200 6700 2650 6700
Wire Wire Line
	2200 6800 2750 6800
Text Label 2250 6700 0    39   ~ 0
I2C_SCL
Text Label 2250 6800 0    39   ~ 0
I2C_SDA
$Comp
L CustomLibraries_connectors:Conn_01x03 J?
U 1 1 5F3D7AAB
P 9250 1100
F 0 "J?" H 9330 1142 50  0000 L CNN
F 1 "Conn_01x03" H 9330 1051 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 9250 1100 50  0001 C CNN
F 3 "~" H 9250 1100 50  0001 C CNN
	1    9250 1100
	1    0    0    -1  
$EndComp
Wire Notes Line
	8750 1550 9900 1550
Wire Notes Line
	9900 1550 9900 600 
Wire Notes Line
	9900 600  8750 600 
Wire Notes Line
	8750 600  8750 1550
Text Notes 8800 700  0    39   ~ 0
Lamp button
Wire Wire Line
	6150 5000 5900 5000
Wire Wire Line
	6150 5100 5900 5100
Wire Wire Line
	8100 5500 8350 5500
Wire Wire Line
	8100 5100 8350 5100
Text Label 5900 5000 0    39   ~ 0
GPIO12
Text Label 5900 5100 0    39   ~ 0
GPIO13
Text Label 8150 5500 0    39   ~ 0
GPIO4
Text Label 8150 5100 0    39   ~ 0
GPIO5
Text Label 4750 2350 0    39   ~ 0
GPIO4
Text Label 5600 2350 0    39   ~ 0
GPIO5
Text Label 6450 2350 0    39   ~ 0
GPIO12
Text Label 7300 2350 0    39   ~ 0
GPIO13
$Comp
L CustomLibraries_connectors:Conn_01x02 J?
U 1 1 5F401437
P 8650 4700
F 0 "J?" H 8600 4400 50  0000 C CNN
F 1 "Conn_01x02" H 8800 4500 50  0000 C CNN
F 2 "" H 8650 4700 50  0001 C CNN
F 3 "~" H 8650 4700 50  0001 C CNN
	1    8650 4700
	-1   0    0    1   
$EndComp
Wire Wire Line
	8850 4700 8850 5000
Connection ~ 8850 5000
Wire Wire Line
	8850 5000 9300 5000
Text Label 8850 4550 0    39   ~ 0
Reset
Wire Wire Line
	8850 4600 8850 4500
$EndSCHEMATC
