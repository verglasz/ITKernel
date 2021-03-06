#include <stddef.h>
#include <stdint.h>

//  Standard values for definitions
#define DISPLAY_BUFFER_SIZE 512 // Bytes for the display pixels (32 x 128 pixels)
#define DISPLAY_PAGES 4
#define DISPLAY_COLS 128
#define DISPLAY_ROWS 32

// Display Commands
#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)


extern uint8_t displaybuffer[DISPLAY_BUFFER_SIZE]; // Declare memory for the display pixels to be written/read.

int display_addstring(uint8_t x, uint8_t y, const char *text, int invert);

void display_string(uint8_t x, uint8_t y, const char *text);
void display_string_inverted(uint8_t x, uint8_t y, const char *text);

void display_update(void);
void display_clear(void);
void display_white(void);
void display_init(void);

