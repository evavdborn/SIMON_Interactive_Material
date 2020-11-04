#include <Wire.h>
#include <PWMServo.h>
#include <Adafruit_VL6180X.h>
#include <Adafruit_CAP1188.h>

//definitions of the State
#define RELAXED_STATE_ID 1
#define SCARY_PULLBACK_ID 2
#define PULLBACK_ID 3
#define ACTIVE_ID 4
#define OPENUP_ID 5
#define NEW_ADDRESS 0x29

int SimonLow = 30;

// SETUP SERVO MOTOR
PWMServo Simon;  // create servo object to control servo "Simon"
// twelve servo objects can be created on most boards

// USE I2C FOR THE FOLLOWING SLAVES
Adafruit_VL6180X distanceSensor = Adafruit_VL6180X();
Adafruit_CAP1188 cap = Adafruit_CAP1188();

int pos = 0;           // variable to store the servo position,
int counter;      //  count how many times the outer edges are touched
bool scared;          // boolean that stores if Simon got scared
int another_counter = 0;

//define range numbera
int error1 = 25;
int error2 = 30; 
int tooClose = 30;
int farEnough = 40;
int range_error = 0; 
int outofRange = 250; 
int state = RELAXED_STATE_ID;
int newState;
boolean shakeItBaby = false;
boolean pulledBaby = false;
boolean openUpBaby = false;
boolean range_boolean = false; 

//handle the millis
int previousMillisServo = 0;
long servoIntervalLong = 20;      //delay (effects the speed of the servo movement)
long servoIntervalShort = 10;
long waitNextEffect = 2000;             //wait inbetween effects of the servo
long waitTouch = 200;

unsigned long currentMillis = 0;
unsigned long millisBegin;
unsigned long millisNow;
int resetBorder = 15000; // maximum of 10 sec

void setup() {
  // CHANGED FROM 115200 TO 9600 --> WE MUST TRY WHAT WORKS
  Serial.begin(9600);

  // FIND THE MOTOR
  Simon.attach(9);  // attaches the servo on pin 9 to the servo object

  //while (!Serial) {
  // delay(1);
  //}

  Serial.println("Adafruit VL6180x test!");
  if (!distanceSensor.begin()) {
    Serial.println("Failed to find sensor");
    distanceSensor.setAddress(NEW_ADDRESS);
  }
  Serial.println("Sensor found!");
  Serial.println("CAP1188 test!");
  // Initialize the sensor, if using i2c you can pass in the i2c address
  if (!cap.begin(0x28)) {
    Serial.println("CAP1188 not found");
  }
  Serial.println("CAP1188 found!");

  uint8_t reg = cap.readRegister( 0x1f ) & 0x0f;
  cap.writeRegister( 0x1f, reg | 0x6F ); // or whatever value you want
  state = RELAXED_STATE_ID;
  Serial.println("SETUP COMPLETED!");
  Serial.println();
}


void loop() {
  setState(state);
  Serial.println("setState COMPLETED"); 
  Serial.print("State: "); Serial.println(state);
  Serial.println();
  handle_State();
  Serial.println("handleState COMPLETED"); 
  Serial.println();
  handle_Serial(); 
  Serial.println("handleSerial COMPLETED"); 
  Serial.println();
}

void setState(int newState){
   //guard: check if the state is changed
  if (newState == state) {
    return;
  }

  // set all state flags
  shakeItBaby = false;
  pulledBaby = false;
  openUpBaby = false;
  range_boolean = false; 
  //not sure about this
  scared = false;
  // update to new state
  state = newState;

  Serial.print("Just updated the state to:"); Serial.println(state);
  
}

void handle_Serial(){
  
  //CHECK DISTANCE SENSOR
  uint8_t range = distanceSensor.readRange();
  uint8_t status = distanceSensor.readRangeStatus();
  if (status == VL6180X_ERROR_NONE) {
    Serial.print("Range: "); Serial.println(range);
  }
  
  //check capsensing
  uint8_t touched = cap.touched();
  for (uint8_t i = 0; i < 2; i++) {
    if (touched & (1 << i)) {
      Serial.println();
      Serial.print("Capyarn touched : "); Serial.print(i + 1); Serial.println("\t");
      counter = counter + 1;
      Serial.print("counter: "); Serial.println(counter);
    }
      previousMillisServo = millis();
      while ((millis() - previousMillisServo) < waitTouch) ;
  }

  if (range > error1 && range < error2) {
    range_error = range_error + 1;
    range_boolean = true; 
  }

   // if in 5 seconds the counter does not change, reset the counter
   if (millisNow - millisBegin >= resetBorder && counter == counter) {
    counter == 0;
   }

  Serial.print("counter: "); Serial.println(counter);
  Serial.print("range error: "); Serial.println(range_error); 
  
  if (counter <=1 && range < error1 || range > outofRange) {
    setState(RELAXED_STATE_ID);
  }
  
  else if (counter <=1 && range < tooClose && range > error1) { //when not touched, but middle is touched, then pullback and shake (really scared)
    Serial.println("hallow JUMBO");
    setState(SCARY_PULLBACK_ID);

    // so if range is smaller than 
  } else if (counter >= 2 && range < tooClose && range > error1) { //when touched, but middle is touched, then pullback (scared)
    setState(PULLBACK_ID);

    // so if range is bigger than
  } else if (counter >= 5 &&  range > farEnough || range < error1) { //when touched at least 10 times and not touching middle part, activate Simon
    setState(ACTIVE_ID);  
    } 

  if (scared && counter >=5) { //when Simon was scared and capsens is touched again, he will get back up a bit
    setState(OPENUP_ID);
    setState(ACTIVE_ID); 
  }
}

