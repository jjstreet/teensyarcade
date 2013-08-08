#include "usb_common.h"
#include "usb_private.h"

static const uint8_t PROGMEM endpoint_cfg_table[] = {
	1, EP_TYPE_INTERRUPT_IN, EP_SIZE(KBD_SIZE) | KBD_BUFFER,				// 1
	1, EP_TYPE_INTERRUPT_IN, EP_SIZE(PAD1_SIZE) | PAD1_BUFFER,				// 2
	1, EP_TYPE_INTERRUPT_IN, EP_SIZE(PAD2_SIZE) | PAD2_BUFFER,				// 3
	1, EP_TYPE_INTERRUPT_IN, EP_SIZE(DEBUG_TX_SIZE) | DEBUG_TX_BUFFER,		// 4
	1, EP_TYPE_INTERRUPT_OUT, EP_SIZE(DEBUG_RX_SIZE) | DEBUG_RX_BUFFER		// 5
};

// Device Descriptor, USB spec 9.6.1
static const uint8_t PROGMEM device_desc[] = {
	0x12,							// bLength,
	0x01,							// bDescriptorType,
	0x00, 0x02,						// bcdUSB,
	0x00,							// bDeviceClass,
	0x00,							// bDeviceSubClass,
	0x00,							// bDeviceProtocol,
	ENDPOINT0_SIZE,					// bMaxPacketSize,							;Control Endpoint
	LSB(VENDOR_ID),					// idVendor(1),
	MSB(VENDOR_ID),					// idVendor(2),
	LSB(PRODUCT_ID),				// idProduct(1),
	MSB(PRODUCT_ID),				// idProduct(2),
	0x05, 0x01,						// bcdDevice,
	0x00,							// iManufacturer,
	0x02,							// iProduct,
	0x00,							// iSerialNumber,
	0x01,							// bNumConfigurations
};

// Keyboard Protocol, HID 1.11 spec NON-BOOT
static const uint8_t PROGMEM kbd_hid_report_desc[] = {
	0x05, 0x01,					// Usage Page (Generic Desktop),
	0x09, 0x06,					// Usage (Keyboard),
	0xA1, 0x01,					// Collection (Application),
	// Modifier Keys
	0x05, 0x07,						// Usage Page (Key Codes),
	0x19, 0xE0,						// Usage Minimum (224),
	0x29, 0xE7,						// Usage Maximum (231),
	0x15, 0x00,						// Logical Minimum (0),
	0x25, 0x01,						// Logical Maximum (1),
	0x75, 0x01,						// Report Size (1),
	0x95, 0x08,						// Report Count (8),
	0x81, 0x02,						// Input (Data, Variable, Absolute),		;Modifier byte (0)
	// LEDS
	0x05, 0x08,						// Usage Page (LEDs),
	0x19, 0x01,						// Usage Minimum (1),
	0x29, 0x05,						// Usage Maximum (5),
	0x75, 0x01,						// Report Size (1),
	0x95, 0x05,						// Report Count (5),
	0x91, 0x02,						// Output (Data, Variable, Absolute),		;LED Report (5/8)
	0x75, 0x03,						// Report Size (3),
	0x95, 0x01,						// Report Count (1),
	0x91, 0x03,						// Output (Constant, Variable, Absolute),	;LED Report padding (8/8)
	// Keys
	0x05, 0x07,						// Usage Page (Key Codes),
	0x19, 0x00,						// Usage Minimum (0),
	0x29, KBD_KEY_COUNT - 1,		// Usage Maximum (103),
	0x15, 0x00,						// Logical Minimum (0),
	0x25, 0x01,						// Logical Maximum (1),
	0x75, 0x01,						// Report Size (1),
	0x95, KBD_KEY_COUNT,			// Report Count (104),
	0x81, 0x02,						// Input (Data, Variable, Absolute),		;Key byte (1-13)
	0x75, 0x08,						// Report Size (8),
	0x95, 0x02,						// Report Count (2),
	0x81, 0x03,						// Input (Constant, Variable, Absolute),	;Key byte padding (14-15)
	0xC0						// End Collection
};

