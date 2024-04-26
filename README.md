# zedBoy

####

## Abstract

For our project we plan to develop a Game System for the ARM Cortex-A9 Zedboard Zynq-7000 development board. This will impliment a driver for the on-board OLED screen (ug-2832hsweg04) for setting up and drawing to the screen as well as a draw.h library for simple graphics. This libarary will be used in the development of several games such as snake, tetris, pong, and breakout and will allow for implementation of new games as desired by the user.

## Intro
The project is an exmaple of an embedded system. Combinining kernel-space device creation with user-space accessing of the internal hardware allowing a user to interact with the on board OLED screen.

People who use the same development board will find that there is no easy way to interact with the OLED screen, and the similar implementation is purely userspace and does not have any extended implementation for the drawing API and just has a function to draw pixels individually.

We are trying to simplify the game creation process to make a system that can be modular having the ability to add games easily. 


## Related Work

There is a similar library known as Libzed which allows for simplistic use of the functions on the development board. Some of the functionality included is GPIO accessessing and editing, hardware specific peripheral mapping information, and OLED screen implementation. Although their implementation is not avalible we took inspiration from the structure of availible APIs. 

## System Diagram

![System Diagram](<System_Diagram.png>)

## Contributions

Jakob Brattke (brattke.j@northeastern.edu)

Worked on creating games for the zedboy application, this included [pong](/zedboy/games/pong) and the not fully implemented [flappy bird](/zedboy/games/flappy). Also worked on menu controls and registering games to the zedboy application in a modular fashion, [gameConstants](/zedboy/utils).

Thane Gallo (gallo.th@northeastern.edu) 

Developed [kOLED.ko](kOLED/) module to create a character device which when written to would send a buffer to fill the graphical display ram and would update the OLED screen accordingly. Also developed [draw](/zedboy/utils) library for OLED device management to open, close, and write text and shapes to the graphic device ram for development of other games. Also helped move pong and tetris from their terminal implementations and embed them within the zedBoy application.


Dmitry Matveev (matveev.d@northeastern.edu)

Will focus on creating the game logic for the game (Snake.c) as well as the graphical data that the game will be outputting to the OLED display. This will be the input to the OLED module and will take in button interrupts as control inputs for the game.

Sym Cunningham (sym.cunningham@gmail.com)

## Implementation Progress

Overall we succeeded in creating a system that allows for easy game creation and implementation. The OLED screen was able to be controlled and the draw.h library was able to be used to draw shapes and text to the screen. The gameConstants.h file was able to be used to register games to the zedboy application. Overall we created four fully implemented and functional games, snake, pong, tetris, and breakout. The games were able to be played on the OLED screen and GPIO buttons were used to control the games. Although the user experience is not perfect and could be improved to create a smoother and more responsive experience, our overall project goals were met.

## Future Work

In the future we would like to improve the user experience of the games. This would include improving the responsiveness of the controls and the speed of the games. An obvious extension would be to simply create more games and implement them into the zedboy application. Another improvement we could have gotten to with more time would be a more user friendly menu system that can support more games.

More reach goals could include implementing a leaderboard/highscore system, adding sound effects, and adding multiplayer support to console(and implementing it in certain games).

## Experimental Results

The way the controls were implimented utilized polling of the state of the control buttons. This caused a few errors and decreased responsiveness. The decreased responsiveness would make it so that it only reads the last input which is read when the polling ticks so there would be a possibility several "ghost" inputs before the game control activated and the graphics updated accordingly. This polling also increased CPU utilization greatly as it was polling much faster than the screen was able to update. THe polling rate was then decreased to improve performance while not entirely redesigning the control scheme and there was noticble change in CPU utilization going down a whole 10% .The next revision of the system will read button inputs in a seperate thread and push them into a queue so each input is read as the game screen updates.
  
As each game has a very similar frame rate (4.25 frames per second) it is believed that the main slowdown for this system is the OLED screen. There are a few reasons this could be the case one is that the screen is half-duplex and between every 16 byte write to the screen another 16 bytes of meaningless data is beaing sent back to the SPI peripheral to fill the RX FIFO. This data has no purpose but must be read to prevent an RX overrun error and therefore greatly slows down transmission speed of data. This is not something that can be changed given the wiring setup of the Development board's PCB. There is however a change that can be made on the software side to increase the game speed. This would be that every frame most games clear the whole screen before writing the next frame this rewrites the whole buffer of 512 bytes rather than turning off the pixels individually and only drawing the ones that change between frames. This should make a small improvement and increase user experience.

