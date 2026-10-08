# Project CLAW — Circuit Connection Guide

This document outlines the complete hardware wiring and circuit connections for **Project CLAW** (Simplified configuration without inline DC fuse and without physical limit/micro switches).

---

## 1. Hardware Component List

| Component | Qty | Role in System |
|---|:---:|---|
| **ESP8266 NodeMCU** | 1 | Microcontroller (PWM signal generation, Wi-Fi web control) |
| **BTS7960 (IBT-2) Motor Driver** | 2 | High-current H-bridge drivers for Motor 1 and Motor 2 |
| **12 V Johnson DC Geared Motor (100 RPM)** | 2 | Actuators: Motor 1 (Contraction) & Motor 2 (Retraction) |
| **12 V Rechargeable Battery** | 1 | Main system power source |
| **12 V → 5 V Step-Down Buck Converter** | 1 | Powers NodeMCU logic and BTS7960 driver logic |
| **Main / Emergency Kill Switch** | 1 | Manual physical cutoff for the entire 12 V power line |

---

## 2. Power Architecture Diagram

```text
       [ 12V Battery (+) ]
                │
         [ Main Switch ]
                │
       12V Positive Rail
       ├───> BTS7960 #1 [ B+ / VCC ] (High-Power terminal)
       ├───> BTS7960 #2 [ B+ / VCC ] (High-Power terminal)
       └───> Buck Converter [ IN+ ]
                    │
            [ 12V → 5V Buck ]
                    │
             +5V Logic Rail
             ├───> ESP8266 NodeMCU [ VIN / 5V ]
             ├───> BTS7960 #1 [ VCC ] (8-pin header pin 7)
             ├───> BTS7960 #1 [ R_EN & L_EN ] (8-pin header pins 3 & 4)
             ├───> BTS7960 #2 [ VCC ] (8-pin header pin 7)
             └───> BTS7960 #2 [ R_EN & L_EN ] (8-pin header pins 3 & 4)

──────────────── COMMON GROUND BUS (GND) ────────────────
       ├───> 12V Battery (-)
       ├───> BTS7960 #1 [ B- / GND ] (High-Power screw terminal)
       ├───> BTS7960 #1 [ GND ] (8-pin header pin 8)
       ├───> BTS7960 #2 [ B- / GND ] (High-Power screw terminal)
       ├───> BTS7960 #2 [ GND ] (8-pin header pin 8)
       ├───> Buck Converter [ IN- ] & [ OUT- ]
       └───> ESP8266 NodeMCU [ GND ]
```

---

## 3. Detailed Pin-by-Pin Wiring Tables

### A. Power Supply & Buck Converter

| Source Pin | Destination Component | Destination Pin | Description |
|---|---|---|---|
| **Battery (+)** | Main / Kill Switch | Input Terminal | Main 12 V battery feed |
| **Main Switch Out** | BTS7960 #1 | **B+ / VCC** | 12 V high-power motor rail |
| **Main Switch Out** | BTS7960 #2 | **B+ / VCC** | 12 V high-power motor rail |
| **Main Switch Out** | Buck Converter | **IN (+)** | 12 V step-down input |
| **Battery (-)** | Common Ground Bus | **GND Rail** | System reference ground |
| **Buck OUT (+)** | ESP8266 NodeMCU | **VIN** (or **5V**) | Regulated 5 V logic power |
| **Buck OUT (+)** | BTS7960 #1 & #2 | **VCC** (Pin 7) | 5 V logic supply for drivers |
| **Buck OUT (+)** | BTS7960 #1 & #2 | **R_EN** & **L_EN** | Tied HIGH to enable bridges |
| **Buck OUT (-)** | Common Ground Bus | **GND Rail** | Tied to shared ground |

---

### B. ESP8266 NodeMCU Pin Assignment

| ESP8266 Pin | GPIO | Signal Direction | Connected To | Function |
|:---:|:---:|:---:|---|---|
| **VIN** / **5V** | — | Power IN | Buck Converter OUT (+) | 5 V supply to onboard 3.3 V regulator |
| **GND** | — | Power | Common Ground Bus | System Ground |
| **D1** | GPIO5 | Output (PWM) | BTS7960 #1 — **RPWM** | Motor 1 (Contraction) Clockwise / Pull |
| **D2** | GPIO4 | Output (PWM) | BTS7960 #1 — **LPWM** | Motor 1 (Contraction) Counter-Clockwise / Release |
| **D5** | GPIO14 | Output (PWM) | BTS7960 #2 — **RPWM** | Motor 2 (Retraction) Clockwise / Pull |
| **D6** | GPIO12 | Output (PWM) | BTS7960 #2 — **LPWM** | Motor 2 (Retraction) Counter-Clockwise / Release |

*Note: Pins D1, D2, D5, and D6 boot safely without interfering with ESP8266 boot-strap pins (GPIO0, GPIO2, GPIO15).*

---

### C. BTS7960 #1 — Contraction Motor Driver

