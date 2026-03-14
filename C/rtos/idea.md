# RTOS-based ECU Communication System with Security Monitoring

## 1. Project Overview
This project involves building a simulated Automotive ECU (Electronic Control Unit) network. Rather than building a full smart car, we focus on the critical communication and security backbone using the **CAN (Controller Area Network) bus** and a **Real-Time Operating System (RTOS)**.

## 2. System Architecture
The system simulates the interaction between multiple ECUs found in modern vehicles (e.g., Engine, Brake, and Sensor ECUs).

### Hardware/Node Layout
* **ECU 1 (Sensor Node):** Collects data (Heat, Distance, or Speed) and broadcasts it to the bus.
* **ECU 2 (Control/Actuator Node):** Receives sensor data via CAN and triggers an actuator (e.g., a fan or LED) based on logic.
* **Monitoring Node:** Observes traffic for security anomalies.



## 3. RTOS Implementation
To ensure timing guarantees and stability, the firmware is built on an RTOS (e.g., FreeRTOS). The system is divided into concurrent tasks:
* **Task 1 (Data Acquisition):** Periodic sensor polling.
* **Task 2 (TX Engine):** Formatting and sending CAN messages.
* **Task 3 (RX Engine):** Interrupt-driven or high-priority message reception.
* **Task 4 (Logic/Actuator):** Decision-making and hardware control.

## 4. Security Framework
Standard CAN bus communication lacks inherent encryption and authentication. We will implement and demonstrate:
1.  **Integrity Verification:** Using Checksums or Hashes to ensure data hasn't been tampered with.
2.  **ID Filtering:** Rejecting unauthorized Message IDs at the hardware/software level.
3.  **Intrusion Detection (IDS):** Monitoring the bus for frequency anomalies (e.g., an attacker flooding the bus with "Speed=200" messages).

## 5. Development Timeline (3 Months)
* **Month 1:** Environment setup, RTOS task configuration, and basic CAN communication between two nodes.
* **Month 2:** Integration of real/simulated sensors and actuator logic. Implementation of the security layer.
* **Month 3:** Testing "Attack Scenarios," debugging, and final documentation/presentation prep.
