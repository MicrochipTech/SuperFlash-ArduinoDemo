# SuperFlash® Chip Erase Timing Demo for Arduino

## Overview of Demo

Invented by Silicon Storage Technologies (SST) and later acquired by Microchip, SuperFlash® technology is an innovative flash memory technology providing erase times up to 1,000 times faster than competing flash memory technologies on the market. This demo illustrates this significant speed advantage by allowing the user to perform chip erase operations on serial flash memory devices while the board measures the erase time in real-time. (Flash memory devices feature multiple erase functions for various regions of memory. Chip erase was chosen for this demo as it requires the longest time, and therefore has the maximum impact.) The table below lists the supported flash devices from Microchip. It includes the typical chip erase times measured on the devices to indicate the general range of timing to expect.

| Device | Density | Typical Chip Erase Time |
| :---- | :---: | :---: |
| [SST26VF064B](https://www.microchip.com/en-us/product/SST26VF064B) | 64 Mbit | 36ms |
| [SST26VF032B](https://www.microchip.com/en-us/product/SST26VF032B) | 32 Mbit | 38ms |
| [SST26WF016B](https://www.microchip.com/en-us/product/SST26WF016B) | 16 Mbit | 36ms |

## List of Required Items

The following items are required to build the SuperFlash Chip Erase Timing Demo for Arduino: 
1. Arduino Uno or similar Arduino-compatible board 
2. Arduino Uno Click Shield - [TMIK033](https://www.microchip.com/en-us/development-tool/TMIK033)
3. SQI SuperFlash Kit - [AC243009](https://www.microchip.com/en-us/development-tool/AC243009)

## Preparing the Demo 

Before the first use of the demo, the Arduino and Click Shield boards must be prepared. The Arduino must be programmed with the SuperFlash® Chip Erase Timing Demo firmware.

Follow these steps to prepare the demo: 

1. Insert the Click Shield on top of the Arduino 
2. Connect the USB cable to the Arduino’s port 
3. Program the firmware using the Arduino software 

## Running the Demo 

Follow these steps to run the demo: 

1. Open the Serial Monitor in the Arduino software 
2. Unplug the USB cable from the Arduino 
3. Insert SuperFlash daughter board to test into mikroBUS 1 socket 
4. Plug the USB cable back into the Arduino 
5. Wait for chip erase to complete 
6. Note the chip erase time reported on the Serial Monitor 
7. Repeat steps 2 through 5 with different SuperFlash daughter boards