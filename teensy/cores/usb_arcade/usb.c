#include "usb_common.h"
#include "usb_private.h"

static const uint8_t PROGMEM endpoint_cfg_table[] = {
	EP_TYPE_INTERRUPT_IN,	EP_SIZE(PAD1_REPORT_SIZE) | PAD1_BUFFER,			// 1
	EP_TYPE_INTERRUPT_IN,	EP_SIZE(PAD2_REPORT_SIZE) | PAD2_BUFFER,			// 2
	EP_TYPE_INTERRUPT_IN,	EP_SIZE(KBD_REPORT_SIZE) | KBD_BUFFER				// 3
};

// Device Descriptor, USB Spec 9.6.1
static const uint8_t PROGMEM device_desc[] = {
	0x12,											// bLength,
	0x01,											// bDescriptorType,
	0x00, 0x02,										// bcdUSB,
	0x00,											// bDeviceClass,
	0x00,											// bDeviceSubClass,
	0x00,											// bDeviceProtocol,
	ENDPOINT0_SIZE,									// bMaxPacketSize0,
	LSB(VENDOR_ID), MSB(VENDOR_ID),					// idVendor,
	LSB(PRODUCT_ID), MSB(PRODUCT_ID),				// idProduct,
	0x02, 0x01,										// bcdDevice,
	0x01,											// iManufacturer,
	0x02,											// iProduct,
	0x00,											// iSerialNumber,
	0x01											// bNumConfigurations
};

// Gamepad (SNES) Protocol, HID 1.11 spec
static const uint8_t PROGMEM pad_hid_report_desc[] = {
	0x05, 0x01,										// Usage Page (Generic Desktop),
	0x09, 0x05,										// Usage (Gamepad),
	0xA1, 0x01,										// Collection (Application),
	0xA1, 0x00,											// Collection (Physical),
	0x05, 0x09,												// Usage Page (Button),
	0x19, 0x01,												// Usage Minimum (Button 1),
	0x29, 0x08,												// Usage Maximum (Button 8),
	0x15, 0x00,												// Logical Minimum (0),
	0x25, 0x01,												// Logical Maximum (1),
	0x75, 0x01,												// Report Size (1),
	0x95, 0x08,												// Report Count (8),
	0x81, 0x02,												// Input (Data, Variable, Absolute),
	0x05, 0x01,												// Usage Page (Generic Desktop),
	0x09, 0x01,												// Usage (Pointer),
	0xA1, 0x00,												// Collection (Physical),
	0x09, 0x30,													// Usage (X),
	0x09, 0x31,													// Usage (Y),
	0x15, 0x00,													// Logical Minimum (0),
	0x25, 0x02,													// Logical Maximum (2),
	0x75, 0x08,													// Report Size (8),
	0x95, 0x02,													// Report Count (2),
	0x81, 0x02,													// Input (Data, Variable, Absolute),
	0xC0,													// End Collection,
	0xC0,												// End Collection,
	0xC0											// End Collection
};

// Keyboard Protocol, HID 1.11 Spec NON-BOOT
static const uint8_t PROGMEM kbd_hid_report_desc[] = {
	0x05, 0x01,										// Usage Page (Generic Desktop),
	0x09, 0x06,										// Usage (Keyboard),
	0xA1, 0x01,										// Collection (Application),
	// Modifier Keys
	0x05, 0x07,											// Usage Page (Key Codes),
	0x19, 0xE0,											// Usage Minimum (224),
	0x29, 0xE7,											// Usage Maximum (231),
	0x15, 0x00,											// Logical Minimum (0),
	0x25, 0x01,											// Logical Maximum (1),
	0x75, 0x01,											// Report Size (1),
	0x95, 0x08,											// Report Count (8),
	0x81, 0x02,											// Input (Data, Variable, Absolute),				;Modifier byte (0)
	// LEDS
	0x05, 0x08,											// Usage Page (LEDs),
	0x19, 0x01,											// Usage Minimum (1),
	0x29, 0x05,											// Usage Maximum (5),
	0x75, 0x01,											// Report Size (1),
	0x95, 0x05,											// Report Count (5),
	0x91, 0x02,											// Output (Data, Variable, Absolute),				;LED Report (5/8)
	0x75, 0x03,											// Report Size (3),
	0x95, 0x01,											// Report Count (1),
	0x91, 0x03,											// Output (Constant, Variable, Absolute),			;LED Report padding (8/8)
	// Keys
	0x05, 0x07,											// Usage Page (Key Codes),
	0x19, 0x00,											// Usage Minimum (0),
	0x29, KBD_KEY_COUNT - 1,							// Usage Maximum (103),
	0x15, 0x00,											// Logical Minimum (0),
	0x25, 0x01,											// Logical Maximum (1),
	0x75, 0x01,											// Report Size (1),
	0x95, KBD_KEY_COUNT,								// Report Count (104),
	0x81, 0x02,											// Input (Data, Variable, Absolute),				;Key byte (1-13)
	0x75, 0x08,											// Report Size (8),
	0x95, 0x02,											// Report Count (2),
	0x81, 0x03,											// Input (Constant, Variable, Absolute),			;Key byte padding (14-15)
	0xC0											// End Collection
};

