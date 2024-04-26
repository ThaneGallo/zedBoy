# Pong Game

## General Information

This folder contains the Pong game implemented using the zedboy utils. The game is a simple two player game where each player controls a paddle. The players must hit the ball with their paddle to prevent the ball from hitting the wall behind them. The game ends when the ball hits the wall behind a player. In this implementation, the game is played vs an AI.

## Implementation

The game is implemented using the zedboy utils. The game has two paddles, one controlled by the player and one controlled by the AI. The player controls the paddle using the up and down buttons. The AI controls the paddle by following the ball. The ball moves in a straight line and bounces off the walls and paddles. The game ends when the ball hits the wall behind a player. The player scores a point when the ball hits the wall behind the AI. The AI scores a point when the ball hits the wall behind the player.
This implementation includes a simple scoreboard displayed to the right of the game screen.

## pong.c

This file contains the implementation of the Pong game. The game is implemented using the draw.h and gameConstants.h files from the zedboy utils folder. In the current zedboy application implementation this game is fully functional and playable.