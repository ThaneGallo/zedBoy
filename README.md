# Abstract

For our project we plan to develop a Game System for the ARM Cortex-A9 Zedboard Zynq-7000 development board. This will impliment a driver for the on-board OLED screen (ug-2832hsweg04) for setting up and drawing to the screen as well as a draw.h library for simple graphics. This libarary will be used in the development of several games such as snake, tetris, pong, and breakout and will allow for implimentation of new games as desired by the user.

# Intro
The project is an exmaple of an embedded system. 


# System Diagram

![System Diagram](<System_Diagram.png>)

# Related Work

There is a similar library known as Libzed which allows for simplistic use of the functions on the development board. Some of the functionality included is GPIO accessessing and editing, hardware specific peripheral mapping information, and OLED screen implimentation. Although their implimentation is not avalible we took inspiration from the structure of avalible APIs. 

# Contributions

Jakob Brattke (brattke.j@northeastern.edu)

Will focus on user input through buttons and interrupts, as well as other menu functions that will focus on game start, customization, and general menu functions. This work will happen in OLED.ko, where we will implement interrupts to listen for user input to direct game input/setup.

Thane Gallo (gallo.th@northeastern.edu) and Sym Cunningham (sym.cunningham@gmail.com)

Developed kOLED.ko module to create a character device which when written to would send a buffer to fill the graphical display ram and would update the OLED screen accordingly. Also developed draw.h library for OLED device management to open, close, and write text and shapes to the graphic device ram for development of other games.


Dmitry Matveev (matveev.d@northeastern.edu)

Will focus on creating the game logic for the game (Snake.c) as well as the graphical data that the game will be outputting to the OLED display. This will be the input to the OLED module and will take in button interrupts as control inputs for the game.

# Related work

# Contributions
