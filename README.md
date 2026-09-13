# IOT-Battery-Monitoring
ESP32-based UPS battery monitoring system with WiFi telemetry, MySQL logging, PHP API, and web dashboard for lead-acid battery health tracking.

## Features

- Battery voltage monitoring
- SOC calculation
- Daily upload at 9:00 AM
- MySQL database logging
- PHP API
- Web dashboard
- Multi-device support

## Hardware

- ESP32 DevKit V1
- INA226 (planned)
- Lead Acid Battery
- WiFi

## Software

- Arduino IDE
- Wokwi Simulator
- PHP
- MySQL
- HTML Dashboard

## Architecture

Battery
↓
ESP32
↓
WiFi
↓
battery.php
↓
MySQL
↓
dashboard.php

## Wokwi Simulation

https://wokwi.com/projects/474936305160841217

## Dashboard Features

- Device Status
- Voltage Monitoring
- SOC Monitoring
- SOH Monitoring
- Resistance Monitoring
- Historical Logs

## Author

Chiang Wing Hoe
