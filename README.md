<!-- Navigable Item -->
<a id="readme-top"></a>

<!-- General Information -->
<div align="center">
  <h1 align="center">Microcontrollers Assignments</h1>
  <img width="500" src="https://github.com/Zehond/MicroControllersOpdrachten/blob/main/Images/UNI-DS6.jpg"/>
</div>

<!-- Divider -->
___
<!-- Table Of Contents -->
<details open>
 <summary><strong>📚 Table of Contents</strong></summary>
  
- [About This Repository](#about-this-repository)<br>
- [Final Assignment](#final-assignment)<br>
   - [Creation](#creation)<br>
   - [Wiring](#wiring)<br>
</details>

<!-- Divider -->
___
<!-- Badges -->
[![GitHub Release](https://img.shields.io/github/v/release/Zehond/MicroControllersOpdrachten?style=for-the-badge&labelColor=%23000000)](https://github.com/Zehond/MicroControllersOpdrachten/releases)

<!-- About This Repository -->
# About This Repository
This repository contains the school assignments for the module: Microcontrollers. In this module we had lectures about the ATmega128 and UNI-DS6 microcontrollers. The assignments were made for the UNI-DS6 board, using [Microchip Studio](https://www.microchip.com/en-us/tools-resources/develop/microchip-studio) with [AVRFLASH](https://avrflash.software.informer.com/2.1/).<br><br>

<!-- Final Assignment -->
# Final Assignment
For the [final assignment](https://github.com/Zehond/MicroControllersOpdrachten/tree/main/Eindopdracht) we had to make use of 3 interface blocks that we could choose ourselves. We chose:
- SPI: 7-segmented display.
- Timers and interrupts.
- LCD Screen.<br>

We also made use of:
- Built in piezo buzzer.
- DHT11 sensor.

<!-- Creation -->
## Creation
We started by integrating the temperature sensor with the 7-segment display via SPI. Initially, we used an MCP9701 temperature sensor, which was already included in the kit. We were able to do this fairly quickly, as we had already worked on it during previous lab sessions.<br>

After that, we looked into using the built-in piezo buzzer. For this, we used pin 1 on PORTE — initially, we had used pin RC1. We chose that one first for quick testing. With a tip from the instructor, we got the buzzer working quickly. Specifically, we connected pin 1 on PORTE directly to the buzzer, which allowed the signal to be beeped out.<br>

Later, the instructor suggested we try using a different temperature sensor: the DHT11. We were able to implement it with the help of an example. Later on, we largely rewrote existing methods to better display numbers on the 7-segment display. We also added handling for negative temperatures (even though this isn’t really supported by the hardware).<br>

We tested the DHT11 sensor by taking an outdoor measurement and comparing it to the actual outside temperature:
- Outdoor reading of 14°C on 28-3-2025.
- Actual temperature on 28-3-2025, meaning the sensor has a deviation of approximately ±3 degrees.<br>

For controlling the LCD display, we were able to reuse code from earlier lab sessions. However, we also initialized a second row with the following code in order to display additional data.<br>

We also used Timers for updating the 7-segment display and Interrupts for the beeping. For the beeping, we used different frequencies to distinguish between ones, tens, hundreds, and thousands.<br>

Timer1 settings:

    Compare Match A (19530): 2-second delay

    Compare Match B (78120): 8-second delay
<br>
Initially, we intended to display decimal numbers on the 7-segment display. However, after spending several hours trying to make this work, we discovered that the DHT11 sensor cannot measure decimal values. The sensor can only measure temperatures from 0 to 50 degrees Celsius.

<!-- Wiring -->
## Wiring
Below are some pictures of the wiring on the UNI-DS6 microcontroller board.
<div align="center">
  <img width="800" src="https://github.com/Zehond/MicroControllersOpdrachten/blob/main/Images/Wiring-1.jpeg"/>
  <img width="800" src="https://github.com/Zehond/MicroControllersOpdrachten/blob/main/Images/Wiring-2.jpeg"/>
  <img width="800" src="https://github.com/Zehond/MicroControllersOpdrachten/blob/main/Images/Wiring-3.jpeg"/>
</div>

<!-- Divider -->
___

<!-- Back To Top -->
<p align="right"><a href="#readme-top">Back To Top</a></p>
