#ifndef USBarcade_h_
#define USBarcade_h_

#include <inttypes.h>
#include "keycode.h"
#include "gamepad.h"

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

class usb_gamepad_class {
	private:
		uint8_t m_size;
		uint8_t m_endpoint;
		uint8_t m_interface;
		uint8_t * m_data;
		
		usb_gamepad_class() {}
	public:
		usb_gamepad_class(uint8_t interface, uint8_t endpoint, uint8_t size, uint8_t data[]) {
			m_interface = interface;
			m_endpoint = endpoint;
			m_size = size;
			m_data = data;
		};
		void pressButton(uint8_t);
		void releaseButton(uint8_t);
		void setX(uint8_t);
		void setY(uint8_t);
		void releaseAll(void);
		void send(void);
};

extern usb_gamepad_class Gamepad1;

extern usb_gamepad_class Gamepad2;

#endif
