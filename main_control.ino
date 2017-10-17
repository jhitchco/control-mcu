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

// get all the pins figured out
const string fan_voltage = "A0";
const int left_bink_button = 2;
const int right_blink_button = 3;
const int low_beams_button = 4;
const int high_beams_button = 5;
const int low_beams_nmos = 6;
const int fan_mosfet = 7;
const int left_blink_nmos = 10;
const int right_blink_nmos = 11;
const int high_beams_nmos = 12;

// set the initial states of the agent state
string blinker = "n";
string lights = "n";
int fan_on = 0;

// init the buttons and previous states;
int left_bink_button_state = LOW;
int right_blink_button = LOW;
int low_beams_button = LOW;
int high_beams_button = LOW;
int left_bink_button_state_last = LOW;
int right_blink_button_last = LOW;
int low_beams_button_last = LOW;
int high_beams_button_last = LOW;


unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 250;

void setup() {
  pinMode(left_bink_button, INPUT);
  pinMode(right_blink_button, INPUT);
  pinMode(low_beams_button, INPUT);
  pinMode(high_beams_button, INPUT);
  
  pinMode(low_beams_nmos, INPUT);
  pinMode(fan_mosfet, OUTPUT);
  pinMode(left_blink_nmos, INPUT);
  pinMode(right_blink_nmos, OUTPUT);
  pinMode(high_beams_nmos, OUTPUT);

  digitalWrite(low_beams_nmos, LOW);
  digitalWrite(fan_mosfet, LOW);
  digitalWrite(left_blink_nmos, LOW);
  digitalWrite(right_blink_nmos, LOW);
  digitalWrite(high_beams_nmos, LOW);
}

void loop() {
  // process here, read the buttons, check for debounce, and then do the magic
  
  int left_bink_button_last = digitalRead(left_bink_button);
  int right_blink_button_last = digitalRead(right_blink_button); 
  int low_beams_button_last = digitalRead(low_beams_button);
  int high_beams_button_last = digitalRead(high_beams_button);

}