//  // if in 5 seconds the counter does not change, reset the counter
//  if (millisNow - millisBegin >= resetBorder && counter == counter) {
//    counter == 0;
//  }
//}

void handle_State(){
  Serial.println("checking state:");
  // debug Simon

  switch(state) {
    case RELAXED_STATE_ID:
      Serial.println("Simon is relaxed");
      relaxed();
    break;

    case SCARY_PULLBACK_ID:
      Serial.println("Simon is REALLYY Scareedd");
      pullback();
      delay(1500);
      scared = true; //activate "scared" mode --> needs to get back up
      shake();
      setState(RELAXED_STATE_ID);     
    break;
      
    case PULLBACK_ID:
      Serial.println("PULLbackbaby");
      pullback();
      scared = true;
      setState(RELAXED_STATE_ID);
      break;
      
    case ACTIVE_ID:
     active();
     Serial.println(millisNow);
     Serial.println(millisBegin);
     Serial.println(another_counter);
     if (another_counter > 5 && counter == counter){
      setState(RELAXED_STATE_ID);
      counter = 0;
      another_counter = 0; 
     }
     break;

    case OPENUP_ID:
      openup();
      relaxed();
      openUpBaby = true;
    break;
  }
 }

void relaxed() {
  for (pos = 60; pos <= 120; pos += 1) { // goes from 0 degrees to 180 degrees
    Simon.write(pos);
    delay(20);
    }
    
    uint8_t range = distanceSensor.readRange();
    uint8_t status = distanceSensor.readRangeStatus();
    Serial.println(range);
    
    if (counter <=1 && range < tooClose && range > error1) 
    { 
      //when not touched, but middle is touched, then pullback and shake (really scared)
      Serial.println("hallow JUMBO");
      setState(SCARY_PULLBACK_ID);  
      return;
        // so if range is smaller than 
    } 
    
    else if (counter >= 2 && range < tooClose && range > error1)
    { //when touched, but middle is touched, then pullback (scared)
      setState(PULLBACK_ID);
      return;
    }
  

  for (pos = 120; pos >= 60; pos -= 1) { // goes from 180 degrees to 0 degrees
    Simon.write(pos);
    delay(20);
    uint8_t range = distanceSensor.readRange();
    uint8_t status = distanceSensor.readRangeStatus();
    Serial.println(range);
    
    if (counter <=1 && range < tooClose && range > error1) 
    { 
      //when not touched, but middle is touched, then pullback and shake (really scared)
      Serial.println("hallow JUMBO");
      setState(SCARY_PULLBACK_ID);  
      return;
        // so if range is smaller than 
    } 
    
    else if (counter >= 2 && range < tooClose && range > error1) 
    { //when touched, but middle is touched, then pullback (scared)
      setState(PULLBACK_ID);
      return;
    }
  }
}


void active() {
  Serial.println("Simon is active");
    for (pos = 30; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
      Simon.write(pos);
      delay(10);
     }
    for (pos = 180; pos >= 30; pos -= 1) { // goes from 180 degrees to 0 degrees
      Simon.write(pos);
      delay(10);
    }
    another_counter = another_counter +1;
}

void pullback() {
  Serial.println("Simon pulls back, don't touch his sensitive part!");
  Simon.write(SimonLow);
  delay(10);
}

void openup() {
  //When simon is touched 
  Serial.println("Simon likes your touch, he is trusting you again");
  for (int counter = 0; counter <= 180; counter = counter + 5) {
    Simon.write(SimonLow+counter);
    delay(20);
  }
}

void shake() {
  //First shake wild
  for (int i = SimonLow; i < 60; i += 5) {
    Serial.println("Simon is scared, brrr");
    for (pos = i; pos <= 100; pos += 3) { // goes from 0 degrees to 45 degrees
      Simon.write(pos);
      delay(10);
    }
    for (pos = 100; pos >= i; pos -= 3) { // goes from 45 degrees to 0 degrees
      Simon.write(pos);
      delay(10);
    }
  }

  for (int i = SimonLow; i < 30; i += 5) {
    Serial.println("Simon is scared, brrr");
    for (pos = i; pos <= 70; pos += 3) { // goes from 0 degrees to 45 degrees
      Simon.write(pos);
      delay(10);
    }
    for (pos = 70; pos >= i; pos -= 3) { // goes from 45 degrees to 0 degrees
      Simon.write(pos);
      delay(10);
    }
  }
}
