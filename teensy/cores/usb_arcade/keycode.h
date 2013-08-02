#ifndef KEYCODE_H
#define KEYCODE_H

// USB HID Keyboard/Keypad Modifier Keys
#define M_L_CTRL		0x01;
#define M_L_SHIFT 		0x02;
#define M_L_ALT			0x04;
#define M_L_GUI			0x08;
#define M_R_CTRL		0x10;
#define M_R_SHIFT		0x20;
#define M_R_ALT			0x40;
#define M_R_GUI			0x80;


// USB HID Keyboard/Keypad Usage IDs (0x07)
enum hid_keyboard_usage {
	K_RESERVED				= 0x00,
	K_ROLL_OVER,
	K_POST_FAIL,
	K_UNDEFINED,
	K_A,
	K_B,
	K_C,
	K_D,
	K_E,
	K_F,
	K_G,
	K_H,
	K_I,
	K_J,
	K_K,
	K_L,
	K_M,			  		// 0x10
	K_N,
	K_O,
	K_P,
	K_Q,
	K_R,
	K_S,
	K_T,
	K_U,
	K_V,
	K_W,
	K_X,
	K_Y,
	K_Z,
	K_1,
	K_2,
	K_3,					// 0x20
	K_4,
	K_5,
	K_6,
	K_7,
	K_8,
	K_9,
	K_0,
	K_ENTER,
	K_ESCAPE,
	K_BSPACE,
	K_TAB,
	K_SPACE,
	K_MINUS,
	K_EQUAL,
	K_LBRACKET,
	K_RBRACKET,				// 0x30
	K_BSLASH,				// \ (and |)
	K_NONUS_HASH,			// Non-US # and ~
	K_SCOLON,				// ; (and :)
	K_QUOTE,				// ' and "
	K_GRAVE,				// Grave accent and tilde
	K_COMMA,				// , and <
	K_DOT,					// . and >
	K_SLASH,				// / and ?
	K_CAPSLOCK,
	K_F1,
	K_F2,
	K_F3,
	K_F4,
	K_F5,
	K_F6,
	K_F7,					// 0x40
	K_F8,
	K_F9,
	K_F10,
	K_F11,
	K_F12,
	K_PSCREEN,
	K_SCROLLLOCK,
	K_PAUSE,
	K_INSERT,
	K_HOME,
	K_PGUP,
	K_DELETE,
	K_END,
	K_PGDOWN,
	K_RIGHT,
	K_LEFT,					// 0x50
	K_DOWN,
	K_UP,
	K_NUMLOCK,
	K_KP_SLASH,
	K_KP_ASTERISK,
	K_KP_MINUS,
	K_KP_PLUS,
	K_KP_ENTER,
	K_KP_1,
	K_KP_2,
	K_KP_3,
	K_KP_4,
	K_KP_5,
	K_KP_6,
	K_KP_7,
	K_KP_8,					// 0x60
	K_KP_9,
	K_KP_0,
	K_KP_DOT,
	K_NONUS_BSLASH,			// Non-US \ and |
	K_APPLICATION,
	K_POWER,
	K_KP_EQUAL,
};

#endif