// Gamepad (SNES) Protocol, HID 1.11 spec
static const uint8_t PROGMEM pad_hid_report_desc[] = {
	0x05, 0x01,					// Usage Page (Generic Desktop),
	0x09, 0x05,					// Usage (Gamepad),
	0xA1, 0x01,					// Collection (Application),
	// D-Pad (2bits per X/Y)
	0x09, 0x01,						// Usage (Pointer),							;D-Pad
	0xA1, 0x00,						// Collection (Physical),
	0x09, 0x30,							// Usage (X),
	0x09, 0x31,							// Usage (Y),
	0x15, 0xFF,							// Logical Minimum (0),					;-X/Y
	0x26, 0xFF, 0x00,					// Logical Maximum (255),				;X/Y
	0x75, 0x02,							// Report Size (8),
	0x95, 0x02,							// Report Count (2),
	0x81, 0x02,							// Input (Data, Variable, Absolute),	;D-Pad bytes (2/3)
	0xC0,							// End Collection,
	// 8 Buttons (1bit per button)
	0x05, 0x09,						// Usage Page (Button),
	0x19, 0x01,						// Usage Minimum (Button 1),
	0x29, 0x08,						// Usage Maximum (Button 8),
	0x15, 0x00,						// Logical Minimum (0),
	0x25, 0x01,						// Logical Maximum (1),
	0x75, 0x01,						// Report Size (1),
	0x95, 0x08,						// Report Count (8),						;Buttons 1-8
	0x81, 0x02,						// Input (Constant, Variable, Absolute)		;Button byte (3/3)
	0xC0,						// End Collection
};

// Arduino Serial Protocol
static const uint8_t PROGMEM debug_hid_report_desc[] = {
	0x06, 0xC9, 0xFF,			// Usage Page (0xFFC9),							;Vendor defined
	0x09, 0x04,					// Usage (0x04),
	0xA1, 0x5C,					// Collection (0x5C),
	0x15, 0x00,						// Logical Minimum (0),
	0x26, 0xFF, 0x00,				// Logical Maximum (255),
	0x75, 0x08,						// Report Size (8),
	0x09, 0x75,						// Usage (Local),
	0x95, DEBUG_TX_SIZE,			// Report Count (DEBUG_TX_SIZE),
	0x81, 0x02,						// Input (Data, Variable, Absolute),
	0x09, 0x75,						// Usage (Local),
	0x95, DEBUG_RX_SIZE,			// Report Count (DEBUG_RX_SIZE),
	0x91, 0x02,						// Output (Data, Variable, Absolute),
	0x09, 0x76,						// Usage (Local),
	0x95, 0x04,						// Report Count (4),
	0xB1, 0x02,						// Feature (Data, Variable, Absolute),
	0xC0,						// End Collection
};

#define KBD_HID_DESC_OFFSET			(9 + 9)
#define PAD1_HID_DESC_OFFSET		(9 + 9+9+7 + 9)
#define PAD2_HID_DESC_OFFSET		(9 + 9+9+7 + 9+9+7 + 9)
#define DEBUG_HID_DESC_OFFSET		(9 + 9+9+7 + 9+9+7 + 9+9+7 + 9)
#define CFG1_DESC_SIZE				(9 + 9+9+7 + 9+9+7 + 9+9+7 + 9+9+7+7)


