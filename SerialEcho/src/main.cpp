/*
 * serial_echo.pde
 * ----------------- 
 * Echoes what is sent back through the serial port.
 *
 * http://spacetinkerer.blogspot.com
 */
#include <Arduino.h>
int incomingByte = 0;    // for incoming serial data

void setup() {
    Serial.begin(9600);    // opens serial port, sets data rate to 9600 bps
    delay(1000);
    Serial.println("Beginning echo program");
}

void loop() {
  // send data only when you receive data:
  if (Serial.available() > 0) {
  
    // read the incoming byte:
    incomingByte = Serial.read();
  
    // say what you got:
    Serial.print((char)incomingByte);
  }
  
}