#define PAD1_HID_DESC_OFFSET			(9 + 9)
#define PAD2_HID_DESC_OFFSET			(9 + 9+9+7 + 9)
#define KBD_HID_DESC_OFFSET				(9 + 9+9+7 + 9+9+7 + 9)
#define CFG1_DESC_SIZE					(9 + 9+9+7 + 9+9+7 + 9+9+7)

// Configuration/Interface/HID/Endpoint Descriptor
static const uint8_t PROGMEM cfg1_desc[] = {
	// Configuration Descriptor, USB Spec 9.6.3
	0x09,											// bLength
	0x02,											// bDescriptorType,
	LSB(CFG1_DESC_SIZE), MSB(CFG1_DESC_SIZE),		// wTotalLength,
	NUM_INTERFACES,									// bNumInterfaces,
	0x01,											// bConfigurationValue,
	0x00,											// iConfiguration,
	0x80,											// bmAttributes,
	0x32,											// bMaxPower											;100mA
	
	// Pad 1 (Gamepad)
	
	// Interface Descriptor, USB Spec 9.6.5																	;Pad 1 (Gamepad)
	0x09,											// bLength,
	0x04,											// bDescriptorType,
	PAD1_INTERFACE,									// bInterfaceNumber,
	0x00,											// bAlternateSetting,
	0x01,											// bNumEndpoints,
	0x03,											// bInterfaceClass (HID),
	0x00,											// bInterfaceSubClass (None),
	0x00,											// bInterfaceProtocol (None),
	0x00,											// iInterface,
	
	// HID Descriptor, HID 1.11 Spec 6.2.1																	;Pad 1 (Gamepad)
	0x09,											// bLength,
	0x21,											// bDescriptorType,
	0x11, 0x01,										// bcdHID,
	0x00,											// bCountryCode,
	0x01,											// bNumDescriptors,
	0x22,											// bDescriptorType,
	sizeof(pad_hid_report_desc), 0x00,				// wDescriptorLength,
	
	// Endpoint Descriptor, USB Spec 9.6.6																	;Pad 1 (Gamepad)
	0x07,											// bLength,
	0x05,											// bDescriptorType,
	PAD1_ENDPOINT | 0x80,							// bEndpointAddress (Input),
	0x03,											// bmAttributes (Interrupt),
	PAD1_REPORT_SIZE, 0x00,							// wMaxPacketSize,
	PAD1_REPORT_INTERVAL,							// bInterval,
	
	// Pad 2 (Gamepad)
	
	// Interface Descriptor, USB Spec 9.6.5																	;Pad 2 (Gamepad)
	0x09,											// bLength,
	0x04,											// bDescriptorType,
	PAD2_INTERFACE,									// bInterfaceNumber,
	0x00,											// bAlternateSetting,
	0x01,											// bNumEndpoints,
	0x03,											// bInterfaceClass (HID),
	0x00,											// bInterfaceSubClass (None),
	0x00,											// bInterfaceProtocol (None),
	0x00,											// iInterface,
	
	// HID Descriptor, HID 1.11 Spec 6.2.1																	;Pad 2 (Gamepad)
	0x09,											// bLength,
	0x21,											// bDescriptorType,
	0x11, 0x01,										// bcdHID,
	0x00,											// bCountryCode,
	0x01,											// bNumDescriptors,
	0x22,											// bDescriptorType,
	sizeof(pad_hid_report_desc), 0x00,				// wDescriptorLength,
	
	// Endpoint Descriptor, USB Spec 9.6.6																	;Pad 2 (Gamepad)
	0x07,											// bLength,
	0x05,											// bDescriptorType,
	PAD2_ENDPOINT | 0x80,							// bEndpointAddress (Input),
	0x03,											// bmAttributes (Interrupt),
	PAD2_REPORT_SIZE, 0x00,							// wMaxPacketSize,
	PAD2_REPORT_INTERVAL,							// bInterval
	
	// Keyboard (Non-Boot, NKRO)
	
	// Interface Descriptor, USB Spec 9.6.5																	;Keyboard (Non-Boot, NKRO)
	0x09,											// bLength,
	0x04,											// bDescriptorType,
	KBD_INTERFACE,									// bInterfaceNumber,
	0x00,											// bAlternateSetting,
	0x01,											// bNumEndpoints,
	0x03,											// bInterfaceClass (HID),
	0x00,											// bInterfaceSubClass (None),
	0x00,											// bInterfaceProtocol (None),
	0x00,											// iInterface,
	
	// HID Descriptor, HID 1.11 Spec 6.2.1																	;Keyboard (Non-Boot, NKRO)
	0x09,											// bLength,
	0x21,											// bDescriptorType,
	0x11, 0x01,										// bcdHID,
	0x00,											// bCountryCode,
	0x01,											// bNumDescriptors,
	0x22,											// bDescriptorType,
	sizeof(kbd_hid_report_desc), 0x00,				// wDescriptorLength,
	
	// Endpoint Descriptor, USB Spec 9.6.6																	;Keyboard (Non-Boot, NKRO)
	0x07,											// bLength,
	0x05,											// bDescriptorType,
	KBD_ENDPOINT | 0x80,							// bEndpointAddress (Input),
	0x03,											// bmAttributes (Interrupt),
	KBD_REPORT_SIZE, 0x00,							// wMaxPacketSize,
	KBD_REPORT_INTERVAL								// bInterval
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
	sizeof(STR_MANUFACTURER),
	3,
	STR_MANUFACTURER
};

