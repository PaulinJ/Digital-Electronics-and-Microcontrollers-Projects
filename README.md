Pharmaceutical Sterilisation Line Monitoring System

Arduino-Based Educational Simulator

📌 Overview

This repository contains a proof-of-concept embedded systems solution for a Pharmaceutical Sterilisation Autoclave Monitoring System, developed using only standard Arduino educational kit components.

The project demonstrates:

Sensor calibration under component constraints

Software-based safety interlocks

Fault detection with noisy analogue sensors

System resilience and data integrity strategies

The implementation is designed for educational demonstrations, stakeholder presentations, and embedded systems training scenarios.

🎯 Project Objectives

Simulate a complete sterilisation cycle

Implement safety-critical logic without additional hardware

Improve sensor accuracy through software calibration

Reduce false alarms using signal processing techniques

Preserve system data during unexpected resets

⚙️ System Constraints

Hardware: Standard Arduino educational kit only

Sensors: Analogue temperature sensor, phototransistor, IR LED

No additional components permitted

Software-based solutions required for safety and reliability

Program 1:
Problem:
The analogue temperature sensor exhibited ±5°C drift, exceeding the acceptable ±2°C accuracy requirement.

🔹 Solution
Designed a two-point calibration procedure using reference temperatures

Implemented a linear calibration model using software

No additional sensors or hardware used

Program 2:
🔹 Problem:
The door lock failed to engage during the STERILIZING state, creating a safety risk.

🔹 Solution
Developed a systematic troubleshooting procedure

Implemented software-enforced safety interlocks

Prevented door unlocking during unsafe conditions

Program 3
🔹 Problem:
The phototransistor-based pump monitor generated frequent false alarms, reducing operator trust.

🔹 Solution
Identified root causes of false triggers

Introduced:

   - Moving average filtering

   - Adaptive (dynamic) thresholds

Debouncing logic

Improved reliability using signal processing only

Program 4:
Problem:
Unexpected resets during long sterilisation cycles caused loss of critical process data.

🔹 Solution
Implemented:

Watchdog timer recovery

Circular buffer data logging

Power stability monitoring

Ensured graceful recovery after resets

🧠 Engineering Concepts Demonstrated

Embedded safety systems

Sensor calibration techniques

Analogue signal conditioning

State-based control logic

Watchdog timers

Fault-tolerant system design

🛠 Technologies Used

Arduino (C/C++) - simulated on Tinkercad

Analogue sensor processing

Finite State Machines (FSM)

Embedded software safety principles

