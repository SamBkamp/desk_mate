#include <screen_io.h>
#include "WifiCredentials.h"
#include "helpers.h"
#include <TimeLib.h>
#include <WiFi.h>

#define RETRIES_MAX 30
#define PORT 8080

WiFiClient client; //yucky OOP
slide slides[2];
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
  if(!client.connect(base, PORT)){ //if cannot connect to server
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
    format_slide(1, construction);
  }else{
    format_slide(0, construction);
  }
  if(pre_connection == 0){
    digitalWrite(D10, LOW);
    WiFi.disconnect();
  } //only disconnect wifi if this function connected it
}

void setup() {
  //init slides
  slides[0].requests = 0;
  *slides[0].top_row = 0;
  *slides[0].bottom_row = 0;
  slides[1].requests = 10; //stagger second slide
  *slides[1].top_row = 0;
  *slides[1].bottom_row = 0;

  //init lcd and wifi led
  pinMode(D10, OUTPUT); //wifi LED
  digitalWrite(D10, LOW);
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
  if(slides[1].requests > 90){
    make_req("/gold", 0);
    slides[1].requests = 0;
  }
  slides[1].requests++;
  clear_display();
  strcpy(buff, slides[1].top_row);
  screen_put_string(buff);
  set_cursor(0, 1);
  strcpy(buff, slides[1].bottom_row);
  screen_put_string(buff);

  delay(7000);

  //print stock quotes
  if(slides[0].requests > 60){
    make_req("/q", 0);
    slides[0].requests = 0;
  }
  slides[0].requests++;
  clear_display();
  strcpy(buff, slides[0].top_row);
  screen_put_string(buff);
  set_cursor(0, 1);
  strcpy(buff, slides[0].bottom_row);
  screen_put_string(buff);

  delay(7000);
}
