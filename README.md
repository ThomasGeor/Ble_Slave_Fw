# ESP32 BLE Slave sample application activating on GPIO interupt


## Cabling

This is for ESP32 WROVER B series.
GND   -> GND
VDD   -> 3.3v
SDA   ->  
SCLK  ->  

### Hardware Required

This example can be executed on any ESP32 board. For this example i am using an ESP32 WROVER-B board. 

### Configure the project

* When using Make build system, set `Default serial port` under `Serial flasher config`.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor (CMAKE configuration)
or
make flash monitor (MAKEFILE configuration)
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

