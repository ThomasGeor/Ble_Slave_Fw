# Slave module

The slave module consists of a magnetic reed sensor and an ESP32 module.
The ESP32 module is mainly in sleep mode and wakes up whenever it receives an open door
interrupt signal from the pin that the sensor was attached to. On wakeup the ESP32 module
configures it's bluetooth module and connects to the master module to report the incident.
The master module then decides if it should fire the buzzer based on it's state.
