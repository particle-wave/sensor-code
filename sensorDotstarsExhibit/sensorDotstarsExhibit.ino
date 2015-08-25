/* TIPS: Make sure that the echem board and Arduino share a common ground.
 *  if sensor values are reading at 4.57 instead of what is expected, reconnect Arduino to Joe's magic laptop
*/

//Light libraries
#include <Adafruit_DotStar.h>
#include "SPI.h" // Comment out this line if using Trinket or Gemma
//#include "Adafruit_WS2801.h"
#ifdef __AVR_ATtiny85__
#include <avr/power.h>
#endif

//time library - At the moment this doesn't involve synching with an outside time, so it is just an internal clock
//#include <Time.h>

//math library
#include <math.h>

//Light initializing
#define NUMPIXELS 60 // Number of LEDs in strip
#define DATAPIN    4    // Green wire on DotStar
#define CLOCKPIN   5    // Yellow wire on DotStar
Adafruit_DotStar strip = Adafruit_DotStar(
                           NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);

int onInterval = 750;
int offInterval = 750;
int timeInCycle;


const int numlights = 60;
const int light_low = 1;
const int light_high = numlights - 1;
const int top_light = 0;
int light_val = 0;
int light_direction = 1;
uint8_t change = 1;

uint8_t brightness = 255;

//color variables
uint32_t curr_color;

//Software serial initializing

#define rxPin 10 //White cable from sensor
#define txPin 11 //Infrequently used

#include <SoftwareSerial.h>
SoftwareSerial mySerial(rxPin, txPin); // RX, TX
char delimiter = '\n';
String dataFlag = "PPM";
boolean startListening = false;




char inChar; // Data received from the serial port
String stringVal;
float floatVal;
const int COmin = 250; //multiply manually by sigMultiplier
const int COmax = 1400; //multiply manually by sigMultiplier
const int sigMultiplier = 1000;

void setup() {
  strip.begin();
  // Update LED contents, to start they are all 'off'
  strip.show();

  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  Serial.begin(9600);
  mySerial.begin(9600); // Start serial communication at 9600 bps

}
void loop() {
    
/*Read the serial port for values from the echem and map it to variables for controling the lights.*/
  //strip.setPixelColor(top_light, 0x000000); //the top indicator light defualts to OFF
  if (mySerial.available() > 0) {
    
    inChar = mySerial.read();
    if (inChar != ' ' && inChar != ',' && inChar != '\n') { //ignore commas and whitespace
      stringVal += inChar;
    }
    if (inChar == '\n') { //we've read the end of a value
      //Serial.print("Time:");
      //Serial.print(now()); //Print time in seconds since activation
      //Serial.print(" ");
      //Serial.print(stringVal);
      //Serial.print(" ");
      floatVal = stringVal.toFloat();
      //Serial.print(floatVal, 6);
      //Serial.print(" ");
      light_val = map(floatVal * sigMultiplier, COmin, COmax, light_low, light_high);
      light_val = constrain(light_val, light_low, light_high);

      Serial.println("Arduino read " + String(floatVal) + " Light val: " + String(light_val));
      stringVal = "";

      //set the top light to on since we've read a value
      
    }
  }

/*Update lights and add delays for flashing*/
  
 
  timeInCycle = millis()%(onInterval+offInterval);
  if(timeInCycle >= onInterval){
    //dim lights
    update_light_height();
    brightness = map(timeInCycle, onInterval, onInterval+offInterval, 255,50);
    strip.setBrightness(brightness);    
  }else{
    //turn lights on
    update_light_height();
    brightness = map(timeInCycle, 0, onInterval, 50, 255);
    strip.setBrightness(brightness);        
  }
  strip.show(); //show current light height 
}    
  




void update_light_height()
{
  //update to new value
  int redIntensity;
  //turn all off including top
  for (int i = 1; i <= light_high; i++) {
    strip.setPixelColor(i, 0x000000);
  }

  for (int i = light_high; i > light_high - light_val; i--)
  {
     colorMapRed(i);

  //this line for setting to just one color
  //strip.setPixelColor(i, 0x4B0082)
 }
  strip.setPixelColor(top_light, 0xffffff);
}

void colorMapRed(int pixelIndex){
  int redIntensity = map(pixelIndex, light_high, light_low, 0, 0xff);
  constrain(redIntensity, 0,0xff); 
  strip.setPixelColor(pixelIndex, 0x00, redIntensity, 0xff);    
}

void test() {
  if (light_val > light_high) {
    light_direction = -1;
  }
  else if (light_val < 0) {
    light_direction = 1;
  }
  light_val += change * light_direction;
  Serial.print(light_val);
  Serial.print(" ");
  Serial.println(light_direction);
  update_light_height();
}
