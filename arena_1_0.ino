/*
  ____________________

  ARENA - Arduboy 3d Shooter
  ____________________

    Welcome to ARENA (Arduboy 3D-Shooter)
    Version 1.0 ... (c) 2018 by deetee/zooxo
    This software is covered by the 3-clause BSD license.

  ____________________

    PREAMBLE
  ____________________

    Actually I am not a gamer - except puzzels and logic games. But my first
    contacts with Wolfenstein and Quake were very impressive because of the
    overwhelming plastic effects. Doing the math seemed to be a real challenge
    when programming a 3D-game. Many years (and the fantastic Arduboy
    hardware) later I am giving this challenge a try.
    I asserted that using approximating approaches are often faster and more memory
    friendly than using the exact vector math. And the eyes don't really care
    if the tiles of a wall are drawn exactly.
    So I tried to program a game which has very open setting possibilities.
    You can determine how many enemies and medipacks you want to play with. You
    can even draw your own arena maze with the built-in map editor and save it
    permanently to the EEPROM memory. With the OFF feature ("deep sleep mode")
    you can pause your game anytime and go on weeks later without charging the
    battery. You can even toggle between a 3D an 2D view (even if the latter
    one is to small and was inteded for orientation purposes).

    Enjoy!
    deetee

  ____________________

    KEYBOARD
  ____________________

    GAME MODE
                 go_forward              FIRE
      turn_left  go_back     turn_right  MENU

    MENU MODE
               up               OK
      page_up  down  page_down  ESC

  ____________________

    ENEMIES AND OBJECTS
  ____________________

      ROBO ... Reliable soldier with harmless distance shoots
      TAUR ... Fast beast which hurts when close
      NITE ... Slow knight which hurts severe when near
      EVIL ... Slow but persistent devil with very severe shoots from distance
      OCTO ... Fast octopus that squashes lethal when close

      MEDI ... Medipack: Refreshes your health to maximum
      AMMO ... Ammunition pack: Reloads your gun to maximum

  ____________________

    MENU
  ____________________

      NEW GAME   ... Start a new game (doesn't reset settings -> see RESET)
      SETTINGS   ... Reset all setting variables
        2D/3D    ... Toggle between 2d- and 3d-view
        MEDI     ... Set number of medipacks (1-5)
        AMMO     ... Set number of ammopacks (1-5)
        ENEMIES  ... Set number of enemies (1-9)
        SOUND    ... Sound on/off
        CONTRAST ... Set contrast/brightness of screen (0...255)
        STATUS   ... Show/hide status view (health, ammo, compass)
        SKY      ... Show/hide sky view (ceiling and floor line)
        MESSAGES ... Show/hide messages (comments)
        RESET    ... Reset settings to default values
      EDIT MAP   ... Map editor (edit current map)
        WHITE    ... Draw white pixel at cursor
        BLACK    ... Draw black pixel at cursor (clear pixel)
        CLEAR    ... Clear map (no walls at all)
        LOAD     ... Load map from EEPROM (slot 1-3)
        SAVE     ... Save current map to EEPROM (slot 1-3)
      BATT       ... Show supply voltage x 100 - approx. 310 (empty) to 430 (full)
      ABOUT      ... About info
      OFF        ... Deep sleep mode (wake up with button A)

  ____________________

    SPECIALITIES
  ____________________

    FLASHLIGHTMODE
      Secure wait to flash software if UP is pressed when switching on.

    REINITIALIZE
      Reset saved variables (EEPROM) if DOWN is pressed when switching on.
  ____________________

*/


/************************************************************
  SYSTEM, KERNEL
*************************************************************/

// INCLUDES
//#include <SPI.h>       // Needed to control screen
#include <avr/power.h> // Needed for power management
#include <avr/sleep.h>
#include <EEPROM.h>    // For saving data to EEPROM

// DEFINES
#define EECONTRAST 0 // EEPROM address to save screencontrast
#define EESCREENOFF 1 // EEPROM address to save screenofftime
#define FRAMERATE 10 // Maximal number of screen refreshes per second (>3)
#define SCREENWIDTH 64 // Virtual screen width (quarter of real screen)
#define SCREENHEIGHT 32 // Virtual screen height (quarter of real screen)
#define SCREENBYTES 256 // Number of bytes to address SCREEN (SCREENWIDTH x SCREENHEIGHT)/8
#define KEYREPEATTIME 250 // Time in ms when pressed key repeats
#define MAXCHAR 10 // Maximum of characters in a line (due to font size)
#define MAXCHAR2 5
#define MAXCHAR4 3
#define MAXLIN 4 // Maximum of lines
#define MAXLIN2 2
#define MAXLIN4
#define PRINTMAXX 59 // Boundaries to restrict coordinates
#define PRINTMAXY 3
#define PRINT2MAXX 54
#define PRINT2MAXY 2
#define PRINT4MAXX 44
#define PRINT4MAXY 0

// Pins, ports
// Display
#define PIN_CS 12       // CS pin number
#define CS_PORT PORTD   // CS port
#define CS_BIT PORTD6   // CS physical bit number
#define PIN_DC 4        // DC pin number
#define DC_PORT PORTD   // DC port
#define DC_BIT PORTD4   // DC physical bit number
#define PIN_RST 6       // RST pin number
#define RST_PORT PORTD  // RST port
#define RST_BIT PORTD7  // RST physical bit number
// RGB LED
#define RED_LED 10           // Red LED pin
#define RED_LED_PORT PORTB   // Red LED port
#define RED_LED_BIT PORTB6   // Red LED physical bit number
#define GREEN_LED 11         // Green LED pin
#define GREEN_LED_PORT PORTB // Red LED port
#define GREEN_LED_BIT PORTB7 // Green LED physical bit number
#define BLUE_LED 9           // Blue LED pin
#define BLUE_LED_PORT PORTB  // Blue LED port
#define BLUE_LED_BIT PORTB5  // Blue LED physical bit number
// Buttons
#define LEFT_BUTTON _BV(5)  // Left button value for functions requiring a bitmask
#define RIGHT_BUTTON _BV(6) // Right button value for functions requiring a bitmask
#define UP_BUTTON _BV(7)    // The Up button value for functions requiring a bitmask
#define DOWN_BUTTON _BV(4)  // The Down button value for functions requiring a bitmask
#define A_BUTTON _BV(3)     // The A button value for functions requiring a bitmask
#define B_BUTTON _BV(2)     // The B button value for functions requiring a bitmask
#define PIN_LEFT_BUTTON A2 // Left Button
#define LEFT_BUTTON_PORT PORTF
#define LEFT_BUTTON_PORTIN PINF
#define LEFT_BUTTON_DDR DDRF
#define LEFT_BUTTON_BIT PORTF5
#define PIN_RIGHT_BUTTON A1 // Right button
#define RIGHT_BUTTON_PORT PORTF
#define RIGHT_BUTTON_PORTIN PINF
#define RIGHT_BUTTON_DDR DDRF
#define RIGHT_BUTTON_BIT PORTF6
#define PIN_UP_BUTTON A0
#define UP_BUTTON_PORT PORTF // Up button
#define UP_BUTTON_PORTIN PINF
#define UP_BUTTON_DDR DDRF
#define UP_BUTTON_BIT PORTF7
#define PIN_DOWN_BUTTON A3 // Down button
#define DOWN_BUTTON_PORT PORTF
#define DOWN_BUTTON_PORTIN PINF
#define DOWN_BUTTON_DDR DDRF
#define DOWN_BUTTON_BIT PORTF4
#define PIN_A_BUTTON 7 // A button
#define A_BUTTON_PORT PORTE
#define A_BUTTON_PORTIN PINE
#define A_BUTTON_DDR DDRE
#define A_BUTTON_BIT PORTE6
#define PIN_B_BUTTON 8 // B button
#define B_BUTTON_PORT PORTB
#define B_BUTTON_PORTIN PINB
#define B_BUTTON_DDR DDRB
#define B_BUTTON_BIT PORTB4
// Speaker
#define PIN_SPEAKER_1 5  // Pin number of the first lead of the speaker
#define PIN_SPEAKER_2 13 // Pin number of the second lead of the speaker
#define SPEAKER_1_PORT PORTC
#define SPEAKER_1_DDR DDRC
#define SPEAKER_1_BIT PORTC6
#define SPEAKER_2_PORT PORTC
#define SPEAKER_2_DDR DDRC
#define SPEAKER_2_BIT PORTC7
#define RAND_SEED_IN A4
#define RAND_SEED_IN_PORT PORTF
#define RAND_SEED_IN_BIT PORTF1
#define RAND_SEED_IN_ADMUX (_BV(REFS0) | _BV(REFS1) | _BV(MUX0))
// SPI interface
#define SPI_MISO_PORT PORTB
#define SPI_MISO_BIT PORTB3
#define SPI_MOSI_PORT PORTB
#define SPI_MOSI_BIT PORTB2
#define SPI_SCK_PORT PORTB
#define SPI_SCK_BIT PORTB1
#define SPI_SS_PORT PORTB
#define SPI_SS_BIT PORTB0
#define NOTE_c 262 // Musical notes frequencies
#define NOTE_d 294
#define NOTE_e 330
#define NOTE_f 349
#define NOTE_g 392
#define NOTE_a 440
#define NOTE_b 494
#define NOTE_C 523



