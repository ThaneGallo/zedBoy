# kOLED.ko

The OLED screen needs two different sets of memory mapped to be able to access and write information to it. First, we must follow the OLED screen datasheet for the power on sequence which can be seen below:

![Power-On Sequence](images/poweron.png)




Next the SPI module is required to be set up and initialized as it is the only way to communicate with the OLED given the way the FPGA is set up. 

Creates a character device file labeled zedoled1 uses probe for the SPI interface
