# Utilities

## displayImg.h

## draw.h

### How to use:

First verify the [kOLED](../../kOLED) module is inserted correctly

Then, initialize the OLED screen using openOLED() this returns a file descriptor to allow the other functions to write to the OLED screens character device.

Next, use any set of functions to draw shapes, text, and pixels to draw whatever is desired.

Finally, use the sendBuffer() command with the file descriptor from the openOLED to refresh the screen for the next frame. Also deinitialize the OLED screen when finished using oledClose()

### Design:

The simpliest unit for the library is drawPixel, this is expanded upon to create more functions that draw more compex shapes such as lines, rectangles, and characters. Each of these having their own API to allow the user to simplify creation of more complex shapes to allow for each game to have more streamlined graphics development.

## gameConstants.h

### How to use:

This header file contains constants to be used when registering a game to the zedboy application, handling input from the user, or ending the game.
For registering a game, the following constants are used:
- games: An array of function pointers to the game setup functions
- gameTicks: An array of function pointers to the game tick functions
- GameIdx: An enum of the game indexes

Actual registration can be seen as below:
```c
__attribute__((constructor))
void registerSnake() {

    if (game_count < MAX_GAMES) games[SNAKE] = SnakeGame_setup;
    if (game_count < MAX_GAMES) gameTicks[SNAKE] = SnakeGame_tick;
}
```
Where the game setup and tick functions are registered to the games and gameTicks arrays respectively.

For handling input from the user, the following constants are used:
- Direction: An enum of the directions the user can input

For ending the game, the following constants are used:
- endGame: A flag to tell the system that the game is over

### Design:

The gameConstants header file is designed to be a central location for all constants that are used in the zedboy application. This allows for easy access to these constants and for easy modification of these constants. This header file is also designed to be easily expandable, allowing for more constants to be added as needed.