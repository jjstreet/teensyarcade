#ifndef USBarcade_h_
#define USBarcade_h_

#include <inttypes.h>
#include "keycode.h"

class usb_keyboard_class {
	public:
		void pressModifier(uint8_t);
		void releaseModifier(uint8_t);
		void pressKey(uint8_t);
		void releaseKey(uint8_t);
		void releaseAll(void);
		void send(void);
};

extern usb_keyboard_class Keyboard;

#endif
