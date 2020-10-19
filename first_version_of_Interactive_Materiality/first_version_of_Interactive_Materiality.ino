//import dependencies
#include <Wire.h>
#include "Adafruit_VL6180X.h"

Adafruit_VL6180X vl = Adafruit_VL6180X();

//definitions 
#define NEUTRAL_STATE_ID 1
#define TENSED_STATE_ID 2
#define AVOIDANCE_STATE_ID 3
#define RELAXED_STATE_ID 4

//integers
const int ledPin = 13;
int state = NEUTRAL_STATE_ID;

//booleans
boolean pointedCenter = false;
boolean capCounter = false;

void setup() {
  Serial.begin(115200);
  //pinMode(ledPin, OUTPUT);

  // wait for serial port to open on native usb devices
  while (!Serial) {
    delay(1);
  }

  Serial.println("Adafruit VL6180x test!");
  if (! vl.begin()) {
    Serial.println("Failed to find sensor");
    while (1);
  }
  Serial.println("Sensor found!");
}

void loop() {
  // put your main code here, to run repeatedly:
  setState(int);
  
  handleState();
  
  handleSerial();

  uint8_t range = vl.readRange();
  uint8_t status = vl.readRangeStatus();

  if (status == VL6180X_ERROR_NONE) {
    Serial.print("Range: "); Serial.println(range);
}

/**
 * Set a new state
 */
 
void setState(int newState) {

  // guard: check if the state is changed
  if (newState == state) {
    return;
  }
  
  // set all state flags
  pointedCenter = false;
  capCounter = false;
  // update to new state
  state = newState;
  
}


void handleState(){
  Serial.println("checking state:");
  // debugging comes here
  
  // switch on the state
  switch (state) {

    // Do resting behaviour
    case NEUTRAL_STATE_ID:
      slowlyBreating();
      // get values capyarnsensing 
      Serial.println("Neutral state");
      break;

    // Do pull away and pulse
    case TENSED_STATE:
      Serial.println("Tensed state");
      
      if (!capCounter && pointedCenter){
        pullBack360Degrees();
        shakingMovement();
        Serial.println("Shacking!!!!"); 
      }

      if (capCounter && pointedCenter){
        pullBack360Degrees();
        slowlyBackMovement(); 
        Serial.println("No shaking"); 
      }
      
      break;

    case AVOIDANCE STATE:
      Serial.println("avoidance state:");
      // debugging comes here

      if (moveTowardsCenter && capCounter){
        positionBasedUponHands(); 
      }

      break;

    case RELAXED_STATE:
      Serial.println("relaxed state:");

      if (capCounter == true;){
        swingingMotion();
      }
      
      break;
        
      default:
      
      Serial.print("unknownstate: ");
      break;
}

void slowlyBreathing(){
  // slowly breathing movement
}


void pullBack360Degrees(){
  
}

void shakingMovement(){
  
}

void slowlyBackMovement{
  
}

void positionBasedUponHands{
   
}

void swingingMotion{
  // do swinging motion
  // activate capcounter
  // activate pointcounter
}



//void handleSerial() {
//
//  // guard: skip when no data is available
//  if (Serial.available() <= 0) {
//    return;
//  }
//
//  // variables
//  int inByte = Serial.read();
//
//  // debug
//  Serial.println("Received a new byte from the serial: ");
//  Serial.println(inByte);
//
//  // do something different depending on the character received.
//  // The switch statement expects single number values for each case;
//  // in this exmaple, though, you're using single quotes to tell
//  // the controller to get the ASCII value for the character.
//  if (inByte == 97) {  // a
//    setState(RESTING_STATE_ID);
//  }
//  else if (inByte == 98) {  // b
//    setState(NFC_STATE_ID);
//    leftIsScanned = true;
//  }
//  else if (inByte == 99) {  // c
//    setState(NFC_STATE_ID);
//    rightIsScanned = true;
//  }
//  else if (inByte == 100) { // d
//    setState(FOLLOW_STATE_ID);
//    intensity1 = 255;
//    intensity2 = 255;
//  }
//  else if (inByte == 102) { // f
//    setState(CELEBRATION_STATE_ID);
//  }
//
//  //
//  else if (state == FOLLOW_STATE_ID) {
//
//    // variables
//    byte debugLedPosition = inByte;
//
//    // debug
//    Serial.print("Positioning LED to: ");
//    Serial.println(debugLedPosition);
//
//    // set the LED pos
//    ledPos1 = debugLedPosition;
//  }
//}
