
What is it?

This is the continuation of my Video Display Shields.

The Pico Video4 Display is a stand-alone RPi Pico RP2040 board, connected through USB-C. This board provides up to 4 analog composite video display interfaces with integrated frame buffer memory accessible through SPI. The 4 video outputs are accessible through 1 RCA connector and 1 VGA DB15-HD connector that uses the red, green and blue channels for the composite signal. The heart of this design is the VLSI VS23S040 chip. PAL and NTSC output formats are supported. Currently 4 resolutions are implemented: NTSC 320x200 or 426x200 with 256 colours and PAL 300x240 or 500x240 with 256 colours. The chip has a 1Mbit framebuffer per channel, unused memory can be used for graphics tiles, which can be copied into the image data by the internal fast memory block move hardware.

It has 8 inputs that are voltage compatible to the input voltage; up to 12 V.

2 IOs of the VS23S040 chip are routed to the HSYNC/VSYNC pins of the VGA connector, to enable an experimental VGA display in the future. There is no guarantee for this to work and no software is available for this mode at the moment. Please contact me for further details.

A 16Mbit (16MByte) SPI FLASH memory is available on-board. It is pre-loaded with a character bitmap consisting of 94 character ( ASCII code 33-126), a bouncing ball demo and 9 demo images. A 4-pin QWIIK compatible connector is on board to connect to any I2C device.

Specifications:

    Operating supply voltage 5.0 V – 12 V
    Raspberry Pi Pico with 16 Mbit Program Flash memory
    USB-C connector for 5 V power and communication/programming
    SWdebug header
    4x Composite Video Output
    Maximum resolution 720x576 in 4 colours
    Implemented resolutions: NTSC 320x200 or 426x200 with 256 colours and PAL 300x240 or 500x240 with 256 colours
    Crystal: NTSC 3.579545 MHz or PAL 4.43618 MHz
    Communication interface: SPI up to 38 MHz
    Video Frame Buffer: 4x 1 Mbit = 4x 128 KByte
    Data Flash: 16 Mbit = 2 Mbyte
    QWIIK I2C connector
    8 inputs, high voltage up to input voltage level 5-12 V, with LED indicator
    8 dip switches to simulate the inputs
    LED and 2-pin jumper for status and control
    Power output, switched and fused with 100mA polyfuse
    TH prototyping area
    Size: 85mm x 100mm (3.3" x 3.9")

Why did you make it?

I wanted to provide an upgraded device to provide full format display capabilities to the PaspberryPi PR2040 world and make use of all the analog video displays and composite video inputs that are still available on many LCD TV. This board is the progression of my 2 Arduino Shields NTSC/PAL Video Display Shield {https://hackaday.io/project/21097-ntscpal-video-display-shield} and VGA Shield Wing {https://hackaday.io/project/170499-vga-shield-wing}.
What makes it special?

This is an option to provide video capabilities for the RP2040 with 4 outputs, that uses a dedicated framebuffer + hardware to generate the video signal. This keeps the controller free for its main tasks and does display updates through fast SPI transfers. There are a lot of solutions out there that use the microcontroller itself and its internal timing and memory peripherals for video signal generation. Those take valuable resource and computing power away from the microcontroller.
