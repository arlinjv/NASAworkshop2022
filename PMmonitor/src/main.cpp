// Implement serial interface based menu system for painlessMesh
/*
Menu items:
API - print out all commands that this node recognizes
Help - human friendly description of API and other relevant info
Devices - list devices 


SimpleList page: https://github.com/blackhack/ArduLibraries/tree/master/SimpleList/examples/SimpleList_store_ints_example

To do:
- Find way to refer to nodes by index number rather than full Node ID
  - ideally each node will be able to refer to this index
- Menu items
  - delay<n> - measure network trip delay to n node
  - SS<n><msg> - send single message to node
  - BC<msg> - broadcast message 

*/
#include <Arduino.h>
#include <painlessMesh.h>

#define   MESH_SSID       "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

//prototypes
void checkForSerialCMD();
void checkMenu(String selection);
void sendMessage(); 
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback(); 
void nodeTimeAdjustedCallback(int32_t offset); 
void delayReceivedCallback(uint32_t from, int32_t delay);

Scheduler     userScheduler; // to control your personal task
painlessMesh  mesh;
SimpleList<uint32_t> nodes;

String cmdString = "";


void setup(void){
  mesh.init(MESH_SSID, MESH_PASSWORD, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.onNodeDelayReceived(&delayReceivedCallback);

  Serial.begin(115200); 
  Serial.setTimeout(2);  
  cmdString.reserve(100);
  Serial.println();
  Serial.println("PM Monitor");
  Serial.println("Ready for commands");

} // end setup
//================================================================

void loop(void){
  checkForSerialCMD();
  
  mesh.update();
} // end loop()
//=======================================================

void sendMessage() {
  String msg = "Hello from base node ";
  msg += mesh.getNodeId();
  //msg += " myFreeMemory: " + String(ESP.getFreeHeap());
  mesh.sendBroadcast(msg);

  Serial.printf("Sending message: %s\n", msg.c_str());
}

void receivedCallback(uint32_t from, String & msg) {
  Serial.printf("Msg received from %u msg=%s\n", from, msg.c_str());
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> New Connection, nodeId = %u\n", nodeId);
  Serial.printf("--> New Connection, %s\n", mesh.subConnectionJson(true).c_str());
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
 
  nodes = mesh.getNodeList();

  Serial.printf("Num nodes: %d\n", nodes.size());
  Serial.printf("Connection list:");

  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    Serial.printf(" %u", *node);
    node++;
  }
  Serial.println();
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void delayReceivedCallback(uint32_t from, int32_t delay) {
  // received in response to delay measurement response request (startDelayMeas)
  Serial.printf("Delay to node %u is %d us\n", from, delay);
}

void checkForSerialCMD(){
  while (Serial.available()>0) {
    cmdString = Serial.readStringUntil('\n');
  }
        
  if (cmdString != ""){ 
      //cmdString.trim();
      Serial.print("CMD:"); Serial.println(cmdString); // send copy of incoming data to serial monitor
      
      checkMenu(cmdString); // check to see if inString is a menu item
      
      cmdString = "";
    }
}

void checkMenu(String selection){
  
  if (selection == "test"){
    Serial.println("test command received");
    sendMessage();
  }
  else if (selection == "api"){
    Serial.println("api command received");
  }
  else if (selection == "help"){
    Serial.println("help command received");
    // Todo: check to see if selection starts with 'help' then call help menu to parse further
  }
  else if (selection == "list"){
    Serial.println("list devices command received");
    nodes = mesh.getNodeList();
    Serial.printf("Num nodes: %d\n", nodes.size());
    Serial.printf("Connection list:");
    SimpleList<uint32_t>::iterator node = nodes.begin();
    while (node != nodes.end()) {
      Serial.printf(" %u", *node);
      node++;
    }
  Serial.println();
  }
  
} // end checkMenu()
