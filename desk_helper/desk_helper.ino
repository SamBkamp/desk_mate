 #include <screen_io.h>
#include "WifiCredentials.h"
#include <TimeLib.h>
#include <WiFi.h>
WiFiClient client; //yucky OOP
int port = 8080;

int data_requests = 0;
char buff[64];
char gold[30];
char *dow[] = {"Sunday",
	      "Monday",
	      "Tuesday",
	      "Wednesday",
	      "Thursday",
	      "Friday",
	      "Saturday"
};


void make_req(char* endpoint){
  int pre_connection = 0;
  if(WiFi.status() != WL_CONNECTED){
    digitalWrite(D10, HIGH);
    pre_connection = 1;
    clear_display();
    screen_put_string("Connecting");
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED){
      delay(700);
    }
    clear_display();
  }
  IPAddress base(192,168,50,196);
  int res; //connection result
  res = client.connect(base, port);
  if(res){
    screen_put_string("success!");
    client.println(endpoint);
    while(!client.available()){ //check if ready to recieve bytes TODO: create an auto stop
      delay(20);
    }
    int index = 0;
    char construction[13];
    while(client.available()){
      construction[index++] = client.read();
    }
    client.flush();
    client.stop();
    construction[index] = 0;
    if(strcmp(endpoint, "HIII")==0){
      setTime(strtoul(construction, NULL, 10));
    }else{
      strcpy(gold, construction);
    }
    clear_display();
  }else{
    screen_put_string("failed!");
    delay(2000);
    clear_display();
  }
  if(pre_connection == 1){
    digitalWrite(D10, LOW);
    WiFi.disconnect();
  } //only disconnect wifi if this function connected it

}


void setup() {
  pinMode(D10, OUTPUT); //wifi LED
  digitalWrite(D10, LOW);
  pin_init();
  function_set();
  display_init(1, 0, 0);
  entry_mode(1, 0);

  setTime(0);
  clear_display();
  screen_put_string("Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED){
    delay(700);
  }
  digitalWrite(D10, HIGH); //set wifi LED
  clear_display();
  screen_put_string("Connected!");
  delay(500);
  clear_display();
  make_req("HIII");
  delay(500);
  make_req("/gold");
  delay(500);
  WiFi.disconnect();
  digitalWrite(D10, LOW);
  //unset wifi LED
}

void loop() {
  clear_display();
  time_t t = now();
  sprintf(buff, "%s %d:%02d", dow[weekday(t)-1], hour(t), minute(t));
  screen_put_string(buff);
  set_cursor(0, 1);
  sprintf(buff, "%02d.%02d.%02d", day(t), month(t), year(t));
  screen_put_string(buff);

  delay(7000);
  if(data_requests > 90){
    make_req("/gold");
    data_requests = 0;
  }
  data_requests++;
  clear_display();
  screen_put_string("GOLD:");
  set_cursor(0, 1);
  sprintf(buff, "$%s", gold);
  screen_put_string(buff);
  delay(7000);
}
