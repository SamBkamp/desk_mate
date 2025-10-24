#include <screen_io.h>
#include "WifiCredentials.h"
#include "helpers.h"
#include <TimeLib.h>
#include <WiFi.h>

#define RETRIES_MAX 30
#define PORT 8080
#define WIFI_LED D10
#define SLIDES_SPACE 2

WiFiClient client; //yucky OOP
slide slides[SLIDES_SPACE];
char buff[64];
char *dow[] = {"Sunday",
	      "Monday",
	      "Tuesday",
	      "Wednesday",
	      "Thursday",
	      "Friday",
	      "Saturday"
};


void format_slide(int slide_index, char* raw_data){
  char* delim = raw_data;
  while(*delim != '\n' && *delim != 0)
    delim++;

  if(*delim == '\n'){
    *delim = 0; //set newline to null terminator
    delim++; //delim points to second string
  }//if delim reached end of string, leave it pointing there

  sprintf(slides[slide_index].top_row, "%s", raw_data);
  sprintf(slides[slide_index].bottom_row, "%s", delim);
}

void connect_wifi(){
  clear_display();
  screen_put_string("Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED){
    delay(700);
  }
  digitalWrite(WIFI_LED, HIGH);
  clear_display();
  screen_put_string("Connected!");
}

void make_req(char* endpoint, int pre_connection){
  if(pre_connection == 0){ //connect to wifi if not already
    connect_wifi();
  }
  IPAddress base(192,168,50,196);
  if(!client.connect(base, PORT)){ //if cannot connect to server
    blink();
    if(pre_connection == 0){
      WiFi.disconnect();
      digitalWrite(WIFI_LED, LOW);
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
    if(pre_connection == 0){
      digitalWrite(WIFI_LED, LOW);
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
    format_slide(1, construction);
  }else{
    format_slide(0, construction);
  }
  if(pre_connection == 0){
    digitalWrite(WIFI_LED, LOW);
    WiFi.disconnect();
  } //only disconnect wifi if this function connected it
}

void setup() {
  //init slides
  strcpy(slides[1].endpoint, "/q");
  slides[0].requests = 0;
  slides[0].requests_max = 60;
  *slides[0].top_row = 0;
  *slides[0].bottom_row = 0;

  strcpy(slides[1].endpoint, "/gold");
  slides[1].requests = 10; //stagger second slide
  slides[1].requests_max = 90;
  *slides[1].top_row = 0;
  *slides[1].bottom_row = 0;

  //init lcd and wifi led
  pinMode(WIFI_LED, OUTPUT); //wifi LED
  digitalWrite(WIFI_LED, LOW);
  init_lcd_screen();
  clear_display();

  setTime(0);

  connect_wifi(); //keep singular wifi connection open for inital data requests
  make_req("HIII", 1); //time data
  delay(500);
  make_req("/gold", 1);
  delay(500);
  make_req("/q", 1);
  WiFi.disconnect();
  digitalWrite(WIFI_LED, LOW);
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

  for(int i = 0; i < SLIDES_SPACE; i++){
    if(slides[i].requests > slides[i].requests_max){
      make_req(slides[i].endpoint, 0);
      slides[i].requests = 0;
    }
    slides[i].requests++;
    clear_display();
    strcpy(buff, slides[i].top_row);
    screen_put_string(buff);
    set_cursor(0, 1);
    strcpy(buff, slides[i].bottom_row);
    screen_put_string(buff);
    delay(7000);
  }
}