// FONTS
const byte font5x8 [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, // sp
  0x00, 0x00, 0x2f, 0x00, 0x00, // !
  0x00, 0x07, 0x00, 0x07, 0x00, // "
  0x14, 0x7f, 0x14, 0x7f, 0x14, // #
  0x24, 0x2a, 0x7f, 0x2a, 0x12, // $
  0x62, 0x64, 0x08, 0x13, 0x23, // %
  0x36, 0x49, 0x55, 0x22, 0x50, // &
  0x00, 0x05, 0x03, 0x00, 0x00, // '
  0x00, 0x1c, 0x22, 0x41, 0x00, // (
  0x00, 0x41, 0x22, 0x1c, 0x00, // )
  0x14, 0x08, 0x3E, 0x08, 0x14, // *
  0x08, 0x08, 0x3E, 0x08, 0x08, // +
  0x00, 0x00, 0xA0, 0x60, 0x00, // ,
  0x08, 0x08, 0x08, 0x08, 0x08, // -
  0x00, 0x60, 0x60, 0x00, 0x00, // .
  0x20, 0x10, 0x08, 0x04, 0x02, // /
  0x7f, 0x41, 0x41, 0x41, 0x7f, // 0 high digital
  0x00, 0x00, 0x00, 0x00, 0x7f, // 1 high digital
  0x79, 0x49, 0x49, 0x49, 0x4f, // 2 high digital
  0x41, 0x49, 0x49, 0x49, 0x7f, // 3 high digital
  0x0f, 0x08, 0x08, 0x08, 0x7f, // 4 high digital
  0x4f, 0x49, 0x49, 0x49, 0x79, // 5 high digital
  0x7f, 0x49, 0x49, 0x49, 0x79, // 6 high digital
  0x01, 0x01, 0x01, 0x01, 0x7f, // 7 high digital
  0x7f, 0x49, 0x49, 0x49, 0x7f, // 8 high digital
  0x4f, 0x49, 0x49, 0x49, 0x7f, // 9 high digital
  0x00, 0x36, 0x36, 0x00, 0x00, // :
  0x00, 0x56, 0x36, 0x00, 0x00, // ;
  0x08, 0x14, 0x22, 0x41, 0x00, // <
  0x14, 0x14, 0x14, 0x14, 0x14, // =
  0x00, 0x41, 0x22, 0x14, 0x08, // >
  0x02, 0x01, 0x51, 0x09, 0x06, // ?
  0x32, 0x49, 0x59, 0x51, 0x3E, // @
  0x7f, 0x09, 0x09, 0x09, 0x7f, // A
  0x7f, 0x49, 0x49, 0x4f, 0x78, // B
  0x7f, 0x41, 0x41, 0x41, 0x40, // C
  0x7F, 0x41, 0x41, 0x63, 0x3e, // D
  0x7F, 0x49, 0x49, 0x49, 0x41, // E
  0x7F, 0x09, 0x09, 0x09, 0x01, // F
  0x7f, 0x41, 0x49, 0x49, 0x79, // G
  0x7F, 0x08, 0x08, 0x08, 0x7F, // H
  0x00, 0x00, 0x7F, 0x00, 0x00, // I
  0x40, 0x40, 0x40, 0x40, 0x7f, // J
  0x7F, 0x08, 0x08, 0x0f, 0x78, // K
  0x7F, 0x40, 0x40, 0x40, 0x40, // L
  0x7F, 0x01, 0x07, 0x01, 0x7F, // M
  0x7F, 0x01, 0x7f, 0x40, 0x7F, // N
  0x7f, 0x41, 0x41, 0x41, 0x7f, // =
  0x7F, 0x09, 0x09, 0x09, 0x0f, // P
  0x7f, 0x41, 0x71, 0x41, 0x7f, // Q
  0x7F, 0x09, 0x09, 0x79, 0x0f, // R
  0x4f, 0x49, 0x49, 0x49, 0x78, // S
  0x01, 0x01, 0x7F, 0x01, 0x01, // T
  0x7F, 0x40, 0x40, 0x40, 0x7F, // U
  0x1F, 0x70, 0x40, 0x70, 0x1F, // V
  0x7F, 0x40, 0x70, 0x40, 0x7F, // W
  0x78, 0x0f, 0x08, 0x0f, 0x78, // X
  0x0f, 0x08, 0x7c, 0x08, 0x0f, // Y
  0x71, 0x51, 0x5d, 0x45, 0x47, // Z
  0x00, 0x7F, 0x41, 0x41, 0x00, // [
  0x02, 0x04, 0x08, 0x10, 0x20, // b
  0x00, 0x41, 0x41, 0x7F, 0x00, // ]
  0x04, 0x02, 0x01, 0x02, 0x04, // ^
  0x40, 0x40, 0x40, 0x40, 0x40, // _
  0x14, 0x36, 0x77, 0x36, 0x14, // ' updown
  0x5c, 0x76, 0x77, 0x76, 0x5c, // a ammo small
  0x3e, 0x36, 0x22, 0x36, 0x3e, // b medi
  0x3a, 0xfd, 0x7f, 0xfd, 0x3a, // c robo
  0xeb, 0x74, 0x44, 0x74, 0xeb, // d taur
  0x1f, 0xe8, 0x7f, 0xc7, 0x7c, // e nite
  0x0e, 0x3b, 0x4f, 0x3b, 0x0e, // f ghost
  0xce, 0x37, 0xff, 0x37, 0xce, // g octo
  0x7F, 0x08, 0x04, 0x04, 0x78, // h
  0x00, 0x44, 0x7D, 0x40, 0x00, // i
  0x40, 0x80, 0x84, 0x7D, 0x00, // j
  0x7F, 0x10, 0x28, 0x44, 0x00, // k
  0x00, 0x41, 0x7F, 0x40, 0x00, // l
  0x7C, 0x04, 0x18, 0x04, 0x78, // m
  0x7C, 0x08, 0x04, 0x04, 0x78, // n
  0xfc, 0x6e, 0x3f, 0x6e, 0xfc, // o A logo
  0x7e, 0x1a, 0x7e, 0x76, 0x5c, // p R logo
  0x7e, 0xff, 0xdb, 0xc3, 0x66, // q E logo
  0x7e, 0x1e, 0x3c, 0x78, 0x7e, // r N logo
  0x20, 0x70, 0xf8, 0x20, 0xf8, // s compassO
  0x78, 0x38, 0xb8, 0x48, 0x20, // t compassNE
  0xa0, 0xb0, 0xf8, 0xb0, 0xa0, // u compassN
  0x20, 0x48, 0xb8, 0x38, 0x78, // v compassNW
  0xf8, 0x20, 0xf8, 0x70, 0x20, // w compassW
  0x20, 0x90, 0xe8, 0xe0, 0xf0, // x compassSW
  0x28, 0x68, 0xf8, 0x68, 0x28, // y compassS
  0xf0, 0xe0, 0xe8, 0x90, 0x20, // z compassSE
  0x08, 0x1c, 0x3e, 0x7f, 0x00, // { left
  0x00, 0x00, 0xff, 0x00, 0x00, // |
  0x00, 0x7f, 0x3e, 0x1c, 0x08, // } right
  0x06, 0x7f, 0x01, 0x7f, 0x01  // ~ paragraph
};

// Macros
#define _abs(x) ((x<0)?(-x):(x))      // abs()-substitute macro
#define _min(a,b) (((a)<(b))?(a):(b)) // mix()-substitute macro
#define _max(a,b) (((a)>(b))?(a):(b)) // max()-substitute macro

// GLOBAL VARIABLES
static byte sbuf[SCREENBYTES]; // Buffer for virtual screen (costs 256 bytes of dynamic memory)
static byte eachframemillis, thisframestart, lastframedurationms; // Framing times
static boolean justrendered; // True if frame was just rendered
static boolean isscreensave = false; // True if screensaver is active
static boolean isshift = false; // True if button A was holded (= shift)
static boolean istorch; // True if torch is on
static boolean issound = true; // True to enable sound


// BOOT, BEGIN
void SPItransfer(byte data) { // Write to the SPI bus (MOSI pin)
  SPDR = data;
  asm volatile("nop"); // Tiny delay before wait
  while (!(SPSR & _BV(SPIF))) {} // Wait for byte to be sent
}

static void ScreenCommandMode(void) { // Set screen to command mode
  bitClear(DC_PORT, DC_BIT);
}

static void ScreenDataMode(void) { // Set screen to data mode
  bitSet(DC_PORT, DC_BIT);
}

static void bootpins(void) { // Declare and boot port pins
  PORTB |= _BV(RED_LED_BIT) | _BV(GREEN_LED_BIT) | _BV(BLUE_LED_BIT) | // Port B
           _BV(B_BUTTON_BIT);
  DDRB  &= ~(_BV(B_BUTTON_BIT) | _BV(SPI_MISO_BIT));
  DDRB  |= _BV(RED_LED_BIT) | _BV(GREEN_LED_BIT) | _BV(BLUE_LED_BIT) |
           _BV(SPI_MOSI_BIT) | _BV(SPI_SCK_BIT) | _BV(SPI_SS_BIT);
  PORTD |= _BV(CS_BIT); // Port D
  PORTD &= ~(_BV(RST_BIT));
  DDRD  |= _BV(RST_BIT) | _BV(CS_BIT) | _BV(DC_BIT);
  PORTE |= _BV(A_BUTTON_BIT); // Port E
  DDRE  &= ~(_BV(A_BUTTON_BIT));
  PORTF |= _BV(LEFT_BUTTON_BIT) | _BV(RIGHT_BUTTON_BIT) | // Port F
           _BV(UP_BUTTON_BIT) | _BV(DOWN_BUTTON_BIT);
  PORTF &= ~(_BV(RAND_SEED_IN_BIT));
  DDRF  &= ~(_BV(LEFT_BUTTON_BIT) | _BV(RIGHT_BUTTON_BIT) |
             _BV(UP_BUTTON_BIT) | _BV(DOWN_BUTTON_BIT) |
             _BV(RAND_SEED_IN_BIT));
}

static void bootSPI(void) { // Initialize the SPI interface for the display
  SPCR = _BV(SPE) | _BV(MSTR); // master, mode 0, MSB first, CPU clock / 2 (8MHz)
  SPSR = _BV(SPI2X);
}

const byte PROGMEM ScreenBootProgram[] = { // SSD1306 boot sequence
  // 0xAE, // Display Off
  0xD5, 0xF0, // Set Display Clock Divisor v = 0xF0 - default is 0x80
  // 0xA8, 0x3F, // Set Multiplex Ratio v = 0x3F
  // 0xD3, 0x00, // Set Display Offset v = 0
  // 0x40, // Set Start Line (0)
  0x8D, 0x14, // Charge Pump Setting v = enable (0x14) - default is disabled
  0xA1, // Set Segment Re-map (A0) | (b0001) - default is (b0000)
  0xC8, // Set COM Output Scan Direction
  // 0xDA, 0x12, // Set COM Pins v
  0x81, 0xCF, // Set Contrast v = 0xCF
  0xD9, 0xF1, // Set Precharge = 0xF1
  // 0xDB, 0x40, // Set VCom Detect
  // 0xA4, // Entire Display ON
  // 0xA6, // Set normal/inverse display
  0xAF, // Display On
  0x20, 0x00, // Set display mode = horizontal addressing mode (0x00)
  // 0x21, 0x00, COLUMN_ADDRESS_END, // Set col address range
  // 0x22, 0x00, PAGE_ADDRESS_END // Set page address range
};