static const uint8_t PROGMEM config1_desc[] = {
	// Configuration 1 Descriptor, USB spec 9.6.3
	0x09,							// bLength,
	0x02,							// bDescriptorType,
	LSB(CFG1_DESC_SIZE),			// wTotalLength(1),
	MSB(CFG1_DESC_SIZE),			// wTotalLength(2),
	NUM_INTERFACES,					// bNumInterfaces,
	0x01,							// bConfigurationValue,
	0x00,							// iConfiguration,
	0x80,							// bmAttributes,
	0x32,							// bMaxPower,								;100mA
	
	// Keyboard (Non-Boot, NKRO)
	
	// Interface Descriptor, USB spec 9.6.5										;Keyboard (Non-Boot, NKRO)
	0x09,							// bLength,
	0x04,							// bDescriptorType,
	KBD_INTERFACE,					// bInterfaceNumber,
	0x00,							// bAlternateSetting,
	0x01,							// bNumEndpoints,
	0x03,							// bInterfaceClass (HID),
	0x00,							// bInterfaceSubClass (None),
	0x00,							// bInterfaceProtocol (None),
	0x00,							// iInterface,
	
	// HID Descriptor, HID 1.11 spec 6.2.1										;Keyboard (Non-Boot, NKRO)
	0x09,							// bLength,
	0x21,							// bDescriptorType,
	0x11, 0x01,						// bcdHID,
	0x00,							// bCountryCode,
	0x01,							// bNumDescriptors,
	0x22,							// bDescriptorType,
	sizeof(kbd_hid_report_desc),	// wDescriptorLength(1),
	0x00,							// wDescriptorLength(2),
	
	// Endpoint Descriptor, USB spec 9.6.6										;Keyboard (Non-Boot, NKRO)
	0x07,							// bLength,
	0x05,							// bDescriptorType,
	KBD_ENDPOINT | 0x80,			// bEndpointAddress (Input),
	0x03,							// bmAttributes (Interrupt),
	KBD_SIZE, 0x00,					// wMaxPacketSize,
	KBD_INTERVAL,					// bInterval,
	
	// Pad 1 (Gamepad)
	
	// Interface Descriptor, USB spec 9.6.5										;Pad 1 (Gamepad)
	0x09,							// bLength,
	0x04,							// bDescriptorType,
	PAD1_INTERFACE,					// bInterfaceNumber,
	0x00,							// bAlternateSetting,
	0x01,							// bNumEndpoints,
	0x03,							// bInterfaceClass (HID),
	0x00,							// bInterfaceSubClass (None),
	0x00,							// bInterfaceProtocol (None),
	0x00,							// iInterface,
	
	// HID Descriptor, HID 1.11 spec 6.2.1										;Pad 1 (Gamepad)
	0x09,							// bLength,
	0x21,							// bDescriptorType,
	0x11, 0x01,						// bcdHID,
	0x00,							// bCountryCode,
	0x01,							// bNumDescriptors,
	0x22,							// bDescriptorType,
	sizeof(pad_hid_report_desc),	// wDescriptorLength(1),
	0x00,							// wDescriptorLength(2),
	
	// Endpoint Descriptor, USB spec 9.6.6										;Pad 1 (Gamepad)
	0x07,							// bLength,
	0x05,							// bDescriptorType,
	PAD1_ENDPOINT | 0x80,			// bEndpointAddress (Input),
	0x03,							// bmAttributes (Interrupt),
	PAD1_SIZE, 0x00,				// wMaxPacketSize,
	PAD1_INTERVAL,					// bInterval,
	
	// Pad 2 (Gamepad)
	
	// Interface Descriptor, USB spec 9.6.5										;Pad 2 (Gamepad)
	0x09,							// bLength,
	0x04,							// bDescriptorType,
	PAD2_INTERFACE,					// bInterfaceNumber,
	0x00,							// bAlternateSetting,
	0x01,							// bNumEndpoints,
	0x03,							// bInterfaceClass (HID),
	0x00,							// bInterfaceSubClass (None),
	0x00,							// bInterfaceProtocol (None),
	0x00,							// iInterface,
	
	// HID Descriptor, HID 1.11 spec 6.2.1										;Pad 2 (Gamepad)
	0x09,							// bLength,
	0x21,							// bDescriptorType,
	0x11, 0x01,						// bcdHID,
	0x00,							// bCountryCode,
	0x01,							// bNumDescriptors,
	0x22,							// bDescriptorType,
	sizeof(pad_hid_report_desc),	// wDescriptorLength(1),
	0x00,							// wDescriptorLength(2),
	
	// Endpoint Descriptor, USB spec 9.6.6										;Pad 2 (Gamepad)
	0x07,							// bLength,
	0x05,							// bDescriptorType,
	PAD2_ENDPOINT | 0x80,			// bEndpointAddress (Input),
	0x03,							// bmAttributes (Interrupt),
	PAD2_SIZE, 0x00,				// wMaxPacketSize,
	PAD2_INTERVAL,					// bInterval,
	
	// Debugging
	
	// Interface Descriptor, USB spec 9.6.5										;Debugging
	0x09,							// bLength,
	0x04,							// bDescriptorType,
	DEBUG_INTERFACE,				// bInterfaceNumber,
	0x00,							// bAlternateSetting,
	0x02,							// bNumEndpoints,
	0x03,							// bInterfaceClass (HID),
	0x00,							// bInterfaceSubClass (None),
	0x00,							// bInterfaceProtocol (None),
	0x00,							// iInterface,
	
	// HID Descriptor, HID 1.11 spec 6.2.1										;Debugging
	0x09,							// bLength,
	0x21,							// bDescriptorType,
	0x11, 0x01,						// bcdHID,
	0x00,							// bCountryCode,
	0x01,							// bNumDescriptors,
	0x22,							// bDescriptorType,
	sizeof(debug_hid_report_desc),	// wDescriptorLength(1),
	0x00,							// wDescriptorLength(2),
	
	// Endpoint Descriptor, USB spec 9.6.6										;Debugging (TX)
	0x07,							// bLength,
	0x05,							// bDescriptorType,
	DEBUG_TX_ENDPOINT | 0x80,		// bEndpointAddress,
	0x03,							// bmAttributes (Interrupt),
	DEBUG_TX_SIZE, 0x00,			// wMaxPacketSize,
	DEBUG_TX_INTERVAL,				// bInterval,
	
	// Endpoint Descriptor, USB spec 9.6.6										;Debugging (RX)
	0x07,							// bLength,
	0x05,							// bDescriptorType,
	DEBUG_RX_ENDPOINT,				// bEndpointAddress,
	0x03,							// bmAttributes (Interrupt),
	DEBUG_RX_SIZE, 0x00,			// wMaxPacketSize,
	DEBUG_RX_INTERVAL,				// bInterval,
};