Controls **Motor 1** (Upper motor pulling the **Red Cable** network).

| Connector | Module Label | Connected To | Wire Type / Description |
|---|---|---|---|
| **Screw Terminal** | **B+ / VCC** | Main Switch Output | Thick wire (16–18 AWG) — 12 V Battery (+) |
| **Screw Terminal** | **B- / GND** | Common Ground Bus | Thick wire (16–18 AWG) — 12 V Battery (-) |
| **Screw Terminal** | **M+ / OUT1** | Johnson Motor 1 (+) | Motor Lead 1 |
| **Screw Terminal** | **M- / OUT2** | Johnson Motor 1 (-) | Motor Lead 2 |
| **8-Pin Header (Pin 1)** | **RPWM** | ESP8266 **D1** (GPIO5) | Forward PWM drive |
| **8-Pin Header (Pin 2)** | **LPWM** | ESP8266 **D2** (GPIO4) | Reverse PWM drive |
| **8-Pin Header (Pin 3)** | **R_EN** | Buck Converter **+5 V** | Jumpered to VCC (Always Enabled) |
| **8-Pin Header (Pin 4)** | **L_EN** | Buck Converter **+5 V** | Jumpered to VCC (Always Enabled) |
| **8-Pin Header (Pin 5)** | **R_IS** | *Unconnected* | Current alarm (leave floating) |
| **8-Pin Header (Pin 6)** | **L_IS** | *Unconnected* | Current alarm (leave floating) |
| **8-Pin Header (Pin 7)** | **VCC** | Buck Converter **+5 V** | Driver logic power (+5 V) |
| **8-Pin Header (Pin 8)** | **GND** | Common Ground Bus | Driver logic ground |

---

### D. BTS7960 #2 — Retraction Motor Driver

Controls **Motor 2** (Lower motor pulling the **Blue Cable** network).

| Connector | Module Label | Connected To | Wire Type / Description |
|---|---|---|---|
| **Screw Terminal** | **B+ / VCC** | Main Switch Output | Thick wire (16–18 AWG) — 12 V Battery (+) |
| **Screw Terminal** | **B- / GND** | Common Ground Bus | Thick wire (16–18 AWG) — 12 V Battery (-) |
| **Screw Terminal** | **M+ / OUT1** | Johnson Motor 2 (+) | Motor Lead 1 |
| **Screw Terminal** | **M- / OUT2** | Johnson Motor 2 (-) | Motor Lead 2 |
| **8-Pin Header (Pin 1)** | **RPWM** | ESP8266 **D5** (GPIO14) | Forward PWM drive |
| **8-Pin Header (Pin 2)** | **LPWM** | ESP8266 **D6** (GPIO12) | Reverse PWM drive |
| **8-Pin Header (Pin 3)** | **R_EN** | Buck Converter **+5 V** | Jumpered to VCC (Always Enabled) |
| **8-Pin Header (Pin 4)** | **L_EN** | Buck Converter **+5 V** | Jumpered to VCC (Always Enabled) |
| **8-Pin Header (Pin 5)** | **R_IS** | *Unconnected* | Current alarm (leave floating) |
| **8-Pin Header (Pin 6)** | **L_IS** | *Unconnected* | Current alarm (leave floating) |
| **8-Pin Header (Pin 7)** | **VCC** | Buck Converter **+5 V** | Driver logic power (+5 V) |
| **8-Pin Header (Pin 8)** | **GND** | Common Ground Bus | Driver logic ground |

---

## 4. Software Safety Rules (Since Limit Switches Are Removed)

Because physical end-of-travel limit switches have been omitted:

1. **Strict Runtime Timeouts:**
   - Every motor deployment and retraction routine **must** be governed by a hard-coded time limit (e.g., maximum run time of 2.5 to 3.5 seconds depending on cable spool travel).
   - Once the timeout expires, the firmware must set both `RPWM` and `LPWM` to `0`.
2. **Mutual Exclusion:**
   - Motor 1 and Motor 2 must **never** run simultaneously in opposing directions. Software state logic must verify that Motor 1 has completely stopped before Motor 2 can engage.
3. **Manual Physical Kill Switch is Mandatory:**
   - Keep the main mechanical switch within easy reach of the wearer so power to the 12 V rail can be instantly cut if cables bind or jam.

---

## 5. Wiring Best Practices

- **Wire Gauges:**
  - **12 V Motor Power Lines (Battery, Switch, B+, B-, M+, M-):** Use **16 AWG to 18 AWG** stranded wire to handle peak current and stall spikes safely.
  - **Logic & Signal Lines (ESP8266, RPWM, LPWM, VCC, GND):** Standard **22 AWG to 26 AWG** jumper wires.
- **Common Ground Requirement:**
  - Ensure all negative points connect together. A floating ground between the ESP8266 and BTS7960 will cause erratic motor behavior or failed switching.
- **Pre-Flight Voltage Check:**
  - Before plugging the ESP8266 into the buck converter, verify with a multimeter that the buck output is calibrated precisely to **5.0 V**.
