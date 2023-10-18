#include "DHT11.h"
//
const int BLUE_LED = 6;         
const int GREEN_LED = 5;
const int YELLOW_LED = 7;
const int RED_LED = 4;
const int PIN_KEY1 = 9;
const int PIN_KEY2 = 8;
const int POTENTIO = 14;
const int LDR_IN = A2;
//
const int BOUNCE_DELAY = 50;      // (ms) for the buttons
const int BLINGK_INTERVAL = 500;  // (ms) for the leds
const int SEND_INTERVAl = 5000;   // (ms) for the temp
unsigned long green_lasttime;
unsigned long blue_lasttime;
unsigned long temp_lasttime;

//BUTTONS
int lastkey1state = HIGH;
int lastkey2state = HIGH;
int key1state = HIGH;
int key2state = HIGH;
int lasttime = 0;
//
int L_BTN = 1;
int R_BTN = 1;
//Steering W
int val;
int steering_wheel;
char last_wheel = ' ';
//
const int LIGHT = 200;  // LDR
char last_light_state = 'O';
bool start;
//
bool greenstate = false;
bool bluestate  = false;
char past_blue = 'N';
//
char last_turn_state;   //LOG
// HAZARD MODE
bool bluestate2 = LOW;
bool greenstate2 = LOW;
bool redstate2 = LOW;
unsigned long rd2_lasttime;
const int HAZARD_BLINKING = 1000;
char is_blue_blk = 'D';
char is_green_blk = 'D';

char report = ' ';