struct usb_str_desc_struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	int16_t wString[];
};

static const struct usb_str_desc_struct PROGMEM str0 = {
	4,
	3,
	{0x0409}
};

static const struct usb_str_desc_struct PROGMEM str1 = {
	sizeof(STR_PRODUCT),
	3,
	STR_PRODUCT
};

// Descriptor Data Table
// Determines which data is sent for each specific request from host
static const struct descriptor_list_struct {
	uint16_t		wValue;
	uint16_t		vIndex;
	const uint8_t	*addr;
	uint8_t			length;
} PROGMEM descriptor_list[] = {
	{0x0100, 0x0000, device_desc, sizeof(device_desc)},									// Device
	{0x0200, 0x0000, config1_desc, sizeof(config1_desc)},								// Configuration
	{0x2200, KBD_INTERFACE, kbd_hid_report_desc, sizeof(kbd_hid_report_desc)},			// HID/Report
	{0x2100, KBD_INTERFACE, config1_desc + KBD_HID_DESC_OFFSET, 9},						// HID/Report
	{0x2200, PAD1_INTERFACE, pad_hid_report_desc, sizeof(pad_hid_report_desc)},			// HID/Report
	{0x2100, PAD1_INTERFACE, config1_desc + PAD1_HID_DESC_OFFSET, 9},					// HID/Report
	{0x2200, PAD2_INTERFACE, pad_hid_report_desc, sizeof(pad_hid_report_desc)},			// HID/Report
	{0x2100, PAD2_INTERFACE, config1_desc + PAD2_HID_DESC_OFFSET, 9},					// HID/Report
	{0x2200, DEBUG_INTERFACE, debug_hid_report_desc, sizeof(debug_hid_report_desc)},	// HID/Report
	{0x2100, DEBUG_INTERFACE, config1_desc + DEBUG_HID_DESC_OFFSET, 9},					// HID/Report
	{0x0300, 0x0000, (const uint8_t *)&str0, 4},										// String
	{0x0301, 0x0409, (const uint8_t *)&str1, sizeof(STR_PRODUCT)},						// String
};

#define NUM_DESC_LIST (sizeof(descriptor_list) / sizeof(struct descriptor_list_struct))

// When not configured:		Zero
// When enumerated:			Non-Zero
volatile uint8_t usb_configuration USBSTATE;

volatile uint8_t usb_suspended USBSTATE;

// Time remaining before transmission of partial packet
// or a zero-length packet
volatile uint8_t debug_flush_timer USBSTATE;

// Keyboard Report Data
// 0		Bitmap of modifier keys
// 1 - 13	Bitmap of keycodes (0 - 103)
// 14 - 15  Not used
uint8_t keyboard_report_data[KBD_SIZE] USBSTATE;

// Count until idle timeout
uint8_t keyboard_idle_count USBSTATE;

// Storage for current led state
// 1	Num Lock
// 2	Caps Lock
// 4	Scroll Lock
// 8	Compose
// 16	Kana
volatile uint8_t keyboard_leds USBSTATE;

// Protocol setting for the host. Used to report which setting
// is in use.
static uint8_t keyboard_protocol USBSTATE;

