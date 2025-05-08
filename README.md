# Temperature Control System with Arduino

This project is a temperature-based state tracker using Arduino, a DHT22 sensor, a 16x2 LCD, LEDs, a buzzer, and a slide switch. It features multiple operational states based on temperature readings.

## Hardware Used
- Arduino Uno
- DHT22 (Temp & Humidity Sensor)
- Red LED (Pin 7)
- Green LED (Pin 6)
- Buzzer (Pin 5)
- Slide Switch (Pin 13)
- 16x2 LCD Display (A4: SDA, A5: SCL)

## Functional States
- **Idle:** Temp < 25°C
- **Heating:** 25°C ≤ Temp < 30°C
- **Stabilizing:** 30°C ≤ Temp ≤ 34°C
- **Target Reached:** 34°C < Temp ≤ 36°C
- **Overheat:** Temp > 38°C

## LCD Display
- Line 1: Current temperature
- Line 2: Current state

## Buzzer
- Only activates in Overheat state (beeps every 2 seconds)

## Files
- `TemperatureControlSystem.ino`: Main Arduino sketch
- `wiring_diagram.png`: Circuit schematic
- `flowchart.png`: Logical flow diagram
