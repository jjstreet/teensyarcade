#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include "usb_common.h"
#include "usb_private.h"
#include "usb_api.h"
#include "wiring.h"

void usb_keyboard_class::pressModifier(uint8_t code) {
	keyboard_report_data[0] |= code;
}

void usb_keyboard_class::releaseModifier(uint8_t code) {
	keyboard_report_data[0] &= ~(code);
}

void usb_keyboard_class::pressKey(uint8_t code) {
	if ((code >> 3) < KBD_SIZE - 2) {
		keyboard_report_data[(code >> 3) + 1] |= 1 << (code & 7);
	}
}

void usb_keyboard_class::releaseKey(uint8_t code) {
	if ((code >> 3) < KBD_SIZE - 2) {
		keyboard_report_data[(code >> 3) + 1] &= ~(1 << (code & 7));
	}
}

void usb_keyboard_class::releaseAll(void) {
	for (uint8_t i = 0; i < KBD_SIZE; i++) {
		keyboard_report_data[i] = 0;
	}
}

void usb_keyboard_class::send(void) {
	uint8_t intr_state, timeout, i;
	if (!usb_configuration)
		return;
	intr_state = SREG;
	cli();
	UENUM = KBD_ENDPOINT;
	timeout = UDFNUML + 50;
	while (1) {
		// are we ready to transmit?
		if (UEINTX & (1 << RWAL))
			break;
		SREG = intr_state;
		// has the USB gone offline?
		if (!usb_configuration)
			return;
		// have we waited too long?
		if (UDFNUML == timeout)
			return;
		// get ready to try checking again
		intr_state = SREG;
		cli();
		UENUM = KBD_ENDPOINT;
	}
	for (i = 0; i < KBD_SIZE; i++) {
		UEDATX = keyboard_report_data[i];
	}
	UEINTX = 0x3A;
	keyboard_idle_count = 0;
	SREG = intr_state;
}

usb_keyboard_class Keyboard = usb_keyboard_class();
