teensyarcade
============

An arcade-oriented converter box for SNES controllers to keyboard/joystick USB devices.

Goals
-----

The goal of this project is to create a turn-key converter box capable of allowing the use of SNES controllers on a PC.

The box will be capable of the following:

* Provide 2 ports for SNES controllers that can operate either has a HID keyboard, or as 2 HID joysticks.
* Work within the Teensyduino IDE environment as a separate board configuration.

The box is being developed using a Teensy 2.0 arduino controller board.

Current Status
--------------

The teensy is mounted to a prototype PCB with 2 7-position screw terminals connected. The screw terminals are used to connect SNES controller extensions cables so that a regular controller can plug in with no modification needed. 1 or 2 controllers can be used. Whether or not they trigger HID gamepad or HID keyboard reports is up to what the Teensy was programmed to output.

How to Use
----------

Currently this software should allow you to operate 2 SNES controllers as a keyboard or 2 gamepads. You can play around with it by doing the following:

1. Clone the repo
2. Navigate to your Arduino IDE installation and then to _Arduino/hardware/teensy_
3. Copy the _cores_ folder, overwriting when asked.
4. Overwrite _boards.txt_
5. Restart the IDE if open

If everything was done correctly, you should have a new board under Teensy 2.0 called *Keyboard + Joysticks (Arcade)* which is the board that this software added.

If you receive errors about missing definitions when compiling, its most likely because _teensy_ cores folder hasn't been updated to include the new board definition.

*snes_keyboard* and *snes_gamepad* are sample files that when downloaded to the teensy, should allow you to hookup a SNES pad as either a keyboard, or a gamepad, respectively.

Enjoy!
