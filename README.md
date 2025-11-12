# DIY-F1-Wheel
ESP32-C6 BLE Gamepad (Logitech G29 PCB)

This project converts a Logitech G29 steering wheel PCB into a Bluetooth LE gamepad using an ESP32-C6.
It reads 28 buttons (24 via shift registers + 4 direct GPIO) and two analog Hall sensors, and exposes them as a HID joystick over BLE.

Features

28 total buttons

24 from 3 × 74HC165 parallel-in/serial-out shift registers

4 extra push buttons on GPIO 21 / 17 / 18 / 19 (INPUT_PULLUP)

2 analog axes (X/Y) from Hall-effect paddles on ADC1 channels 0 & 1

BLE HID Gamepad using ESP32-BLE-Gamepad

Sends reports only when:

A button toggles

An axis changes beyond a small deadband

Fully compatible with PC, macOS, Linux, Android

Hardware
Component	
ESP32-C6 Dev Board	Any variant (USB-C recommended)

Logitech G29 Wheel PCB	Contains 3 × 74HC165 shift registers for button scanning

3× 74HC165N	Already on the G29 board

2× Hall Sensors	For paddle axes (analog output 0–3.3 V)

4× Momentary Push Buttons	Connected directly to GPIO 21, 17, 18, 19

Wiring Diagram
Connections
ESP32-C6 Pin	G29 PCB / 74HC165 Signal	Direction	Description
GPIO 10	Q7 (Serial Out)	Input	Data from 74HC165 chain
GPIO 11	CP / CLK	Output	Shift clock shared across all 165s
GPIO 1	PL / LOAD	Output	Parallel-load latch (active LOW)
3.3 V	VCC	Power	Shared VCC for shift registers
GND	GND	—	Common ground

The three 74HC165 chips are daisy-chained (Q7 of one → SER of next).
All share PL and CLK lines from the ESP32.

Additional I/O:

ESP32-C6 Pin	Function
GPIO 21	Button A (pulled up)
GPIO 17	Button B
GPIO 18	Button C
GPIO 19	Button D
GPIO 2 (ADC1_CH0)	Paddle X axis
GPIO 3 (ADC1_CH1)	Paddle Y axis

Optional pins defined but unused for outputs (from original PCB):

ESP32-C6 Pin	Description
GPIO 23	74HC595 SER (data out placeholder)
GPIO 20	74HC595 RCLK (latch placeholder)
Schematic Overview
         ┌───────────────┐
         │   ESP32-C6    │
         │───────────────│
   3V3 ──┤ VCC       GND ├──── GND
 CLK  ───┤ GPIO11         │────> CLK → all 74HC165s
 LOAD ───┤ GPIO1          │────> PL  → all 74HC165s
 DATA ◀──┤ GPIO10         │<──── Q7  ← 165-1
         │                │
 BTN_A───┤ GPIO21         │
 BTN_B───┤ GPIO17         │
 BTN_C───┤ GPIO18         │
 BTN_D───┤ GPIO19         │
 PADDLEX─┤ GPIO2 (ADC1_0) │
 PADDLEY─┤ GPIO3 (ADC1_1) │
         └────────────────┘

 74HC165 #1  Q7 → 74HC165 #2 SER
 74HC165 #2  Q7 → 74HC165 #3 SER
 74HC165 #3  Q7 → ESP32-C6 GPIO10




🧩 Software Setup
1️⃣ Install Arduino IDE

Download from arduino.cc

Open File → Preferences, add this Boards Manager URL:

https://espressif.github.io/arduino-esp32/package_esp32_index.json

2️⃣ Install ESP32 Boards

Go to Tools → Board → Boards Manager

Search “ESP32” and install esp32 by Espressif Systems

Select board: ESP32-C6 Dev Module

3️⃣ Install Required Library

In Sketch → Include Library → Manage Libraries, search:

ESP32 BLE Gamepad


and install ESP32-BLE-Gamepad by lemmingDev.

4️⃣ Flash the Firmware

Open c6wheel.ino in Arduino IDE

Connect ESP32-C6 via USB-C

Select Tools → Port → your device

Click Upload

When flashed, the serial monitor (115200 baud) will show:

>> ESP32-C6 BLE Gamepad – lite loop
>> advertising – connect from host…

🧪 Usage

On your PC or phone, open Bluetooth and pair with device “FW707”

Buttons and axes appear as a standard HID joystick

Open a game controller tester (Windows: joy.cpl) to verify

Pressing buttons or moving paddles updates the HID report instantly

🛠️ Advanced Configuration

Deadband: adjust DEADBAND in code (default = 3 LSB)

Calibration: tune calMinX/Y and calMaxX/Y for your Hall sensors

Button count: controlled via

config.setButtonCount(TOTAL_BUTTONS);


BLE Name: editable in

BleGamepad bleGamepad("FW707", "Evenracing", 100);

🧾 License

feel free to modify or distribute.
Credit appreciated: Evenracing / FW707
