#include <ESP8266WiFi.h>

#define RST_BTN 6
#define LOAD 1
#define H1 0
#define H2 4
#define H3 5
#define B1 2
#define B2 3

#define SSID "artex11"
#define PW "getmenow11"
#define HOST "192.168.4.1"

#define MAX_WAIT_CONNECT 5000

// Connection
const int port = 80;

//Game
const int health_leds[3] = {H1,H2,H3};
const int base_leds[2] = {B1,B2};
int health = 3;

void setup_connection(){
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PW);
  Serial.println("Connecting to AP");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    load_blinker(200);
  }
  Serial.println(" connected");
}

boolean is_connected(){
  return WiFi.status() == WL_CONNECTED;
}

void load_blinker(int delaytime){
  blink(LOAD);
  delay(delaytime);
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

void poll_restart_button(){
  if(digitalRead(RST_BTN == HIGH)){
    restart_game();
  }
}

void restart_game(){
  init_pins();
  init_game();
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

void player_loose(){
  Serial.println("-> player dead");
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
  return 0;
}

WiFiClient* connect_to_host(){
  WiFiClient* client = (WiFiClient*) malloc(sizeof(WiFiClient));    
  Serial.println("Connecting to host");
  if(!client->connect(HOST, port)){
    Serial.println("...connection failed!");
    Serial.printf("Retrying in %d seconds...\n", MAX_WAIT_CONNECT);
    delay(MAX_WAIT_CONNECT);
    return NULL;  
  }
  return client;
}

void player_action(WiFiClient* client){
    int pressed_button = get_pressed_button();
    client->write(pressed_button);
    Serial.println("...TCP message fired!");
    delay(200);
}

void get_enemy_action(WiFiClient* client){
  int enemy_target_id = client->read();
  if(did_hit(enemy_target_id)){
    lower_hp();
    if(is_dead()){
      player_loose();
    }
  }
}

void game_tick(){
  WiFiClient* client = connect_to_host();
  if(has_client(client)){
    player_action(client);
    while(client->available()){
      get_enemy_action(client);
    }
    client->stop(); // disconnect client to cleanup
    // check if restart has been pressed and reset game
    poll_restart_button();
  }
}

void setup() {
  init_pins();
  setup_connection();
  init_game();
}
 
void loop() {
  if(is_connected()){
    game_tick();
  }else{
    setup_connection();
  }
}
