/*
  ESP32-C6 BLE Gamepad (28 buttons + 2 analog axes)
  ─────────────────────────────────────────────────────
  ✦ 24 buttons via 3×74HC165
  ✦ 4 push buttons on GPIO 21 / 17 / 18 / 19 (INPUT_PULLUP)
  ✦ 2 Hall-sensor paddles on ADC1_CH0 & CH1 → X / Y axes

  – Uses ESP32-BLE-Gamepad library (auto-report left ON)
  – Sends a HID report *only* when:
       • a button state toggles
       • an axis moves by ≥ DEADBAND counts (8-bit)
*/

#include <Arduino.h>
#include <BleGamepad.h>
#include <BleGamepadConfiguration.h>

// ── Pins ──────────────────────────────────────────────────────────
#define PIN_165_MISO    10
#define PIN_SHARED_CLK  11
#define PIN_165_PL      1
#define PIN_595_SER     23
#define PIN_595_RCLK    20

#define PIN_BTN_A       21
#define PIN_BTN_B       17
#define PIN_BTN_C       18
#define PIN_BTN_D       19

#define PIN_PADDLE_X     2      // ADC1_CH0
#define PIN_PADDLE_Y     3      // ADC1_CH1

static const int NUM_165       = 3;
static const int NUM_PUSHBTN   = 4;
static const int TOTAL_BUTTONS = NUM_165 * 8 + NUM_PUSHBTN;

// ── Axis calibration (raw 0-4095 → 0-255) ────────────────────────
const int calMinX = 0, calMaxX = 4095;
const int calMinY = 0, calMaxY = 4095;
const uint8_t DEADBAND = 3;          // 0-255 LSBs to ignore

// ── BLE gamepad ───────────────────────────────────────────────────
BleGamepadConfiguration config;      // autoReport defaults to true
BleGamepad bleGamepad("FW707", "Evenracing", 100);

// ── State buffers ────────────────────────────────────────────────
bool     prevBits[TOTAL_BUTTONS] = {false};
uint8_t  prevX = 0, prevY = 0;

// ── Helpers ───────────────────────────────────────────────────────
void readAll24Bits(uint8_t outBits[NUM_165 * 8])
{
  digitalWrite(PIN_165_PL, LOW);      delayMicroseconds(1);
  digitalWrite(PIN_165_PL, HIGH);     delayMicroseconds(1);

  for (int i = 0; i < NUM_165 * 8; ++i) {
    digitalWrite(PIN_SHARED_CLK, LOW);  delayMicroseconds(1);
    digitalWrite(PIN_SHARED_CLK, HIGH); delayMicroseconds(1);
    outBits[i] = digitalRead(PIN_165_MISO);
  }

  digitalWrite(PIN_595_RCLK, HIGH);   delayMicroseconds(1);
  digitalWrite(PIN_595_RCLK, LOW);
}

void readAllButtons(uint8_t outBits[TOTAL_BUTTONS])
{
  readAll24Bits(outBits);
  outBits[24] = (digitalRead(PIN_BTN_A) == LOW);
  outBits[25] = (digitalRead(PIN_BTN_B) == LOW);
  outBits[26] = (digitalRead(PIN_BTN_C) == LOW);
  outBits[27] = (digitalRead(PIN_BTN_D) == LOW);
}

// ── Arduino setup ────────────────────────────────────────────────
void setup()
{
  Serial.begin(115200);
  delay(5);
  Serial.println(">> ESP32-C6 BLE Gamepad – lite loop");

  pinMode(PIN_165_MISO, INPUT);
  pinMode(PIN_SHARED_CLK, OUTPUT);
  pinMode(PIN_165_PL, OUTPUT);
  pinMode(PIN_595_SER, OUTPUT);
  pinMode(PIN_595_RCLK, OUTPUT);
  digitalWrite(PIN_SHARED_CLK, LOW);
  digitalWrite(PIN_165_PL, HIGH);
  digitalWrite(PIN_595_SER, LOW);
  digitalWrite(PIN_595_RCLK, LOW);

  pinMode(PIN_BTN_A, INPUT_PULLUP);
  pinMode(PIN_BTN_B, INPUT_PULLUP);
  pinMode(PIN_BTN_C, INPUT_PULLUP);
  pinMode(PIN_BTN_D, INPUT_PULLUP);

  config.setButtonCount(TOTAL_BUTTONS);
  config.setHatSwitchCount(0);        // no HATs
  bleGamepad.begin(&config);

  Serial.println(">> advertising – connect from host…");
}

// ── Main loop ────────────────────────────────────────────────────
void loop()
{
  if (!bleGamepad.isConnected()) {
    vTaskDelay(pdMS_TO_TICKS(10));
    return;
  }

  /* 1. scan buttons ------------------------------------------------ */
  uint8_t bits[TOTAL_BUTTONS];
  readAllButtons(bits);
  for (uint8_t i = 0; i < TOTAL_BUTTONS; ++i) {
    if (bits[i] != prevBits[i]) {
      bits[i] ? bleGamepad.press(i + 1)
              : bleGamepad.release(i + 1);
      prevBits[i] = bits[i];          // auto-report fires here
    }
  }

  /* 2. scan axes (Hall paddles) ------------------------------------ */
  uint8_t curX = map(analogRead(PIN_PADDLE_X), calMinX, calMaxX, 0, 255);
  uint8_t curY = map(analogRead(PIN_PADDLE_Y), calMinY, calMaxY, 0, 255);
  if (abs((int)curX - prevX) > DEADBAND ||
      abs((int)curY - prevY) > DEADBAND) {
    bleGamepad.setAxes(curX, curY, 0, 0, 0, 0);   // auto-report fires here
    prevX = curX;  prevY = curY;
  }

  /* 3. brief idle so FreeRTOS & NimBLE can breathe ----------------- */
  vTaskDelay(pdMS_TO_TICKS(1));
}
