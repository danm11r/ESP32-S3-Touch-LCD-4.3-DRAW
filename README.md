# ESP32-S3-Touch-LCD-4.3-DRAW
Simple drawing app demo for the Waveshare ESP432-S3-lcd-4.3 development board. Features 5 different colors to choose from, and an erasable canvas.

## About
I wrote this basic app to gain some familiarity with Waveshare's ESP432 dev board. Graphics are generated with the Arduino_GFX library, and touch is handled by the bb_captouch library. 

The panel constructor for Arduino_GFX was provided by Westcott1, check it out here: https://github.com/Westcott1/Waveshare-ESP32-S3-Touch-LCD-4.3-and-Arduino.

Behind the scenes, the touch screen is sampled every 10ms and a small circle is drawn at the coordinates of each touch point. Due to the limited sampling rate, Bresenham's line algorithm is used to draw a line of circles between these touch points to give the appearance of drawing a continuous line.

![LCD_Draw](https://github.com/danm11r/ESP32-S3-Touch-LCD-4.3-DRAW/assets/148667664/375d08ae-1dec-49ca-afe9-23f520990468)

## Installation
Waveshare provides a walkthrough for configuring the Arduino IDE to write programs to the ESP432 dev board: https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-4.3. I recommend using the provided ESP_IOExpander library linked in the wiki, I ran into compatibility issues using a different version.
