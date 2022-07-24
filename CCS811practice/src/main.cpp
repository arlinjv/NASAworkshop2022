/***************************************************************************
  This is a library for the CCS811 air

  This sketch reads the sensor

  Designed specifically to work with the Adafruit CCS811 breakout
  ----> http://www.adafruit.com/products/3566

  These sensors use I2C to communicate. The device's I2C address is 0x5A

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Dean Miller for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_CCS811.h>
#include <painlessMesh.h>
//#include <ArduinoJson.h> // include with painlessMesh

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

//prototypes:
void initCCS811 ();
void checkCCS811();
void receivedCallback( uint32_t from, String &msg );
void sendData(); 

Scheduler userScheduler;
painlessMesh  mesh;
Adafruit_CCS811 ccs;

void sendMessage() ; // Prototype
Task taskSendData( TASK_SECOND * 1, TASK_FOREVER, &sendData ); // start with a one second interval

void setup() {
  Serial.begin(115200);
  Serial.println("CCS811 sensor node powering up");
  
  initCCS811();

  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);

  userScheduler.addTask( taskSendData );
  taskSendData.enable();

}

void loop() {
  //checkCCS811();

  mesh.update();
}

void sendData(){
  //static uint16_t lastVal = 0;
  //static unsigned long timeLastMSGsent = 0;

  if(ccs.available()){
    if(!ccs.readData()){
      /*
        To Do: move this to separate routine and chack for fresh reading every loop 
          - then update global variable?
          - maybe do some averaging also
      */
      Serial.print("CO2: ");
      Serial.print(ccs.geteCO2());
      Serial.print("ppm, TVOC: ");
      Serial.print(ccs.getTVOC());
      Serial.print(",Raw: ");
      Serial.println(ccs.getRawADCreading());
      
      int16_t currentVal = ccs.geteCO2();
      //if (abs(currentVal - lastVal) > currentVal*0.1 || millis() - timeLastMSGsent > 2000){
      if (true){

        #if ARDUINOJSON_VERSION_MAJOR==6
        DynamicJsonDocument jsonBuffer(1024);
        JsonObject msg = jsonBuffer.to<JsonObject>();
        #else
        DynamicJsonBuffer jsonBuffer;
        JsonObject& msg = jsonBuffer.createObject();
        #endif

        msg["NODE"] = mesh.getNodeId();
        msg["CO2"] = ccs.geteCO2();
        msg["TVOC"] = ccs.getTVOC();
        msg["RAW"] = ccs.getRawADCreading();
        String str;
        serializeJson(msg, str);
        mesh.sendBroadcast(str);
        Serial.printf("Sending message: %s\n", str.c_str());    

        /*
        To Do: Add LED blink here
        */  

        //timeLastMSGsent = millis();
      }
      //lastVal = currentVal;

    }
    else{
      Serial.println("ERROR!");
    }
  }
}

void receivedCallback( uint32_t from, String &msg ) {
  //Serial.printf("VOC sensor node: Received from %u msg=%s\n", from, msg.c_str());
  //mesh.sendSingle(from, msg);
}

void initCCS811 (){
  
  if(!ccs.begin()){
    Serial.println("Failed to start sensor! Please check your wiring.");
    /*
      To Do: Add fast blink LED or send msg / get rid of blocking language
    */
    while(1);
  }

  // Wait for the sensor to be ready
  while(!ccs.available()){
    if (millis() % 1000 == 0) {
      Serial.println("waiting");
    }
  }

  ccs.setDriveMode(CCS811_DRIVE_MODE_1SEC);
}

void checkCCS811(){
  if(ccs.available()){
    if(!ccs.readData()){
      Serial.print("CO2: ");
      Serial.print(ccs.geteCO2());
      Serial.print("ppm, TVOC: ");
      Serial.println(ccs.getTVOC());
    }
    else{
      Serial.println("ERROR!");
      while(1);
    }
  }
  delay(500);

}