static void bootscreen(void) { // Boot screen - reset the display
  delayshort(5); // Reset pin should be low here - let it stay low a while
  bitSet(RST_PORT, RST_BIT); // Set reset pin high to come out of reset
  delayshort(5); // Wait
  bitClear(CS_PORT, CS_BIT); // Select the display (permanently, since nothing else is using SPI)

  ScreenCommandMode(); // Run customized boot-up command sequence
  for (byte i = 0; i < sizeof(ScreenBootProgram); i++)
    SPItransfer(pgm_read_byte(ScreenBootProgram + i));
  ScreenDataMode();
}

static void setframerate(byte rate) { // Calculate frameduration
  eachframemillis = 1000 / rate;
}


// SYSTEM
static void setscreencontrast(byte c) { // Set screen contrast
  ScreenCommandMode();
  SPItransfer(0x81);
  SPItransfer(c);
  ScreenDataMode();
  EEPROM.write(EECONTRAST, c);
}

static void sbufclr(void) { // Clear display buffer
  //for (int i = 0; i < SCREENBYTES; i++) sbuf[i] = NULL;
  memset(sbuf, NULL, SCREENBYTES); // Memset needs 4 bytes more - but what is faster?
}

static void sbuffill(void) { // Fills display buffer
  //for (int i = 0; i < SCREENBYTES; i++) sbuf[i] = NULL;
  memset(sbuf, 0xff, SCREENBYTES); // Memset needs 4 bytes more - but what is faster?
}

static byte expand4bit(byte b) { // 0000abcd  Expand 4 bits (lower nibble)
  b = (b | (b << 2)) & 0x33;     // 00ab00cd
  b = (b | (b << 1)) & 0x55;     // 0a0b0c0d
  return (b | (b << 1));         // aabbccdd
}

static byte expand2bit(byte b) { // Expand 2 bits 000000ab
  b = (b | (b << 3)) & 0x11;                   // 000a000b
  for (byte i = 0; i < 3; i++) b |= (b << 1);  // aaaabbbb
  return (b);
}

static void display(void) { // Print display buffer (64x32) to real screen (128x64)
  for (byte l = 0; l < MAXLIN; l++) { // Four lines
    for (byte k = 0; k < 2; k++) { // Two nibbles (double height)
      for (byte j = 0; j < SCREENWIDTH; j++) {
        byte tmp = expand4bit((sbuf[j + l * SCREENWIDTH] >> (k * 4)) & 0x0f); // Expand 0000abcd
        for (byte i = 0; i < 2; i++) SPItransfer(tmp); // Double width
      }
    }
  }
}

static void print1(int x, int y, byte ch) { // Print character to sbuf[]
  x = _min(x, PRINTMAXX); y = _min(y, PRINTMAXY); // Restrict x and y
  if (ch >= ' ' && ch <= '~')
    for (int i = 0; i < 5; i++)
      sbuf[x + y * SCREENWIDTH + i] = pgm_read_byte(&font5x8[5 * (ch - ' ') + i]);
}

static void print2(int x, int y, byte ch, boolean slim) { // Print double sized (slim) character
  x = slim ? _min(x, PRINTMAXX) : _min(x, PRINT2MAXX); // Restrict x and y
  y = _min(y, PRINT2MAXY);
  for (byte k = 0; k < 2; k++) { // Two nibbles
    for (byte j = 0; j < 5; j++) {
      byte tmp = expand4bit((pgm_read_byte(&font5x8[5 * (ch - ' ') + j]) >> (k * 4)) & 0x0f); // Expand 0000abcd
      if (slim) sbuf[x + (y + k) * SCREENWIDTH + 1 * j] = tmp;
      else for (byte i = 0; i < 2; i++) sbuf[x + (y + k) * SCREENWIDTH + 2 * j + i] = tmp;
    }
  }
}

static void print4(byte x, byte y, byte ch, boolean slim) { // Print quad-sized character
  x = slim ? _min(x, PRINT2MAXX) : _min(x, PRINT4MAXX); // Restrict x and y
  y = _min(y, PRINT4MAXY);
  for (byte k = 0; k < 4; k++) { // 4 pairs of bits (ab cd ef gh)
    for (byte j = 0; j < 5; j++) {
      byte tmp = expand2bit((pgm_read_byte(&font5x8[5 * (ch - ' ') + j]) >> (2 * k)) & 0x03); // Expand 000000ab
      if (slim) for (byte i = 0; i < 2; i++) sbuf[x + (y + k) * SCREENWIDTH + 2 * j + i] = tmp;
      else for (byte i = 0; i < 4; i++) sbuf[x + (y + k) * SCREENWIDTH + 4 * j + i] = tmp;
    }
  }
}

static void delayshort(byte ms) { // Delay in ms with 8 bit duration
  delay((unsigned long) ms);
}

static void bootpowersaving(void) {
  PRR0 = _BV(PRTWI) | _BV(PRADC); // Disable I2C-Interface and ADC
  PRR1 |= _BV(PRUSART1); // Disable USART1
  //power_adc_disable();
  power_usart0_disable();
  power_twi_disable();
  power_timer1_disable(); // Disable timer 1...3 (0 is used for millis())
  power_timer2_disable();
  //power_timer3_disable(); // Timer 3 is needed for sound
  power_usart1_disable();
  //power_usb_disable(); // Disable USB (needs reset to reprogram)
}

static void screenoff(void) { // Shut down the display
  ScreenCommandMode();
  SPItransfer(0xAE); // Display off
  SPItransfer(0x8D); // Disable charge pump
  SPItransfer(0x10);
  delayshort(100);
  bitClear(RST_PORT, RST_BIT); // Set RST to low (reset state)
}

static void screenon(void) { // Restart the display after a displayOff()
  bootscreen();
  setscreencontrast(EEPROM.read(EECONTRAST));
}

static void idle(void) { // Idle, while waiting for next frame
  SMCR = _BV(SE); // Select idle mode and enable sleeping
  sleep_cpu();
  SMCR = 0; // Disable sleeping
}

static bool nextFrame(void) { // Wait (idle) for next frame
  byte now = (byte) millis(), framedurationms = now - thisframestart;
  if (justrendered) {
    lastframedurationms = framedurationms;
    justrendered = false;
    return false;
  }
  else if (framedurationms < eachframemillis) {
    if (++framedurationms < eachframemillis) idle();
    return false;
  }
  justrendered = true;
  thisframestart = now;
  return true;
}

static void clrline(byte from, byte to) { // Clears line(s) (0...3)
  for (byte j = from; j <= _min(to, MAXLIN - 1); j++)
    for (byte i = 0; i < SCREENWIDTH; i++) sbuf[i + j * SCREENWIDTH] = NULL;
}

static void cleartextarea(byte x1, byte y1, byte x2, byte y2) { // Clear area
  x1 = _min(x1, PRINTMAXX); y1 = _min(y1, PRINTMAXY); // Restrict x and y
  x2 = _min(x2, PRINTMAXX); y2 = _min(y2, PRINTMAXY);
  for (byte j = y1; j <= y2; j++)
    for (byte i = x1; i < x1 + x2; i++) sbuf[i + j * SCREENWIDTH] = NULL;
}

static void printstring(byte x, byte y, char* s) { // Print string at x, y (normal size)
  byte tmp = strlen(s);
  cleartextarea(x, y, tmp * 6, y);
  for (byte i = 0; i < tmp && i < MAXCHAR; i++) print1(x + i * 6, y, s[i]);
}
static void printstring2(byte x, byte y, char* s) { // Print string at x, y (double size)
  byte tmp = strlen(s);
  cleartextarea(x, y, tmp * 12, y + 1);
  for (byte i = 0; i < tmp && i < MAXCHAR2; i++) print2(x + i * 12, y, s[i], false);
}
static void printstring4(byte x, byte y, char* s) { // Print string at x, y (quad size)
  byte tmp = strlen(s);
  cleartextarea(x, y, tmp * 22, y + 3);
  for (byte i = 0; i < tmp && i < MAXCHAR4; i++) print4(x + i * 22, y, s[i], false);
}

static byte button() { // Get keycode for pressed button
  if (!digitalRead(PIN_B_BUTTON)) return ('B');
  if (!digitalRead(PIN_A_BUTTON)) return ('A');
  if (!digitalRead(PIN_RIGHT_BUTTON)) return ('R');
  if (!digitalRead(PIN_DOWN_BUTTON)) return ('D');
  if (!digitalRead(PIN_LEFT_BUTTON)) return ('L');
  if (!digitalRead(PIN_UP_BUTTON)) return ('U');
  return (NULL);
}

static byte getkey() { // Get one debounced key
  static byte oldkey = 0; // Needed for debouncing
  static long keytimestamp = 0; // Needed for key repeating
  byte k = button();
  if (k != oldkey || millis() - keytimestamp > KEYREPEATTIME) {
    oldkey = k;
    keytimestamp = millis();
    return (k);
  }
  return (NULL);
}

static void wakeupnow() {} // Dummy wakeup code

static void sleepnow(void) { // Power down - wake up by pressing A (interrupt 4)
  ledsoff();
  screenoff(); // Display off
  pinMode(A_BUTTON, INPUT_PULLUP);
  attachInterrupt(4, wakeupnow, LOW);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  attachInterrupt(4, wakeupnow, LOW);
  sleep_mode();
  // SLEEP ... till A (interrupt 4 on Leonardo) is pressed //
  sleep_disable();
  detachInterrupt(4);
  screenon(); // Display on
  delayshort(200);
  ledsoff();
  //power_all_enable();
  //bootpowersaving();
}

static int rawadc() { // Measure Vcc
  power_adc_enable();
  ADMUX = (_BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1)); // Set voltage bits
  delayshort(2); // Wait for ADMUX setting to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA, ADSC)); // Measuring
  power_adc_disable();
  return (ADC);
}

static void flashlightmode(void) { //  Secure wait to flash software if  UP is pressed when switching on
  if (!digitalRead(PIN_UP_BUTTON)) {
    sbufclr();
    display();
    digitalWrite(BLUE_LED, LOW);
    while (true) {}; // Wait
  }
}

static void ledsoff(void) { // Disable LEDs
  digitalWrite(RED_LED, HIGH); digitalWrite(GREEN_LED, HIGH); digitalWrite(BLUE_LED, HIGH);
  TXLED1; RXLED1;
  istorch = false;
}

