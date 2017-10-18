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

const int left_blink_pin = 33;      // panel button
const int right_blink_pin = 32;     // panel button
const int low_beams_pin = 31;       // panel button
const int high_beams_pin = 30;      // panel button
const int fan_ect = A0;             // ECT J1 (thermister)
const int sped = A1;                // SPED J1 (speed pulses)
const int wiper_l_pin = 34;         // panel button
const int wiper_h_pin = 35;         // panel button

const int fan_pmos = 24;            //ECF J1
const int left_blink_pmos = 22;     //DIRL J2
const int right_blink_pmos = 23;    //DIRR J2
const int low_beams_nmos = 25;      //HLL J2
const int high_beams_nmos = 26;     //HLH J2
const int wiper_o_pmos = 27;        //WIPERO J2
const int wiper_l_pmos = 29;        //WIPERL J2
const int wiper_h_pmos = 28;        //WIPERH J2

const int fan_voltage_threshold = 1.04; // 158 degrees F and lower V is hotter

// set the initial states of the agent state
int blink = 0;   //  0 is off // 1 is left // 2 is right
int lights = 0;  //  0 is off // 1 is low  // 2 is high 
float fan_voltage = 0;
unsigned long fan_decay = 250;

// init the buttons and previous states;

int left_blink_control = LOW;
int right_blink_control = LOW;
int low_beams_control = LOW;
int high_beams_control = LOW;
int wiper_o_control = LOW;
int wiper_l_control = LOW;
int wiper_h_control = LOW;
int fan_control = LOW;

int left_blink_control_last = LOW;
int right_blink_control_last = LOW;
int low_beams_control_last = LOW;
int high_beams_control_last = LOW;
int wiper_o_control_last = LOW;
int wiper_l_control_last = LOW;
int wiper_h_control_last = LOW;
int fan_control_last = LOW;

unsigned long debounceDelay = 50;

unsigned long left_blink_button_debounce = 0;
unsigned long right_blink_button_debounce = 0;
unsigned long low_beams_button_debounce = 0;
unsigned long high_beams_button_debounce = 0;
unsigned long wiper_l_button_debounce = 0;
unsigned long wiper_h_button_debounce = 0;




#include "messages.h"

/* error codes */
#define NUMERRORCODES 3
#define READY 0
#define CRCFAIL 1
#define POWEROFF 2

#include <SoftwareSerial.h>
SoftwareSerial rpiSerial(9, 8); // RX, TX on the arduino digital pins

int msPerRotation = 0;
unsigned long previousRotationTime = 0;

int SPED_PIN = 0;  // on the arduino analog pins
int numNewCounts = 0;
bool lookingForNewEdge = true;

serialMessage errorMessages[NUMERRORCODES]; // will hold all of the error messages unions

#define NUMDATAVALUES 2
int dataValues[NUMDATAVALUES];







void setup() {
  Serial.begin(9600);
  rpiSerial.begin(9600);

  // populate the errorMessages array
  errorMessages[CRCFAIL] = { 0x00, 0xFF, 0xFF };
  errorMessages[READY] = { 0x00, 0xFE, 0xFE };
  errorMessages[POWEROFF] = { 0x00, 0xFD, 0xFD }; // should be sent when the starter arduino has told the system to poweroff


  pinMode(left_blink_pin, INPUT);
  pinMode(right_blink_pin, INPUT);
  pinMode(low_beams_pin, INPUT);
  pinMode(high_beams_pin, INPUT);
  pinMode(wiper_l_pin, INPUT);
  pinMode(wiper_h_pin, INPUT);
  pinMode(fan_ect, INPUT);
  
  pinMode(fan_pmos, OUTPUT);
  pinMode(left_blink_pmos, OUTPUT);
  pinMode(right_blink_pmos, OUTPUT);
  pinMode(low_beams_nmos, OUTPUT);
  pinMode(high_beams_nmos, OUTPUT);
  pinMode(wiper_o_pmos, OUTPUT);
  pinMode(wiper_l_pmos, OUTPUT);
  pinMode(wiper_h_pmos, OUTPUT);

  digitalWrite(left_blink_pmos, left_blink_control);
  digitalWrite(right_blink_pmos, right_blink_control);
  digitalWrite(low_beams_nmos, low_beams_control);
  digitalWrite(high_beams_nmos, high_beams_control);
  digitalWrite(fan_pmos, fan_control);
  digitalWrite(wiper_o_pmos, wiper_o_control);
  digitalWrite(wiper_l_pmos, wiper_l_control);
  digitalWrite(wiper_h_pmos, wiper_h_control);
}

