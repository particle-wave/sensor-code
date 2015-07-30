   
//Light libraries
#include <Adafruit_DotStar.h>
#include "SPI.h" // Comment out this line if using Trinket or Gemma
#ifdef __AVR_ATtiny85__
 #include <avr/power.h>
#endif

//math library
#include <math.h>

//light strip setup
#define NUMPIXELS 60 // Number of LEDs in strip
#define DATAPIN    4
#define CLOCKPIN   5
Adafruit_DotStar strip = Adafruit_DotStar(
  NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);

//variables for manipulating light height
#define LIGHTLOW 3
#define LIGHTHIGH 59 //manually change to be one less than NUMPIXELS
uint8_t light_val;
uint8_t old_light_val;

//color variables
uint32_t good_color = 0x00A1DD; //RGB 24 bit color 0x00BFFF. 701?
uint32_t bad_color = 0xDE0034; //0xFF4500 
uint32_t curr_color = good_color; 

//variables for receiving data
char inChar; // Data received from the serial port
String stringVal;
float floatVal;
const int COmin = 200; //multiply manually by sigMultiplier
const int COmax = 800; //multiply manually by sigMultiplier
const int sigMultiplier = 1;


void setup() {
   strip.begin();
  // Update LED contents, to start they are all 'off'
  strip.show();
   Serial.begin(9600); // Start serial communication at 9600 bps
   establishContact();
 }
 
 void loop() {
  
   if (Serial.available() > 0){ //if there are characters being sent, read them and append them to the stringVal buffer
     inChar = Serial.read(); 
     if (inChar != '\n'){
       stringVal+=inChar;
     }          
   }
   else {   //no characters are being sent, display the value in the buffer
    floatVal = stringVal.toFloat(); //convert stringVal to float
    stringVal = ""; //reset buffer
   
    //calculate new light value
    old_light_val = light_val; //save previous value
    light_val = map(floatVal * sigMultiplier, COmin, COmax, LIGHTLOW, LIGHTHIGH);
    light_val = constrain(light_val, LIGHTLOW, LIGHTHIGH);
    
    //update color
    curr_color = map(floatVal * sigMultiplier, COmin, COmax, good_color, bad_color);
    constrain(curr_color, good_color, bad_color);
    
    update_light_height(); 
    Serial.println("Arduino read " + String(floatVal) + " Light val: " + String(light_val));
    delay(30);
    turn_all_off();
    delay(30);
   }
}

//method to shake hands with processing
void establishContact(){
  while (Serial.available()<=0){
    Serial.println("A");
    delay(10);
  }
}


void update_light_height(){ //updates and shows light strip to reflect new light val
  //compare old value to new value
  int diff = light_val - old_light_val;
  if(diff >0){ //turn on diff# more lights
    for (int i=old_light_val + 1; i <= light_val; i++){
       strip.setPixelColor(i,curr_color);
    }
  }
  else if(diff < 0){//turn off diff# lights
    for(int i=old_light_val; i >= light_val + 1; i--){
       strip.setPixelColor(i,0x000000);
    }
  }
  strip.show();
}

void turn_all_off(){
   for(int i=0; i <= NUMPIXELS; i++){
    strip.setPixelColor(i,0x000000);
  }
  strip.show();
}

 
/*
void update_light_height()
{ //turn all except light low off
  for(int i=0; i <= NUMPIXELS; i++){
    strip.setPixelColor(i,0x000000);
  }
  for(int i=LIGHTHIGH; i >LIGHTHIGH - LIGHTLOW; i--){
    strip.setPixelColor(i,curr_color);
  }
  //update to new value
  for (int i=LIGHTHIGH - LIGHTLOW; i > LIGHTHIGH - LIGHTLOW - light_val; i--)
 {
  strip.setPixelColor(i, curr_color);
 }
 strip.show();
}*/



