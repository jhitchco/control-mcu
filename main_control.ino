/*
 v0.9
 Features
  Fan control
    if fan read temp every second, if more than 185, trigger fan
    else if fan on
      start timer for 30 seconds
      each second if less than 165 degrees
    
  Blinker toggle
    if button press_l and blinker is not l, set blinker to l
    else if  button press_r and blinker is not r, set blinker to r
    else if button press_l and blinker is l, set blinker to n
    else if button press_r and blinker is r, set blinker to n
    switch blinker
      case l
        blink l
      case r
        blink r
      case n
        no nothing
      
  Light control (high and low)
    if button press_h and light is not h, set light to h
    else if  button press_l and light is not l, set light to l
    else if button press_h and light is h, set light to n
    else if button press_h and light is h, set light to n
    switch light
      case h
        set light h
      case l
        set light l
      case n
        set light n
 Wiper control button (TODO)
    if button wiper and wiper is not l, set blinker to l
    if button wiper and wiper is not n, set wiper to n
 
 Pins
  A0 - fan voltage
 
  D2 - left directional
  D3 - right directional
  D4 - low light
  D5 - high light
  
  D6 - lowbeam NMOST
  D7- fan MOSFET
  
  D10 - left directional NMOS
  D11 - right directional NMOS
  D12 - highbeam NMOS

*/


const int left_blink_pin = 33;
const int right_blink_pin = 32;
const int low_beams_pin = 31; //active high
const int high_beams_pin = 30; //active high
const int fan_ect_pin = A0;
const int sped = A1;
const int wiper_l_pin = 34;
const int wiper_h_pin = 35;

const int fan_mosfet = 26;
const int left_blink_nmos = 22;
const int right_blink_nmos = 23;
const int low_beams_nmos = 24;
const int high_beams_nmos = 25;
const int wiper_o_nmos = 27;
const int wiper_l_nmos = 28;
const int wiper_h_nmos = 29;

const int fan_voltage_threshold = 1.04; // 158 degrees F and lower V is hotter

// set the initial states of the agent state
int blink = 0;   //  0 is off // 1 is left // 2 is right
int lights = 0;  //  0 is off // 1 is low  // 2 is high 
float fan_voltage = 0;
unsigned long fan_decay = 250;

// init the buttons and previous states;
int left_blink_button = LOW;
int right_blink_button = LOW;
int low_beams_button = LOW;
int high_beams_button = LOW;
int low_beams_control = LOW;
int high_beams_control = LOW;
int wiper_l_button = LOW;
int wiper_h_button = LOW;
int wiper_o_control = LOW;
int wiper_l_control = LOW;
int wiper_h_control = LOW;
int fan_control = LOW;


int left_blink_button_last = LOW;
int right_blink_button_last = LOW;
int low_beams_button_last = LOW;
int high_beams_button_last = LOW;
int wiper_l_button_last = LOW;
int wiper_h_button_last = LOW;


unsigned long debounceDelay = 250;
unsigned long left_blink_button_debounce = 0;
unsigned long right_blink_button_debounce = 0;
unsigned long low_beams_button_debounce = 0;
unsigned long high_beams_button_debounce = 0;

void setup() {

  pinMode(left_blink_pin, INPUT);
  pinMode(right_blink_pin, INPUT);
  pinMode(low_beams_pin, INPUT);
  pinMode(high_beams_pin, INPUT);
  pinMode(wiper_l_button, INPUT);
  pinMode(wiper_h_button, INPUT);
  pinMode(fan_ect_pin, INPUT);
  
  pinMode(fan_mosfet, OUTPUT);
  pinMode(left_blink_nmos, INPUT);
  pinMode(right_blink_nmos, OUTPUT);
  pinMode(low_beams_nmos, OUTPUT);
  pinMode(high_beams_nmos, OUTPUT);
  pinMode(wiper_o_nmos, OUTPUT);
  pinMode(wiper_l_nmos, OUTPUT);
  pinMode(wiper_h_nmos, OUTPUT);

  digitalWrite(left_blink_nmos, left_blink_button);
  digitalWrite(right_blink_nmos, right_blink_button);
  digitalWrite(low_beams_nmos, low_beams_control);
  digitalWrite(high_beams_nmos, high_beams_control);
  digitalWrite(fan_mosfet, fan_control);
  digitalWrite(wiper_o_nmos, wiper_o_control);
  digitalWrite(wiper_l_nmos, wiper_l_control);
  digitalWrite(wiper_h_nmos, wiper_h_control);
}

