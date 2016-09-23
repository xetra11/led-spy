#include <ESP8266WiFi.h>

#define RST_BTN 6
#define H1 0
#define H2 4
#define H3 5
#define B1 2
#define B2 3

// Connection
const char* ssid = "artex11";
const char* pw = "getmenow11";
const char* host = "192.168.4.1";
const int port = 80;

//Game
const int health_leds[3] = {H1,H2,H3};
const int base_leds[2] = {B1,B2};
int health = 3;

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

bool is_dead(){
  return health <= 0;
}

bool has_client(WiFiClient* client){
  return client != NULL;
}

bool did_hit(int led_id){
  if (led_id == base_leds[0]) return true;
  if (led_id == base_leds[1]) return true;
  return false;
}

int get_pressed_button(){
  if(digitalRead(1) == HIGH) return 1;
  if(digitalRead(2) == HIGH) return 2;
  if(digitalRead(3) == HIGH) return 3;
  if(digitalRead(4) == HIGH) return 4;
  if(digitalRead(5) == HIGH) return 5;
}

WiFiClient* connect_to_host(){
  WiFiClient* client = (WiFiClient*) malloc(sizeof(WiFiClient));    
  Serial.println("Connecting to host");
  if(!client->connect(host, port)){
    Serial.println("...connection failed!");
    Serial.println("Retrying in 5 seconds...");
    WiFi.printDiag(Serial);
    delay(5000);
    return NULL;  
  }
  return client;
}

void setup() {
  init_pins();
  setup_connection();
  init_game();
}
 
void loop() {
  WiFiClient* client = connect_to_host();
  if(has_client(client)){
    int pressed_button = get_pressed_button();
    client->write('A');
    Serial.println("...TCP message fired!");
    delay(200);
    while(client->available()){
      int enemy_target_id = client->read();
      if(did_hit(enemy_target_id)){
        lower_hp();
      }
    }
    client->stop();
    delay(2000);
  }
}
