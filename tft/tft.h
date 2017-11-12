//-----------------------------------------------------------------------------//  TFT: Experiments interfacing ATmega328 to an ST7735 1.8" LCD TFT display
//
// Author : Bruce E. Hall <bhall66@gmail.com>
//  Website : w8bh.net
// Version : 1.0
// Date : 04 May 2014
// Target : ATmega16A microcontroller
// Language : C, using AVR studio 6
// Size  : 3622 bytes
//
//  Fuse settings: 8 MHz osc with 65 ms Delay, SPI enable; *NO* clock/8
//
// Connections from LCD to Board (ATmega16A):
//
// TFT pin GND: GND
// TFT pin BL: +5V
// TFT pin RESET: PB6
// TFT pin D/C-LD: PB4 
// TFT pin CS-SD: n/c
// TFT pin CS-LD: GND
// TFT pin MOSI: PB5(MOSI)
// TFT pin SCK: PB7(SCK)
// TFT pin MISO: n/c
// TFT pin +5V: +5V
//
//  ---------------------------------------------------------------------------//  GLOBAL DEFINES
#pragma once
#define ClearBit(x,y) x &= ~_BV(y)  // equivalent to cbi(x,y)
#define SetBit(x,y) x |= _BV(y)  // equivalent to sbi(x,y)
//  ---------------------------------------------------------------------------//  ST7735 ROUTINES
#define SWRESET 0x01  // software reset
#define SLPOUT  0x11  // sleep out
#define DISPOFF 0x28  // display off
#define DISPON  0x29  // display on
#define CASET  0x2A  // column address set
#define RASET  0x2B  // row address set
#define RAMWR  0x2C  // RAM write 
#define MADCTL  0x36    // axis control
#define COLMOD  0x3A  // color mode
// ------------------------------------------------------------------------//    1.8" TFT display constants
#define XSIZE  128
#define YSIZE  160
#define XMAX  XSIZE-1
#define YMAX  YSIZE-1
//  -----------------------------------------------------------------------//   Color constants
#define BLACK  0x0000
#define BLUE  0x001F
#define RED  0xF800
#define GREEN  0x0400
#define LIME  0x07E0
#define CYAN  0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE  0xFFFF
//  ---------------------------------------------------------------------------//  INCLUDES
#include <avr/io.h>  // deal with port registers
#include <avr/interrupt.h>  // deal with interrupt calls
#include <avr/pgmspace.h>  // put character data into progmem
#include <util/delay.h>  // used for _delay_ms function
#include <string.h>  // string manipulation routines
#include <avr/sleep.h>  // used for sleep functions
#include <stdlib.h>
//  ---------------------------------------------------------------------------//  TYPEDEFS
typedef uint8_t byte;  // I just like byte & sbyte better
typedef int8_t sbyte;
//  ---------------------------------------------------------------------------//  MISC ROUTINES
void SetupPorts(); //init ports
void msDelay(int delay); // to remove code inlining
unsigned long intsqrt(unsigned long val);// calculate integer value of square root
//  ---------------------------------------------------------------------------//  SPI ROUTINES
//
// b7 b6 b5 b4 b3 b2 b1 b0 
// SPCR: SPIE SPE DORD MSTR CPOL CPHA SPR1 SPR0
// 0 1 0 1 . 0 0 0 1
// 
// SPIE - enable SPI interrupt
// SPE - enable SPI
// DORD - 0=MSB first, 1=LSB first
// MSTR - 0=slave, 1=master
// CPOL - 0=clock starts low, 1=clock starts high
// CPHA - 0=read on rising-edge, 1=read on falling-edge
// SPRx - 00=osc/4, 01=osc/16, 10=osc/64, 11=osc/128
// SPCR = 0x50: SPI enabled as Master, mode 0, at 16/4 = 4 MHz
void OpenSPI(); //SPI enabled as Master, Mode0 at 4 MHz //start of spi transfer
void CloseSPI(); //Clear SPI enable bit //finish of spi transfer
void WriteCmd (byte cmd); //write command to tft
void WriteByte (byte b); //write 8 bit data to tft
void WriteWord (int w); //write 16 bit data to tft
void Write888 (long data, int count); //write 24 bit to tft
void Write565 (int data, unsigned int count);// send 16-bit pixel data to the controller // note: inlined spi xfer for optimization
void HardwareReset(); //reset tft
void InitDisplay(); //init tft
void SetAddrWindow(byte x0, byte y0, byte x1, byte y1); //rectangular area
void ClearScreen(); //clear tft
//  ---------------------------------------------------------------------------//  SIMPLE GRAPHICS ROUTINES
//
// note: many routines have byte parameters, to save space,
// but these can easily be changed to int params for larger displays.
void DrawPixel (byte x, byte y, int color); //draw the pixel
void HLine (byte x0, byte x1, byte y, int color); // draws a horizontal line in given color
void VLine (byte x, byte y0, byte y1, int color);// draws a vertical line in given color
void Line (int x0, int y0, int x1, int y1, int color); // an elegant implementation of the Bresenham algorithm 
void DrawRect (byte x0, byte y0, byte x1, byte y1, int color); // draws a rectangle in given color
void FillRect (byte x0, byte y0, byte x1, byte y1, int color); //filled rectangular
void CircleQuadrant (byte xPos, byte yPos, byte radius, byte quad, int color);// draws circle quadrant(s) centered at x,y with given radius & color
// quad is a bit-encoded representation of which cartesian quadrant(s) to draw.
// Remember that the y axis on our display is 'upside down':
// bit 0: draw quadrant I (lower right)
// bit 1: draw quadrant IV (upper right)
// bit 2: draw quadrant II (lower left)
// bit 3: draw quadrant III (upper left)
void Circle (byte xPos, byte yPos, byte radius, int color); // draws circle at x,y with given radius & color
void RoundRect (byte x0, byte y0, byte x1, byte y1, byte r, int color); // draws a rounded rectangle with corner radius r. // coordinates: top left = x0,y0; bottom right = x1,y1 
void FillCircle (byte xPos, byte yPos, byte radius, int color); // draws filled circle at x,y with given radius & color
void Ellipse (int x0, int y0, int width, int height, int color); // draws an ellipse of given width & height
// two-part Bresenham method
// note: slight discontinuity between parts on some (narrow) ellipses.
void FillEllipse(int xPos,int yPos,int width,int height, int color); // draws a filled ellipse of given width & height
//  ---------------------------------------------------------------------------//  TEXT ROUTINES
// 
// Each ASCII character is 5x7, with one pixel space between characters
// So, character width = 6 pixels & character height = 8 pixels.
//
// In portrait mode: 
// Display width = 128 pixels, so there are 21 chars/row (21x6 = 126).
// Display height = 160 pixels, so there are 20 rows (20x8 = 160).
// Total number of characters in portait mode = 21 x 20 = 420.
//
// In landscape mode:
// Display width is 160, so there are 26 chars/row (26x6 = 156).
// Display height is 128, so there are 16 rows (16x8 = 128).
// Total number of characters in landscape mode = 26x16 = 416. 
void GotoXY (byte x,byte y); // position cursor on character x,y grid, where 0<x<20, 0<y<19.
void GotoLine(byte y); // position character cursor to start of line y, where 0<y<19.
void AdvanceCursor(); // moves character cursor to next position, assuming portrait orientation
void SetOrientation(int degrees); // Set the display orientation to 0,90,180,or 270 degrees
void PutCh (char ch, byte x, byte y, int color); // write ch to display X,Y coordinates using ASCII 5x7 font
void WriteChar(char ch, int color); // writes character to display at current cursor position.
void WriteString(char *text, int color); // writes string to display at current cursor position.
void WriteInt(int i); // writes integer i at current cursor position
void WriteHex(int i); // writes hexadecimal value of integer i at current cursor position
//  ---------------------------------------------------------------------------//  TEST ROUTINES
void PixelTest(); // draws 4000 pixels on the screen
void LineTest(); // sweeps Line routine through all four quadrants.
void CircleTest(); // draw series of concentric circles
void PortraitChars();// Writes 420 characters (5x7) to screen in portrait mode
//  ---------------------------------------------------------------------------//  MAIN PROGRAM

void InitTFT();