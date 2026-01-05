# Pharmaceutical Sterilisation Line Monitoring System
## Arduino-Based Educational Simulator

## 📌 Overview

This repository contains a **proof-of-concept embedded systems solution** for a *Pharmaceutical Sterilisation Autoclave Monitoring System*, developed using **only standard Arduino educational kit components**.

The project demonstrates:
- Sensor calibration under component constraints
- Software-based safety interlocks
- Fault detection with noisy analogue sensors
- System resilience and data integrity strategies

The implementation is designed for **educational demonstrations**, and **embedded systems training scenarios**.

---

## 🎯 Project Objectives

- Simulate a complete sterilisation cycle
- Implement safety-critical logic without additional hardware
- Improve sensor accuracy through software calibration
- Reduce false alarms using signal processing techniques
- Preserve system data during unexpected resets

---

## ⚙️ System Constraints

- **Hardware:** Standard Arduino educational kit only
- **No additional components permitted**
- **Software-based solutions required** for safety and reliability

---

## 🧪 Program 1: Temperature Sensor Calibration

**Problem:**  
The analogue temperature sensor exhibited **±5°C drift**, exceeding the acceptable **±2°C accuracy requirement**.

**Solution:**
- Designed a **two-point calibration procedure** using reference temperatures
- Implemented a **linear calibration model** in software
- Achieved improved accuracy without additional sensors or hardware

## Schematic 
<img width="940" height="663" alt="image" src="https://github.com/user-attachments/assets/cd1a97a4-b791-4ce1-8fad-58672534ac3d" />
## LIVE DEMO: 
https://www.tinkercad.com/things/23s6PqrrBRQ-final-12?sharecode=R7GeWYsxr6MzOKsM5Xoi6LHZ-vfSHMAcRhSu_ClY1kk 

---

## 🔒 Program 2: Door Safety Interlocks

**Problem:**  
The door lock failed to engage during the **STERILIZING** state, creating a potential safety risk.

**Solution:**
- Developed a **systematic troubleshooting procedure**
- Implemented **software-enforced safety interlocks**
- Prevented door unlocking during unsafe operating conditions

## Schematic
<img width="1142" height="844" alt="image" src="https://github.com/user-attachments/assets/dacd83c9-584b-4503-ad64-3f012775613b" />

## LIVE DEMO: 
https://www.tinkercad.com/things/23s6PqrrBRQ-final-12?sharecode=R7GeWYsxr6MzOKsM5Xoi6LHZ-vfSHMAcRhSu_ClY1kk 

---

## 🚨 Program 3: Pump Fault Detection

**Problem:**  
The phototransistor-based pump monitoring system generated **frequent false alarms**, reducing operator trust.

**Solution:**
- Identified root causes of false triggers
- Introduced:
  - Moving average filtering
  - Adaptive (dynamic) thresholds
  - Debouncing logic
- Improved fault detection reliability using **signal processing only**

## Schematic
<img width="1307" height="829" alt="image" src="https://github.com/user-attachments/assets/a25c2575-8cbf-417f-bf81-893b84d163a3" />


## LIVE DEMO: 
https://www.tinkercad.com/things/5aLqi4kpYl0-final-q33?sharecode=iAC6b_Og4_J97zzOsQw6rlqzpKbmRIZ7Qy6HRRbSBAU 

---

## 🔁 Program 4: System Resilience and Data Integrity

**Problem:**  
Unexpected resets during long sterilisation cycles caused **loss of critical process data**.

**Solution:**
- Implemented:
  - Watchdog timer recovery
  - Circular buffer data logging
  - Power stability monitoring
- Ensured **graceful system recovery** after resets

## Schematic
<img width="1389" height="835" alt="image" src="https://github.com/user-attachments/assets/e073ca14-9cbb-4d31-8079-5cbeb13db5d3" />


## LIVE DEMO: 
https://www.tinkercad.com/things/fBEfJOQi2vx-final-q42-?sharecode=jEcAuoFVZ_YcAPXUzP8KjWqUQPUhosl-kw9u_JcZINc 


---

## 🧠 Engineering Concepts Demonstrated

- Embedded safety systems
- Sensor calibration techniques
- Analogue signal conditioning
- State-based control logic
- Watchdog timers
- Fault-tolerant system design

---

## 🛠 Technologies Used

- **Arduino (C/C++)** – simulated on *Tinkercad*
- Analogue sensor processing
- Finite State Machines (FSM)
- Embedded software safety principles
