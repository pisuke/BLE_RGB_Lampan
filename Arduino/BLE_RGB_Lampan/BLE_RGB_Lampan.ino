/*
 * Arup BLE RGB and Circadian IKEA Lampan modification using the Adafruit LPD8806 LED string and RFduino
 * 2015-08-23 Francesco Anselmo - francesco.anselmo@arup.com
 */

#include "LPD8806.h"
#include <SPI.h>
#include <RFduinoBLE.h>

#ifdef __AVR_ATtiny85__
 #include <avr/power.h>
#endif


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

  // Enable serial debug.
  Serial.begin(9600);
  Serial.println("Arup BLE RGB Lampan started");
  Serial.println("Serial rate set to 9600 baud");

  // Check RFduino CPU temperature, and print to log
  float CPU_temperature = RFduino_temperature(CELSIUS);
  Serial.print("RFduino_temperature is: ");
  Serial.print(CPU_temperature);
  Serial.println(" deg C");

  lampan.begin();
  lampan.show(); // The RGB pixels are all off

  // this is the data we want to appear in the advertisement
  // (the deviceName length plus the advertisement length must be <= 18 bytes
  // RFduinoBLE.advertisementData = "ledbtn";
  RFduinoBLE.advertisementInterval = 500;
  Serial.println("RFduino BLE Advertising interval 500ms");
  RFduinoBLE.deviceName = "ArupLamp";
  Serial.println("RFduino BLE DeviceName: ArupLamp");
  RFduinoBLE.txPowerLevel = -20;
  Serial.println("RFduino BLE Tx Power Level: -20dBm");

  // start the BLE stack
  RFduinoBLE.begin();
  Serial.println("RFduino BLE stack started");

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
  /*
  if(3 == ble_available())
  {
    R = ble_read();
    G = ble_read();
    B = ble_read();
    colorWipe( lampan.Color(R, G, B), 0 );
  }
  ble_do_events();
  */

  // switch to lower power mode
  RFduino_ULPDelay(INFINITE);
  
}


void RFduinoBLE_onAdvertisement()
{
  Serial.println("RFduino is doing BLE advertising ...");
  colorWipe( lampan.Color(0, 0, 0), 0 );
}

void RFduinoBLE_onConnect()
{
  Serial.println("RFduino BLE connection successful");
  colorWipe( lampan.Color(255, 0, 0), 50 );
  colorWipe( lampan.Color(0, 255, 0), 50 );
  colorWipe( lampan.Color(0, 0, 255), 50 );
  colorWipe( lampan.Color(0, 0, 0), 50 );
}

void RFduinoBLE_onDisconnect()
{
  Serial.println("RFduino BLE disconnected");
  colorWipe( lampan.Color(255, 0, 0), 10 );
  colorWipe( lampan.Color(0, 0, 0), 10 );
  colorWipe( lampan.Color(255, 0, 0), 10 );
  colorWipe( lampan.Color(0, 0, 0), 10 );
  colorWipe( lampan.Color(255, 0, 0), 10 );
  colorWipe( lampan.Color(0, 0, 0), 10 );
  // don't leave the leds on after disconnection
  colorWipe( lampan.Color(0, 0, 0), 10 );
}

void RFduinoBLE_onReceive(char *data, int len)
{
  // if the first byte is 0x01 / on / true
  Serial.println("Received data over BLE");
  Serial.println(len);
  uint8_t r;
  uint8_t g;
  uint8_t b;
  if (len==3) {
    uint8_t r = data[0];
    uint8_t g = data[1];
    uint8_t b = data[2];
    Serial.println(r);
    Serial.println(g);
    Serial.println(b);
    colorWipe( lampan.Color(r, g, b), 50 );
    Serial.println("Turn light on");
  } else if (len==1) {
    uint8_t myByte = data[0]; // store first char in array to myByte 
    Serial.println(myByte); // print myByte via serial
    if (myByte==0) {
      colorWipe( lampan.Color(0, 0, 0), 50 );
      Serial.println("Turn light off");  
    }
  }

  /*
  if (data[0])
  {
    colorWipe( lampan.Color(255, 255, 255), 50 );
    Serial.println("Turn light on");
  }
  else
  {
    colorWipe( lampan.Color(0, 0, 0), 50 );
    Serial.println("Turn light off");
  }
  */
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


