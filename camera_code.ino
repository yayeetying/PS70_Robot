#include "OV7670.h"

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library

#include <SPI.h>
#include <TFT_eSPI.h>
#include "BMP.h"


const int SIOD = 21; //SDA
const int SIOC = 22; //SCL

const int VSYNC = 13;
const int HREF = 4;

const int XCLK = 27;
const int PCLK = 14;

const int D0 = 36;
const int D1 = 39;
const int D2 = 34;
const int D3 = 35;
const int D4 = 32;
const int D5 = 33;
const int D6 = 25;
const int D7 = 26;


//DIN <- MOSI 23
//CLK <- SCK 18

TFT_eSPI tft = TFT_eSPI();
int scale = 3;

OV7670 *camera;

uint32_t convertRGB565toRGB(uint16_t color565) 
{ 
    uint8_t red = (color565 >> 11) & 0x1F; // Extract the 5-bit red 
    uint8_t green = (color565 >> 5) & 0x3F; // Extract the 6-bit green 
    uint8_t blue = color565 & 0x1F; // Extract the 5-bit blue 
    // Convert the 5-bit color values to 8-bit by shifting and scaling 
    red = red << 3; // Red: shift left by 3 (multiply by 8) 
    green = green << 2; // Green: shift left by 2 (multiply by 4) 
    blue = blue << 3; // Blue: shift left by 3 (multiply by 8) 

    // Construct the 32-bit RGB color value (without alpha) 
    return (red << 16) | (green << 8) | blue; // No alpha, just RGB 
}

void drawRGBBitmapScaled3x(int16_t x, int16_t y, uint8_t *buffer, int16_t w, int16_t h) {
  uint16_t *pixels = (uint16_t *)buffer; // Treat buffer as an array of 16-bit colors
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      uint16_t color = pixels[j * w + i]; // Get the original pixel color
      // Draw a 3x3 block for each pixel
      tft.fillRect(x + i * scale, y + j * scale, scale, scale, color);
    }
  }
}
unsigned char bmpHeader[BMP::headerSize];

void setup() 
{
  Serial.begin(115200);

  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(0);
  
  camera = new OV7670(OV7670::Mode::QQVGA_RGB565, SIOD, SIOC, VSYNC, HREF, XCLK, PCLK, D0, D1, D2, D3, D4, D5, D6, D7);
  BMP::construct16BitHeader(bmpHeader, camera->xres, camera->yres);

  
  // tft.initR(INITR_BLACKTAB);
  // tft.fillScreen(0);
}



void loop() {
  camera->oneFrame();// Capture a frame from the camera
  tft.startWrite(); // Begin batch updates
  drawRGBBitmapScaled3x(0, 0, camera->frame, camera->xres, camera->yres); // Render 3x scaled image
  tft.endWrite();// End batch updates
}