static void sound(int f, byte d) { // Play sound with frequency f for d ms
  if (issound) {
    TCCR3A = bit(COM3A0); // Set toggle on compare mode (which connects the pin)
    OCR3A = (long) (F_CPU / 16 / f); // Load the count (16 bits), which determines the frequency
    delayshort(d); // Delay for ms
    TCCR3A = 0; // Set normal mode (which disconnects the pin)
  }
}


/************************************************************
  AOS, APPLICATION
*************************************************************/

// INCLUDES


// DEFINES
#define MAXBYTE 255 // Maximal value of byte variable
#define MAXSTRBUF 12 // Maximal length of strbuf[]
#define SCREENWIDTH 64  // Virtual screen width (quarter of real screen)
#define SCREENHEIGHT 32 // Virtual screen height (quarter of real screen)
#define SCREENBYTES 256 // Number of bytes to address SCREEN (SCREENWIDTH x SCREENHEIGHT)/8
#define ARENAWIDTH 64 // Arena length (must be double of screen to enable 2d map mode)
#define ARENAHEIGHT 32  // Arena width (must be double of screen to enable 2d map mode)
#define ARENABYTES 256 // Number of bytes to address ARENA (ARENALENGTH x ARENAWIDTH)/8
#define MAXARENA 3 // Maximal number of arenas to fit in EEPROM (1k)
#define EEVAR 2 // EEPROM-address where variables are stored
#define EEARENA 10 // EEPROM-address where arenas are stored (256 bytes per arena)
#define STEPLENGTH 1 // Length of man's step in pixel
#define TINYNUMBER 1e-7 // Number for rounding to 0
#define MAXITERATE 100 // Maximal number of Taylor series loops to iterate
#define COLOR_RED 1 // Colors
#define COLOR_GREEN 2
#define COLOR_BLUE 3
#define COLOR_WHITE 4
#define INPUTTYPEBYTE 1 // Input type 0...255
#define INPUTTYPEONOFF 2 // Input type on/off
#define MSGTIME 1000 // Message showing time in ms
#define MOVEENEMYTIME 1000 // Time intervall for enemies to move in ms
#define FIREENEMYTIME 1000 // Time intervall for enemies to fire in ms
#define VIEWANGLE 40 // Half viewangle in degrees      56 42 40 48
#define DIST 1      // Minimal distance between man and screen in pixel
#define DIST4 5     // Distance to print enemy with print4
#define DIST2 9     // Distance to print enemy with print2
#define DIST1 13    // Distance to print enemy with print
#define VIEWDIST 15 // Maximum view distance in pixel  20 18 15 16
#define DELTAANGLE 5 // Resolution of scan angle        7  6  5  8
#define WALLYMAX 18.0 // Max y screenpixel for far walls (double to ensure double calculation)
#define WALLYMIN 14.0 // Min y screenpixel for far walls (double to ensure double calculation)
#define ACTIVATEDIST 11 // Distance, when enemy will be activated
#define MAXENEMYTYPES 7 // Number of different enemy types (including salvage types)
#define MAXSALVETYPES 2 // Number of different salvage types (ammo, medi)
#define MAXENEMIES 9 // Number of enemies
#define MAXMEDI 5 // Maximal number of medipacks
#define MAXAMMO 5 // Maximal number of ammopacks
#define MAXOBJECTS 19 // Total number of objects (ammos+medis+enemies)
#define OBJECTS (ammonr+medinr+enemynr) // Current number of objects (ammos+medis+enemies)
#define MOVECLOCKLCM 12 // Lowest Common Multiple (LCM) for enemy move frequency
#define FIRECLOCKLCM 12 // Lowest Common Multiple (LCM) for enemy fire frequency

// Keychars
#define KEY_B 'B'
#define KEY_A 'A'
#define KEY_R 'R'
#define KEY_D 'D'
#define KEY_L 'L'
#define KEY_U 'U'

// Icons
#define ICNCOMPASS 's' // ... up to 'z'
#define ICNAMMO 'a'
#define ICNHEALTH 'b'
#define ICNENEMY 'c'

// Structures
struct Man {
  double x;
  double y;
  int angle;
  byte health;
  byte ammo;
};
//              x  y  a  hlt  amm
#define INIMAN {5, 5, 0, 127, 127}


// Enemy struct, types and initials
struct Enemy {
  byte type;
  boolean isalive, isactive, israndomposition;
  double x;
  double y;
  byte health;
  byte moveperiod;
  byte fireperiod;
  byte gundist;
  byte drainhealth;
};

#define TYPAMMO 0
#define TYPMEDI 1
#define TYPROBO 2
#define TYPTAUR 3
#define TYPNITE 4
#define TYPEVIL 5
#define TYPOCTO 6
const byte    etyp[MAXENEMYTYPES] = {TYPAMMO, TYPMEDI, TYPROBO, TYPTAUR, TYPNITE, TYPEVIL, TYPOCTO};
const boolean eali[MAXENEMYTYPES] = {true,    true,    true,    true,    true,    true,    true};
const boolean eact[MAXENEMYTYPES] = {false,   false,   false,   false,   false,   false,   false};
const boolean ernd[MAXENEMYTYPES] = {true,    true,    true,    true,    true,    true,    true};
const byte    ex[MAXENEMYTYPES]   = {12,      20,      28,      36,      42,      50,      58};
const byte    ey[MAXENEMYTYPES]   = {20,      20,      20,      20,      20,      20,      20};
const byte    eh[MAXENEMYTYPES]   = {0,       0,       112,     80,      48,      144,     80};
const byte    mper[MAXENEMYTYPES] = {255,     255,     3,       1,       2,       4,       1};
const byte    fper[MAXENEMYTYPES] = {255,     255,     3,       2,       1,       1,       4};
const byte    egun[MAXENEMYTYPES] = {255,     255,     10,      2,       5,       10,      2};
const byte    edh[MAXENEMYTYPES]  = {255,     255,     16,      16,      32,      48,      255};


// GLOBAL VARIABLES
static char strbuf[MAXSTRBUF]; // Holds string to print
static char msgstr[MAXSTRBUF]; // Holds message to print
static long menutimestamp, menuescapetimestamp, showmsgtimestamp; // Timestamps
static long moveenemytimestamp, fireenemytimestamp;
static Man man; // Man variable
static Enemy enemy[MAXOBJECTS]; // ammo, medi, robo, taur, nite, evil
static byte anglestep; // Rotation step in degrees
static boolean islost, iswon; // Game over indicators
static boolean ismenu; // True in menu mode
static boolean ismap; // Plays on a 2d-map if true
static byte menunr; // Number of menu
static boolean isstatus; // If true status is shown (health, ammo)
static boolean issky; // If true sky/ceiling and floor line is shown
static boolean isshowmsg; // True, if last message is shown
static boolean ismsg; // If true is set messages are displayes
static byte moveclock; // Counts enemy moving steps (1...4)
static byte fireclock; // Counts enemy fire frequency (1...4)
static byte arenanr; // Number of loaded arena
static boolean isedit; // True if map editor is used
static byte editx, edity; // coordinates of editing cursor
static boolean isdrawwhite; // True, if map editor draws white
static byte ammonr, medinr, enemynr; // Number of ammopacks, medipacks and enemies
static byte enemieskilled; // Number of enemies killed
static byte killedbyicon; // Icon number of enemy that killed man
static boolean ishurt; // If true - flash white screen for one frame
static boolean ishit; // True if enemy was hit