void loop() {
  // process here, read the buttons, check for debounce, and then do the magic
  
  
  // read the buttons and debounce
  
  if (left_blink_button_last != digitalRead(left_blink_pin)) {
    left_blink_button_debounce = millis();
  }
  if (right_blink_button_last != digitalRead(right_blink_pin)) {
    right_blink_button_debounce= millis();
  }
  if (low_beams_button_last != digitalRead(low_beams_pin)) {
    low_beams_button_debounce  = millis();
  }
  if (high_beams_button_last != digitalRead(high_beams_pin)) {
    high_beams_button_debounce  = millis();
  }
  
  // update the state
  
  if ((millis() - left_blink_button_debounce) > debounceDelay) {
    if (left_blink_button != digitalRead(left_blink_pin)) {
      digitalRead(left_blink_pin) == LOW ?  left_blink_button = HIGH : left_blink_button = LOW;
      left_blink_button_last == digitalRead(left_blink_pin);
    }
  }

  if ((millis() - right_blink_button_debounce) > debounceDelay) {
    if (right_blink_button != digitalRead(right_blink_pin)) {
      digitalRead(right_blink_pin) == LOW ?  right_blink_button = HIGH : right_blink_button = LOW;
      right_blink_button_last == digitalRead(right_blink_pin);    
    }
  }

  if ((millis() - low_beams_button_debounce) > debounceDelay) {
    if (low_beams_button != digitalRead(low_beams_pin)) {
      // do a button press for low beams
      switch (lights) {
        case 0:
          low_beams_control = HIGH;
          high_beams_control = LOW;
          lights = 1;
          break;
        case 1:
          low_beams_control = LOW;
          high_beams_control = LOW;
          lights = 0;
          break;
        case 2:
          low_beams_control = HIGH;
          high_beams_control = LOW;
          lights = 1;
          break;
      }
      low_beams_button_last == digitalRead(low_beams_pin);
    }    
  }

  if ((millis() - high_beams_button_debounce) > debounceDelay) {
    if (high_beams_button != digitalRead(high_beams_pin)) {
      // do a button press for high beams
      switch (lights) {
        case 0:
          low_beams_control = HIGH;
          high_beams_control = HIGH;
          lights = 2;
          break;
        case 1:
          low_beams_control = HIGH;
          high_beams_control = HIGH;
          lights = 2;
          break;
        case 2:
          low_beams_control = LOW;
          high_beams_control = LOW;
          lights = 0;
          break;
      }
      high_beams_button_last == digitalRead(high_beams_pin);
    }
  }
  
  if (millis() % 1000 == 0) {
    fan_voltage = analogRead(fan_ect_pin) * (5.0 / 1023.0);
    if (fan_control == LOW) {
      if (fan_voltage < fan_voltage_threshold) {
        fan_control = HIGH;
        fan_decay = millis();
      }
    } else { 
      fan_decay =+ 1;
      if (fan_voltage > fan_voltage_threshold && fan_decay > 30) {
        fan_control = LOW;
      }
    }
  } 
  
  if (millis() % 10 == 0) {
    if (digitalRead(wiper_l_button)) {
      digitalWrite(wiper_o_nmos, LOW);
      digitalWrite(wiper_l_nmos, HIGH);
    } else {
      digitalWrite(wiper_o_nmos, HIGH);
      digitalWrite(wiper_l_nmos, LOW);  
    }
  }
  
  
  // set the digital out
  digitalWrite(fan_mosfet, fan_control);
  digitalWrite(left_blink_nmos, left_blink_button);
  digitalWrite(right_blink_nmos, right_blink_button);
  digitalWrite(low_beams_nmos, low_beams_control);
  digitalWrite(high_beams_nmos, high_beams_control);
  
}
