/*
    Ultra-Sensor System
  - HC‑SR04 (external interrupt on D2 + trigger D3)
  - BH1750 (I2C on A4/A5, VCC=3.3V)
  - Slide Potentiometer → A0
  - Red LED (D8), Green LED (D9), Buzzer (D10)
  - Sampling via hardware timer ISR every 500 ms
  - Distance >400 cm or <3 cm is marked invalid
  - Threshold smoothed with low-pass filter
*/

#include <Wire.h>
#include <BH1750.h>
#define USING_TIMER_TC3 true
#include <SAMDTimerInterrupt.h>

const uint8_t TRIG_PIN    = 3;
const uint8_t ECHO_PIN    = 2;
const uint8_t POT_PIN     = A0;
const uint8_t LED_RED_PIN = 8;
const uint8_t LED_GREEN_PIN = 9;
const uint8_t BUZZER_PIN  = 10;

volatile uint32_t echoStart = 0, echoEnd = 0;
volatile bool echoEvent = false, timerEvent = false;
float distanceCm = 0;
uint16_t potRaw = 0, luxVal = 0;
float thresholdDist = 0;
bool alertState = false;

static float thresholdFilt = 10.0;  // initial smoothed threshold
constexpr float ALPHA = 0.10;  // smoothing coefficient

BH1750 lightSensor;
SAMDTimer ITimer(TIMER_TC3);

void echoISR() {
  if (digitalRead(ECHO_PIN)) echoStart = micros();
  else {
    echoEnd = micros();
    echoEvent = true;
  }
}

void onTimerISR() { timerEvent = true; }

void setup() {
  Serial.begin(115200);
  Wire.begin();
  lightSensor.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);  

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(ECHO_PIN), echoISR, CHANGE);

  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  if (!ITimer.attachInterruptInterval(500, onTimerISR)) {
    Serial.println("! Timer Init Failed"); while (1);
  }
}

void loop() {
  // 1. Trigger ultrasonic pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // 2. Read echo if available
  if (echoEvent) {
    echoEvent = false;
    uint32_t dur = echoEnd - echoStart;
    distanceCm = (dur * 0.01715f);
    if (distanceCm < 3.0f || distanceCm > 400.0f) distanceCm = 0.0f;
  }

  // 3. 500 ms sensor sampling
  if (timerEvent) {
    timerEvent = false;
    potRaw = analogRead(POT_PIN);
    luxVal = lightSensor.readLightLevel();
    if (luxVal == 0xFFFF) luxVal = 0;  // fix sometimes invalid readings

    // Lamp threshold smoothing
    float instantThresh = 10.0 + (potRaw * 190.0f / 1023.0f);
    thresholdFilt = ALPHA * thresholdFilt + (1 - ALPHA) * instantThresh;
    thresholdDist = thresholdFilt;
  }

  // 4. Decision logic
  bool distAlert = (distanceCm > 0 && distanceCm <= thresholdDist);
  bool luxAlert = (luxVal > 0 && luxVal <= 20);
  alertState = distAlert || luxAlert;

  // 5. Actuation LEDs + buzzer
  if (alertState) {
    digitalWrite(LED_RED_PIN, HIGH);
    digitalWrite(LED_GREEN_PIN, LOW);
    tone(BUZZER_PIN, 1500);
  } else {
    digitalWrite(LED_RED_PIN, LOW);
    digitalWrite(LED_GREEN_PIN, HIGH);
    noTone(BUZZER_PIN);
  }

  // 6. Serial output once every loop (since views)
  Serial.print("Dist: ");
  Serial.print(distanceCm, 1);
  Serial.print(" cm | Lux: ");
  Serial.print(luxVal);
  Serial.print(" | Thresh: ");
  Serial.print(thresholdDist, 1);
  Serial.print(" cm | Alert: ");
  Serial.println(alertState ? "YES" : "no");

  delay(20);  // small pause to avoid flooding
}
