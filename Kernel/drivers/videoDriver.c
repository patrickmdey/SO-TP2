#include <font.h>
#include <lib.h>
#include <stringLib.h>
#include <videoDriver.h>

#define PIXEL_SIZE 3
struct vbe_mode_info_structure {
      uint16_t attributes;   // deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
      uint8_t window_a;      // deprecated
      uint8_t window_b;      // deprecated
      uint16_t granularity;  // deprecated; used while calculating bank numbers
      uint16_t window_size;
      uint16_t segment_a;
      uint16_t segment_b;
      uint32_t win_func_ptr;  // deprecated; used to switch banks from protected mode without returning to real mode
      uint16_t pitch;         // number of bytes per horizontal line
      uint16_t width;         // width in pixels
      uint16_t height;        // height in pixels
      uint8_t w_char;         // unused...
      uint8_t y_char;         // ...
      uint8_t planes;
      uint8_t bpp;    // bits per pixel in this mode
      uint8_t banks;  // deprecated; total number of banks in this mode
      uint8_t memory_model;
      uint8_t bank_size;  // deprecated; size of a bank, almost always 64 KB but may be 16 KB...
      uint8_t image_pages;
      uint8_t reserved0;

      uint8_t red_mask;
      uint8_t red_position;
      uint8_t green_mask;
      uint8_t green_position;
      uint8_t blue_mask;
      uint8_t blue_position;
      uint8_t reserved_mask;
      uint8_t reserved_position;
      uint8_t direct_color_attributes;

      uint32_t framebuffer;  // physical address of the linear frame buffer; write here to draw to the screen
      uint32_t off_screen_mem_off;
      uint16_t off_screen_mem_size;  // size of memory in the framebuffer but not being displayed on the screen
      uint8_t reserved1[206];
} __attribute__((packed));

static int getPixelDataByPosition(uint32_t x, uint32_t y);

static struct vbe_mode_info_structure* screen_info = (void*)0x5C00;
static uint32_t SCREEN_WIDTH = 1024;  //VESA default values
static uint32_t SCREEN_HEIGHT = 768;

t_screen aux = { 0 };

static t_screen* currentScreen = &aux;

void initVideoDriver(t_colour BGColour, t_colour FontColour) {

      currentScreen->defaultBGColour = BGColour;
      currentScreen->defaultFontColour = FontColour;
      currentScreen->blink = 0;
      currentScreen->currentX = 0;
      currentScreen->currentY = 0;
      currentScreen->offset = 0;
      currentScreen->height = SCREEN_HEIGHT;
      currentScreen->width = SCREEN_WIDTH;

}

void draw(char* bitmap, t_colour colour, int multiplier) {
      if (bitmap[0] == 0 || multiplier <= 0)
            return;
      int originalX = currentScreen->currentX;
      int originalY = currentScreen->currentY;
      char c = 0;
      int auxY = 0;
      char* aux = bitmap;
      do {
            c = *aux;
            switch (c) {
            case '_':
                  if ((currentScreen->currentX) + multiplier <= currentScreen->width)
                        currentScreen->currentX += multiplier;
                  else
                        c = 0;
                  break;
            case 'X':
                  for (int i = 0; i < multiplier && c != 0;i++) {
                        if (currentScreen->currentX <= currentScreen->width && currentScreen->currentY <= currentScreen->height) {

                              writePixel(currentScreen->currentX, currentScreen->currentY, colour);
                              auxY = currentScreen->currentY;
                              for (int j = 0; j <= multiplier && c != 0;j++) {
                                    if (currentScreen->currentY <= currentScreen->height) {
                                          writePixel(currentScreen->currentX, currentScreen->currentY, colour);
                                          currentScreen->currentY++;
                                    }
                                    else
                                          c = 0;

                              }
                              currentScreen->currentY = auxY;
                              currentScreen->currentX++;
                        }
                        else
                              c = 0;
                  }
                  break;
            case '\n':
                  if (currentScreen->currentY <= currentScreen->height) {
                        currentScreen->currentX = originalX;
                        currentScreen->currentY += multiplier;
                  }
                  else
                        c = 0;
                  break;
            default:
                  c = 0;
                  break;
            }
            aux++;
      }       while (c != 0 && currentScreen->currentX < currentScreen->width && currentScreen->currentY < currentScreen->height);

      currentScreen->currentX = originalX;
      currentScreen->currentY = originalY;
}

void moveCursor(int x, int y) {
      if (currentScreen->currentX + x < 0 || currentScreen->currentX + x > currentScreen->width
            || currentScreen->currentY + y < 0 || currentScreen->currentY + y > currentScreen->height)
            return;
      currentScreen->currentX += x;
      currentScreen->currentY += y;
}

void moveCursorTo(int x, int y) {
      if (x < 0 || x > currentScreen->width || y < 0 || y > currentScreen->height)
            return;
      currentScreen->currentX = x;
      currentScreen->currentY = y;
}

