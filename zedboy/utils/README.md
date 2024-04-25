# Utilities

## displayImg.h

## draw.h

### How to use:

First, initialize the OLED screen using openOLED() this returns a file descriptor to allow the other functions to write to the OLED screens character device.

Next, use any set of functions to draw shapes, text, and pizels to draw whatever is desired.

Finally, use the sendBuffer() command with the file descriptor from the openOLED to refresh the screen for the next frame. Also deinitialize the OLED screen when finished using oledClose()

## gameConstants.h
