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

How to Use
----------

Currently this software is considered very unstable and is not guaranteed to produce results. However, you can play around with it by doing the following:

1. Clone the repo
2. Navigate to your Arduino IDE installation and then to _Arduino/hardware/teensy/cores_
3. Copy the _usb\_arcade_ to the _cores_ directory
4. Overwrite the _boards.txt_ file with the one in the repo
5. Restart the Arduino IDE

If everything was done correctly, you should have a new board under Teensy 2.0 called *Keyboard + Joysticks (Arcade)* which is the board that this software added.

*snes_keyboard* and *snes_gamepad* are sample files that when downloaded to the teensy, should allow you to hookup a SNES pad as either a keyboard, or a gamepad, respectively.

Enjoy!