void cursorPosition(int* array) {
      array[0] = currentScreen->currentX;
      array[1] = currentScreen->currentY;
}

void writePixel(uint32_t x, uint32_t y, t_colour colour)  //BGR
{
      char* currentFrame = (char*)((uint64_t)screen_info->framebuffer);  //casteo a uint64 para evitar warning
      int offset = getPixelDataByPosition(x, y);
      currentFrame[offset] = (char)((colour >> 16) & 0xFF);     //BLUE
      currentFrame[offset + 1] = (char)((colour >> 8) & 0xFF);  //GREEN
      currentFrame[offset + 2] = (char)(colour & 0xFF);         //RED
}

void printCharOnScreen(char c, t_colour bgColour, t_colour fontColour, int advance) {
      if (currentScreen->currentX != 0 && currentScreen->width - currentScreen->currentX < CHAR_WIDTH) {
            currentScreen->currentY += CHAR_HEIGHT;
            currentScreen->currentX = 0;
            if (currentScreen->height - currentScreen->currentY < CHAR_HEIGHT) {
                  currentScreen->currentY -= CHAR_HEIGHT;
                  scrollDownScreen();
            }
      }

      char* charMap = getCharMap(c);

      uint32_t x = currentScreen->currentX + currentScreen->offset, y = currentScreen->currentY;

      for (int i = 0; i < CHAR_HEIGHT; i++) {
            for (int j = 0; j < CHAR_WIDTH; j++) {
                  int8_t isFont = (charMap[i] >> (CHAR_WIDTH - j - 1)) & 0x01;  //-1 para no romper el decalaje, primera vez tengo q decalar 7
                  if (isFont) {
                        writePixel(x, y, fontColour);
                  }
                  else {
                        writePixel(x, y, bgColour);
                  }
                  x++;
            }
            x = currentScreen->currentX + currentScreen->offset;
            y++;
      }

      if (advance) {
            currentScreen->currentX += CHAR_WIDTH;
      }
}

void scrollDownScreen() {
      for (int i = 0; i < CHAR_HEIGHT; i++) {
            for (int y = 0; y < SCREEN_HEIGHT; y++) {
                  memcpy((void*)((uint64_t)screen_info->framebuffer + y * SCREEN_WIDTH * PIXEL_SIZE),
                        (void*)((uint64_t)screen_info->framebuffer + (y + 1) * SCREEN_WIDTH * PIXEL_SIZE),
                        SCREEN_WIDTH * PIXEL_SIZE);
            }
      }

      clearLineOnScreen();
}

void clearLineOnScreen() {
      for (int y = 0; y < currentScreen->height; y++) {
            for (int x = 0; x < currentScreen->width; x++) {
                  writePixel(x + currentScreen->offset, currentScreen->currentY + y, currentScreen->defaultBGColour);
            }
      }
}

void removeCharFromScreen() {
      if (currentScreen->currentX == 0) {
            if (currentScreen->currentY == 0) {
                  return;
            }
            currentScreen->currentY -= CHAR_HEIGHT;
            currentScreen->currentX = currentScreen->width;
      }

      currentScreen->currentX -= CHAR_WIDTH;

      printCharOnScreen(' ', BLACK, WHITE, 0);  //remove char
}

void changeLineOnScreen() {
      currentScreen->currentY += CHAR_HEIGHT;
      currentScreen->currentX = 0;
      if (currentScreen->height - currentScreen->currentY < CHAR_HEIGHT) {
            currentScreen->currentY -= CHAR_HEIGHT;
            scrollDownScreen();
      }
}

void clearScreen() {
      for (int y = 0; y < currentScreen->height; y++) {
            for (int x = 0; x < currentScreen->width; x++) {
                  writePixel(x + currentScreen->offset, y, currentScreen->defaultBGColour);
            }
      }
      currentScreen->currentX = 0;
      currentScreen->currentY = 0;
}

void clearScreenFromTo(int fromWidth, int fromHeight, int toWidth, int toHeight) {
      if (fromWidth < 0 || toWidth < fromWidth || fromHeight < 0 || toHeight < fromHeight || toWidth > SCREEN_WIDTH || toHeight>SCREEN_HEIGHT)
            return;
      for (int y = fromHeight; y < toHeight; y++) {
            for (int x = fromWidth; x < toWidth; x++) {
                  writePixel(x + currentScreen->offset, y, currentScreen->defaultBGColour);
            }
      }
      currentScreen->currentX = fromWidth;
      currentScreen->currentY = fromHeight;
}

void blinkCursor() {
      if (currentScreen->blink) {
            staticputchar('|');
            currentScreen->blink = 0;
      }
      else {
            staticputchar(' ');
            currentScreen->blink = 1;
      }
}

void stopBlink() {
      currentScreen->blink = 0;
      staticputchar(' ');
}

static int getPixelDataByPosition(uint32_t x, uint32_t y) {
      return PIXEL_SIZE * (x + y * SCREEN_WIDTH);
}