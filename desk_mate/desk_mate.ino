#include <screen_io.h>
#include "WifiCredentials.h"
#include <TimeLib.h>
#include <WiFi.h>

#define RETRIES_MAX 30

WiFiClient client; //yucky OOP
int port = 8080;

int data_requests = 0;
char buff[64];
char gold[64];
char tem[64];
char *dow[] = {"Sunday",
	      "Monday",
	      "Tuesday",
	      "Wednesday",
	      "Thursday",
	      "Friday",
	      "Saturday"
};


void blink(){
  for(int i = 0; i < 10; i++){
    digitalWrite(D10, HIGH);
    delay(200);
    digitalWrite(D10, LOW);
    delay(200);
  }
}

void connect_wifi(){
  digitalWrite(D10, HIGH);
  clear_display();
  screen_put_string("Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED){
    delay(700);
  }
  clear_display();
  screen_put_string("connected!");
}

void make_req(char* endpoint, int pre_connection){
  if(pre_connection == 0){ //connect to wifi if not already
    connect_wifi();
  }
  IPAddress base(192,168,50,196);
  if(!client.connect(base, port)){ //if cannot connect to server
    blink();
    if(pre_connection == 0){
      WiFi.disconnect();
      digitalWrite(D10, LOW);
    }
    return;
  }

  uint8_t retries = 0; //client.available retries
  client.println(endpoint); //send endpoint data to server
  while(!client.available() && retries < RETRIES_MAX){ //check if ready to recieve bytes
    delay(20);
    retries++;
  }

  if(retries >= RETRIES_MAX){ //client.available timeout
    client.stop();
    if(pre_connection == 1){
      digitalWrite(D10, LOW);
      WiFi.disconnect();
    } //only disconnect wifi if this function connected it
    return;
  }

  uint8_t index = 0;
  char construction[64];
  while(client.available() && index < 64)
    construction[index++] = client.read(); //read all the data from server

  client.flush();
  client.stop();
  construction[index] = 0;
  if(strcmp(endpoint, "HIII")==0){
    setTime(strtoul(construction, NULL, 10));
  }else if(strcmp(endpoint, "/gold") == 0){
    strncpy(gold, construction, 64);
  }else{
    strncpy(tem, construction, 64);
  }

  if(pre_connection == 0){
    digitalWrite(D10, LOW);
    WiFi.disconnect();
  } //only disconnect wifi if this function connected it
}

void init_lcd_screen(){
  pin_init(); //screen pin init
  function_set(); //screen function set
  display_init(1, 0, 0); //screen display init
  entry_mode(1, 0); //screen set entry mode
}

void setup() {
  strncpy(gold, "N/A", 4);
  pinMode(D10, OUTPUT); //wifi LED
  digitalWrite(D10, LOW);
  init_lcd_screen();

  setTime(0);
  clear_display();

  connect_wifi(); //keep singular wifi connection open for inital data requests
  make_req("HIII", 1);
  delay(500);
  make_req("/gold", 1);
  delay(500);
  make_req("/q", 1);
  WiFi.disconnect();
  digitalWrite(D10, LOW);
  //unset wifi LED
}

void loop() {
  //print date time
  clear_display();
  time_t t = now();
  sprintf(buff, "%s %d:%02d", dow[weekday(t)-1], hour(t), minute(t));
  screen_put_string(buff);
  set_cursor(0, 1);
  sprintf(buff, "%02d.%02d.%02d", day(t), month(t), year(t));
  screen_put_string(buff);

  delay(7000);

  //print gold quotes
  if(data_requests > 90){
    make_req("/gold");
    data_requests = 0;
  }
  data_requests++;
  clear_display();
  sprintf(buff, "GOLD: $%s", gold);
  screen_put_string(buff);

  delay(7000);
}
