#include <ESP8266WiFi.h>

// Connection
const char* ssid = "artex11";
const char* pw = "getmenow11";
const char* host = "192.168.4.1";
const int port = 80;

//Game
const int health_leds[3] = {0,4,5};
int health = 5;
/* const int target_leds[3] = {4,5,6}; */

void setup_connection(){
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pw);
  Serial.println("Connecting to AP");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
    blink(5);
  }
  Serial.println(" connected");
}

void blink(int pin){
  if(digitalRead(pin) == LOW){
    digitalWrite(pin, HIGH);
  }else{
    digitalWrite(pin, LOW);
  }
}

void init_pins(){
  pinMode(0,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
}

void init_game(){
  digitalWrite(0, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH);
}

void lower_hp(){
  if(health < 4){
    digitalWrite(0, LOW);
  }
  digitalWrite(health, LOW);
  health--;
}

void setup() {
  init_pins();
  setup_connection();
  init_game();
}

void loop() {
  WiFiClient client;
  Serial.println("Connecting to host");
  if(!client.connect(host, port)){
    Serial.println("...connection failed!");
    Serial.println("Retrying in 5 seconds...");
    WiFi.printDiag(Serial);
    delay(5000);
    return;  
  }
  client.write('A');
  Serial.println("...TCP message fired!");
  delay(200);
  while(client.available()){
    if(client.read() == 'B'){
      lower_hp();
    }
  }
  client.stop();
  delay(2000);
}
