#pragma once

namespace turbo::keyboard {

	enum keys
	{
		CAPSLOCK = 0x3A,
		NUMLOCK = 0x45,
		SCROLLLOCK = 0x46,

		L_SHIFT_DOWN = 0x2A,
		R_SHIFT_DOWN = 0x36,
		L_SHIFT_UP = 0xAA,
		R_SHIFT_UP = 0xB6,

		CTRL_DOWN = 0x1D,
		CTRL_UP = 0x9D,

		ALT_DOWN = 0x38,
		ALT_UP = 0xB8,

		HOME = 0x47,
		END = 0x52,

		PGUP = 0x49,
		PGDN = 0x51,

		INSERT = 0x52,
		DELETE = 0x53,

		UP = 0x48,
		DOWN = 0x50,
		LEFT = 0x4B,
		RIGHT = 0x4D,
	};

	unsigned char escapes[] =
	{
		'\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08', '\x09',
		'\x0A', '\x0B', '\x0C', '\x0D', '\x0E', '\x0F', '\x10', '\x11', '\x12', '\x13',
		'\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1A', '\x1B', '\x1C', '\x1D',
		'\x1E', '\x1F', [0x7F] = '\x7F'
	};
	unsigned char kbdus[128] =
	{
		0,  27, '&', 'e', '"', '\'', '(', '-', 'e', '_',	/* 9 */
		'c', 'a', ')', '=', '\b',	/* Backspace */
		'\t',			/* Tab */
		'a', 'z', 'e', 'r',	/* 19 */
		't', 'y', 'u', 'i', 'o', 'p', '^', '$', '\n',	/* Enter key */
		0,			/* 29   - Control */
		'q', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm',	/* 39 */
		'u', '*',   0,		/* Left shift */
		'<', 'w', 'x', 'c', 'v', 'b', 'n',			/* 49 */
		',', ';', ':', '!',   0,				/* Right shift */
		'*',
		0,	/* Alt */
		' ',	/* Space bar */
		0,	/* Caps lock */
		0,	/* 59 - F1 key ... > */
		0,   0,   0,   0,   0,   0,   0,   0,
		0,	/* < ... F10 */
		0,	/* 69 - Num lock*/
		0,	/* Scroll Lock */
		0,	/* Home key */
		0,	/* Up Arrow */
		0,	/* Page Up */
		'-',
		0,	/* Left Arrow */
		0,
		0,	/* Right Arrow */
		'+',
		0,	/* 79 - End key*/
		0,	/* Down Arrow */
		0,	/* Page Down */
		0,	/* Insert Key */
		0,	/* Delete Key */
		0,   0,   0,
		0,	/* F11 Key */
		0,	/* F12 Key */
	0,	/* All other keys are undefined */
	};

	unsigned char kbdus_shft[128] =
	{
		0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
		'9', '0', '*', '+', '\b',	/* Backspace */
		'\t',			/* Tab */
		'A', 'Z', 'E', 'R',	/* 19 */
		'T', 'Y', 'U', 'I', 'O', 'P', '^', 'E', '\n',	/* Enter key */
		0,			/* 29   - Control */
		'Q', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'M',	/* 39 */
		'%', 'u',   0,		/* Left shift */
		'<', 'W', 'X', 'C', 'V', 'B', 'N',			/* 49 */
		'?', '.', '/', 'S',   0,				/* Right shift */
		'*',
		0,	/* Alt */
		' ',	/* Space bar */
		0,	/* Caps lock */
		0,	/* 59 - F1 key ... > */
		0,   0,   0,   0,   0,   0,   0,   0,
		0,	/* < ... F10 */
		0,	/* 69 - Num lock*/
		0,	/* Scroll Lock */
		0,	/* Home key */
		0,	/* Up Arrow */
		0,	/* Page Up */
		'-',
		0,	/* Left Arrow */
		0,
		0,	/* Right Arrow */
		'+',
		0,	/* 79 - End key*/
		0,	/* Down Arrow */
		0,	/* Page Down */
		0,	/* Insert Key */
		0,	/* Delete Key */
		0,   0,   0,
		0,	/* F11 Key */
		0,	/* F12 Key */
		0,	/* All other keys are undefined */
	};

	unsigned char kbdus_capsshft[128] =
	{
		0,  27, '&', 'e', '"', '\'', '(', '-', 'e', '_',	/* 9 */
		'c', 'a', ')', '=', '\b',	/* Backspace */
		'\t',			/* Tab */
		'a', 'z', 'e', 'r',	/* 19 */
		't', 'y', 'u', 'i', 'o', 'p', '^', '$', '\n',	/* Enter key */
		0,			/* 29   - Control */
		'q', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm',	/* 39 */
		'u', '*',   0,		/* Left shift */
		'<', 'w', 'x', 'c', 'v', 'b', 'n',			/* 49 */
		',', ';', ':', '!',   0,				/* Right shift */
		'*',
		0,	/* Alt */
		' ',	/* Space bar */
		0,	/* Caps lock */
		0,	/* 59 - F1 key ... > */
		0,   0,   0,   0,   0,   0,   0,   0,
		0,	/* < ... F10 */
		0,	/* 69 - Num lock*/
		0,	/* Scroll Lock */
		0,	/* Home key */
		0,	/* Up Arrow */
		0,	/* Page Up */
		'-',
		0,	/* Left Arrow */
		0,
		0,	/* Right Arrow */
		'+',
		0,	/* 79 - End key*/
		0,	/* Down Arrow */
		0,	/* Page Down */
		0,	/* Insert Key */
		0,	/* Delete Key */
		0,   0,   0,
		0,	/* F11 Key */
		0,	/* F12 Key */
	0,	/* All other keys are undefined */
	};

	unsigned char kbdus_caps[128] =
	{
		0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
		'9', '0', '*', '+', '\b',	/* Backspace */
		'\t',			/* Tab */
		'A', 'Z', 'E', 'R',	/* 19 */
		'T', 'Y', 'U', 'I', 'O', 'P', '^', 'E', '\n',	/* Enter key */
		0,			/* 29   - Control */
		'Q', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'M',	/* 39 */
		'%', 'u',   0,		/* Left shift */
		'<', 'W', 'X', 'C', 'V', 'B', 'N',			/* 49 */
		'?', '.', '/', 'S',   0,				/* Right shift */
		'*',
		0,	/* Alt */
		' ',	/* Space bar */
		0,	/* Caps lock */
		0,	/* 59 - F1 key ... > */
		0,   0,   0,   0,   0,   0,   0,   0,
		0,	/* < ... F10 */
		0,	/* 69 - Num lock*/
		0,	/* Scroll Lock */
		0,	/* Home key */
		0,	/* Up Arrow */
		0,	/* Page Up */
		'-',
		0,	/* Left Arrow */
		0,
		0,	/* Right Arrow */
		'+',
		0,	/* 79 - End key*/
		0,	/* Down Arrow */
		0,	/* Page Down */
		0,	/* Insert Key */
		0,	/* Delete Key */
		0,   0,   0,
		0,	/* F11 Key */
		0,	/* F12 Key */
		0,	/* All other keys are undefined */
	};
}