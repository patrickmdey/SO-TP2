#ifndef VIDEO_DRIVER_H
#define VIDEO_DRIVER_H

#include <colours.h>
#include <screens.h>
#include <stdint.h>

void writePixel(uint32_t x, uint32_t y, t_colour colour);
void printCharOnScreen(char c, t_colour bgColour, t_colour fontColour, int advance);
void removeCharFromScreen();
void changeLineOnScreen();
void clearScreen();
void scrollDownScreen();
void clearLineOnScreen();
void initVideoDriver(t_colour BGColour, t_colour FontColour);
void blinkCursor();
void stopBlink();
void draw(char *bitmap, t_colour colour, int multiplier);
void moveCursor(int x, int y);
void moveCursorTo(int x, int y);
void cursorPosition(int *array);
void clearScreenFromTo(int fromWidth, int fromHeight, int toWidth, int toHeight);
#endif
