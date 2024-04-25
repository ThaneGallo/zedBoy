# zedBoy

####

## Abstract

For our project we plan to develop a Game System for the ARM Cortex-A9 Zedboard Zynq-7000 development board. This will impliment a driver for the on-board OLED screen (ug-2832hsweg04) for setting up and drawing to the screen as well as a draw.h library for simple graphics. This libarary will be used in the development of several games such as snake, tetris, pong, and breakout and will allow for implimentation of new games as desired by the user.

## Intro
The project is an exmaple of an embedded system. Combinining kernel space device creation with user 

People who use the same development board will find that there is no easy way to interact with the OLED screen, and the similar implimentation is purely userspace and does not have any extended implimentation for the drawing API and just has a function to deaw pixels individually.

We are trying to simplify the game creation process to make a system that can be modular having the ability to add games easily. 


## System Diagram

![System Diagram](<System_Diagram.png>)

## Related Work

There is a similar library known as Libzed which allows for simplistic use of the functions on the development board. Some of the functionality included is GPIO accessessing and editing, hardware specific peripheral mapping information, and OLED screen implimentation. Although their implimentation is not avalible we took inspiration from the structure of availible APIs. 

## Contributions

Jakob Brattke (brattke.j@northeastern.edu)

Will focus on user input through buttons and interrupts, as well as other menu functions that will focus on game start, customization, and general menu functions. This work will happen in OLED.ko, where we will implement interrupts to listen for user input to direct game input/setup.

Thane Gallo (gallo.th@northeastern.edu) 

Developed kOLED.ko module to create a character device which when written to would send a buffer to fill the graphical display ram and would update the OLED screen accordingly. Also developed draw.h library for OLED device management to open, close, and write text and shapes to the graphic device ram for development of other games. Also helped move pong and tetris from their terminal implimentations and embed them within the zedBoy menu / OLED screen


Dmitry Matveev (matveev.d@northeastern.edu)

Will focus on creating the game logic for the game (Snake.c) as well as the graphical data that the game will be outputting to the OLED display. This will be the input to the OLED module and will take in button interrupts as control inputs for the game.

Sym Cunningham (sym.cunningham@gmail.com)

## Experimental Results
The way the controls were implimented utilized polling of the state of the control buttons. This caused a few errors and decreased responsiveness. The decreased responsiveness would make it so that it only reads the last input which is read when the polling ticks so there would be a possibility several "ghost" inputs before the game control activated and the graphics updated accordingly. This polling also increased CPU utilization greatly as it was polling much faster than the screen was able to update. THe polling rate was then decreased to improve performance while not entirely redesigning the control scheme and there was noticble change in CPU utilization going down a whole 10% .The next revision of the system will read button inputs in a seperate thread and push them into a queue so each input is read as the game screen updates.
  
As each game has a very similar tickrate it is believed that the main slowdown for this system is the OLED screen. There are a few reasons this could be the case one is that the screen is half-duplex and between every 16 byte write to the screen another 16 bytes of meaningless data is beaing sent back to the SPI peripheral to fill the RX FIFO. This data has no purpose but must be read to prevent an RX overrun error and therefore greatly slows down transmission speed of data. This is not something that can be changed given the wiring setup of the Development board's PCB. There is however a change that can be made on the software side to increase the game speed. This would be that every frame most games clear the whole screen before writing the next frame this is an extra set of 32 transmissions rather than turning off the pixels individually and only drawing the ones that change between frames.

  
## Lessons Learned


