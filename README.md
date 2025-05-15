# X-1 Vehicle Firmware

The firmware of X-1 Vehicle STM32 microcontroller for 2025 AIAA DBF competition by HKU DBF Team

__WARNING: FULL FUNCTIONALITY OF THIS FIRMWARE IS NOT GUARANTEED!__

## Toolchain
- STM32CubeMX
- VSCode + stm32-for-vscode extension
- OpenOCD

## Program Overview
- Code organization:
    | Directory | Content |
    | --- | --- |
    | [algorithm](algorithm/) | hardware-independent algorithms |
    | [application](application/) | application-specific tasks and functions |
    | [board](board/) | board support package of the customized MCU board |
    | [component](component/) | APIs of sensors and actuators |

- Libraries:
    - STM32HAL
    - CMSIS V2

- Todo...

Coding style is heavily inspired by and some modules are borrown from the projects below:  
- DJI Robomaster. RoboRTS-Firmware. [https://github.com/RoboMaster/roboRTS-Firmware/](https://github.com/RoboMaster/roboRTS-Firmware/)

## Hardware block diagram
todo.

## Tasks
doc todo.

## Application notes
- Remote control function works with ground station program
[https://github.com/EricEricEricJin/ELEC3442-Group-Project-Ground/tree/x1_rc](https://github.com/EricEricEricJin/ELEC3442-Group-Project-Ground/tree/x1_rc)
- Due to on-site hardware error, this firmware was not tested in the competition.
- The control parameters had NOT been proved working on the plane.