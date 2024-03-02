# Project Overview

For our project, we plan to develop a straightforward Snake game on the ZedBoard, using its OLED screen for display and the onboard buttons for control. We'll start with getting the basic game running smoothly. Once we have that foundation, we aim to add a few enhancements. These might include a simple start menu for basic settings like difficulty or level select and a game over/leaderboard screen at the end to keep track of high scores. This step-by-step method will help keep us on track to succeed with the project.

# System Diagram

![System Diagram](<System Diagram.png>)

# Members and Responsibilities

Jackob Brattke (brattke.j@northeastern.edu)

Will focus on …

Thane Gallo (gallo.th@northeastern.edu) and Sym Cunningham (sym.cunningham@gmail.com)

Will focus on creating the kernel module (OLED.ko) which will probe for the OLED device from the Zed-Board device tree and will create and initialize the character device for it. So the other parts of the project can interface with it and write to it.

Dmitry Matveev (matveev.d@northeastern.edu)

Will focus on creating the game logic for the game (Snake.c) as well as the graphical data that the game will be outputting to the OLED display. This will be the input to the OLED module and will take in button interrupts as control inputs for the game.
