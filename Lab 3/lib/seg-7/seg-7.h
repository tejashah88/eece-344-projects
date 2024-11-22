#ifndef SEG_7
#define SEG_7

/* LED Setup Reference: https://www.geeksforgeeks.org/seven-segment-displays/ */
#define SEG_7_NUM_0 0x3Fu // = 0x20 (PB5) | 0x10 (PB4) | 0x08 (PB3) | 0x04 (PB2) | 0x02 (PB1) | 0x01 (PB0)
#define SEG_7_NUM_1 0x06u // = 0x04 (PB2) | 0x02 (PB1)
#define SEG_7_NUM_2 0x5Bu // = 0x40 (PB6) | 0x10 (PB4) | 0x08 (PB3) | 0x02 (PB1) | 0x01 (PB0)
#define SEG_7_NUM_3 0x4Fu // = 0x40 (PB6) | 0x08 (PB3) | 0x04 (PB2) | 0x02 (PB1) | 0x01 (PB0)
#define SEG_7_NUM_4 0x66u // = 0x40 (PB6) | 0x20 (PB5) | 0x04 (PB2) | 0x02 (PB1)
#define SEG_7_NUM_5 0x6Du // = 0x40 (PB6) | 0x20 (PB5) | 0x08 (PB3) | 0x04 (PB2) | 0x01 (PB0)
#define SEG_7_NUM_6 0x7Du // = 0x40 (PB6) | 0x20 (PB5) | 0x10 (PB4) | 0x08 (PB3) | 0x04 (PB2) | 0x01 (PB0)
#define SEG_7_NUM_7 0x07u // = 0x04 (PB2) | 0x02 (PB1) | 0x01 (PB0)
#define SEG_7_NUM_8 0x7Fu // = 0x40 (PB6) | 0x20 (PB5) | 0x10 (PB4) | 0x08 (PB3) | 0x04 (PB2) | 0x02 (PB1) | 0x01 (PB0)
#define SEG_7_NUM_9 0x6Fu // = 0x40 (PB6) | 0x20 (PB5) | 0x08 (PB3) | 0x04 (PB2) | 0x02 (PB1) | 0x01 (PB0)

const unsigned char SEG_7_PATTERNS[10] = {
    SEG_7_NUM_0,
    SEG_7_NUM_1,
    SEG_7_NUM_2,
    SEG_7_NUM_3,
    SEG_7_NUM_4,
    SEG_7_NUM_5,
    SEG_7_NUM_6,
    SEG_7_NUM_7,
    SEG_7_NUM_8,
    SEG_7_NUM_9,
};


#endif /* SEG_7 */
