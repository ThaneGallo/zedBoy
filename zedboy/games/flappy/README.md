# Flappy Bird Game

## General Information

This folder contains the Flappy Bird game implemented in the console and using the zedboy utils. The game is a simple side-scrolling game where the player must navigate a bird through a series of pipes. The player must avoid the pipes by jumping over them. The player scores points by passing through the pipes. The game ends when the player collides with a pipe or the ground.

## Implementation

Actual implementation of the game is relatively simple. This implementation has three pipes on the screen at any given time. The pipes are generated with gaps at random heights in them. The player must navigate the bird through these gaps in the pipes. The bird is controlled by the a 
single input, when triggered the bird jumps up. The bird falls down due to gravity. The game ends when the bird collides with a pipe or the ground. Score is incremented by 1 for each pipe passed.

## main.c

This file contains a simple implementation of the Flappy Bird game. It is implemented in the console, and the only input is the space bar to jump. To compile and run the game, use the following commands:

```bash
gcc main.c -o flappy
./flappy
```

## flappy.c

This file contains a zedboy implementation of the Flappy Bird game. The game is implemented using the draw.h and gameConstants.h files from the zedboy utils folder. In the current zedboy application implementation this game is not included, however could be simply added by registering the game setup and tick functions in the gameConstants.h file(instructions on how to do this can be found in the utils/README.md file).