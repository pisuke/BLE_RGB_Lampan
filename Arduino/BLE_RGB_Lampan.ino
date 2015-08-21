#include "LPD8806.h"
#include <SPI.h>
#ifdef __AVR_ATtiny85__
 #include <avr/power.h>
#endif
#include <boards.h>
#include <RBL_nRF8001.h>
#include <services.h>


/*
 * orange  5V
 * brown  GND
 * yellow  DI
 * green   CI
*/

int nLEDs = 16;
int dataPin  = 2;
int clockPin = 3;

LPD8806 lampan = LPD8806(nLEDs, dataPin, clockPin);

int R = 0;
int G = 0;
int B = 0;


void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
#endif

  lampan.begin();
  lampan.show(); // The RGB pixels are all off
  ble_begin();
}

void loop() {
  /*
  Off(0);
  delay(500);
  */
  //rainbow(50);
  //rainbowCycle(50);
  //theaterChase(lampan.Color(255,0,0),1000);
  /*
  colorWipe(lampan.Color(255,0,0),80);
  colorWipe(lampan.Color(255,0,255),80);
  colorWipe(lampan.Color(255,200,150),80);
  colorWipe(lampan.Color(255,0,255),80);
  colorWipe(lampan.Color(0,0,255),80);
  colorWipe(lampan.Color(0,0,0),80);
  */
  if(3 == ble_available())
  {
    R = ble_read();
    G = ble_read();
    B = ble_read();
    colorWipe( lampan.Color(R, G, B), 0 );
  }
  ble_do_events();
}

void Off(uint8_t wait) {
  int i;  
  for (i=0; i < lampan.numPixels(); i++) {
      lampan.setPixelColor(i, lampan.Color(0,   0,   0));
      lampan.show();
      delay(wait);
  }
}

void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  
    for (int q=0; q < 3; q++) {
      for (int i=0; i < lampan.numPixels(); i=i+3) {
        lampan.setPixelColor(i+q, c);    
      }
      lampan.show();
     
      delay(wait);
     
      for (int i=0; i < lampan.numPixels(); i=i+3) {
        lampan.setPixelColor(i+q, 0);    
      }
    }
  }
}

void colorWipe(uint32_t c, uint8_t wait) {
  int i;

  for (i=0; i < lampan.numPixels(); i++) {
      lampan.setPixelColor(i, c);
      lampan.show();
      delay(wait);
  }
}


void rainbowCycle(uint8_t wait) {
  uint16_t i, j;
  
  for (j=0; j < 384 * 5; j++) {    
    for (i=0; i < lampan.numPixels(); i++) {
      lampan.setPixelColor(i, Wheel( ((i * 384 / lampan.numPixels()) + j) % 384) );
    }  
    lampan.show();   
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  int i, j;
   
  for (j=0; j < 384; j++) {     // 3 cycles of all 384 colors in the wheel
    for (i=0; i < lampan.numPixels(); i++) {
      lampan.setPixelColor(i, Wheel( (i + j) % 384));
    }  
    lampan.show();   // write all the pixels out
    delay(wait);
  }
}

uint32_t Wheel(uint16_t WheelPos)
{
  byte r, g, b;
  switch(WheelPos / 128)
  {
    case 0:
      r = 127 - WheelPos % 128;   //Red down
      g = WheelPos % 128;      // Green up
      b = 0;                  //blue off
      break; 
    case 1:
      g = 127 - WheelPos % 128;  //green down
      b = WheelPos % 128;      //blue up
      r = 0;                  //red off
      break; 
    case 2:
      b = 127 - WheelPos % 128;  //blue down 
      r = WheelPos % 128;      //red up
      g = 0;                  //green off
      break; 
  }
  return(lampan.Color(r,g,b));
}

// Fill strip with a color
void colorSet(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<lampan.numPixels(); i++) {
      lampan.setPixelColor(i, c);
  }
   lampan.show();
   delay(wait);
}


