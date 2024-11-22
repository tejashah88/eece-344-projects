#ifndef LCD__DRIVER
#define LCD__DRIVER


/* LCD pin definitions */
#define LCD_RS_PIN 0x01u // = 0x01 (PB0)
#define LCD_RW_PIN 0x02u // = 0x02 (PB1)
#define LCD_EN_PIN 0x04u // = 0x04 (PB2)
#define LCD_CTRL_PINS 0x07u // = 0x04 (PB2) | 0x02 (PB1) | 0x01 (PB0)

#define LCD_DATA0_PIN	0x10u // = 0x10 (PB4)
#define LCD_DATA1_PIN	0x20u // = 0x20 (PB5)
#define LCD_DATA2_PIN	0x40u // = 0x40 (PB6)
#define LCD_DATA3_PIN	0x80u // = 0x80 (PB7)
#define LCD_DATA_PINS	0xF0u // = 0x80 (PB7) | 0x40 (PB6) | 0x20 (PB5) | 0x10 (PB4)

#define LCD_ALL_PINS 	(LCD_CTRL_PINS | LCD_DATA_PINS)


/* LCD command values */
/* See pg. 24-25 for reference */
#define LCD_CLEAR_DISPLAY	  0x01u // Delay: (unknown or 0)
#define LCD_RETURN_HOME		  0x02u // Delay: 1.52 ms
#define LCD_ENTRY_MODE_SET	0x04u // Delay: 37 us
#define LCD_DISPLAY_CTRL	  0x08u // Delay: 37 us
#define LCD_SHIFT_CTRL		  0x10u // Delay: 37 us
#define LCD_FUNCTION_SET	  0x20u // Delay: 37 us
#define LCD_SET_CGRAM_ADDR	0x40u // Delay: 37 us
#define LCD_SET_DDRAM_ADDR	0x80u // Delay: 37 us

// See Figure 5 of pg. 11 of the LCD datasheet
#define LCD_LINE1_START 0x00u
#define LCD_LINE2_START 0x20u

/* LCD Entry Mode flags - controls automatic behavior after character insertion */
#define LCD_ENTRY_CURSOR_AUTO_MOVE_RIGHT	0x02u
#define LCD_ENTRY_CURSOR_AUTO_MOVE_LEFT		0x00u
#define LCD_ENTRY_SHIFT_DISPLAY		  0x01u
#define LCD_ENTRY_NO_SHIFT_DISPLAY	0x00u

/* LCD Display control flags - controls which elements are displayed */
#define LCD_DISPLAY_FULL_ON		  0x04u
#define LCD_DISPLAY_FULL_OFF	  0x00u
#define LCD_DISPLAY_CURSOR_ON	  0x02u
#define LCD_DISPLAY_CURSOR_OFF	0x00u
#define LCD_DISPLAY_BLINKING_CURSOR_ON	0x01u
#define LCD_DISPLAY_BLINKING_CURSOR_OFF	0x00u

/* LCD Shift control flags */
#define LCD_SHIFT_MODE_DISPLAY	0x08u
#define LCD_SHIFT_MODE_CURSOR	  0x00u
#define LCD_SHIFT_DIR_RIGHT		  0x04u
#define LCD_SHIFT_DIR_LEFT		  0x00u

/* LCD Function set flags */
#define LCD_8_BIT_MODE		  0x10u
#define LCD_4_BIT_MODE		  0x00u
#define LCD_2_LINE_MODE		  0x08u
#define LCD_1_LINE_MODE		  0x00u
#define LCD_5x10_FONT_MODE	0x04u
#define LCD_5x8_FONT_MODE	  0x00u

/* LCD Control flags */
#define LCD_RS_DATA_MODE	  0x01u
#define LCD_RS_COMMAND_MODE 0x00u
#define LCD_RW_READ_MODE	  0x02u
#define LCD_RW_WRITE_MODE	  0x00u
#define LCD_EN_ENABLE_MODE	0x04u
#define LCD_EN_DISABLE_MODE	0x00u


// Function signatures for LCD device driver
extern void LCD_4Bits_Init(void);
extern void Setup_LCD_GPIO_Pins(void);
extern void LCD_Write4Bits(unsigned char data, unsigned char control);
extern void LCD_Write8Bits_4BitMode(unsigned char data, unsigned char control);
extern void LCD4Bits_Cmd(unsigned char command);
extern void LCD4Bits_Data(unsigned char data);


#endif /* LCD__DRIVER */
