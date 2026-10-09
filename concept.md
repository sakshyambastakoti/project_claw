# Project CLAW — Concept Document

## 1. Project Overview

**Project name:** Project CLAW  
**Type:** Wearable robotic / animatronic mechanism for a fancy-dress competition  
**Controller:** ESP8266 NodeMCU  
**Actuators:** 2 × 12 V, 100 RPM Johnson geared DC motors  
**Motor drivers:** 2 × BTS7960 high-current H-bridge modules  
**Main power:** 12 V rechargeable battery  
**Logic power:** 12 V → 5 V buck converter

Project CLAW is a wearable mechanical system that deploys and retracts **six pipe-based mechanical claws/arms** using two independent cable mechanisms. The goal is a visually impressive robotic costume while keeping the mechanism simple, controllable, repairable, and safe.

## 2. Physical Architecture

The central mechanical chassis is approximately:

- Central plywood frame width: **6 cm**
- Main central mechanism height: approximately **36 cm**
- Upper section: approximately **3 cm**
- Main middle section: approximately **30 cm**
- Lower section: approximately **3 cm**
- Six horizontal pipe/arm assemblies extend from the central frame.
- Three pipe levels/pairs give six total claw/arm assemblies.

The uploaded mechanical drawing is the primary mechanical reference.

### Drawing legend

- **Grey:** central plywood chassis
- **Yellow:** six pipe/arm assemblies
- **Red:** contraction cable system
- **Blue:** retraction cable system
- **Green:** cable pulley/guide points
- **Pink:** motors
- Upper motor: contraction mechanism
- Lower motor: retraction mechanism

The exact pipe length, diameter, cable material, joint geometry, spool diameter, and pulley dimensions are parameters to be finalized during prototyping.

## 3. Actuation Concept

There are two independent motor-driven cable systems.

### Motor 1 — Contraction / Deployment

The upper 12 V 100 RPM Johnson motor drives the **red cable system** through a spool/drum. It pulls the contraction cables and produces the desired movement of the six pipe/claw assemblies.

### Motor 2 — Retraction

The lower 12 V 100 RPM Johnson motor drives the **blue cable system** through a spool/drum. It pulls the second cable network and returns/retracts the six pipe/claw assemblies.

### Automated 6-Step Cycle with Mathematical Homing to 0

The system operates an automated bidirectional cycle with mathematical homing:
- **Initial Condition:** Both Motor 1 and Motor 2 start at initial 0 rotation.
- **Step 1:** Motor 1 turns Clockwise for $x$ seconds (`Rotation Time`). Motor 2 is stopped.
- **Step 2:** Motor 1 turns Anticlockwise for $x$ seconds (`Rotation Time`). Motor 2 is stopped.
- **Step 3:** All rotation stops for $y$ seconds (`Pause Interval`). Both motors stopped at 0 rotation.
- **Step 4:** Motor 2 turns Clockwise for $x$ seconds (`Rotation Time`). Motor 1 is stopped.
- **Step 5:** Motor 2 turns Anticlockwise for $x$ seconds (`Rotation Time`). Motor 1 is stopped.
- **Step 6:** Loops automatically back to Step 1 as long as the master toggle is ON.

#### Mathematical Homing on Toggle OFF:
When the user switches the toggle OFF at any arbitrary point in the cycle:
1. **If during Step 1 (M1 CW for elapsed time $t \le x$):** Motor 1 has displaced by $+t$ (CW). The controller reverses Motor 1 (CCW) for exactly $t$ seconds, bringing Motor 1 back to 0 rotation.
2. **If during Step 2 (M1 CCW for elapsed time $t \le x$):** Motor 1 already went $+x$ in Step 1 and has reversed by $-t$. The remaining distance to 0 is $(x - t)$ in the CCW direction. The controller continues rotating CCW for $(x - t)$ seconds until 0 is reached.
3. **If during Step 3 (Pause $y$):** Both motors are already at 0 rotation; the controller stops immediately.
4. **If during Step 4 (M2 CW for elapsed time $t \le x$):** Motor 2 has displaced by $+t$ (CW). The controller reverses Motor 2 (CCW) for exactly $t$ seconds, bringing Motor 2 back to 0 rotation.
5. **If during Step 5 (M2 CCW for elapsed time $t \le x$):** Motor 2 already went $+x$ in Step 4 and has reversed by $-t$. The remaining distance to 0 is $(x - t)$ in the CCW direction. The controller continues rotating CCW for $(x - t)$ seconds until 0 is reached.

