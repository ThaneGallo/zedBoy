# Snake Game

## General Information

This folder contains the Snake game implemented using zedboy utils. Snake is a popular arcade game where the player maneuvers a line which grows in length, with the line itself being a primary obstacle. The goal is to navigate the snake around the screen and consume fruits to increase the snake's length and score. The game ends if the snake runs into itself or the walls. This version features simple controls and an increasing difficulty as the snake grows longer.

## Implementation

The game is implemented using zedboy utils. The player controls the snake using directional input commands to steer the snake towards the fruit. Each fruit consumed increases the length of the snake and adds to the score. The game is designed to wrap around when the snake moves off the edges of the display, reappearing on the opposite side. Collisions with the snake's own tail result in game over, and the game also ends if the player manually terminates it. The game updates continuously by redrawing the snake and fruit based on the snake's movement and player input.

## snake.c

This file contains the full implementation of the Snake game, utilizing draw.h and gameConstants.h from the zedboy utils folder. The setup function initializes the game environment, including the snake's starting position and the placement of the first fruit. The draw function handles all graphics output to the display, such as drawing the snake and the fruit. Player inputs are processed to change the direction of the snake, and the game logic function updates the state of the snake and checks for collisions. This file also includes a tick function that integrates input, logic, and drawing updates into a continuous game loop. The game is playable and functional as per the current zedboy application implementation.
