/* 
LCD_Draw 
6/15/2024
danm11r
*/

#include <bb_captouch.h>
#include <Arduino_GFX_Library.h>
#include <ESP_IOExpander_Library.h>

/* -----------------------------------------------
Panel constructor, helpfully provided by Westcott1
-------------------------------------------------- */
#define GFX_DEV_DEVICE WAVESHARE_ESP32_S3_TFT_4_3
//define GFX_BL 2

Arduino_ESP32RGBPanel* rgbpanel = new Arduino_ESP32RGBPanel(
  5 /* DE */,
  3 /* VSYNC */,
  46 /* HSYNC */,
  7 /* PCLK */,

  1 /* R0 */,
  2 /* R1 */,
  42 /* R2 */,
  41 /* R3 */,
  40 /* R4 */,

  39 /* G0 */,
  0 /* G1 */,
  45 /* G2 */,
  48 /* G3 */,
  47 /* G4 */,
  21 /* G5 */,

  14 /* B0 */,
  38 /* B1 */,
  18 /* B2 */,
  17 /* B3 */,
  10 /* B4 */,

  0 /* hsync_polarity */, 40 /* hsync_front_porch */, 48 /* hsync_pulse_width */, 88 /* hsync_back_porch */,
  0 /* vsync_polarity */, 13 /* vsync_front_porch */, 3 /* vsync_pulse_width */, 32 /* vsync_back_porch */,
  1 /* pclk_active_neg */, 16000000 /* prefer_speed */
);

Arduino_RGB_Display* gfx = new Arduino_RGB_Display(
  800 /* width */,
  480 /* height */,
  rgbpanel,
  0 /* rotation */,
  true /* auto_flush */
);

// Touchscreen config
BBCapTouch bbct;
#define TOUCH_SDA 8
#define TOUCH_SCL 9
#define TOUCH_INT 4
#define TOUCH_RST 0

int selectedColor = RED;
int avaliableColors[5] = {RED, GREEN, BLUE, YELLOW, PURPLE};
int penSize = 5;
bool holding = false;

// Vars for line generation
float dist = 0;
int prevX = 0; 
int prevY = 0;
int xPos = 0;
int yPos = 0;
int numCircles = 0;

void GFXinit() {
  Serial.print("Initializing GFX...");
  if (!gfx->begin())
  {
    Serial.println("gfx->begin() failed!");
  }
}

void touchInit() {
    bbct.init(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_INT);
}

void touchRead() {
  TOUCHINFO ti;

  if (bbct.getSamples(&ti)) {  // if touch event happened
    
    // If touch is within clear button, clear the canvas
    if ((ti.x[0] <= 120)) {

      for (int i = 0; i < 6; i++) {

        if ((ti.y[0] >= i*80) && (ti.y[0] <= (i+1)*80)) {
          
          if (i == 0)
          {
            Serial.printf("Clearing display...");
            clearCanvas();
          }
          else {
            selectedColor = avaliableColors[i-1];
            drawColorButtons();
          }
        }
      }
    }
    else {
      Serial.printf("Touch x: %d y: %d size: %d\n", ti.x[0], ti.y[0], ti.area[0]);
      
      // Future functionality to adjust pen size depending on touch area. Not yet implimented...
      //penSize = map(ti.area[0], 0, 100, 0, 100);
      
      // From the touch coordinates, draw a line. Ideally, we would use the drawLine() function, however the thickness of the line
      // cannot be changed, and 1px wide is too small on a high DPI display...

      if (holding == false) {
        holding = true;
        prevX = ti.x[0];
        prevY = ti.y[0];
        gfx->fillCircle(ti.x[0], ti.y[0], penSize, selectedColor);
        Serial.printf("\nHold beggining...\n");
      } 
      
      drawLine(prevX, prevY, ti.x[0], ti.y[0]);

      prevX = ti.x[0];
      prevY = ti.y[0];
    }
  }

  else {
    holding = false;
  }
}

// Impliment Bresenham's line algorithm to draw line between two touch sample points
void drawLine(int x1, int y1, int x2, int y2) {
  
  if (abs(y2 - y1) < abs(x2 - x1)) {
    if (x1 > x2) {
      drawLineLow(x2, y2, x1, y1);
    }
    else {
      drawLineLow(x1, y1, x2, y2);
    }
  }
  else {
    if (y1 > y2) {
      drawLineHigh(x2, y2, x1, y1);
    }
    else {
      drawLineHigh(x1, y1, x2, y2);
    }
  }
}

void drawLineLow(int x1, int y1, int x2, int y2) {

  int dx, dy, y, D = 0;
  int yi = 1;

  dx = x2 - x1;
  dy = y2 - y1;

  if (dy < 0) {
    yi = -1;
    dy = -dy;
  }

  D = (2*dy) - dx;
  y = y1;

  for (int x = x1; x <= x2; x++) {
    gfx->fillCircle(x, y, penSize, selectedColor);
    if (D > 0) {
      y += yi;
      D += 2*(dy - dx);
    }
    else {
      D += 2*dy;
    }
  }
}

void drawLineHigh(int x1, int y1, int x2, int y2) {

  int dx, dy, x, D = 0;
  int xi = 1;

  dx = x2 - x1;
  dy = y2 - y1;

  if (dx < 0) {
    xi = -1;
    dx = -dx;
  }

  D = (2*dx) - dy;
  x = x1;

  for (int y = y1; y <= y2; y++) {
    gfx->fillCircle(x, y, penSize, selectedColor);
    if (D > 0) {
      x += xi;
      D += 2*(dx - dy);
    }
    else {
      D += 2*dx;
    }
  }
}

void clearCanvas() {
  gfx->flush();

  // Draw the clear button
  gfx->fillScreen(WHITE);
  gfx->fillRect(0, 0, 120, 80, BLACK);
  gfx->setTextColor(WHITE);
  gfx->setTextSize(4);
  gfx->setCursor(0, 0);
  gfx->println("CLEAR");

  drawColorButtons();
}

void drawColorButtons() {
  gfx->fillRect(100, 80, 20, 400, WHITE);

  // Draw the buttons for the 5 avaliable colors
  for (int i = 0; i < 5; i++) {
    if (selectedColor == avaliableColors[i]) {
      gfx->fillRect(0, (i+1)*80, 120, 80, avaliableColors[i]);
    }
    else {
      gfx->fillRect(0, (i+1)*80, 100, 80, avaliableColors[i]); 
    }
  }
}

void setup() {
  // Begin serial communication for debug
  Serial.begin(115200);
  
  GFXinit();

  // Print welcome message
  gfx->fillScreen(BLACK);
  gfx->setTextColor(WHITE);
  gfx->setTextSize(4);
  gfx->setCursor(0, 0);
  gfx->println("Welcome to the LCD Draw Demo!");

  gfx->setCursor(0, 50);
  gfx->println(" -danm11r");

  delay(3000);
  
  clearCanvas();
  touchInit();
}

void loop() {
  touchRead();
  delay(10);
}