#### Non-Volatile Memory (Flash EEPROM):
Parameters $x$ (`Rotation Time`), $y$ (`Pause Interval`), and Motor Speed (PWM) are stored in the ESP8266 flash EEPROM (address 320) so they persist across power loss.

## 4. Electronics

Main components:

1. ESP8266 NodeMCU
2. 2 × BTS7960 motor-driver modules
3. 2 × 12 V 100 RPM Johnson geared DC motors
4. 12 V rechargeable battery
5. 12 V → 5 V buck converter
6. Limit/micro switches
7. Main power switch
8. Emergency motor kill switch
9. Fuse and holder
10. Push buttons
11. PCB/perfboard
12. Wiring and connectors

### Power architecture

```text
12V Battery
     |
     +---- Fuse ---- Main/Kill Switch ----+---- BTS7960 #1 ---- Motor 1
                                           |
                                           +---- BTS7960 #2 ---- Motor 2
                                           |
                                           +---- 12V→5V Buck ---- ESP8266
                                                               |
                                                               +-- BTS7960 logic VCC
```

The ESP8266 must **never** receive 12 V directly. Use a regulated 5 V supply for its VIN/5V input as appropriate for the exact NodeMCU board.

All logic grounds must share a common ground.

## 5. Proposed ESP8266 Pin Assignment

Initial pin map:

| ESP8266 | GPIO | Function |
|---|---:|---|
| D1 | GPIO5 | BTS7960 #1 RPWM |
| D2 | GPIO4 | BTS7960 #1 LPWM |
| D5 | GPIO14 | BTS7960 #2 RPWM |
| D6 | GPIO12 | BTS7960 #2 LPWM |
| D7 | GPIO13 | Limit/control input |
| D0 | GPIO16 | Limit/control input where appropriate |

Verify the exact ESP8266 board pinout before final wiring.

## 6. BTS7960 Connections

One BTS7960 controls one motor.

### BTS7960 #1 — Contraction

- RPWM ← ESP8266 D1 / GPIO5
- LPWM ← ESP8266 D2 / GPIO4
- VCC ← regulated 5 V
- GND ← common ground
- R_EN/L_EN → enabled according to the exact module
- R_IS/L_IS → not required for first prototype
- B+ / B− → 12 V motor supply
- M+ / M− → Motor 1

### BTS7960 #2 — Retraction

- RPWM ← ESP8266 D5 / GPIO14
- LPWM ← ESP8266 D6 / GPIO12
- VCC ← regulated 5 V
- GND ← common ground
- R_EN/L_EN → enabled according to the exact module
- R_IS/L_IS → not required for first prototype
- B+ / B− → 12 V motor supply
- M+ / M− → Motor 2

Always verify the pin labels and electrical requirements of the exact BTS7960 module.

## 7. Safety

Because this is wearable, safety is a primary requirement.

### Hardware emergency stop
A physical emergency/kill switch must disconnect motor power independently of software.

### Fuse
Install a suitable fuse close to the battery positive terminal. Fuse selection must be based on measured motor operating/stall current and wiring rating.

### Limit switches
Recommended minimum:

- Motor 1: two end-position switches
- Motor 2: two end-position switches

The exact locations depend on the final mechanical motion.

### Mechanical safety

- Use blunt/lightweight claw ends.
- Limit travel mechanically.
- Protect rotating shafts and pinch points.
- Keep cables away from the wearer.
- Provide a manual release where practical.
- Do not rely only on software for safety.

## 8. Control Modes

### Manual Deploy
Run the appropriate motor until its limit switch, timeout, or emergency stop occurs.

### Manual Retract
Run the appropriate motor until its limit switch, timeout, or emergency stop occurs.

### Demo Mode

Suggested sequence:

