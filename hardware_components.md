# Project CLAW — Hardware Components List (Bill of Materials)

This document provides a comprehensive list of all required hardware, electrical, and mechanical components for **Project CLAW**, reflecting the simplified electronics setup (excluding physical limit switches and DC fuse).

---

## 1. Electronics & Power

| # | Item | Key Specifications | Qty | Role & Notes |
|:---:|---|---|:---:|---|
| 1 | **ESP8266 NodeMCU** | CP2102 / CH340, ESP-12E/F module, 3.3V logic, Wi-Fi enabled | 1 | Master controller; generates PWM signals for motor speed/direction and hosts local Wi-Fi control interface. |
| 2 | **BTS7960 (IBT-2) Motor Driver** | Dual half-bridge high-power module, up to 43 A peak, 5.5 V–27 V DC operating range | 2 | High-current H-bridges. Module #1 drives Contraction Motor; Module #2 drives Retraction Motor. |
| 3 | **Johnson Geared DC Motor** | 12 V DC, 100 RPM, high-torque geared motor, 6 mm D-shaft | 2 | Actuators. Motor 1 (Upper) winds contraction cables; Motor 2 (Lower) winds retraction cables. |
| 4 | **12 V Rechargeable Battery** | 12 V (3S Li-ion 11.1 V–12.6 V or 12 V LiFePO4 / SLA), 2200 mAh – 4000 mAh, ≥ 10 A continuous discharge | 1 | Main system power supply for motors and stepped-down logic. |
| 5 | **DC-DC Step-Down Buck Converter** | LM2596 or XL4015 adjustable module, Input: 12 V, Output: Regulated 5.0 V @ 2 A–3 A | 1 | Steps down 12 V battery voltage to clean 5.0 V for ESP8266 (`VIN`) and BTS7960 driver logic (`VCC`). |
| 6 | **Main / Emergency Kill Switch** | Heavy-duty toggle or rocker switch, rated ≥ 10 A at 12 V DC (SPST or DPST) | 1 | Physical master switch to immediately cut 12 V power to the motor drivers and electronics. |
| 7 | **Micro-USB Cable** | Standard USB-A to Micro-USB (data-capable) | 1 | Firmware flashing and initial USB serial debugging for ESP8266. |

---

## 2. Wiring, Connectors & Distribution

| # | Item | Key Specifications | Qty | Role & Notes |
|:---:|---|---|:---:|---|
| 8 | **High-Current Power Wire** | 16 AWG or 18 AWG flexible silicone wire (Red & Black) | ~2–3 m | Heavy-duty wiring for 12 V battery lines, switch, and motor high-power screw terminals. |
| 9 | **Logic Jumper Wires** | 24 AWG – 26 AWG Dupont wires (Female-to-Female, Female-to-Male) | 1 pack (20–30 pcs) | Connects ESP8266 GPIO pins to BTS7960 8-pin headers and power rails. |
| 10 | **Power Distribution Blocks / WAGO Clips** | WAGO 221 lever nuts or terminal distribution blocks | 4–6 pcs | Safe splitting of the 12 V positive line and common ground bus without messy wire splicing. |
| 11 | **Battery Connector** | XT60 / T-Plug (Deans) / DC Barrel (matching battery type) | 1 pair | Secure, high-current connection between battery and harness switch. |
| 12 | **Prototyping Perfboard / Mini Breadboard** | Small perfboard or 170-point mini breadboard | 1 | Optional base for neat distribution of 5 V and GND rail connections. |

---

## 3. Mechanical & Structural Components

*(Derived from physical specifications in `concept.md` and `artichture.png`)*

| # | Item | Key Specifications | Qty | Role & Notes |
|:---:|---|---|:---:|---|
| 13 | **Central Chassis Frame** | Plywood or rigid lightweight plate (approx. 6 cm width × 36 cm length × 6–10 mm thickness) | 1 | Main backbone mounting the motors, central guide pulleys, and arm hinges. |
| 14 | **Claw / Arm Assemblies** | Lightweight PVC pipes, conduit, or plastic tubing (equal lengths) | 6 | Horizontal moving arms arranged in 3 tiers/pairs extending from the central frame. |
| 15 | **Actuation Cable (Red - Contraction)** | High-strength braided fishing line (Dyneema / Spectra 50–100 lb) or thin steel braided wire | 1 spool (~10 m) | Connects upper motor spool through the contraction pulley to deploy the six arm segments. |
| 16 | **Actuation Cable (Blue - Retraction)** | High-strength braided fishing line (Dyneema / Spectra 50–100 lb) or thin steel braided wire | 1 spool (~10 m) | Connects lower motor spool through the retraction pulley to retract the six arm segments. |
| 17 | **Central Cable Pulleys** | Small low-friction pulleys or grooved bearings (with M3/M4 pivot bolts) | 2 | Primary redirection pulleys: Upper (Contraction) & Lower (Retraction). |
| 18 | **Cable Guides / Eyelets** | Small metal or nylon screw-in eyelet loops | 6–12 pcs | Routes cable lines smoothly along the chassis to each of the 6 pipe arms. |
| 19 | **Motor Spools / Cable Drums** | 3D-printed or machined cylindrical spools fitted for 6 mm D-shaft | 2 | Securely mounted to motor shafts for winding/unwinding contraction and retraction cables. |
| 20 | **Johnson Motor Mounting Brackets** | L-shaped heavy metal mounting brackets for Johnson gearmotors | 2 | Rigidly fastens both motors to the central plywood chassis. |
| 21 | **Mechanical Fasteners (Hardware)** | Assorted M3 and M4 machine screws, washers, and nylon lock nuts | 1 set | For mounting motors, pulleys, arm hinges, and PCB standoffs. |

---

## 4. Wearable Mount & Ergonomics

| # | Item | Key Specifications | Qty | Role & Notes |
|:---:|---|---|:---:|---|
| 22 | **Backpack Harness / Straps** | Padded adjustable shoulder straps and chest buckle | 1 set | Secures the central 6 cm × 36 cm mechanism comfortably and stably onto the wearer's back/torso. |
| 23 | **Backplate Padding** | High-density EVA foam (10–15 mm thick) | 1 sheet | Cushions the wearer's back against motor vibration and bolt heads. |

---

## 5. Recommended Tools & Bench Equipment

- **Digital Multimeter:** Essential for measuring and calibrating the buck converter output to 5.0 V before connecting logic boards.
- **Soldering Iron & Heat Shrink Tubing:** For solid motor terminal connections and wire splices.
- **Hex Keys / Screwdrivers:** For assembling Johnson motor brackets and tightening BTS7960 screw terminals.
- **Wire Stripper & Crimper:** For clean wire termination.
- **Zip Ties (Cable Ties):** For cable management to prevent snagging during arm motion.
