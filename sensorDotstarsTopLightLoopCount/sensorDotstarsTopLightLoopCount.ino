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


const int numlights = 60;
const int light_low = 1;
const int light_high = numlights - 1;
const int top_light = 0;
int light_val = 0;
int light_direction = 1;
uint8_t change = 1;



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

int onInterval = 40;
int offInterval = 0;
int timeInCycle;


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
  strip.setPixelColor(top_light, 0x000000); //the top indicator light defualts to OFF
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
      strip.setPixelColor(top_light, 0xffffff);
      
    }
  }

/*Update lights and add delays for flashing*/
  
 
  timeInCycle = millis()%(onInterval+offInterval);
  if(timeInCycle >= onInterval){
    //turn lights off
    for (int i = 1; i <= light_high; i++) {
      strip.setPixelColor(i, 0x000000);
    }
  }else{
    //turn lights on
    update_light_height();
    
  }
  strip.show(); //show current light height 
}    
  




void update_light_height()
{
  //update to new value
  int redIntensity;
  //turn all off
  for (int i = 1; i <= light_high; i++) {
    strip.setPixelColor(i, 0x000000);
  }

  for (int i = light_high; i > light_high - light_val; i--)
  {
    //update color
    redIntensity = map(i, light_high, light_low, 0, 0xff);
    constrain(redIntensity, 0, 0xff);
    strip.setPixelColor(i, 0x00, redIntensity, 0xff);
  }
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

void convertColor()
{
  byte redIntensity;
  byte greenIntensity;
  byte blueIntensity;

  if (curr_color <= 255)          // zone 1
  {
    redIntensity = 255 - curr_color;    // red goes from on to offcurr_colorcurr_color
    greenIntensity = curr_color;        // green goes from off to on
    blueIntensity = 0;             // blue is always off
  }
  else if (curr_color <= 511)     // zone 2
  {
    redIntensity = 0;                     // red is always off
    greenIntensity = 255 - (curr_color - 256); // green on to off
    blueIntensity = (curr_color - 256);        // blue off to on
  }
  else // color >= 512       // zone 3
  {
    redIntensity = (curr_color - 512);         // red off to on
    greenIntensity = 0;                   // green is always off
    blueIntensity = 255 - (curr_color - 512);  // blue on to off
  }

  //convert to hex in RGB format
  curr_color = redIntensity;
  curr_color <<= 8;
  curr_color |= greenIntensity;
  curr_color <<= 8;
  curr_color |= blueIntensity;
}
