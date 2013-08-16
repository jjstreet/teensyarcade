#ifndef usb_private_h
#define usb_private_h

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VENDOR_ID					0x16C0
#define PRODUCT_ID					0x0482

#define STR_MANUFACTURER			L"PJRC"
#define STR_PRODUCT					L"Teensy Arcade Controls"

#define ENDPOINT0_SIZE				64

#define PAD1_INTERFACE				0
#define PAD1_ENDPOINT				1
#define PAD1_BUFFER					EP_DOUBLE_BUFFER
#define PAD1_REPORT_SIZE			3								// ;3 Bytes (1 for Buttons 2 for D-Pad)
#define PAD1_REPORT_INTERVAL		2

#define PAD2_INTERFACE				1
#define PAD2_ENDPOINT				2
#define PAD2_BUFFER					EP_DOUBLE_BUFFER
#define PAD2_REPORT_SIZE			3								// ;3 Bytes (1 for Buttons 2 for D-Pad)
#define PAD2_REPORT_INTERVAL		2

#define KBD_INTERFACE				2
#define KBD_ENDPOINT				3
#define KBD_BUFFER					EP_DOUBLE_BUFFER
#define KBD_REPORT_SIZE				16
#define KBD_REPORT_INTERVAL			1
#define KBD_KEY_COUNT				104								// ;104 bits (13 bytes) for key id's

#define NUM_ENDPOINTS				4
#define NUM_INTERFACES				3

void usb_init(void);

void usb_shutdown(void);

extern volatile uint8_t usb_config;
extern volatile uint8_t usb_suspended;

extern uint8_t pad1_report_data[];

extern uint8_t pad2_report_data[];

extern uint8_t kbd_idle_count;
extern uint8_t kbd_report_data[];
extern volatile uint8_t kbd_leds;

#ifdef __cplusplus
}	// extern "C"
#endif

#endif
