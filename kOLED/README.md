# kOLED.ko

## Setup
The OLED screen needs two different sets of memory mapped to be able to access and write information to it. First, we must follow the OLED screen datasheet for the power on sequence which can be seen below:

![Power-On Sequence](images/PowerOn.png)

We edit these values using the corresponding GPIO peripheral on the board and are able to send commands once it is initilized.

Next the SPI module is required to be set up and initialized as it is the only way to communicate with the OLED given the way the FPGA is set up. We need to send the set of setup commands to the OLED using the character device set up earlier depening on the state of the data/command pin. The user is not able to send commands to the OLED after the setup of the module to prevent any unexpected errors. 

An array written to the character device is the size of the number of bytes of the GDDRAM (512). This array fills the RAM with the state of each pixel with each byte controllin 8. In order to simplify the drawing for the user a draw library is implimented.

## How to use

First, you must make and insert the kernel module 
```bash
insmod kOLED.ko
```

Then the draw library opens the OLED device and allows for filling a userspace buffer which is then sent each frame drawing what is desired. It currently draws at a rate of around 
