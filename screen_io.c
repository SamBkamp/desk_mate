#include <stdint.h>

#define SCREEN_E D1
#define SCREEN_RS D0
#define SCREEN_D0 D2
#define SCREEN_D1 D3
#define SCREEN_D2 D4
#define SCREEN_D3 D5
#define SCREEN_D4 D6
#define SCREEN_D5 D7
#define SCREEN_D6 D8
#define SCREEN_D7 D9
#define PINS 10 //total amount of pins

uint16_t RS_OFFSET = 1 << 10;

uint8_t d_pin_list[] = {SCREEN_D0, SCREEN_D1, SCREEN_D2, SCREEN_D3, SCREEN_D4, SCREEN_D5, SCREEN_D6, SCREEN_D7};

void eight_bit_to_out(uint8_t d){
  for(uint8_t i = 0; i < 8; i++){
    uint8_t l = (d>>i) & 0x1;
    digitalWrite(d_pin_list[i], l==1?HIGH:LOW);
  }
}

void all_zero(){
  eight_bit_to_out(0);
  digitalWrite(SCREEN_RS, LOW);
}

void toggle_enable(){
  digitalWrite(SCREEN_E, HIGH);
  delayMicroseconds(5);
  digitalWrite(SCREEN_E, LOW);
  delayMicroseconds(300);
}

void screen_put_char(uint8_t c){
  digitalWrite(SCREEN_RS, HIGH);
  eight_bit_to_out(c);
  toggle_enable();
}

void function_set(){
  digitalWrite(SCREEN_D5, HIGH); //function set
  digitalWrite(SCREEN_D4, HIGH);
  digitalWrite(SCREEN_D3, HIGH);
  toggle_enable();
}

void pin_init(){
  pinMode(SCREEN_E , OUTPUT);
  pinMode(SCREEN_RS, OUTPUT);
  pinMode(SCREEN_D0, OUTPUT);
  pinMode(SCREEN_D1, OUTPUT);
  pinMode(SCREEN_D2, OUTPUT);
  pinMode(SCREEN_D3, OUTPUT);
  pinMode(SCREEN_D4, OUTPUT);
  pinMode(SCREEN_D5, OUTPUT);
  pinMode(SCREEN_D6, OUTPUT);
  pinMode(SCREEN_D7, OUTPUT);
  all_zero();
  digitalWrite(SCREEN_E, LOW);
  digitalWrite(SCREEN_RS, LOW);
  delay(500);
}

void display_init(int display, int cursor, int blink){
  all_zero();
  digitalWrite(SCREEN_D3, HIGH); //display on off ctrl
  digitalWrite(SCREEN_D2, display>0?HIGH:LOW); //display on/off
  digitalWrite(SCREEN_D1, cursor>0?HIGH:LOW); //cursor on/off
  digitalWrite(SCREEN_D0, blink>0?HIGH:LOW); //blink on/off
  toggle_enable();
}

void cursor_home(){
  all_zero();
  digitalWrite(SCREEN_D1, HIGH);
  toggle_enable();
  delay(2); //this one takes a while
}

void set_cursor(int col, int row){
  cursor_home();
  digitalWrite(SCREEN_D1, LOW);
  for(int i = 0; i < (col+(row*40)); i++){
    digitalWrite(SCREEN_D4, HIGH);
    digitalWrite(SCREEN_D3, LOW);
    digitalWrite(SCREEN_D2, HIGH);
    toggle_enable();
    delayMicroseconds(50);
  }  
}

void entry_mode(uint8_t dir, uint8_t shift){
  all_zero();
  digitalWrite(SCREEN_D2, HIGH);  //entry mode set
  digitalWrite(SCREEN_D1, dir>0?HIGH:LOW); //direction
  digitalWrite(SCREEN_D0, shift>0?HIGH:LOW); //direction
  toggle_enable();
}
void clear_display(){
  all_zero();
  digitalWrite(SCREEN_D0, HIGH); //clear display
  toggle_enable();
  delay(5);
}

void screen_put_string(char* s){
  for(int i = 0; s[i] != 0; i++){
    screen_put_char(s[i]);
  }
}