// Arena - Uncomment your favourite arena, flash the software and save it to the EEPROM
static byte arena[] =
  //*/ ARENA0 (colums)
    {
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };
    //*/

  /*/ ARENA1 (hallways)
    {
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
    };
    //*/

  /*/ ARENA2 (bunks)
    {
    0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7E,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x7E,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x7E,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x7E,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x7E,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x7E,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x7E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xFC,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xFC,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xFC,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xFC,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xFC,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    };
    //*/

  /*/ ARENA3 (zigzag)
    {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0xFF,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0xC1,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0xFF,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0xC1,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0xFF,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0xC1,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0xFF,0x01,0x00,0x00,0x00,0x01,0x01,0x01,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x00,0x00,0x00,0x00,0x01,0x01,0x01,
    };
    //*/

  /*/ ARENA4 (spiral)
    {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0xF0, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0xF0, 0x00, 0x00, 0x00, 0xF0, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0xF0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xF0, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0xF0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x1F, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x01, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x11, 0x11, 0x10, 0x10, 0x10, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x11, 0x11, 0x10, 0x10, 0x10,
    0x01, 0x01, 0x01, 0x01, 0xF1, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x10, 0x10, 0x10, 0xF1, 0x01, 0x01, 0x01, 0xF1, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x10, 0x10, 0x10, 0xFF, 0x00, 0x00, 0x00, 0x1F, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x11, 0x1F, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0xFF, 0x01, 0x01, 0x01, 0xFF, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x1F, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x11, 0x1F, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x1F, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x11, 0x1F, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x01, 0x01, 0x01, 0x11, 0x11, 0x11, 0x11, 0x1F, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x1F, 0x10, 0x10, 0x10, 0x11, 0x10, 0x10, 0x10, 0x1F, 0x00, 0x00, 0x00,
    };
    //*/

  /*/ ARENA5 (endless)
    {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x00,0x00,0x00,
    0x01,0x01,0x01,0x01,0xF1,0x01,0x01,0x01,0xFF,0x10,0x10,0x10,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x01,0x01,0x01,
    0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x1F,0x10,0x10,0x10,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x01,0x01,0x01,
    0x10,0x10,0x10,0x10,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x01,0x01,0x01,
    };
    //*/

  /*/ ARENA6 (uturn)
    {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x10,0x10,0x10,0xF0,0x00,0x00,0x00,0x10,0x10,0x10,0x10,0x1F,0x00,0x00,0x00,0xF0,0x10,0x10,0x10,0x10,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0x1F,0x10,0x10,0x10,0x10,0x00,0x00,0x00,0x10,0x10,0x10,0x10,
    0x00,0x00,0x00,0x00,0x1F,0x11,0x11,0x11,0x11,0x00,0x00,0x00,0x11,0x10,0x10,0x10,0xF1,0x00,0x00,0x00,0x1F,0x10,0x10,0x10,0x1F,0x00,0x00,0x00,0xF1,0x11,0x11,0x11,0xF1,0x00,0x00,0x00,0x11,0x11,0x11,0x11,0x1F,0x00,0x00,0x00,0xF1,0x11,0x11,0x11,0x11,0x00,0x00,0x00,0x1F,0x01,0x01,0x01,0x1F,0x00,0x00,0x00,0xF1,0x01,0x01,0x01,
    0x00,0x00,0x00,0x00,0x1F,0x01,0x01,0x01,0x1F,0x00,0x00,0x00,0xF1,0x01,0x01,0x01,0xF1,0x00,0x00,0x00,0x1F,0x11,0x11,0x11,0x11,0x00,0x00,0x00,0x11,0x10,0x10,0x10,0xF1,0x00,0x00,0x00,0x1F,0x10,0x10,0x10,0x1F,0x00,0x00,0x00,0xF1,0x11,0x11,0x11,0xF1,0x00,0x00,0x00,0x11,0x11,0x11,0x11,0x1F,0x00,0x00,0x00,0xF1,0x11,0x11,0x11,
    0x00,0x00,0x00,0x00,0x11,0x11,0x11,0x11,0x1F,0x00,0x00,0x00,0xF1,0x11,0x11,0x11,0x11,0x00,0x00,0x00,0x1F,0x01,0x01,0x01,0x1F,0x00,0x00,0x00,0xF1,0x01,0x01,0x01,0xF1,0x00,0x00,0x00,0x1F,0x11,0x11,0x11,0x11,0x00,0x00,0x00,0x11,0x10,0x10,0x10,0xF1,0x00,0x00,0x00,0x1F,0x10,0x10,0x10,0x1F,0x00,0x00,0x00,0xF1,0x01,0x01,0x01,
    };
    //*/

  /*/ ARENA7 (ses)
    {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x10,0x10,0x10,0xF0,0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0xF0,0x10,0x10,0x10,0xF0,0x00,0x00,0x00,0xF0,0x10,0x10,0x10,0x10,0x00,0x00,0x00,0x10,0x10,0x10,0x10,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x10,0x10,0x10,0xF0,0x00,0x00,0x00,
    0x11,0x11,0x11,0x11,0x11,0x00,0x00,0x00,0xF1,0x01,0x01,0x01,0xF1,0x10,0x10,0x10,0xF1,0x01,0x01,0x01,0xF1,0x10,0x10,0x10,0x11,0x00,0x00,0x00,0x11,0x11,0x11,0x11,0x1F,0x00,0x00,0x00,0x1F,0x10,0x10,0x10,0x1F,0x01,0x01,0x01,0x1F,0x00,0x00,0x00,0x1F,0x11,0x11,0x11,0xF1,0x00,0x00,0x00,0xF1,0x01,0x01,0x01,0xF1,0x10,0x10,0x10,
    0x01,0x01,0x01,0x01,0xF1,0x10,0x10,0x10,0xF1,0x01,0x01,0x01,0xF1,0x10,0x10,0x10,0x11,0x00,0x00,0x00,0x11,0x11,0x11,0x11,0x1F,0x00,0x00,0x00,0x1F,0x10,0x10,0x10,0x1F,0x01,0x01,0x01,0x1F,0x00,0x00,0x00,0x1F,0x11,0x11,0x11,0xF1,0x00,0x00,0x00,0xF1,0x01,0x01,0x01,0xF1,0x10,0x10,0x10,0xF1,0x01,0x01,0x01,0x11,0x10,0x10,0x10,
    0x01,0x01,0x01,0x01,0xF1,0x00,0x00,0x00,0xF1,0x00,0x00,0x00,0x11,0x11,0x11,0x11,0x1F,0x00,0x00,0x00,0x1F,0x10,0x10,0x10,0x1F,0x01,0x01,0x01,0x1F,0x00,0x00,0x00,0x1F,0x11,0x11,0x11,0xF1,0x00,0x00,0x00,0xF1,0x01,0x01,0x01,0xF1,0x10,0x10,0x10,0xF1,0x01,0x01,0x01,0xF1,0x10,0x10,0x10,0x11,0x00,0x00,0x00,0x11,0x11,0x11,0x11,
    };
    //*/

  /*/ ARENA8 (plus)
    {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x10,0x10,0x10,0xFF,0x10,0x10,0x10,0x10,0x00,0x00,0x00,0x10,0x10,0x10,0x10,0xFF,0x10,0x10,0x10,0x10,0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0x10,0x10,0x10,0x10,0x1F,0x00,0x00,0x00,0x10,0x10,0x10,0x10,0xFF,0x10,0x10,0x10,0x10,0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,
    0x01,0x01,0x01,0x01,0x11,0x10,0x10,0x10,0xFF,0x10,0x10,0x10,0x11,0x00,0x00,0x00,0x10,0x10,0x10,0x10,0xFF,0x10,0x10,0x10,0x11,0x00,0x00,0x00,0xF1,0x01,0x01,0x01,0x1F,0x01,0x01,0x01,0x11,0x10,0x10,0x10,0xFF,0x10,0x10,0x10,0x10,0x00,0x00,0x00,0xF1,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x1F,0x01,0x01,0x01,0x41,0x40,0x40,0x40,
    0x10,0x10,0x10,0x10,0xFF,0x10,0x10,0x10,0x11,0x00,0x00,0x00,0x10,0x10,0x10,0x10,0xFF,0x10,0x10,0x10,0x11,0x00,0x00,0x00,0xF1,0x01,0x01,0x01,0x1F,0x01,0x01,0x01,0x11,0x10,0x10,0x10,0xFF,0x10,0x10,0x10,0x11,0x00,0x00,0x00,0xF1,0x01,0x01,0x01,0x1F,0x01,0x01,0x01,0x11,0x10,0x10,0x10,0xFF,0x10,0x10,0x10,0x10,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x11,0x10,0x10,0x10,0xFF,0x10,0x10,0x10,0x10,0x00,0x00,0x00,0xF1,0x10,0x10,0x10,0x11,0x01,0x01,0x01,0x1F,0x01,0x01,0x01,0x11,0x10,0x10,0x10,0xFF,0x10,0x10,0x10,0x11,0x00,0x00,0x00,0xF1,0x01,0x01,0x01,0x1F,0x01,0x01,0x01,0x11,0x10,0x10,0x10,0xFF,0x10,0x10,0x10,0x11,0x00,0x00,0x00,0xF1,0x01,0x01,0x01,
    };
    //*/

  /*/ ARENA9 (mix)
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xF1, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0xF1, 0x01, 0x01, 0x01, 0xF1, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x1F, 0x01, 0x01, 0x01, 0xF1, 0x11, 0x11, 0x11, 0xF1, 0x01, 0x01, 0x01, 0xFF, 0x00, 0x00, 0x00, 0x1F, 0x01, 0x01, 0x01, 0xF1, 0x01, 0x01, 0x01, 0x1F, 0x00, 0x00, 0x00, 0xFF, 0x01, 0x01, 0x01, 0xF1, 0x01, 0x01, 0x01, 0xFF, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xF1, 0x10, 0x10, 0x10, 0x1F, 0x00, 0x00, 0x00, 0xF1, 0x01, 0x01, 0x01, 0x1F, 0x01, 0x01, 0x01, 0x11, 0x11, 0x11, 0x11, 0xF1, 0x01, 0x01, 0x01, 0x1F, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x11, 0x11, 0x10, 0x10, 0x10,
  0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0xF1, 0x00, 0x00, 0x00, 0xF1, 0x01, 0x01, 0x01, 0x1F, 0x01, 0x01, 0x01, 0xF1, 0x00, 0x00, 0x00, 0xF1, 0x01, 0x01, 0x01, 0xF1, 0x00, 0x00, 0x00, 0x1F, 0x10, 0x10, 0x10, 0x1F, 0x01, 0x01, 0x01, 0x1F, 0x00, 0x00, 0x00,
};
//*/

// Menu
#define MNUNEW 0
#define MNUVIEW 2
#define MNUMEDI 3
#define MNUAMMO 4
#define MNUENEMIES 5
#define MNUSOUND 6
#define MNULIT 7
#define MNUSTATUS 8
#define MNUSKY 9
#define MNUMSG 10
#define MNURESET 11
#define MNUWHITE 13
#define MNUBLACK 14
#define MNUCLR 15
#define MNULOAD 16
#define MNUSAVE 17
#define MNUBATT 18
#define MNUABOUT 19
#define MNUOFF 20
const char m0[] PROGMEM  = "NEW GAME";  // New game
const char m1[] PROGMEM  = "SETTINGS";  // Settings
const char m2[] PROGMEM  = " 2D/3D";    // Toggle between 2d- and 3d-view
const char m3[] PROGMEM  = " MEDI";     // Set number of medipacks
const char m4[] PROGMEM  = " AMMO";     // Set number of ammopacks
const char m5[] PROGMEM  = " ENEMIES";  // Set number of enemies
const char m6[] PROGMEM  = " SOUND";    // Sound on/off
const char m7[] PROGMEM  = " CONTRAST"; // Set contrast (0...255)
const char m8[] PROGMEM  = " STATUS";   // Show/hide status
const char m9[] PROGMEM  = " SKY";      // Show/hide sky
const char m10[] PROGMEM = " MESSAGES"; // Show/hide messages
const char m11[] PROGMEM = " RESET";    // Reset settings
const char m12[] PROGMEM = "EDIT MAP";  // Map editor
const char m13[] PROGMEM = " WHITE";    // Draw white pixel
const char m14[] PROGMEM = " BLACK";    // Draw black pixel
const char m15[] PROGMEM = " CLEAR";    // Clear map
const char m16[] PROGMEM = " LOAD";     // Load map
const char m17[] PROGMEM = " SAVE";     // Save map
const char m18[] PROGMEM = "BATT";      // Show supply voltage
const char m19[] PROGMEM = "ABOUT";     // About info
const char m20[] PROGMEM = "OFF";       // Deep sleep mode
const char* const menu_table[] PROGMEM = {
  m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15, m16, m17, m18, m19, m20
};
#define numberofmenuentries (sizeof(menu_table)/sizeof(const char *))

