/*
Program for generating 38khz LED pulses for testing IR receiver modules
12 micro delay gives about 38 khz. (Ideally 13, but there is some overhead)

Task 1: use this setup to determine what the shortest pulse train is that will give an
output from the controller (then find out if this holds out at range)
- Initial test results: 
  - outdoors in shade at three inches can get down to 5 pulses

- pulse count math: At 38 khz, if one bit can be sent with 5 pulses, the time needed 
  for that bit is 5*26us = 130 uS. Minimal message length to indicate side of bot could be something 
  like: long start bit 300 uS, on 130 uS, off 130 uS, on 130 uS, off 130 uS - total time = 820 uS
Task 2: see what effect carrier frequency will have on reception range
*/

#include <Arduino.h>

//prototypes
void checkForSerialCMD();
void firePulseTrain(int numPulses);

int IRledPin = 21;
int pw = 13; // pulse width in micro seconds
int pulseCount = 10; 
int numTrains = -1; //fire continuously if -1
int intervalDelay = 0; //interval between pulses

void setup() {
    Serial.begin(115200);
    pinMode (LED_BUILTIN, OUTPUT);
    pinMode (IRledPin, OUTPUT);
    Serial.println("Ready to blink");
}

void loop() {

    /*
        To do: implement fire on button press
    */

    if (numTrains != 0){
        firePulseTrain(pulseCount);
        if (numTrains != -1){ //if it is -1 we want it to stay that way
            numTrains--;
        }
        delayMicroseconds(intervalDelay); //time between pulse train
    }

    checkForSerialCMD();

}

void checkForSerialCMD(){
  String cmdString = "";
  
  while (Serial.available()>0) {
    cmdString = Serial.readStringUntil('\n');
  }
        
  if (cmdString != ""){ 
      //cmdString.trim();
      Serial.print("CMD:"); Serial.println(cmdString); // send copy of incoming text to serial monitor
      
      if (cmdString.startsWith("w")){ // set pulse width
        pw = cmdString.substring(1).toInt();
        Serial.printf("Pulse width set to %d microseconds", pw); Serial.println();
      }
      if (cmdString.startsWith("c")){
        pulseCount = cmdString.substring(1).toInt();
        Serial.printf("Pulse count set to %d", pulseCount); Serial.println();
      }
      if (cmdString.startsWith("i")){
        intervalDelay = cmdString.substring(1).toInt();
        Serial.printf("Interval delay set to %d", intervalDelay); Serial.println();
      }
      if (cmdString.startsWith("f")){
        numTrains = cmdString.substring(1).toInt();
        Serial.printf("Number of trains delay set to %d", numTrains); Serial.println();
      }
      if (cmdString.startsWith("h")){
        Serial.printf("Pulse width (w): %d", pw); Serial.println();
        Serial.printf("Pulse count (c): %d", pulseCount); Serial.println();
        Serial.printf("Interval delay (i): %d", intervalDelay); Serial.println();
        Serial.printf("Num trains (f): %d", numTrains); Serial.println();
      }
      cmdString = "";
    }
}

void firePulseTrain(int numPulses){
    for (int i = 0; i< numPulses; i++){
        
        digitalWrite(LED_BUILTIN, HIGH);
        digitalWrite(IRledPin, HIGH);

        delayMicroseconds(pw);

        digitalWrite(LED_BUILTIN, LOW);
        digitalWrite(IRledPin, LOW);

        delayMicroseconds(pw);
    }
}