static const struct usb_str_desc_struct PROGMEM str2 = {
	sizeof(STR_PRODUCT),
	3,
	STR_PRODUCT
};

static const struct descriptor_list_struct {
	uint16_t	wValue;
	uint16_t	wIndex;
	const uint8_t	*addr;
	uint8_t		length;
} PROGMEM descriptor_list[] = {
	{0x0100, 0x0000, device_desc, sizeof(device_desc)},										// Device
	{0x0200, 0x0000, cfg1_desc, sizeof(cfg1_desc)},											// Configuration
		{0x2200, PAD1_INTERFACE, pad_hid_report_desc, sizeof(pad_hid_report_desc)},			// HID/Report
		{0x2100, PAD1_INTERFACE, cfg1_desc + PAD1_HID_DESC_OFFSET, 9},						// HID/Report
		{0x2200, PAD2_INTERFACE, pad_hid_report_desc, sizeof(pad_hid_report_desc)},			// HID/Report
		{0x2100, PAD2_INTERFACE, cfg1_desc + PAD2_HID_DESC_OFFSET, 9},						// HID/Report
		{0x2200, KBD_INTERFACE, kbd_hid_report_desc, sizeof(kbd_hid_report_desc)},			// HID/Report
		{0x2100, KBD_INTERFACE, cfg1_desc + KBD_HID_DESC_OFFSET, 9},						// HID/Report
	{0x0300, 0x0000, (const uint8_t *)&str0, 4},											// String
	{0x0301, 0x0409, (const uint8_t *)&str1, sizeof(STR_MANUFACTURER)},							// String
	{0x0302, 0x0409, (const uint8_t *)&str2, sizeof(STR_PRODUCT)},							// String
};

