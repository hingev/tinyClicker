EESchema Schematic File Version 4
EELAYER 29 0
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
$Comp
L MCU_Microchip_ATtiny:ATtiny85-20PU U1
U 1 1 5CE6A669
P 3400 3000
F 0 "U1" H 2871 3046 50  0000 R CNN
F 1 "ATtiny85-20PU" H 2871 2955 50  0000 R CNN
F 2 "Housings_DIP:DIP-8_W10.16mm" H 3400 3000 50  0001 C CIN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/atmel-2586-avr-8-bit-microcontroller-attiny25-attiny45-attiny85_datasheet.pdf" H 3400 3000 50  0001 C CNN
	1    3400 3000
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR0101
U 1 1 5CE6B5E8
P 3400 2300
F 0 "#PWR0101" H 3400 2150 50  0001 C CNN
F 1 "VCC" H 3417 2473 50  0000 C CNN
F 2 "" H 3400 2300 50  0001 C CNN
F 3 "" H 3400 2300 50  0001 C CNN
	1    3400 2300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 5CE6BBE9
P 3400 3700
F 0 "#PWR0102" H 3400 3450 50  0001 C CNN
F 1 "GND" H 3405 3527 50  0000 C CNN
F 2 "" H 3400 3700 50  0001 C CNN
F 3 "" H 3400 3700 50  0001 C CNN
	1    3400 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	3400 2300 3400 2400
Wire Wire Line
	3400 3600 3400 3700
$Comp
L Connector:Conn_01x02_Female J1
U 1 1 5CE6C3B6
P 1600 3000
F 0 "J1" H 1492 2675 50  0000 C CNN
F 1 "Conn_01x02_Female" H 1492 2766 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Angled_1x02_Pitch1.00mm" H 1600 3000 50  0001 C CNN
F 3 "~" H 1600 3000 50  0001 C CNN
	1    1600 3000
	-1   0    0    1   
$EndComp
$Comp
L power:VCC #PWR0103
U 1 1 5CE6CE68
P 1900 2800
F 0 "#PWR0103" H 1900 2650 50  0001 C CNN
F 1 "VCC" H 1917 2973 50  0000 C CNN
F 2 "" H 1900 2800 50  0001 C CNN
F 3 "" H 1900 2800 50  0001 C CNN
	1    1900 2800
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0104
U 1 1 5CE6D663
P 1900 3100
F 0 "#PWR0104" H 1900 2850 50  0001 C CNN
F 1 "GND" H 1905 2927 50  0000 C CNN
F 2 "" H 1900 3100 50  0001 C CNN
F 3 "" H 1900 3100 50  0001 C CNN
	1    1900 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	1800 2900 1900 2900
Wire Wire Line
	1900 2900 1900 2800
Wire Wire Line
	1900 3100 1900 3000
Wire Wire Line
	1900 3000 1800 3000
Text Notes 850  2950 0    50   ~ 0
4.5 V of the laser\n
$Comp
L LED:IR204A D1
U 1 1 5CE6E6DE
P 4700 2700
F 0 "D1" H 4650 2485 50  0000 C CNN
F 1 "IR204A" H 4650 2576 50  0000 C CNN
F 2 "LEDs:LED_D3.0mm_Horizontal_O1.27mm_Z6.0mm" H 4700 2875 50  0001 C CNN
F 3 "http://www.everlight.com/file/ProductFile/IR204-A.pdf" H 4650 2700 50  0001 C CNN
	1    4700 2700
	-1   0    0    1   
$EndComp
$Comp
L Device:R R1
U 1 1 5CE6FE4A
P 4200 2450
F 0 "R1" H 4270 2496 50  0000 L CNN
F 1 "10k" H 4270 2405 50  0000 L CNN
F 2 "Resistors_SMD:R_0805" V 4130 2450 50  0001 C CNN
F 3 "~" H 4200 2450 50  0001 C CNN
	1    4200 2450
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 2300 3400 2300
Connection ~ 3400 2300
Wire Wire Line
	4200 2600 4200 3200
Wire Wire Line
	4200 3200 4000 3200
Wire Wire Line
	4000 2800 4350 2800
Wire Wire Line
	4350 2800 4350 2700
Wire Wire Line
	4350 2700 4600 2700
$Comp
L Device:R R2
U 1 1 5CE7169A
P 5050 2900
F 0 "R2" H 5120 2946 50  0000 L CNN
F 1 "100" H 5120 2855 50  0000 L CNN
F 2 "Resistors_SMD:R_0805" V 4980 2900 50  0001 C CNN
F 3 "~" H 5050 2900 50  0001 C CNN
	1    5050 2900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0105
U 1 1 5CE71D52
P 5050 3550
F 0 "#PWR0105" H 5050 3300 50  0001 C CNN
F 1 "GND" H 5055 3377 50  0000 C CNN
F 2 "" H 5050 3550 50  0001 C CNN
F 3 "" H 5050 3550 50  0001 C CNN
	1    5050 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	5050 2700 4900 2700
$Comp
L Switch:SW_Push SW1
U 1 1 5CE72997
P 4550 3150
F 0 "SW1" H 4550 3435 50  0000 C CNN
F 1 "NEXT" H 4550 3344 50  0000 C CNN
F 2 "Buttons_Switches_THT:SW_PUSH_6mm_h4.3mm" H 4550 3350 50  0001 C CNN
F 3 "~" H 4550 3350 50  0001 C CNN
	1    4550 3150
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW2
U 1 1 5CE73AEC
P 4550 3350
F 0 "SW2" H 4550 3250 50  0000 C CNN
F 1 "PREV" H 4550 3150 50  0000 C CNN
F 2 "Buttons_Switches_THT:SW_PUSH_6mm_h4.3mm" H 4550 3550 50  0001 C CNN
F 3 "~" H 4550 3550 50  0001 C CNN
	1    4550 3350
	1    0    0    -1  
$EndComp
Wire Wire Line
	5050 2700 5050 2750
Wire Wire Line
	5050 3050 5050 3150
Wire Wire Line
	4750 3350 5050 3350
Connection ~ 5050 3350
Wire Wire Line
	5050 3350 5050 3550
Wire Wire Line
	4750 3150 5050 3150
Connection ~ 5050 3150
Wire Wire Line
	5050 3150 5050 3350
Wire Wire Line
	4000 2900 4350 2900
Wire Wire Line
	4350 2900 4350 3150
Wire Wire Line
	4000 3000 4300 3000
Wire Wire Line
	4300 3000 4300 3350
Wire Wire Line
	4300 3350 4350 3350
$EndSCHEMATC
