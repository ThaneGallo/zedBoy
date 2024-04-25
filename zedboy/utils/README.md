# Utilities

## displayImg.h

## draw.h

### How to use:

First, initialize the OLED screen using openOLED() this returns a file descriptor to allow the other functions to write to the OLED screens character device.

Next, use any set of functions to draw shapes, text, and pixels to draw whatever is desired.

Finally, use the sendBuffer() command with the file descriptor from the openOLED to refresh the screen for the next frame. Also deinitialize the OLED screen when finished using oledClose()

### Design:

The simpliest unit for the library is drawPixel, this is expanded upon to create more functions that draw more compex shapes such as lines, rectangles, and characters. Each of these having their own API to allow the user to simplify creation of more complex shapes to allow for each game to have more streamlined graphics development.

## gameConstants.h