// Messages and names
#define MSGNOAMMO 7
#define MSGKILLED 8
#define MSGABOUT 9
const char n0[] PROGMEM = "AMMO";  // Ammunition pack
const char n1[] PROGMEM = "MEDI";  // Medipack
const char n2[] PROGMEM = "ROBO";  // Robot
const char n3[] PROGMEM = "TAUR";  // Taurus
const char n4[] PROGMEM = "NITE";  // Knight
const char n5[] PROGMEM = "EVIL";  // Evil ghost
const char n6[] PROGMEM = "OCTO";  // Octopus
const char n7[] PROGMEM = "EMPTY"; // Empty gun
const char n8[] PROGMEM = "GONE";  // Enemy/object inactive
const char n9[] PROGMEM = "ARENA"; // Game name
const char n10[] PROGMEM = "V1.0  2018"; // Version and year
const char n11[] PROGMEM = "[C] DEETEE"; // Programmer
const char* const msg_table[] PROGMEM = {n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11};

// Macros
#define _ones(x) ((x)%10)        // Calculates ones unit
#define _tens(x) (((x)/10)%10)   // Calculates tens unit
#define _huns(x) (((x)/100)%10)  // Calculates hundreds unit
#define _tsds(x) (((x)/1000)%10) // Calculates thousands unit
#define _pcts(x) (_tens((int)((x) / 2.56)) + '0') // Calculates decade digit of percentage of 256
#define _sgn(x)  (((x)>0)-((x)<0)) // Signum function


// SUBROUTINES
static void fillstrbuf(byte n) { // Fills strbuf with n blanks
  byte tmp = _min(n, MAXSTRBUF - 1);
  for (byte i = 0; i < tmp; i++) strbuf[i] = ' ';
  strbuf[tmp] = NULL;
}

static byte ledblink(byte color) { // Flash led (RGBW)
  if (color == COLOR_RED) digitalWrite(RED_LED, LOW);
  else if (color == COLOR_GREEN) digitalWrite(GREEN_LED, LOW);
  else if (color == COLOR_BLUE) digitalWrite(BLUE_LED, LOW);
  else {
    digitalWrite(RED_LED, LOW); digitalWrite(GREEN_LED, LOW); digitalWrite(BLUE_LED, LOW);
  }
  delayshort(1);
  digitalWrite(RED_LED, HIGH); digitalWrite(GREEN_LED, HIGH); digitalWrite(BLUE_LED, HIGH);
}

static byte input(byte x, byte y, int mini, int maxi, int select, byte type) { // Input byte at x,y
  byte page = sqrt(maxi - mini);
  byte initialselect = select;
  for (;;) {
    byte k = getkey(); // Query key
    if (k) { // Interpret key
      if (k == KEY_B) return (select); // Select
      if (k == KEY_A) return (initialselect); // ESC
      else if (k == KEY_R) { // Page up
        if (select <= maxi - page) select += page;
        else select = maxi;
      }
      else if (k == KEY_D) { // Next
        if (select > mini) select--;
        else select = maxi;
      }
      else if (k == KEY_L ) { // Page down
        if (select >= mini + page) select -= page;
        else select = mini;
      }
      else if (k == KEY_U) { // Previous
        if (select < maxi) select++;
        else select = mini;
      }
    }
    fillstrbuf(5);
    strbuf[0] = '`';
    if (type == INPUTTYPEBYTE) {
      strbuf[1] = _huns(select) + '0';
      strbuf[2] = _tens(select) + '0';
      strbuf[3] = _ones(select) + '0';
    }
    else { // ON/OFF
      strbuf[1] = 'O';
      if (select) strbuf[2] = 'N';
      else strbuf[2] = strbuf[3] = 'F';
    }
    printstring(x, y, strbuf);
    display();
  }
}

static byte getpixel(byte x, byte y) { // Returns 2 if pixel at x,y is set and 1 if limit is reached
  if (x >= 0 && x < ARENAWIDTH && y >= 0 && y < ARENAHEIGHT) {
    byte row = y / 8;
    byte bitpos = y % 8;
    //return (arena[x + (row * ARENALENGTH)] & _BV(bitpos)) >> bitpos;
    if (arena[x + (row * ARENAWIDTH)] & _BV(bitpos)) return (2); // Wall detected
    else return (0); // Nothing
  }
  else return (1); // Limit passed
}

static byte screenpixel(byte x, byte y) { // Sets sbuf at x,y
  if (x < SCREENWIDTH && y < SCREENHEIGHT) sbuf[x + (y / 8 * SCREENWIDTH)] |= _BV(y % 8);
}
static byte screenpixelclr(byte x, byte y) { // Unsets sbuf at x,y
  if (x < SCREENWIDTH && y < SCREENHEIGHT) sbuf[x + (y / 8 * SCREENWIDTH)] &= ~_BV(y % 8);
}
static byte arenapixel(byte x, byte y) { // Sets sbuf at x,y
  if (x < ARENAWIDTH && y < ARENAHEIGHT) arena[x + (y / 8 * ARENAWIDTH)] |= _BV(y % 8);
}
static byte arenapixelclr(byte x, byte y) { // Unsets sbuf at x,y
  if (x < ARENAWIDTH && y < ARENAHEIGHT) arena[x + (y / 8 * ARENAWIDTH)] &= ~_BV(y % 8);
}

static void cleararea(byte x1, byte y1, byte x2, byte y2) { // Clears area of screen
  for (byte x = x1; x <= x2; x++)
    for (byte y = y1; y <= y2; y++)
      if (x < SCREENWIDTH && y < SCREENHEIGHT) screenpixelclr(x, y);
}

static void printmenu() { // Prints menu tree
  clrline(0, 3);
  for (byte i = 0; i < 4; i++) {
    if (i + menunr < numberofmenuentries) {
      strcpy_P(strbuf, (char*)pgm_read_word(&(menu_table[i + menunr]))); // Copy string to buffer
      printstring(6, i, strbuf);
    }
  }
  print1(0, 0, '}');
}

static int alignangle(int angle) { // Aligns angle to positive values
  return ((angle + 360) % 360);
}

static boolean checkstep(double x, double y) { // Returns true if next step has no hurdles or wall
  if (x < 0) return (false);
  else if (x > ARENAWIDTH - 1) return (false);
  else if (y < 0) return (false);
  else if (y > ARENAHEIGHT - 1) return (false);
  else if (getpixel(round(x), round(y))) return (false);
  return (true);
}

static byte distance(byte x, byte y) { // Calculates distance between man and coordinates
  int dx = ((int)x - man.x); int dy = ((int)y - man.y);
  return (sqrt(dx * dx + dy * dy));
}

static void drawstatus() { // Draw status (health, ammo, gun, compass)
  print1(0, 0, _pcts(man.health)); print1(0, 1, ICNHEALTH); // Health
  print1(59, 0, _pcts(man.ammo)); print1(59, 1, ICNAMMO); // Ammo
  cleararea(0, 23, 5, 23); print1(0, 3, (enemynr - enemieskilled) + '0'); // Enemies remaining
  print1(59, 3, ICNCOMPASS + alignangle(man.angle + 15) / 45); // Compass
  screenpixel(31, 30); screenpixel(31, 31); // Gun
}

static void drawsky(void) { // Draw sky and floor line due to man.angle
  for (byte i = 0; i < 3; i++) {
    byte n = (5 -  i) * i; // Position 0, 4, 6
    byte tmp = 2 * n * tan(man.angle * PI / 180) + 31;
    screenpixel(tmp, n);
    screenpixel(tmp, SCREENHEIGHT - n - 1);
  }
}

static void printmsg(byte nr) { // Copies message text to message buffer
  strcpy_P(msgstr, (char*)pgm_read_word(&(msg_table[nr])));
  showmsgtimestamp = millis();
  isshowmsg = true;
}

static void showmsg() { // Prints last message
  for (byte i = 0; i < strlen(msgstr) * 6; i++) screenpixelclr(i, 23); // Clear line above message
  printstring(0, 3, msgstr);
}

static void printenemy(byte nr, byte screenx, byte d) { // Print enemy
  byte icon = enemy[nr].type + 'a';
  byte y = 18;
  if (d < DIST4 && screenx > 10) print4(screenx - 10, 0, icon, false); // Very close
  else if (d < DIST2 && screenx > 5) print2(screenx - 5, 2, icon, false); // Close
  else if (d < DIST1 && screenx > 3) print1(screenx - 3, 2, icon); // Far
  else { // Very far
    screenpixel(screenx - 2, y); screenpixel(screenx - 1, y); screenpixel(screenx, y);
    screenpixel(screenx - 2, y + 1); screenpixel(screenx - 1, y + 1); screenpixel(screenx, y + 1);
  }
}

static boolean isfreeview(byte xe, byte ye) { // Returns true, if nothing is between man and enemy
  byte x, y;
  for (x = xe, y = ye; x != (byte)man.x || y != (byte)man.y ;) {
    if (y > (byte)man.y) y--;
    if (y < (byte)man.y) y++;
    if (x > (byte)man.x) x--;
    if (x < (byte)man.x) x++;
    if (getpixel(x, y)) return (false);
  }
  return (true);
}

static void scan() { // Scan view for walls and enemies
  boolean isenemyprinted[OBJECTS];
  for (byte i = 0; i < OBJECTS; i++) isenemyprinted[i] = false;
  for (int angle = 0; _abs(angle) <= VIEWANGLE; angle = _sgn(angle) > 0 ? -angle : -angle + DELTAANGLE) { // Scan angle
    byte screenx = (double)SCREENWIDTH / 2 - (double)SCREENWIDTH / VIEWANGLE / 2 * (angle);
    for (byte dist = VIEWDIST; dist >= DIST; dist -= 1) { // Scan length
      int tmpangle = alignangle(man.angle + angle);
      byte x = round(man.x + dist * cos(tmpangle * PI / 180));
      byte y = round(man.y - dist * sin(tmpangle * PI / 180));
      byte examine = getpixel(x, y);
      if (examine) { // Wall detected -> print
        byte screenymin = WALLYMIN / (VIEWDIST - DIST) * (dist - DIST);
        double k = (SCREENHEIGHT - WALLYMAX) / (DIST - VIEWDIST);
        byte screenymax = dist * k + (SCREENHEIGHT - DIST * k);
        for (byte i = screenymax + 1; i > screenymin; i--) screenpixel(screenx + ((examine > 1) ? (i % 2) : 0), i - 1);
      }
      for (byte i = 0; i < OBJECTS; i++) { // Scan for enemy
        if (x == enemy[i].x && y == enemy[i].y && !isenemyprinted[i] && enemy[i].isalive) {
          if (isfreeview(x, y)) { // Print enemy if visible
            if (dist < 12 && !enemy[i].isactive) { // Wake up enemy
              enemy[i].isactive = true;
              printmsg(enemy[i].type);
            }
            if (ishit) {
              printenemy(i, screenx - 2, dist);
              ishit = false;
            }
            else printenemy(i, screenx, dist);
            isenemyprinted[i] = true;
          }
        }
      }
    }
  }
}