#define NUM_DESC_LIST (sizeof(descriptor_list) / sizeof(struct descriptor_list_struct))

// USB Configuration
// Zero			Not enumerated
// Non-Zero		Enumerated
volatile uint8_t usb_config USBSTATE;

volatile uint8_t usb_suspended USBSTATE;

// Pad 1 Report Data
// 0		Buttons 1-8
// 1		X
// 2		Y
uint8_t pad1_report_data[PAD1_REPORT_SIZE] USBSTATE;

// Pad 2 Report Data
// 0		Buttons 1-8
// 1		X
// 2		Y
uint8_t pad2_report_data[PAD2_REPORT_SIZE] USBSTATE;

static uint8_t kbd_protocol USBSTATE;

static uint8_t kbd_idle_config USBSTATE;

uint8_t kbd_idle_count USBSTATE;

// Keyboard Report Data
// 0		Bitmap of modifier keys
// 1 - 13	Bitmap of keycodes (0 - 103)
// 14 - 15  Not used
uint8_t kbd_report_data[KBD_REPORT_SIZE] USBSTATE;

// Storage for current led state
// 1	Num Lock
// 2	Caps Lock
// 4	Scroll Lock
// 8	Compose
// 16	Kana
volatile uint8_t kbd_leds USBSTATE;

void usb_init(void) {
	uint8_t u, i;
	
	u = USBCON;
	if ((u & (1 << USBE)) && !(u & (1 << FRZCLK)))
		return;
	HW_CONFIG();
	USB_FREEZE();
	PLL_CONFIG();
	while (!(PLLCSR & (1 << PLOCK))) ;
	USB_CONFIG();
	UDCON = 0;
	
	usb_config = 0;
	usb_suspended = 0;
	
	pad1_report_data[0] = 0;
	pad1_report_data[1] = 1;
	pad1_report_data[2] = 1;
	
	pad2_report_data[0] = 0;
	pad2_report_data[1] = 1;
	pad2_report_data[2] = 1;
	
	kbd_protocol = 1;
	kbd_idle_config = 125;
	kbd_idle_count = 0;
	for (i = 0; i < KBD_REPORT_SIZE; i++) {
		kbd_report_data[i] = 0;
	}
	kbd_leds = 0;
	
	UDINT = 0;
	UDIEN = (1 << EORSTE) | (1 << SOFE) | (1 << SUSPE);
}

void usb_shutdown(void) {
	UDIEN = 0;
	UDCON = 1;
	USBCON = 0;
	PLLCSR = 0;
	usb_config = 0;
	usb_suspended = 1;
}

