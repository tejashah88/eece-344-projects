#ifndef KEYPAD__DRIVER
#define KEYPAD__DRIVER


// Keypad pin definitions
#define KEYPAD_ROW1		0x01 // = 0x01 (PE0)
#define KEYPAD_ROW2		0x02 // = 0x02 (PE1)
#define KEYPAD_ROW3		0x04 // = 0x04 (PE2)
#define KEYPAD_ROW4		0x08 // = 0x08 (PE3)
#define KEYPAD_ALL_ROWS 0x0F // = 0x08 (PE3) | 0x04 (PE2) | 0x02 (PE1) | 0x01 (PE0)

#define KEYPAD_COL1		0x10 // = 0x80 (PC4)
#define KEYPAD_COL2		0x20 // = 0x40 (PC5)
#define KEYPAD_COL3		0x40 // = 0x20 (PC6)
#define KEYPAD_COL4		0x80 // = 0x10 (PC7)
#define KEYPAD_ALL_COLS 0x0F // = 0x80 (PC7) | 0x40 (PC6) | 0x20 (PC5) | 0x10 (PC4)

#define KEYPAD_ALL_PINS (KEYPAD_ALL_ROWS | KEYPAD_ALL_COLS)


// Constant definitions for Keypad device driver
extern const unsigned char KEYMAP[4][4];

// Function signatures for Keypad device driver
extern void Keypad_Init(void);
extern void Setup_Keypad_GPIO_Column_Pins(void);
extern void Setup_Keypad_GPIO_Row_Pins(void);
extern unsigned char getKey(void);


#endif /* KEYPAD__DRIVER */