static void savevar(void) { // Saves user settings to EEPROM
  int addr = EEVAR;
  EEPROM.write(addr++, ismap); // 2D/3D
  EEPROM.write(addr++, medinr); EEPROM.write(addr++, ammonr); // Equipment
  EEPROM.write(addr++, enemynr);
  EEPROM.write(addr++, issound); EEPROM.write(addr++, isstatus); // Environment
  EEPROM.write(addr++, issky); EEPROM.write(addr++, ismsg);
}

static void loadvar(void) { // Loads user settings from EEPROM
  int addr = EEVAR;
  ismap = EEPROM.read(addr++); // 2D/3D
  medinr = EEPROM.read(addr++); ammonr = EEPROM.read(addr++); // Equipment
  enemynr = EEPROM.read(addr++);
  issound = EEPROM.read(addr++); isstatus = EEPROM.read(addr++); // Environment
  issky = EEPROM.read(addr++); ismsg = EEPROM.read(addr++);
}

static void initial(void) { // Initializes game (random enemies and positions)
  man = INIMAN; // Init man
  for (byte i = 0; i < OBJECTS; i++) { // Init enemies
    byte typ;
    if (i < ammonr) typ = TYPAMMO;
    else if (i < ammonr + medinr) typ = TYPMEDI;
    else typ = random(MAXENEMYTYPES - MAXSALVETYPES) + MAXSALVETYPES;
    enemy[i].type = etyp[typ];
    enemy[i].isalive = eali[typ];
    enemy[i].isactive = eact[typ];
    enemy[i].israndomposition = ernd[typ];
    enemy[i].x = ex[typ];
    enemy[i].y = ey[typ];
    enemy[i].health = eh[typ];
    enemy[i].moveperiod = mper[typ];
    enemy[i].fireperiod = fper[typ];
    enemy[i].gundist = egun[typ];
    enemy[i].drainhealth = edh[typ];
    if (enemy[i].israndomposition) { // Assign random position
      byte x = random(ARENAWIDTH) - 1; byte y = random(ARENAHEIGHT) - 1;
      while (!checkstep(x, y)) { // Prevent assigning "in" a wall
        x = random(ARENAWIDTH) - 1;  y = random(ARENAHEIGHT) - 1;
      }
      enemy[i].x = x; enemy[i].y = y;
    }
  }
}

static void resetenvironment(void) { // Resets user defined settings (saved in EEPROM)
  ismap = false;
  ammonr = medinr = enemynr = 1;
  issound = true;
  isstatus = issky = ismsg = true;
  savevar();
}

static void reset(void) { // Resets all variables
  menutimestamp = menuescapetimestamp = showmsgtimestamp = millis();
  ismenu = ismsg = isdrawwhite = true;
  isshowmsg = isedit = ishurt = ishit = false;
  islost = iswon = false;
  anglestep = 15;
  menunr = 0;
  moveclock = fireclock = arenanr = 1;
  editx = edity = 0;
  enemieskilled = 0;
  killedbyicon = ICNENEMY;
  initial();
}


// SETUP & LOOP
void setup() {
  bootpins(); // System boot procedure
  bootSPI();
  bootscreen();
  bootpowersaving();
  setframerate(FRAMERATE);
  flashlightmode(); // Secure waiting for flahsing new software (when UP+on)

  TCCR3A = 0; // Init sound (Speaker pin 1, Timer 3A, Port C bit 6, Arduino pin 5)
  TCCR3B = (bit(WGM32) | bit(CS31)); // CTC mode. Divide by 8 clock prescale
  bitSet(SPEAKER_1_DDR, SPEAKER_1_BIT); // Enable audio (on, unmute)
  bitSet(SPEAKER_2_DDR, SPEAKER_2_BIT); // Enable audio (on, unmute)

  reset();
  if (!digitalRead(PIN_DOWN_BUTTON)) resetenvironment(); // Reset saved variables
  loadvar();
  initial();

  digitalWrite(GREEN_LED, LOW); // Logo
  print2(0, 1, 'o', false);
  print4(11, 0, 'p', true);
  print4(22, 0, 'q', false);
  print4(43, 0, 'r', true);
  print2(54, 1, 'o', false);
  display();

  delay(1500); // Needed to stop serial communication to switch LEDs off
  sbufclr();
  setscreencontrast(EEPROM.read(EECONTRAST));
  ledsoff();
}