// How often report is sent to host even when no changes
// have been made (ms * 4)
static uint8_t keyboard_idle_config USBSTATE;

// Pad 1 Report Data
// 0-1		X
// 2-3		Y
// 4-7		Not Used
// 8-15		Buttons 1-8
uint8_t pad1_report_data[PAD1_SIZE] USBSTATE;

// Pad 2 Report Data
// 0-1		X
// 2-3		Y
// 4-7		Not Used
// 8-15		Buttons 1-8
uint8_t pad2_report_data[PAD2_SIZE] USBSTATE;



void usb_init(void) {
	uint8_t u, i;
	
	u = USBCON;
	if ((u & (1 << USBE)) && !(u & (1 << FRZCLK)))
		return;
	HW_CONFIG();
	USB_FREEZE();							// Enable USB
	PLL_CONFIG();							// Configure PLL
	while (!(PLLCSR & (1 << PLOCK))) ;		// Wait for PLL lock
	USB_CONFIG();							// Start USB clock
	UDCON = 0;								// Connect attach resistor
	
	usb_configuration = 0;
	usb_suspended = 0;
	debug_flush_timer = 0;
	
	for (i = 0; i < KBD_SIZE; i++) {
		keyboard_report_data[i] = 0;
	}
	keyboard_idle_count = 0;
	keyboard_leds = 0;
	
	keyboard_protocol = 1;
	keyboard_idle_config = 125;
	
	for (i = 0; i < PAD1_SIZE; i++) {
		pad1_report_data[i] = 0;
	}
	
	for (i = 0; i < PAD2_SIZE; i++) {
		pad2_report_data[i] = 0;
	}
	
	
	UDINT = 0;
	UDIEN = (1 << EORSTE) | (1 << SOFE);
	
	// sei();								// init() in wiring.c handles this
}

void usb_shutdown(void) {
	UDIEN = 0;								// Disable interrupts
	UDCON = 1;								// Disconnect attach resistor
	USBCON = 0;								// Shut off USB peripheral
	PLLCSR = 0;								// Shut off PLL
	usb_configuration = 0;
	usb_suspended = 1;
}

// USB Device Interrupt
// Handles all device-level events
// Transmission buffer flush is triggered by start of frame
ISR(USB_GEN_vect) {
	uint8_t intbits, t, i;
	static uint8_t div4 = 0;
	
	intbits = UDINT;
	UDINT = 0;
	if (intbits & (1 << EORSTI)) {
		UENUM = 0;
		UECONX = 1;
		UECFG0X = EP_TYPE_CONTROL;
		UECFG1X = EP_SIZE(ENDPOINT0_SIZE) | EP_SINGLE_BUFFER;
		UEIENX = (1 << RXSTPE);
		usb_configuration = 0;
	}
	if ((intbits & (1 << SOFI)) && usb_configuration) {
		t = debug_flush_timer;
		if (t) {
			debug_flush_timer = -- t;
			if (!t) {
				UENUM = DEBUG_TX_ENDPOINT;
				while((UEINTX & (1 << RWAL))) {
					UEDATX = 0;
				}
				UEINTX = 0x3A;
			}
		}
		if (keyboard_idle_config && (++div4 & 3) == 0) {
			UENUM = KBD_ENDPOINT;
			if (UEINTX & (1 << RWAL)) {
				keyboard_idle_count++;
				if (keyboard_idle_count = keyboard_idle_config) {
					keyboard_idle_count = 0;
					for (i = 0; i < KBD_SIZE; i++) {
						UEDATX = keyboard_report_data[i];
					}
					UEINTX = 0x3A;
				}
			}
		}
	}
	if (intbits & (1 << SUSPI)) {
		// USB Suspend (inactivity for 3ms)
		UDIEN = (1 << WAKEUPE);
		usb_configuration = 1;
		usb_suspended = 1;
		#if (F_CPU >= 8000000L)
		// WAKEUPI does not work with USB clock freeze at
		// CPU freq. < 8Mhz
		USB_FREEZE();						// Shut off USB
		PLLCSR = 0;							// Shut off PLL
		#endif
	}
	if (usb_suspended && (intbits & (1 << WAKEUPI))) {
		// USB Resume (almost any activity)
		#if (F_CPU >= 8000000L)
		PLL_CONFIG();
		while (!(PLLCSR & (1 << PLOCK))) ;	// Wait for PLL Lock
		USB_CONFIG();
		#endif
		UDIEN = (1 << EORSTE) | (1 << SOFE) | (1 << SUSPE);
		usb_suspended = 0;
	}
}

