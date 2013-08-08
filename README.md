teensyarcade
============

An arcade-oriented converter box for SNES controllers to keyboard/joystick USB devices.

Goals
-----

The goal of this project is to create a turn-key converter box capable of allowing the use of SNES controllers on a PC.

The box will be capable of the following:

* Provide 2 ports for SNES controllers that can operate either has a HID keyboard, or as 2 HID joysticks.
* Work within the Teensyduino IDE environment as a separate board configuration.

The box is being developed using a Teensy 2.0 arduino controller board. A Teensy++ 2.0 may be required for allowing more than 2 controllers depending on available I/O pins.

Current Status
--------------

Currently the box is bread boarded and works with a single SNES controller using a test harness instead of the SNES controller cable. A new Keyboard USB endpoint was created to allow for NKRO of up to 104 simultaneous key presses.
