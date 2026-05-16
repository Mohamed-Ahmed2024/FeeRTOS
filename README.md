# Temperature-Based Motor Control using FreeRTOS

## Overview
This project implements a temperature-based motor control system on **ATmega169PA** using **FreeRTOS**.

The system continuously reads temperature from an **LM35 sensor**.

- If temperature > 25°C → Motor runs forward
- If temperature ≤ 25°C → Motor stops

A push button has higher priority than the temperature task:

- Button OPEN → Override ON → Motor forced STOP
- Button PRESS → Override OFF → Temperature control resumes

UART is used to print:

- Temperature readings
- Motor state updates
- Button override events

---

## FreeRTOS Design

### Tasks

1. **Button Task** (Priority 3)
   - Polls button every 50 ms
   - Handles override

2. **Temperature Task** (Priority 2)
   - Reads LM35 every 500 ms
   - Decides motor state

3. **Motor Task** (Priority 1)
   - Receives motor commands from queue
   - Drives motor

---

## Synchronization

### Queue
Used to pass motor state between tasks:

- MOTOR_STOP
- MOTOR_FORWARD

### Mutex
UART mutex protects serial output from concurrent access.

---

## Hardware

- ATmega169PA
- LM35 temperature sensor
- Push Button
- L293D motor driver
- DC Motor
- UART terminal

---

## Proteus Simulation

Proteus project file included.

---

## Author

Mohamed Ahmed