void setup() {
  Serial.begin(9600);
  pinMode(PIN_KEY1, INPUT_PULLUP);
  pinMode(PIN_KEY2, INPUT_PULLUP);
  pinMode(POTENTIO, INPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
}

void loop() {

  read_key();         //Constantly reading the buttons
  read_wheel();       //Reading the potentio
  read_temp();        //Reading the temperature
  read_ldr();         //Reading the ldr
  send_ms();          //Send messages with the last wheel position for the log in visual studio
  read_serial();      //Wait for messages
  read_report();      //Read the messages and turn on the alarm if it's the case

  if (L_BTN == 2 && is_blue_blk == 'D')         //If btn press blink but only if the 
    BLINK_BLUE();
  if (R_BTN == 2 && is_green_blk == 'D')
    BLINK_GREEN();

  if (TURN_LEFT() == false && last_wheel == 'l') {    //If the potentio is not left anymore reset the variable value, clear the last position andstop the blink
    L_BTN = 1;
    last_wheel = ' ';
    stop_blink_blue();
  }
  if (TURN_RIGHT() == false && last_wheel == 'r') {     //Same thing
    R_BTN = 1;
    last_wheel = ' ';
    stop_blink_green();
  }
}

void read_key()
{
  int key1 = digitalRead(PIN_KEY1);
  int key2 = digitalRead(PIN_KEY2);

  if (key1 != lastkey1state)
  {
    lasttime = millis();
  }
  if (key2 != lastkey2state)
  {
    lasttime = millis();
  }

  if (millis() - lasttime > BOUNCE_DELAY) {     

    if (key1state != key1) {
      key1state = key1;
      if (lastkey1state == LOW) {
        L_BTN = ((L_BTN++) % 2) + 1;      //Increase the L_BTN from 1 to 2 and repet
        stop_blink_blue();
      }
    }
    if (key2state != key2) {
      key2state = key2;
      if (lastkey2state == LOW) {
        R_BTN = ((R_BTN++) % 2) + 1;      //Increase the R_BTN from 1 to 2 and repet
        stop_blink_green();
      }
    }
  }
  lastkey1state = key1;
  lastkey2state = key2;
}

void read_wheel() {
  steering_wheel = analogRead(POTENTIO);
  val = map(steering_wheel, 0, 1023, 1, 13);
}

bool TURN_LEFT() {
  if (val < 6) {
    last_wheel = 'l';       //Return true if the wheel is turned left and store the last wheel position.
    return true;
  }

  return false;
}

bool TURN_RIGHT() {
  if (val > 8) {
    last_wheel = 'r';       //Same for right
    return true;
  }

  return false;
}

void flipLedGreen() {
  greenstate = !greenstate ;
  digitalWrite(GREEN_LED, greenstate);
}

void BLINK_GREEN() {
  unsigned long  currenttime = millis ();
  if (currenttime - green_lasttime > BLINGK_INTERVAL) {       //Blink led at the specific interval
    flipLedGreen();
    green_lasttime = currenttime ;
  }

}
void BLINK_BLUE() {
  unsigned long  currenttime2 = millis ();
  if (currenttime2 - blue_lasttime > BLINGK_INTERVAL) {       //Blink led at the specific interval
    flipLedBlue();
    blue_lasttime = currenttime2 ;
  }
  past_blue = 'A';
}

void flipLedBlue() {
  bluestate = !bluestate ;
  digitalWrite(BLUE_LED, bluestate);
}

void stop_blink_blue() {
  if (L_BTN == 1)
    digitalWrite(BLUE_LED, LOW);        //If L_BTN is 2 the R_BTN cannot be 2 becasue 2 means that the other led is blinking
  if (L_BTN == 2) {
    R_BTN = 1;
    digitalWrite(GREEN_LED, LOW);       //Make sure that is off
  }
}

void stop_blink_green() {
  if (R_BTN == 1)
    digitalWrite(GREEN_LED, LOW);     //Same thing but for right button
  if (R_BTN == 2) {
    L_BTN = 1;
    digitalWrite(BLUE_LED, LOW);
  }
}

void read_temp() {
  unsigned long  send_current_time = millis ();
  if ((send_current_time - temp_lasttime) > SEND_INTERVAl) {
    float temperature = DHT11.getTemperature();
    Serial.println("TEMP");               //Get the temperature at a specific interval and send through the serial port the message TEMP with the temperature(u will need this later in visual studio)
    Serial.println(temperature);
    temp_lasttime = send_current_time;
  }
}

void read_ldr() {
  int brightness = analogRead(LDR_IN);
  if (brightness < LIGHT && last_light_state == 'F') {      //Read the light and turn on the light if the last light state was different then the actual one and also send the message "HEADLIGHT_ON" in the serial port
    digitalWrite(YELLOW_LED, HIGH);
    Serial.println("HEADLIGHT_ON");
    last_light_state = 'O';
  }
  else if (brightness > LIGHT + 200 && last_light_state == 'O') {       //The same thing but OFF
    digitalWrite(YELLOW_LED, LOW);
    Serial.println("HEADLIGHT_OFF");
    last_light_state = 'F';
  }
}

void send_ms() {
  if (TURN_LEFT() && last_turn_state != 'L') {        //If the potentio is left and the last posittion was not left send the message "LEFT" in the serial port and also save the last position.
    Serial.println("LEFT");
    last_turn_state = 'L';
  }
  else if (TURN_RIGHT() && last_turn_state != 'R')    //Same thing
  {
    Serial.println("RIGHT");
    last_turn_state = 'R';
  }
  else if (TURN_RIGHT() == false && TURN_LEFT() == false && last_turn_state != 'C')   //Same thing
  {
    Serial.println("STEADY");
    last_turn_state = 'C';
  }
}

void hazar_blink() {
  unsigned long  currenttime2 = millis ();
  if (currenttime2 - rd2_lasttime > HAZARD_BLINKING) {      //Blinking for hazard mode
    flipLedshazard();

    rd2_lasttime = currenttime2 ;
  }
}

void flipLedshazard() {
  bluestate2 = !bluestate2 ;
  greenstate2 = !greenstate2;
  redstate2 = !redstate2;

  digitalWrite(BLUE_LED, bluestate2);
  digitalWrite(GREEN_LED, greenstate2);
  digitalWrite(RED_LED, redstate2);
}

void read_serial() {
  if (Serial.available()) {
    String message = Serial.readStringUntil('\n');      //Read serial if message is "ALARM" send a report(ON)
    if (message == "ALARM") {
      report = 'O';
    } else if (message == "NORMAL") {
      report = 'F';
    }
  }
}

void read_report() {      // If report is O then stop the normal blinking for the moment and start the hazard blinking
  if (report == 'O') {
    is_blue_blk = ' ';
    is_green_blk = ' ';
    hazar_blink();
  }
  else if (report == 'F') {     //If the report is F then stop the hazard, clear the report and turn on the normal blinking ifit'sthe case
    stop_hazard_blinking();
    report = ' ';
    is_blue_blk = 'D';
    is_green_blk = 'D';
  }
}

void stop_hazard_blinking() {
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
}
