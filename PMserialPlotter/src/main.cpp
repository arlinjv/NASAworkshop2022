//************************************************************
// this is a simple example that uses the painlessMesh library and echos any JSON
// messages it receives to the Serial port. Print messages are formatted for the 
// Arduino IDE Serial Plotter.
//
// Serial Plotter info: https://github.com/arduino/Arduino/blob/ba34eb640e5f6c2f8618debf61022f731c0eab74/build/shared/ArduinoSerialPlotterProtocol.md
//
//************************************************************
#include <Arduino.h>
#include "painlessMesh.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

// Prototypes
void receivedCallback( uint32_t from, String &msg );

painlessMesh  mesh;

void setup() {
  //Serial.begin(115200);
  //Serial.println("Echo node starting up");
    
  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT );
  mesh.onReceive(&receivedCallback);

  mesh.setDebugMsgTypes( ERROR );
  //Serial.begin(115200);
}

void loop() {
  mesh.update();
}

void receivedCallback( uint32_t from, String &msg ) {
  //Serial.printf("echoNode: Received from %u msg=%s\n", from, msg.c_str());
  //mesh.sendSingle(from, msg);
  static bool firstMsg = true;

  if (firstMsg){
    Serial.begin(115200); //trying to minimize random messages to Serial - not doing much good
    firstMsg = false;
  }
  
  if (msg.startsWith("{")){
    StaticJsonDocument<200> doc;
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, msg);

    String node = doc["NODE"];
    String raw = doc["RAW"];
    String co2 = doc["CO2"];
    String tvoc = doc["TVOC"];

    //String str = "Node_" + node.substring(node.length()-3) + "_RAW:" + raw + "\t" + "Node_" + node.substring(node.length()-3) + "_CO2:" + co2;
    String str = "RAW:" + raw + "\t" + "CO2:" + co2 + "\t" + "TVOC:" + tvoc;
    
    //String str = "Node_" + node.substring(node.length()-3) + "_TVOC:" + tvoc;
    
    Serial.println(str);
    
    //Serial.print(Serial.print("raw:");
    //Serial.print(raw);
    //Serial.print("\t");
    //labelString = "Node_" + node.substring(6) + "_CO2:";
    //Serial.print(labelString);
    //Serial.print("CO2:");
    //Serial.println(co2);
  }

}
