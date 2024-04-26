# Breakout Game

## General Information

This folder contains the Breakout game implemented using the zedboy utils. Breakout is a classic arcade game where the player controls a paddle at the bottom of the screen to hit a ball upwards to break blocks located at the top. The objective is to clear all the blocks without letting the ball pass the paddle. This version includes a simple scoring system and a life counter. The game progresses by bouncing the ball off the paddle and blocks, with the game ending either by clearing all blocks or losing all lives.

## Implementation

The game is implemented using zedboy utils and features a paddle controlled by the player, a bouncing ball, and multiple destructible blocks. The player controls the paddle using left and right movement commands to prevent the ball from falling below the paddle. The ball changes direction upon hitting the paddle, walls, or blocks. Each destroyed block increases the player's score. The game includes a mechanism to handle game over conditions either by scoring or by running out of lives. Graphics for the game are handled by drawing and updating the display buffer to show the paddle, ball, and blocks.

## breakout.c

This file contains the implementation of the Breakout game, leveraging draw.h and gameConstants.h from the zedboy utils folder. The setup function initializes the game objects including the paddle, ball, and blocks. The game dynamics such as ball movement, collision detection with the paddle, blocks, and walls, as well as score and life management are handled within the update and draw functions. The file includes methods to handle input for moving the paddle and managing the game state based on player actions and game conditions. In the current zedboy application implementation, this game is fully functional and playable.