ISR(USB_GEN_vect) {
	uint8_t intbits, t, i;
	static uint8_t div4 = 0;
	
	intbits = UDINT;
	UDINT = 0;
	
	if (intbits & (1 << EORSTI)) {
		// End of Reset ?
		UENUM = 0;
		UECONX = 1;
		UECFG0X = EP_TYPE_CONTROL;
		UECFG1X = EP_SIZE(ENDPOINT0_SIZE) | EP_SINGLE_BUFFER;
		UEIENX = (1<<RXSTPE);
		usb_config = 0;
	}
	
	if (intbits & (1 << SOFI)) {
		// Start of Frame
		if (usb_config) {
			if (kbd_idle_config && (++div4 & 3) == 0) {
				UENUM = KBD_ENDPOINT;
				if (UEINTX & (1<<RWAL)) {
					kbd_idle_count++;
					if (kbd_idle_count == kbd_idle_config) {
						kbd_idle_count = 0;
						for (i = 0; i < KBD_REPORT_SIZE; i++) {
							UEDATX = kbd_report_data[i];
						}
						UEINTX = 0x3A;
					}
				}
			}
		}
	}
	
	// Active State
	
	if (intbits & (1 << SUSPI)) {
		// Suspend (inactivity for 3ms)
		UDIEN = (1<<WAKEUPE);
		usb_config = 0;
		usb_suspended = 1;
		#if (F_CPU >= 8000000L)
		USB_FREEZE();
		PLLCSR = 0;
		#endif
	}
	
	if (usb_suspended && (intbits & (1 << WAKEUPI))) {
		// USB Resume (almost any activity)
		#if (F_CPU >= 8000000L)
		PLL_CONFIG();
		while (!(PLLCSR & (1 << PLOCK))) ;
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

static inline void usb_wait_rcv_out(void) {
	while (!(UEINTX & (1 << RXOUTI))) ;
}

static inline void usb_ack_out(void) {
	UEINTX = ~(1 << RXOUTI);
}

ISR(USB_COM_vect) {
	uint8_t intbits;
	const uint8_t *list;
	const uint8_t *cfg;
	uint8_t i, n, len, en;
	volatile uint8_t *p;
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
			list = (const uint8_t *)descriptor_list;
			for (i=0; ; i++) {
				if (i >= NUM_DESC_LIST) {
					UECONX = (1 << STALLRQ) | (1 << EPEN);
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
				do {
					i = UEINTX;
				}
				while (!(i & ((1 << TXINI)|(1 << RXOUTI))));
				if (i & (1 << RXOUTI))
					return;
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
			UDADDR = wValue | (1 << ADDEN);
			return;
		}
		
		if (bRequest == SET_CONFIGURATION && bmRequestType == 0) {
			usb_config = wValue;
			usb_send_in();
			cfg = endpoint_cfg_table;
			for (i = 1; i < NUM_ENDPOINTS; i++) {
				UENUM = i;
				UECONX = 1;
				pgm_read_byte_postinc(UECFG0X, cfg);
				pgm_read_byte_postinc(UECFG1X, cfg);
			}
			UERST = 0x1E;
			UERST = 0;
			return;
		}
		
		if (bRequest == GET_CONFIGURATION && bmRequestType == 0x80) {
			usb_wait_in_ready();
			UEDATX = usb_config;
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
					UECONX = (1 << STALLRQC) | (1 << RSTDT) | (1 << EPEN);
					UERST = (1 << i);
					UERST = 0;
				}
				return;
			}
		}
		
		if (wIndex == PAD1_INTERFACE) {
			if (bmRequestType == 0xA1) {
				if (bRequest == HID_GET_REPORT) {
					usb_wait_in_ready();
					for (i = 0; i < PAD1_REPORT_SIZE; i++) {
						UEDATX = pad1_report_data[i];
					}
					usb_send_in();
					return;
				}
			}
		}
		
		if (wIndex == PAD2_INTERFACE) {
			if (bmRequestType == 0xA1) {
				if (bRequest == HID_GET_REPORT) {
					usb_wait_in_ready();
					for (i = 0; i < PAD2_REPORT_SIZE; i++) {
						UEDATX = pad2_report_data[i];
					}
					usb_send_in();
					return;
				}
			}
		}
		
		if (wIndex == KBD_INTERFACE) {
			if (bmRequestType == 0xA1) {
				if (bRequest == HID_GET_REPORT) {
					usb_wait_in_ready();
					for (i = 0; i < KBD_REPORT_SIZE; i++) {
						UEDATX = kbd_report_data[i];
					}
					usb_send_in();
					return;
				}
				if (bRequest == HID_GET_IDLE) {
					usb_wait_in_ready();
					UEDATX = kbd_idle_config;
					usb_send_in();
					return;
				}
				if (bRequest == HID_GET_PROTOCOL) {
					usb_wait_in_ready();
					UEDATX = kbd_protocol;
					usb_send_in();
					return;
				}
			}
			if (bmRequestType == 0x21) {
				if (bRequest == HID_SET_REPORT) {
					usb_wait_rcv_out();
					kbd_leds = UEDATX;
					usb_ack_out();
					usb_send_in();
					return;
				}
				if (bRequest == HID_SET_IDLE) {
					kbd_idle_config = (wValue >> 8);
					kbd_idle_count = 0;
					usb_send_in();
					return;
				}
				if (bRequest == HID_SET_PROTOCOL) {
					kbd_protocol = wValue;
					usb_send_in();
					return;
				}
			}
		}
	}
	UECONX = (1 << STALLRQ) | (1 << EPEN);
}
