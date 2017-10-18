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

void setup() {
  
  Serial.begin(115200);

  Serial.println("serial init");

  pinMode(left_blink_pin, INPUT);
  pinMode(right_blink_pin, INPUT);
  pinMode(low_beams_pin, INPUT);
  pinMode(high_beams_pin, INPUT);
  pinMode(wiper_l_pin, INPUT);
  pinMode(wiper_h_pin, INPUT);
  
  pinMode(fan_mosfet, OUTPUT);
  pinMode(left_blink_nmos, OUTPUT);
  pinMode(right_blink_nmos, OUTPUT);
  pinMode(low_beams_nmos, OUTPUT);
  pinMode(high_beams_nmos, OUTPUT);
  pinMode(wiper_o_nmos, OUTPUT);
  pinMode(wiper_l_nmos, OUTPUT);
  pinMode(wiper_h_nmos, OUTPUT);

  
}

void loop() {
  
  // set the digital out
  digitalWrite(fan_mosfet, LOW);
  digitalWrite(left_blink_nmos, LOW);
  digitalWrite(right_blink_nmos, LOW);
  digitalWrite(low_beams_nmos, LOW);
  digitalWrite(high_beams_nmos, LOW);
  digitalWrite(wiper_o_nmos, HIGH);
  digitalWrite(wiper_l_nmos, LOW);
  digitalWrite(wiper_h_nmos, LOW);
  
  if (millis() % 500 == 0) {
    Serial.print(analogRead(fan_ect_pin));
    Serial.println(" -- fan ect");
    Serial.print(analogRead(sped));
    Serial.println(" -- sped");

    Serial.print(digitalRead(left_blink_pin));
    Serial.println(digitalRead(left_blink_pin) + " -- dirl");
    Serial.print(digitalRead(right_blink_pin));
    Serial.println(digitalRead(right_blink_pin) + " -- dirr");
    
    Serial.print(digitalRead(low_beams_pin));
    Serial.println(digitalRead(low_beams_pin) + " ++ lightl");
    Serial.print(digitalRead(high_beams_pin));
    Serial.println(digitalRead(high_beams_pin) + " ++ lighth");

    Serial.print(digitalRead(wiper_l_pin));
    Serial.println(digitalRead(wiper_l_pin) + " ++ wiperl");
  
    Serial.print(digitalRead(wiper_h_pin));
    Serial.println(digitalRead(wiper_h_pin) + " ++ wiperh");
  
    Serial.println("-----------------------");
  }
}
