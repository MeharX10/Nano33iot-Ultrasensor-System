# Nano33IoT - Ultrasensor-System
A compact, interrupt-driven sensor system for the Arduino Nano 33 IoT. It reads distance (HC-SR04), ambient light (BH1750) and a slide potentiometer threshold, and actuates LEDs + a buzzer. Designed as a single-file, assessor-ready project with timer and external-interrupt logic.

# Overview
This project implements a Sense → Think → Act system on an Arduino Nano 33 IoT.
Sensors: HC-SR04 ultrasonic (distance), BH1750 light sensor (I²C), slide potentiometer (threshold).
Actuators: Red LED, Green LED, Buzzer.
Control: HC-SR04 echo via external interrupt (D2); Periodic sampling via hardware timer ISR (TC3, 500 ms). All logic is non-blocking and contained in one file TaskM1.cpp.

# Repository contents
• TaskM1.cpp — Final Arduino sketch (single file).
• Reflection_Report.md — 300–500 word reflection describing architecture, interrupts, and issues/resolutions.
• circuit_diagram.png — annotated breadboard/schematic image for wiring.
• README.md — this file.
• serial_sample.png — screenshot of Serial Monitor showing sensor and timer events.

# Key features 
• External interrupt: HC-SR04 echo captured using attachInterrupt() on D2 (rising/falling edges).
• Hardware timer ISR: SAMD TC3 via SAMDTimerInterrupt library triggers sampling every 500 ms.
• Separation of logic: ISRs only set flags; main loop performs sensor reads, processing and actuation.
• Multiple sensors + actuators: HC-SR04, BH1750, potentiometer; red/green LEDs and buzzer.
• Robustness: distance values <3 cm or >400 cm are clamped/ignored; pot readings are smoothed; I²C sensor handled in continuous mode.
• Single-file delivery: everything required to run the system is in TaskM1.cpp.

# Hardware (components)
• Arduino Nano 33 IoT (SAMD21)
• HC-SR04 ultrasonic distance sensor
• BH1750 light intensity sensor (I²C breakout)
• Keyestudio slide potentiometer module (use correct wiper OTA/OTB)
• LEDs: red, green (220 Ω resistors each)
• Active buzzer (or buzzer + transistor if high current)
• Breadboard, jumper wires, power from Nano 3.3 V rail

# Pin mapping
# _Power & Ground_
• Nano 3.3V → breadboard + rail (use for sensors and pot)
• Nano GND → breadboard − rail

# _HC-SR04_
• VCC → 3.3V
• GND → GND
• TRIG → D3
• ECHO → D2 (external interrupt)

# _BH1750 (I²C)_
• VCC → 3.3V
• GND → GND
• SDA → A4
• SCL → A5
• AD0/ADDR → leave floating or tie to GND (default 0x23)

# _Potentiometer_
• VCC → 3.3V
• GND → GND
• Wiper → A0 (confirm OTA/OTB; test with simple analogRead sketch)

# _Actuators_
• Red LED anode → D8 → (220 Ω) → GND
• Green LED anode → D9 → (220 Ω) → GND
• Buzzer + → D10, − → GND (add resistor or transistor if needed)

# Software: install & upload
• Install Arduino IDE (prefer 1.8+ or Arduino IDE 2.x).
• In Library Manager, install:
  - BH1750 (Christopher Laws or equivalent)
  - SAMD TimerInterrupt (by Khoi Hoang) — ensure samd support (v1.10.1+)
• Open TaskM1.cpp in the IDE. Select Arduino Nano 33 IoT board and correct port.
• Upload. Open Serial Monitor at 115200 baud.
