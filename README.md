#  ESP32-C6 BLE Gamepad (Logitech G29 PCB)

This project converts a **Logitech G29 steering wheel PCB** into a **Bluetooth LE gamepad** using an **ESP32-C6**.  
It reads 28 buttons (24 via shift registers + 4 direct GPIO) and two analog Hall sensors, and exposes them as a HID joystick over BLE.

---

##  Features
- **28 total buttons**
  - 24 from **3 × 74HC165** parallel-in/serial-out shift registers  
  - 4 extra push buttons on GPIO 21 / 17 / 18 / 19 (`INPUT_PULLUP`)
- **2 analog axes** (X/Y) from Hall-effect paddles on ADC1 channels 0 & 1  
- **BLE HID Gamepad** using [`ESP32-BLE-Gamepad`](https://github.com/lemmingDev/ESP32-BLE-Gamepad)  
- Sends reports only when:
  - A button toggles  
  - An axis changes beyond a small **deadband**  
- Fully compatible with PC, macOS, Linux, Android

---

##  Hardware

| Component | Notes |
|------------|-------|
| **ESP32-C6 Dev Board** | Any variant (USB-C recommended) |
| **Logitech G29 Wheel PCB** | Contains 3 × 74HC165 shift registers for button scanning |
| **3× 74HC165N** | Already on the G29 board |
| **2× Hall Sensors** | For paddle axes (analog output 0–3.3 V) |
| **4× Microswitches** | Connected directly to GPIO 21, 17, 18, 19, for up/down shift + optional extra buttons |
| **1x JST PH 7 pin connector like the 7 pin connectors in this kit https://www.amazon.ca/dp/B09D35FQN2?_encoding=UTF8&psc=1&ref_=cm_sw_r_cp_ud_dp_AAF8HJ26T9T95VD5EJKY and dupont wires crimped or whatever into the JST 7 pin connector.
---

##  The back of the pcb has one 7 pin connection and it is what we can use to connect to our microcontroller with a 7 pin jst ph connector and dupont wires. The layout is this of the 7 pin connector

Pin 1: Ground

Pin 2: Data from the 165

Pin 3: Data to the 595

Pin 4: Clock to both the 165 and 595.

Pin 5: Reset Clock to the 595

Pin 6: Parallel Load to the 165

Pin 7: VCC

The 165s control controls, and the 595s control lights, so really you only need 5 pins for a functional build including the 2 power pins always. Don't mix up the GND and 5v!

###  Connections

| ESP32-C6 Pin | G29 PCB / 74HC165 Signal | Direction | Description |
|---------------|--------------------------|------------|--------------|
| **GPIO 10** | Q7 (Serial Out) | Input | Data from 74HC165 chain | This is the SECOND PIN FROM THE TOP of the 7 pin JST PH Connector at the back of the G29 pcb.
| **GPIO 11** | CP / CLK | Output | Shift clock shared across all 165s | This is the FOURTH PIN FROM THE TOP of the 7 pin JST PH Connector at the back of the G29 pcb. 
| **GPIO 1** | PL / LOAD | Output | Parallel-load latch (active LOW) |This is the SIXTH PIN FROM THE TOP of the 7 pin JST PH Connector at the back of the G29 pcb.
| **3.3 V** | VCC | Power | Shared VCC for shift registers | This is the SEVENTH OR LAST PIN FROM THE TOP of the 7 pin JST PH Connector at the back of the G29 pcb.
| **GND** | GND | — | Common ground | This is the FIRST PIN FROM THE TOP of the 7 pin JST PH Connector at the back of the G29 pcb.


Additional I/O:

| ESP32-C6 Pin | Function |
|---------------|-----------|
| **GPIO 21** | Upshift 1|
| **GPIO 17** | Upshift 2|
| **GPIO 18** | Downshift 1 |
| **GPIO 19** | Downshift 2 |
| **GPIO 2 (ADC1_CH0)** | Clutch Paddle X axis |
| **GPIO 3 (ADC1_CH1)** | Clutch Paddle Y axis |

Optional pins defined but unused for outputs (from original PCB):

| ESP32-C6 Pin | Description |
|---------------|-------------|
| **GPIO 23** | 74HC595 SER (data out placeholder) |
| **GPIO 20** | 74HC595 RCLK (latch placeholder) |

These pins can in theory control the rpm lights onboard the PCB but I haven't coded for it yet.

---

## 🤌 Software Setup

### 1️⃣ Install Arduino IDE
- Download from [arduino.cc](https://www.arduino.cc/en/software)  
- Open **File → Preferences**, add this Boards Manager URL:  
  ```
  https://espressif.github.io/arduino-esp32/package_esp32_index.json
  ```

### 2️⃣ Install ESP32 Boards
- Go to **Tools → Board → Boards Manager**
- Search “ESP32” and install **esp32 by Espressif Systems**
- Select board: **ESP32-C6 Dev Module**

### 3️⃣ Install Required Library
- In **Sketch → Include Library → Manage Libraries**, search:  
  ```
  ESP32 BLE Gamepad
  ```
  and install **ESP32-BLE-Gamepad** by lemmingDev.

### 4️⃣ Flash the Firmware
1. Open `c6wheel.ino` in Arduino IDE  
2. Connect ESP32-C6 via USB-C  
3. Select **Tools → Port → your device**  
4. Click **Upload**

When flashed, the serial monitor (115200 baud) will show:
```
>> ESP32-C6 BLE Gamepad – lite loop
>> advertising – connect from host…
```

---

##  Usage
- On your PC or phone, open Bluetooth and pair with device **“FW707”**  
- Buttons and axes appear as a standard HID joystick  
- Open a game controller tester (Windows: *joy.cpl*) to verify  
- Pressing buttons or moving paddles updates the HID report instantly  

---

##  Advanced Configuration
- **Deadband:** adjust `DEADBAND` in code (default = 3 LSB)  
- **Calibration:** tune `calMinX/Y` and `calMaxX/Y` for your Hall sensors  
- **Button count:** controlled via  
  ```cpp
  config.setButtonCount(TOTAL_BUTTONS);
  ```
- **BLE Name:** editable in  
  ```cpp
  BleGamepad bleGamepad("FW707", "Evenracing", 100);
  ```
One other cool thing I did with the g29: Knowing what the 7 pins on the back of the g29 wheel pcb mean, I deduced which of the interior wires in the wheelbase were intended for 5v and GND, and wired them up to the prongs of the Kyostar QR I'm using. Then wired up the wheel-side Kyostar QR part to go from prongs to 5v and GND inside the wheel itself. So when I place the wheel on the steering column, it powers up. 
---

## 📾 License
MIT License – feel free to modify or distribute.  
Credit appreciated: **Evenracing / FW707**

