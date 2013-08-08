#ifndef usb_arcade_h__
#define usb_arcade_h__

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

#define VENDOR_ID					0x16C0
#define PRODUCT_ID					0x0482
#define TRANSMIT_FLUSH_TIMEOUT		4						// milliseconds
#define TRASNMIT_TIMEOUT			25						// milliseconds

// Some OSs cache USB device info.
// Alter Vendor or Product IDs or "bcdDevice" revision code to
// force changes

#define STR_PRODUCT					L"Teensy Arcade Controls"
#define ENDPOINT0_SIZE				64

#define KBD_INTERFACE				0
#define KBD_ENDPOINT				1
#define KBD_BUFFER					EP_DOUBLE_BUFFER
#define KBD_SIZE					16
#define KBD_KEY_COUNT				104						// ;104 bits (13 bytes) for key id's
#define KBD_INTERVAL				1

#define PAD1_INTERFACE				1
#define PAD1_ENDPOINT				2
#define PAD1_BUFFER					EP_DOUBLE_BUFFER
#define PAD1_SIZE					3						// ;3 Bytes (2 for D-Pad 1 for Buttons)
#define PAD1_INTERVAL				2

#define PAD2_INTERFACE				2
#define PAD2_ENDPOINT				3
#define PAD2_BUFFER					EP_DOUBLE_BUFFER
#define PAD2_SIZE					3						// ;3 Bytes (2 for D-Pad 1 for Buttons)
#define PAD2_INTERVAL				2

#define DEBUG_INTERFACE				3
#define DEBUG_TX_ENDPOINT			4
#define DEBUG_TX_BUFFER				EP_DOUBLE_BUFFER
#define DEBUG_TX_SIZE				64
#define DEBUG_TX_INTERVAL			1
#define DEBUG_RX_ENDPOINT			5
#define DEBUG_RX_BUFFER				EP_DOUBLE_BUFFER
#define DEBUG_RX_SIZE				32
#define DEBUG_RX_INTERVAL			2

#define NUM_ENDPOINTS				6
#define NUM_INTERFACES				4

void usb_init(void);

void usb_shutdown(void);

extern volatile uint8_t usb_configuration;
extern volatile uint8_t usb_suspended;
extern volatile uint8_t debug_flush_timer;
extern uint8_t keyboard_report_data[];
extern uint8_t keyboard_idle_count;
extern volatile uint8_t keyboard_leds;
extern uint8_t pad1_report_data[];
extern uint8_t pad2_report_data[];

#ifdef __cplusplus
}	// extern "C"
#endif

#endif