void loop() {
  if (!(nextFrame())) return; // Pause render (idle) until it's time for the next frame

  if (!digitalRead(PIN_B_BUTTON) && !digitalRead(PIN_A_BUTTON)) reset(); // Reset

  if (!digitalRead(PIN_A_BUTTON) && !ismenu && millis() - menuescapetimestamp > KEYREPEATTIME) {
    ismenu = true; // Menu
    menutimestamp = millis();
  }

  if (islost || iswon) { // *** GAME OVER ***
    sbufclr();
    print2(2, 0, 'U', false);
    if (islost) { // Lost
      print2(19, 0, 'D', false);
      print2(29, 0, 'I', false);
      print2(39, 0, 'E', false);
      print2(52, 0, 'D', false);
    }
    else { // Won
      print2(22, 0, 'W', false);
      print2(35, 0, 'O', false);
      print2(48, 0, 'N', false);
    }
    print1(3, 2, ICNHEALTH); print1(3, 3, _pcts(man.health)); // Health
    print1(21, 2, ICNAMMO); print1(21, 3, _pcts(man.ammo)); // Ammo
    print1(48, 2, killedbyicon); // Enemies
    fillstrbuf(3);
    strbuf[0] = _ones(enemieskilled) + '0';
    strbuf[1] = '/';
    strbuf[2] = _ones(enemynr) + '0';
    printstring(42, 3, strbuf);
    display();
    if (!digitalRead(PIN_A_BUTTON)) reset();
  }

  else if (isedit) { // *** EDIT MAP ***
    sbufclr();
    for (int i = 0; i < SCREENBYTES; i++) sbuf[i] = arena[i];
    if (!digitalRead(PIN_B_BUTTON)) { // Draw
      if (isdrawwhite) {
        screenpixel(editx, edity);
        arenapixel(editx, edity);
      }
      else {
        screenpixelclr(editx, edity);
        arenapixelclr(editx, edity);
      }
    }
    if (!digitalRead(PIN_A_BUTTON)) { // ESC
      isedit = false;
    }
    if (!digitalRead(PIN_RIGHT_BUTTON)) { // Right
      if (editx < ARENAWIDTH - 1) editx++;
      else editx = 0;
    }
    if (!digitalRead(PIN_DOWN_BUTTON)) { // Down
      if (edity < ARENAHEIGHT - 1) edity++;
      else edity = 0;
    }
    if (!digitalRead(PIN_LEFT_BUTTON)) { // Left
      if (editx > 1) editx--;
      else editx = ARENAWIDTH - 1;
    }
    if (!digitalRead(PIN_UP_BUTTON)) { // Up
      if (edity > 1) edity--;
      else edity = ARENAHEIGHT - 1;
    }
    if (editx < 42) { // Print coordinates
      cleararea(51, 0, 63, 7);
      print1(52, 0, _tens(editx) + '0'); print1(58, 0, _ones(editx) + '0');
    }
    else {
      cleararea(19, 0, 31, 7);
      print1(20, 0, _tens(editx) + '0'); print1(26, 0, _ones(editx) + '0');
    }
    if (edity < 19) {
      cleararea(0, 23, 11, 31);
      print1(0, 3, _tens(edity) + '0'); print1(6, 3, _ones(edity) + '0');
    }
    else {
      cleararea(0, 7, 11, 16);
      print1(0, 1, _tens(edity) + '0'); print1(6, 1, _ones(edity) + '0');
    }
    for (byte i = 1; i <= 2; i++) { // Print cursor (x)
      if (editx > i && edity > i) screenpixel(editx - i, edity - i);
      if (editx < SCREENWIDTH - i && edity > i) screenpixel(editx + i, edity - i);
      if (editx > i && edity < SCREENHEIGHT - i) screenpixel(editx - i, edity + i);
      if (editx < SCREENWIDTH - i && edity < SCREENHEIGHT - i) screenpixel(editx + i, edity + i);
    }
    display();
  }

  else if (ismenu) { // *** MENU ***
    byte key = getkey(); // Query key
    if (key) { // Interpret key
      if (key == KEY_B) {  // Choose menu entry
        if (menunr == MNUNEW) { // New game
          randomSeed(millis());
          reset();
          ismenu = false;
        }
        else if (menunr == MNUVIEW) { // Show map ... SETTINGS
          ismap = ismap ? false : true;
          ismenu = false;
          savevar();
        }
        else if (menunr == MNUMEDI) { // Set number of medipacks
          medinr = input(36, 0, 1, MAXMEDI, medinr, INPUTTYPEBYTE);
          savevar();
        }
        else if (menunr == MNUAMMO) { // Set number of ammopacks
          ammonr = input(36, 0, 1, MAXAMMO, ammonr, INPUTTYPEBYTE);
          savevar();
        }
        else if (menunr == MNUENEMIES) { // Set number of enemies
          enemynr = input(36, 0, 1, MAXENEMIES, enemynr, INPUTTYPEBYTE);
          savevar();
        }
        else if (menunr == MNUSOUND) { // Sound on/off
          issound = input(36, 0, 0, 1, issound ? 1 : 0, INPUTTYPEONOFF);
          savevar();
        }
        else if (menunr == MNUSTATUS) { // Status on/off
          isstatus = input(36, 0, 0, 1, isstatus ? 1 : 0, INPUTTYPEONOFF);
          savevar();
        }
        else if (menunr == MNUSKY) { // Sky on/off
          issky = input(36, 0, 0, 1, issky ? 1 : 0, INPUTTYPEONOFF);
          savevar();
        }
        else if (menunr == MNUMSG) { // Show messages on/off
          ismsg = input(36, 0, 0, 1, ismsg ? 1 : 0, INPUTTYPEONOFF);
          savevar();
        }
        else if (menunr == MNURESET) resetenvironment(); // Reset environment
        else if (menunr == MNUWHITE) { // Draw white pixel ... MAP EDITOR
          isdrawwhite = true;
          isedit = true;
        }
        else if (menunr == MNUBLACK) { // Draw black pixel
          isdrawwhite = false;
          isedit = true;
        }
        else if (menunr == MNUCLR) { // Clear map
          for (int i = 0; i < ARENABYTES; i++) arena[i] = NULL;
        }
        else if (menunr == MNULOAD || menunr == MNUSAVE) { // Load/Save map from/to EEPROM
          arenanr = input(36, 0, 1, MAXARENA, arenanr, INPUTTYPEBYTE); // Slot
          for (int i = 0; i < ARENABYTES; i++)
            if (menunr == MNULOAD) // Load map
              arena[i] = EEPROM.read(EEARENA + (int)(arenanr - 1) * ARENABYTES + i);
            else // Save map
              EEPROM.write(EEARENA + (int)(arenanr - 1) * ARENABYTES, arena[i] + i);
        }
        else if (menunr == MNULIT) { // Brightness
          byte contrast = input(36, 0, 0, 255, EEPROM.read(EECONTRAST), INPUTTYPEBYTE);
          setscreencontrast(contrast);
          EEPROM.write(EECONTRAST, contrast);
        }
        else if (menunr == MNUBATT) { // Show supply voltage /100
          int batt = (double)(1126400L / rawadc()) / 10;
          input(36, 0, batt, batt, batt, INPUTTYPEBYTE);
        }
        else if (menunr == MNUABOUT) { // About
          strcpy_P(strbuf, (char*)pgm_read_word(&(msg_table[MSGABOUT])));
          printstring2(0, 0, strbuf);
          strcpy_P(strbuf, (char*)pgm_read_word(&(msg_table[MSGABOUT + 1])));
          printstring(0, 2, strbuf);
          strcpy_P(strbuf, (char*)pgm_read_word(&(msg_table[MSGABOUT + 2])));
          printstring(0, 3, strbuf);
          display();
          while (!getkey()) ;
        }
        else if (menunr == MNUOFF) sleepnow(); // OFF
      }
      else if (key == KEY_A && millis() - menutimestamp > KEYREPEATTIME) { // Play
        ismenu = false;
        menuescapetimestamp = millis();
      }
      else if (key == KEY_R) { // Cursor right
        if (menunr < numberofmenuentries - 4) menunr += 4;
        else menunr = numberofmenuentries - 1;
      }
      else if (key == KEY_D) { // Cursor down
        if (menunr < numberofmenuentries - 1) menunr++;
        else menunr = 0;
      }
      else if (key == KEY_L ) { // Cursor left
        if (menunr > 4) menunr -= 4;
        else menunr = 0;
      }
      else if (key == KEY_U) { // Cursor up
        if (menunr > 0) menunr--;
        else menunr = numberofmenuentries - 1;
      }
    }
    printmenu();
  }

  else { // *** PLAY ***
    if (millis() - showmsgtimestamp > MSGTIME) isshowmsg = false; // Message timer

    if (!digitalRead(PIN_UP_BUTTON) || !digitalRead(PIN_DOWN_BUTTON)) { // Go forward or back
      int sign = 1; // Forward
      if (!digitalRead(PIN_DOWN_BUTTON)) sign = -1; // Back
      double tmpx = man.x + sign * STEPLENGTH * cos(man.angle * PI / 180);
      double tmpy = man.y - sign * STEPLENGTH * sin(man.angle * PI / 180);
      if (checkstep(tmpx, tmpy)) {
        man.x = tmpx;
        man.y = tmpy;
      }
    }
    if (!digitalRead(PIN_LEFT_BUTTON)) { // Turn left
      man.angle += anglestep;
      if (man.angle >= 360) man.angle = 0;
    }
    if (!digitalRead(PIN_RIGHT_BUTTON)) { // Turn right
      man.angle -= anglestep;
      if (man.angle < 0) man.angle += 360;
    }
    if (!digitalRead(PIN_B_BUTTON) && man.ammo > 0) { // Fire
      sound(5000, 2); // Fire sound
      if (!ismap) {
        screenpixel(31, 30);  // Gunfire
        screenpixel(30, 29); screenpixelclr(31, 29); screenpixel(32, 29);
        screenpixelclr(30, 30); screenpixel(31, 30); screenpixelclr(32, 30);
        screenpixel(30, 31); screenpixelclr(31, 31); screenpixel(32, 31);
        display();
      }
      for (byte i = 0; i < OBJECTS; i++) { // Shoot enemy +++
        if (enemy[i].type > TYPMEDI && enemy[i].isalive) { // Not for ammo or medi
          int deltaangle, d;
          d = distance(enemy[i].x, enemy[i].y);
          if (d < VIEWDIST && isfreeview(enemy[i].x, enemy[i].y)) { // Only if close and free view
            int a = atan((double)((int)enemy[i].y - man.y) / ((int)enemy[i].x - man.x)) * 180 / PI;
            if (man.x <= enemy[i].x) { // East enemy side
              a = alignangle(-a);
              deltaangle = alignangle(360 + man.angle - a);
              if (deltaangle > (int)360 - VIEWANGLE) deltaangle = _abs(360 - deltaangle);
            }
            else { // West enemy side
              a = 180 - a;
              deltaangle = alignangle(360 + man.angle - a);
              if (deltaangle > (int)360 - VIEWANGLE) deltaangle = _abs(360 - deltaangle);
            }
            if (deltaangle < VIEWANGLE) {
              int gunpower = (VIEWDIST - d) * (1 - (double)deltaangle / VIEWANGLE);
              if (enemy[i].health > gunpower) {
                enemy[i].health -= gunpower;
                sound(1000, 5); // Enemy hurt
                ishit = true;
                ledblink(COLOR_GREEN);
              }
              else { // Enemy dead
                enemy[i].health = 0;
                enemy[i].isalive = false;
                printmsg(MSGKILLED);
                enemieskilled++;
                boolean oneisalive = false;
                for (byte i = ammonr + medinr; i < ammonr + medinr + enemynr; i++) {
                  if (enemy[i].isalive) oneisalive = true;
                }
                if (!oneisalive) { // Won
                  iswon = true;
                  sound(NOTE_c, 200); sound(NOTE_e, 200); sound(NOTE_g, 200); sound(NOTE_C, 255);
                }
              }
            }
          }
        }
      }
      man.ammo--;
      if (!man.ammo) printmsg(MSGNOAMMO);
    }

    if (millis() - fireenemytimestamp > FIREENEMYTIME) { // Enemy fire
      for (byte i = 0; i < OBJECTS; i++) {
        if (enemy[i].type == TYPAMMO && enemy[i].isalive && // AMMO? alive?
            distance(enemy[i].x, enemy[i].y) < DIST4) {     // Close?
          if (man.ammo < MAXBYTE - enemy[i].drainhealth) man.ammo += enemy[i].drainhealth;
          else man.ammo = MAXBYTE;
          enemy[i].isalive = false;
          sound(NOTE_C, 60);
        }
        if (enemy[i].type == TYPMEDI && enemy[i].isalive && // MEDI? alive?
            distance(enemy[i].x, enemy[i].y) < DIST4) {     // Close?
          if (man.health < MAXBYTE - enemy[i].drainhealth) man.health += enemy[i].drainhealth;
          else man.health = MAXBYTE;
          enemy[i].isalive = false;
          sound(NOTE_C, 60);
        }
        else if (enemy[i].isalive && enemy[i].isactive &&                // Alive? active?
                 !(fireclock % enemy[i].fireperiod) &&                   // In clock?
                 isfreeview(enemy[i].x, enemy[i].y) &&                   // Visible?
                 distance(enemy[i].x, enemy[i].y) <= enemy[i].gundist) { // Close enough?
          ishurt = true;
          if (man.health >= enemy[i].drainhealth) {
            man.health = man.health - enemy[i].drainhealth;
            sound(100, 20); // Man hurt
            ledblink(COLOR_RED);
          }
          else { // Game over
            man.health = 0;
            killedbyicon = enemy[i].type + 'a';
            islost = true;
            sound(NOTE_C, 200); sound(NOTE_g, 200); sound(NOTE_e, 200); sound(NOTE_c, 255);
          }
        }
      }
      fireenemytimestamp = millis();
      if (fireclock < FIRECLOCKLCM) fireclock++;
      else fireclock = 1;
    }

    if (millis() - moveenemytimestamp > MOVEENEMYTIME) { // Move enemies
      for (byte i = 0; i < OBJECTS; i++) {
        if (enemy[i].isalive && enemy[i].isactive && !(moveclock % enemy[i].moveperiod)) {
          if (enemy[i].x > man.x && checkstep(enemy[i].x - 1, enemy[i].y)) enemy[i].x --;
          if (enemy[i].x < man.x && checkstep(enemy[i].x + 1, enemy[i].y)) enemy[i].x ++;
          if (enemy[i].y > man.y && checkstep(enemy[i].x , enemy[i].y - 1)) enemy[i].y --;
          if (enemy[i].y < man.y && checkstep(enemy[i].x , enemy[i].y + 1)) enemy[i].y ++;
        }
      }
      moveenemytimestamp = millis();
      if (moveclock < MOVECLOCKLCM) moveclock++;
      else moveclock = 1;
    }

    sbufclr(); // Clear screen buffer

    if (ismap) { // Draw 2d map
      for (int i = 0; i < ARENABYTES; i++) sbuf[i] = arena[i]; // Draw walls
      screenpixel(man.x, man.y); // Draw man
      for (byte i = 0; i < OBJECTS; i++) { // Draw enemies
        if (enemy[i].isalive) screenpixel(enemy[i].x, enemy[i].y);
      }
    }
    else { // Draw 3d view
      scan(); // Scan sector infront and draw objects to screen buffer
      if (issky) drawsky(); // Draw sky/ceiling and floor line
      if (isstatus) drawstatus(); // Draw status
      if (isshowmsg && ismsg) showmsg(); // Show message
      if (ishurt) { // Flash white screen
        sbuffill();
        ishurt = false;
      }
    } // End of ismap loop
  } // End of play loop

  display();
}
