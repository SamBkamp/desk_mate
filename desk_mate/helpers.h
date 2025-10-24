//helper functions
#ifndef DESK_MATE_HELPERS
#define DESK_MATE_HELPERS

typedef struct{
  uint8_t requests;
  uint8_t requests_max;
  char endpoint[32];
  char top_row[17];
  char bottom_row[17];
}slide;


void blink(){
  for(int i = 0; i < 10; i++){
    digitalWrite(D10, HIGH);
    delay(200);
    digitalWrite(D10, LOW);
    delay(200);
  }
}


void init_lcd_screen(){
  pin_init(); //screen pin init
  function_set(); //screen function set
  display_init(1, 0, 0); //screen display init
  entry_mode(1, 0); //screen set entry mode
}


#endif
