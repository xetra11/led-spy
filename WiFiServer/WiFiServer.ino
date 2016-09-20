#include <ESP8266WiFi.h>

const char* ssid = "artex11";
const char* pw = "getmenow11";
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, pw);
  Serial.println("AP started at: ");
  Serial.print(WiFi.softAPIP());
  server.begin();
  Serial.println("Webserver started...");
}

void loop() {
 WiFiClient client = server.available(); 
  // wait for a client (web browser) to connect
  if (client)
  {
    Serial.println("\n[Client connected]");
    while (client.connected())
    {
      // read line by line what the client (web browser) is requesting
      if (client.available())
      {
        Serial.println("replying...");
        client.write('B'); 
        break;
      }
    }
    delay(1); // give the web browser time to receive the data

    // close the connection:
    client.stop();
    Serial.println("[Client disonnected]");
  }
}