static inline void usb_wait_in_ready(void) {
	while (!(UEINTX & (1 << TXINI))) ;
}

static inline void usb_send_in(void) {
	UEINTX = ~(1 << TXINI);
}

static inline void usb_wait_receive_out(void) {
	while (!(UEINTX & (1 << RXOUTI))) ;
}

static inline void usb_ack_out(void) {
	UEINTX = ~(1 << RXOUTI);
}

// USB Endpoint 0 Interrupt
// Other endpoint interrupts are handled by api
ISR(USB_COM_vect) {
	uint8_t intbits;
	const uint8_t *list;
	const uint8_t *cfg;
	uint8_t i, n, len, en;
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
	uint16_t desc_val;
	const uint8_t *desc_addr;
	uint8_t	desc_length;
	
	UENUM = 0;
	intbits = UEINTX;
	if (intbits & (1 << RXSTPI)) {
		bmRequestType = UEDATX;
		bRequest = UEDATX;
		read_word_lsbfirst(wValue, UEDATX);
		read_word_lsbfirst(wIndex, UEDATX);
		read_word_lsbfirst(wLength, UEDATX);
		UEINTX = ~((1 << RXSTPI) | (1 << RXOUTI) | (1 << TXINI));
		if (bRequest == GET_DESCRIPTOR) {
			list = (const uint8_t *) descriptor_list;
			for (i = 0; ;i++) {
				if (i >= NUM_DESC_LIST) {
					UECONX = (1<<STALLRQ)|(1<<EPEN);  //stall
					return;
				}
				pgm_read_word_postinc(desc_val, list);
				if (desc_val != wValue) {
					list += sizeof(struct descriptor_list_struct) - 2;
					continue;
				}
				pgm_read_word_postinc(desc_val, list);
				if (desc_val != wIndex) {
					list += sizeof(struct descriptor_list_struct) - 4;
					continue;
				}
				pgm_read_word_postinc(desc_addr, list);
				desc_length = pgm_read_byte(list);
				break;
			}
			len = (wLength < 256) ? wLength : 255;
			if (len > desc_length)
				len = desc_length;
			list = desc_addr;
			do {
				// wait for host ready for IN packet
				do {
					i = UEINTX;
				}
				while (!(i & ((1 << TXINI) | (1 << RXOUTI))));
				if (i & (1 << RXOUTI))
					return;	// abort
				// Send IN packet
				n = len < ENDPOINT0_SIZE ? len : ENDPOINT0_SIZE;
				for (i = n; i; i--) {
					pgm_read_byte_postinc(UEDATX, list);
				}
				len -= n;
				usb_send_in();
			}
			while (len || n == ENDPOINT0_SIZE);
			return;
		}
		if (bRequest == SET_ADDRESS) {
			usb_send_in();
			usb_wait_in_ready();
			UDADDR = wValue | (1<<ADDEN);
			return;
		}
		if (bRequest == SET_CONFIGURATION && bmRequestType == 0) {
			usb_configuration = wValue;
			debug_flush_timer = 0;
			usb_send_in();
			cfg = endpoint_cfg_table;
			for (i = 1; i < NUM_ENDPOINTS; i++) {
				UENUM = i;
				pgm_read_byte_postinc(en, cfg);
				UECONX = en;
				if (en) {
					pgm_read_byte_postinc(UECFG0X, cfg);
					pgm_read_byte_postinc(UECFG1X, cfg);
				}
			}
			UERST = 0x1E;
			UERST = 0;
			return;
		}
		if (bRequest == GET_CONFIGURATION && bmRequestType == 0x80) {
			usb_wait_in_ready();
			UEDATX = usb_configuration;
			usb_send_in();
			return;
		}
		if (bRequest == GET_STATUS) {
			usb_wait_in_ready();
			i = 0;
			if (bmRequestType == 0x82) {
				UENUM = wIndex;
				if (UECONX & (1 << STALLRQ))
					i = 1;
				UENUM = 0;
			}
			UEDATX = i;
			UEDATX = 0;
			usb_send_in();
			return;
		}
		if ((bRequest == CLEAR_FEATURE || bRequest == SET_FEATURE)
				 && bmRequestType == 0x02
				 && wValue == 0) {
			i = wIndex & 0x7F;
			if (i >= 1 && i <= NUM_ENDPOINTS) {
				usb_send_in();
				UENUM = i;
				if (bRequest == SET_FEATURE) {
					UECONX = (1 << STALLRQ) | (1 << EPEN);
				}
				else {
					UECONX = (1 << STALLRQC) | (1 << RSTDT)|(1 << EPEN);
					UERST = (1 << i);
					UERST = 0;
				}
				return;
			}
		}
		
		// Keyboard
		
		if (wIndex == KBD_INTERFACE) {
			if (bmRequestType == 0xA1) {
				if (bRequest == HID_GET_REPORT) {
					 usb_wait_in_ready();
					 for (i = 0; i < KBD_SIZE; i++) {
						UEDATX = keyboard_report_data[i];
					 }
					 usb_send_in();
					 return;
				}
				if (bRequest == HID_GET_IDLE) {
					usb_wait_in_ready();
					UEDATX = keyboard_idle_config;
					usb_send_in();
					return;
				}
				if (bRequest == HID_GET_PROTOCOL) {
					usb_wait_in_ready();
					UEDATX = keyboard_protocol;
					usb_send_in();
					return;
				}
			}
			if (bmRequestType == 0x21) {
				if (bRequest == HID_SET_REPORT) {
					usb_wait_receive_out();
					keyboard_leds = UEDATX;
					usb_ack_out();
					usb_send_in();
					return;
				}
				if (bRequest == HID_SET_IDLE) {
					keyboard_idle_config = (wValue >> 8);
					keyboard_idle_count = 0;
					usb_send_in();
					return;
				}
				if (bRequest == HID_SET_PROTOCOL) {
					keyboard_protocol = wValue;
					usb_send_in();
					return;
				}
			}
		}
		
		// Gamepads
		
		// Pad 1
		if (wIndex == PAD1_INTERFACE) {
			if (bmRequestType = 0xA1) {
				if (bRequest == HID_GET_REPORT) {
					usb_wait_in_ready();
					for (i = 0; i < PAD1_SIZE; i++) {
						UEDATX = pad1_report_data[i];
					}
					usb_send_in();
					return;
				}
			}
		}
		
		// Pad 2
		if (wIndex == PAD2_INTERFACE) {
			if (bmRequestType = 0xA1) {
				if (bRequest == HID_GET_REPORT) {
					usb_wait_in_ready();
					for (i = 0; i < PAD2_SIZE; i++) {
						UEDATX = pad2_report_data[i];
					}
					usb_send_in();
					return;
				}
			}
		}
		
		// Debugging
		
		if (wIndex == DEBUG_INTERFACE) {
			if (bRequest == HID_GET_REPORT && bmRequestType == 0xA1) {
				len = wLength;
				do {
					// wait for host ready for IN packet
					do {
						i = UEINTX;
					}
					while (!(i & ((1 << TXINI) | (1 << RXOUTI))));
					if (i & (1 << RXOUTI))
						return;    // abort
					// send IN packet
					n = len < ENDPOINT0_SIZE ? len : ENDPOINT0_SIZE;
					for (i = n; i; i--) {
						UEDATX = 0;
					}
					len -= n;
					usb_send_in();
				}
				while (len || n == ENDPOINT0_SIZE);
				return;
			}
			if (bRequest == HID_SET_REPORT && bmRequestType == 0x21) {
				if (wValue == 0x0300 && wLength == 0x0004) {
					uint8_t b1, b2, b3, b4;
					usb_wait_receive_out();
					b1 = UEDATX;
					b2 = UEDATX;
					b3 = UEDATX;
					b4 = UEDATX;
					usb_ack_out();
					usb_send_in();
					if (b1 == 0xA9 && b2 == 0x45 && b3 == 0xC2 && b4 == 0x6B)
						_reboot_Teensyduino_();
					if (b1 == 0x8B && b2 == 0xC5 && b3 == 0x1D && b4 == 0x70)
						_restart_Teensyduino_();
				}
			}
		}
		if (bRequest == 0xC9 && bmRequestType == 0x40) {
			usb_send_in();
			usb_wait_in_ready();
			_restart_Teensyduino_();
		}
	}
	UECONX = (1 << STALLRQ) | (1 << EPEN);	// stall
}
