# Tetris Game

## General Information

This folder contains the Tetris game implemented using zedboy utils. Tetris is a tile-matching puzzle video game where players must manipulate tetromino pieces that descend onto the playing field. The objective is to create horizontal lines of blocks without gaps and when such a line is created, it disappears, and any block above the deleted line will fall. This version of Tetris challenges players to place the tetrominoes efficiently to maximize their score and prevent the stack of tetrominoes from reaching the top of the game area, which ends the game.

## Implementation

The game is implemented using zedboy utils and features different shapes of tetrominoes. Players control the tetrominoes, moving them left or right and rotating them to fit them into place at the bottom of the playing field. The game includes functions for rotating the tetrominoes, checking for collisions, merging the tetrominoes into the playing field, and clearing completed lines. Each cleared line increases the player's score. The game continually checks for game over conditions, such as tetrominoes filling up to the top of the playing field.

## tetris.c

This file contains the full implementation of the Tetris game, utilizing draw.h and gameConstants.h from the zedboy utils folder. The setup function initializes the game board and selects the initial tetromino. The draw function updates the game display to show the placement of the tetrominoes. User inputs that alter the position or orientation of the tetrominoes are handled, and game logic is applied to update the state of the game after each move. The tick function integrates these updates into a continuous loop, making the game responsive and interactive. The game is playable and functional as per the current zedboy application implementation.