1. Wait for trigger.
2. Activate status lighting/buzzer if installed.
3. Deploy/contract six claws.
4. Hold.
5. Retract six claws.
6. Return to idle.

The timing must be configurable.

## 9. Firmware Requirements

Use Arduino-compatible C++ for ESP8266 unless another framework is explicitly selected.

Prefer a **non-blocking state machine** over long `delay()` calls.

Suggested states:

```text
IDLE
DEPLOYING
DEPLOYED
RETRACTING
RETRACTED
DEMO
FAULT
EMERGENCY_STOP
```

Suggested functions:

```cpp
setMotor1(int direction, int pwm);
stopMotor1();

setMotor2(int direction, int pwm);
stopMotor2();

stopAllMotors();
```

Do not drive both RPWM and LPWM of the same BTS7960 in conflicting states.

Every motor command needs a maximum runtime. If the expected limit switch is not reached:

```text
MOTOR STOP
    ↓
FAULT STATE
    ↓
manual reset required
```

## 10. Wi-Fi Control

The ESP8266 may provide a local Wi-Fi control page:

```text
PROJECT CLAW
----------------
[ DEPLOY ]
[ RETRACT ]
[ DEMO ]
[ STOP ]

Motor 1: IDLE
Motor 2: IDLE
System: READY
```

Wi-Fi must never be the only safety mechanism. Emergency stop and travel limits must remain hardware-based.

## 11. Development Plan

### Stage 1 — Electronics
Test ESP8266, both BTS7960s, and both motors without connecting the mechanism.

### Stage 2 — One cable
Test one motor and one spool. Measure cable travel, current, torque, movement time, and heating.

### Stage 3 — Six-claw mechanism
Connect all cable paths and balance cable tension.

### Stage 4 — Limit switches
Add and test physical end stops.

### Stage 5 — Wearable integration
Mount the reliable mechanism to the costume.

### Stage 6 — Effects
Add Wi-Fi, LEDs, sound, and automatic demo sequence.

## 12. Values That Must Be Measured

Do not invent these values:

- Motor stall current
- Normal operating current
- Motor torque
- Spool diameter
- Required cable travel
- Cable tension
- Pipe weight
- Claw weight
- Pulley friction
- Battery capacity
- Runtime
- BTS7960 temperature
- Buck-converter current capacity

Driver suitability must be determined using the actual motor stall current, not only the 12 V / 100 RPM rating.

## 13. Instructions for Any AI Working on Project CLAW

Treat this document and the uploaded mechanical drawing as the primary project specification.

1. Do not silently change the mechanical architecture.
2. Do not invent missing mechanical dimensions.
3. Clearly state assumptions.
4. Verify ESP8266 GPIO limitations and boot behavior.
5. Verify the exact BTS7960 module pinout before final wiring.
6. Never connect 12 V to an ESP8266 GPIO or 3.3 V pin.
7. Include common-ground requirements.
8. Include hardware emergency-stop behavior.
9. Include motor timeout protection.
10. Include limit-switch protection.
11. Prefer a non-blocking firmware state machine.
12. Do not claim a motor/driver is adequate without considering stall current.
13. Keep the first prototype simple and testable.
14. When providing code, include a clear pin map and configuration section.
15. When changing the design, explain exactly what changed and why.

## 14. Final System

```text
                    PROJECT CLAW

                 ┌──────────────┐
                 │   ESP8266    │
                 │  Controller  │
                 └──────┬───────┘
                        │
              ┌─────────┴─────────┐
              │                   │
        BTS7960 #1          BTS7960 #2
              │                   │
           Motor 1             Motor 2
        CONTRACTION            RETRACTION
              │                   │
          RED CABLE            BLUE CABLE
              │                   │
              └─────────┬─────────┘
                        │
                  SIX PIPE CLAWS
                        │
                   WEARABLE BODY

POWER:
12V Battery → Fuse → Kill Switch
       ├──→ BTS7960 #1 → Motor 1
       ├──→ BTS7960 #2 → Motor 2
       └──→ 12V→5V Buck → ESP8266 + driver logic
```

The target is a reliable, wearable, two-motor, six-claw mechanism with safe end-stop detection, manual control, optional Wi-Fi control, and a programmable demonstration sequence.
