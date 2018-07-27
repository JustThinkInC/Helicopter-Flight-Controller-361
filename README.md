# Helicopter Rig Flight Controller Project
Embedded systems project: flight controller for helicopter using Tiva microcontroller. 
This flight controller is designed to work on a Tiva C Series ARM Cortex M4 microcontroller, linked to a helicopter rig. 
The rig contains a linear ADC transducer height monitor and optic based yaw monitor.
Helicopter control was carried out through a web interface.

## Source Files
* PID.c: PID module to accurately control motor speeds.
* PWMSetup.c: Handles PWM setup and output to motors. 
* Project.c: The flight controller code. Handles critical tasks and general flight functions.
* priorityScheduler.c: Simple priority scheduler for task handling.
* uart.c: Module to display current flight details through the Tiva's UART.
* circBufT.c: Circular buffer module, used for storing ADC values for the height.
* buttons6.c: Buttons module with debouncing implemented.

## Contributors
[Liam Laing](https://github.com/liamlaing)

Connor Adamson

Dr Phil Bones (Original author for some modules)

## License
Usage of this code (whether partial or full) is NOT permitted*. However, you may examine the code for your own private study.

*Unless you are a contributer or original developer of the code.
