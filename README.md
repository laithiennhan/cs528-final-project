# Wearable Fall Detection System

## 1. Project Overview

### What is the project about?
Develop a prototype for a wearable device capable of detecting when wearers fall. This device is designed to blink or make sounds to alert others nearby and can also send real-time text messages or emails to designated contacts with the time of the incident and location.

#### System Overview
The system comprises the following main components:
- **Wearable Device:** The core component housing the IMU sensor, a microcontroller unit (MCU), a power source (battery), and a wireless communication module.
- **IMU Sensor:** Detects acceleration and orientation changes to identify potential falls.
- **Microcontroller Unit (MCU):** Processes the data from the IMU sensor, running the fall detection algorithm to determine if a fall has occurred.
- **Wireless Communication Module:** Enables the device to communicate with an external device (smartphone, computer, or dedicated receiver) to send alerts or log data.
- **Power Source:** A battery with a power management system to ensure long-term usage and reliability.

### High-Level Design Components

1. **Data Acquisition:** The IMU sensor continuously monitors the wearer's movements, capturing acceleration and orientation data in real-time.
2. **Data Processing and Fall Detection Algorithm:** The MCU processes the data using a predefined algorithm to distinguish between normal activities and potential falls.
3. **Alert Generation and Communication:** Upon detecting a fall, the system generates an alert and communicates this, along with relevant data, to predefined contacts.
4. **Power Management:** Ensures the device remains powered and operational.

### System Interaction Flow

1. **Initialization:** The device initializes the IMU sensor and starts monitoring movements upon being turned on.
2. **Continuous Monitoring:** The device continuously processes data from the IMU sensor.
3. **Fall Detection and Alerting:** It triggers an alert sequence upon detecting a fall.
4. **User Feedback:** The user is informed of the device's status and any alerts.

## 2. Project Significance

This project is exciting for its potential real-world impact, particularly for the elderly or those at higher risk of falls. Combining hardware interfacing, software development, and data analysis, it allows us to apply a wide range of skills from our embedded programming class, aiming to provide in-time support and prevent further medical complications for elders.

## 3. Additional Hardware Required

- Wearable casing
- Battery and power management system

## 4. Team Members and Roles

- Nhan Lai
- Steve Cao
- Hieu Nguyen

The team will collaborate on all milestones to deliver the final product.

## 5. Milestones and Deliverables

- **Week 1-2:** Project Proposal and Design
- **Week 3:** Hardware Acquisition and Initial Testing
- **Week 4-6:** Algorithm Development and Integration
- **Week 7:** Prototype Assembly
- **Week 8-9:** System Testing and Refinement
- **Week 10:** Final Presentation and Demonstration; Documentation and Future Work

## 6. Project's GitHub Link

[CS528 Final Project Repository](https://github.com/laithiennhan/cs528-final-project)