void loop() {
  
  if (millis() % 1000 == 0) {
    
    Serial.print(analogRead(fan_ect));
    Serial.print(" -- fan ect ||  ");
    Serial.print(analogRead(sped));
    Serial.print(" -- sped ||  ");

    Serial.print(digitalRead(left_blink_pin));
    Serial.print(digitalRead(left_blink_pin) + " -- dirl ||  ");
    Serial.print(digitalRead(right_blink_pin));
    Serial.print(digitalRead(right_blink_pin) + " -- dirr ||  ");
    
    Serial.print(digitalRead(low_beams_pin));
    Serial.print(digitalRead(low_beams_pin) + " -- lightl ||  ");
    Serial.print(digitalRead(high_beams_pin));
    Serial.print(digitalRead(high_beams_pin) + " -- lighth ||  ");

    Serial.print(digitalRead(wiper_l_pin));
    Serial.print(digitalRead(wiper_l_pin) + " -- wiperl ||  " );
  
    Serial.print(digitalRead(wiper_h_pin));
    Serial.print(digitalRead(wiper_h_pin) + " -- wiperh ||  " );
    Serial.println("--");
  }
  // process here, read the buttons, check for debounce, and then do the magic
  
  
  // read the buttons and debounce
  
  if (left_blink_control_last != digitalRead(left_blink_pin)) {
    left_blink_button_debounce= millis();
  }
  if (right_blink_control_last != digitalRead(right_blink_pin)) {
    right_blink_button_debounce= millis();
  }


  // update the state
  
  if ((millis() - left_blink_button_debounce) > debounceDelay) {
    Serial.println("left dedup");
    if (left_blink_control != digitalRead(left_blink_pin)) {
      Serial.println("BUTTON");
      digitalRead(left_blink_pmos) == LOW ?  digitalWrite(left_blink_pmos, HIGH) : digitalWrite(left_blink_pmos, LOW);
      left_blink_control_last == left_blink_pmos;
    }
  }

  if (low_beams_control != digitalRead(low_beams_pin)) {
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
    low_beams_control_last == digitalRead(low_beams_pin);
  }    

  
  if (millis() % 1000 == 0) {
    fan_voltage = analogRead(fan_ect) * (5.0 / 1023.0);
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

  // set the digital out
  //digitalWrite(fan_pmos, fan_control);
  //digitalWrite(left_blink_pmos, left_blink_button);
  //digitalWrite(right_blink_pmos, right_blink_button);
  //digitalWrite(low_beams_nmos, low_beams_control);
  //digitalWrite(high_beams_nmos, high_beams_control);
  // logic for filling datavalues
  int threshold = 100;
  if ((analogRead(SPED_PIN) >= threshold) && lookingForNewEdge) {
    numNewCounts++;
    
    unsigned long current = millis();
    msPerRotation = current - previousRotationTime;
    previousRotationTime = current;
    
    lookingForNewEdge = false;
  }
  else if (analogRead(SPED_PIN) < threshold) {
    lookingForNewEdge = true;
  }

  //fill datavalues
  dataValues[0] = msPerRotation;
  dataValues[1] = numNewCounts;


  // try and process a message on the port
  tryProcessMessage(rpiSerial, true);


 
}

int tryProcessMessage(SoftwareSerial &port, bool debugMode) {

  if (port.available() >= MESSAGESIZE) { // wait until a full MESSAGE to arrive

    serialMessage masterMessage = readMsg(port);

    if (debugMode) {
      Serial.print("Got message from master: ");
      printMsg(masterMessage);
    }

    byte incomingMasterCRC = calcCRC(masterMessage.dataBytes, MESSAGESIZE - 1);

    if (incomingMasterCRC == masterMessage.CRC) { // if the CRC from the master passes

      intBytes transmission; // used to convert the int to a byte array for transmission

      switch (masterMessage.M0) {
      
      case 0: // read mode -> Send values to master
        
        transmission.i = dataValues[masterMessage.M1];

        // post processing filling a message
        if (masterMessage.M1 == 1) {
          numNewCounts = 0;
        }

        break;
      
      case 1: // write mode -> Set values on arduino

        // do nothing in V1
        
        break;
      
      case 2:
        
        if (debugMode) {
          Serial.println("Sending Ready");
        }
        
        writeMsg(port, errorMessages[READY]);
        return true;
        
        break;
      
      default:
        
        if (debugMode) {
          Serial.println("Sending CRC Error - But CRC passed");
        }
        writeMsg(port, errorMessages[CRCFAIL]);
        break;
      }

      serialMessage slaveMessage;
      memcpy(slaveMessage.dataBytes, transmission.bytes, MESSAGESIZE - 1);
      slaveMessage.CRC = calcCRC(slaveMessage.dataBytes, MESSAGESIZE - 1);

      writeMsg(port, slaveMessage);

      if (debugMode) {
        Serial.println("Sending Regular Message");
        Serial.print("Reading: ");
        Serial.println(transmission.i);
        printMsg(slaveMessage);
      }

    }
    else { // Bad master CRC

      if (debugMode) {
        Serial.print("Bad Master Message: ");
        printMsg(masterMessage);
      }

      drainBuffer(port);
      writeMsg(port, errorMessages[CRCFAIL]);
    }
    return true;
  }
  return false;
}

void drainBuffer(SoftwareSerial &port) {
  while (port.available()) {
    port.read();
  }
}

bool compareByteArrays(byte array1[], byte array2[], int arrayLength) {
  for (int index = 0; index < arrayLength; index++) {
    if (array1[index] != array2[index]) {
      return false;
    }
  }
  return true;
}

serialMessage readMsg(SoftwareSerial &port) {
  serialMessage incomingMessage;
  port.readBytes(incomingMessage.rawBytes, MESSAGESIZE); // read the raw bytes from the serial port into a message struct
  return incomingMessage;
}

void writeMsg(SoftwareSerial &port, serialMessage m) {
  for (int index = 0; index < MESSAGESIZE; index++) {
    port.write(m.rawBytes[index]);
  }
}

void printMsg(serialMessage m) {
  Serial.print("Message [");
  Serial.print("M0: ");
  Serial.print(m.rawBytes[0], HEX);
  Serial.print(" M1: ");
  Serial.print(m.rawBytes[1], HEX);
  Serial.print(" CRC: ");
  Serial.print(m.rawBytes[2], HEX);
  Serial.println("]");
}

byte calcCRC(byte byteArray[], int numBytes) {
  byte b = 0;
  for (int index = 0; index < numBytes; index++)
  {
    b = b + byteArray[index];
  }
  return b;
}
