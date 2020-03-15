/*
   IoT ESP8266 Based Mood Lamp (RGB LED) Controller Program
   https://circuits4you.com

   https://circuits4you.com/2019/03/28/esp8266-iot-rgb-led-strip-mood-lamp-controller/#more-1721

*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>   // Include the SPIFFS library
#include "setting.h"	// all settings are defined in a extra file


ESP8266WebServer server(80);


String getContentType(String filename); // convert the file extension to the MIME type
bool handleFileRead(String path);       // send the right file to the client (if it exists)



//=======================================================================
//                    Handle Set Color
//=======================================================================
void handleForm() {
  //Saperate Colors are sent through javascript
  String red = server.arg("r");
  String green = server.arg("g");
  String blue = server.arg("b");
  int r = red.toInt();
  int g = green.toInt();
  int b = blue.toInt();

  Serial.print("Red:"); Serial.println(r);
  Serial.print("Green:"); Serial.println(g);
  Serial.print("Blue:"); Serial.println(b);

  //PWM Correction 8-bit to 10-bit
  r = r * 4;
  g = g * 4;
  b = b * 4;

  //for ULN2003 or Common Cathode RGB Led not needed
  /*
    r = 1024 - r;
    g = 1024 - g;
    b = 1024 - b;
  */
  //ESP supports analogWrite All IOs are PWM
  analogWrite(RedLED, r);
  analogWrite(GreenLED, g);
  analogWrite(BlueLED, b);

  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "Updated-- Press Back Button");

  delay(500);
}
//=======================================================================
//                    SETUP
//=======================================================================
void setup() {
  Serial.begin(115200);   //Start serial connection
  versionsInfo();   //print File information
  pinMode(RedLED, OUTPUT);
  pinMode(GreenLED, OUTPUT);
  pinMode(BlueLED, OUTPUT);

  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println("WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP

  SPIFFS.begin();                           // Start the SPI Flash Files Syste
 
  server.onNotFound([]() {                              // If the client requests any URI
    if (!handleFileRead(server.uri()))                  // send it if it exists
      server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
  });


 // server.on("/", handleFileRead("/"));  //Associate handler function to path
  server.on("/setRGB", handleForm);

  server.begin();                           //Start server
  Serial.println("HTTP server started");
}


//=======================================================================
//                    Content reader
//=======================================================================
String getContentType(String filename) { // convert the file extension to the MIME type
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  return "text/plain";
}

//=======================================================================
//                    File reader function
//=======================================================================
bool handleFileRead(String path) { // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";         // If a folder is requested, send the index file
  String contentType = getContentType(path);            // Get the MIME type
  if (SPIFFS.exists(path)) {                            // If the file exists
    File file = SPIFFS.open(path, "r");                 // Open it
    size_t sent = server.streamFile(file, contentType); // And send it to the client
    file.close();                                       // Then close the file again
    return true;
  }
  Serial.println("\tFile Not Found");
  return false;                                         // If the file doesn't exist, return false
}


//=======================================================================
//                    LOOP
//=======================================================================
void loop() {
  server.handleClient();
}


//=======================================================================
//                    Versionsinfo
//=======================================================================
void versionsInfo()
{
  Serial.print("\nArduino is running Sketch: ");
  Serial.println(__FILE__);
  Serial.print("Compiled on: ");
  Serial.print(__DATE__);
  Serial.print(" at ");
  Serial.print(__TIME__);
  Serial.print("\n\n");
}
