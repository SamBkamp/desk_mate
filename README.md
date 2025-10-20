# Desk Mate

A small desk helper that displays information for you at your desk. It gets its information from a server it queries in regular intervals.

Hardware:
- Arduino Nano esp32 (though any devboard with an internet connection will do)
- 16x2 LCD screen
- Breadbordes, wires, various bits and bobs

The Desk Mate connects to a server periodically to request data. The server program is [another project](https://github.com/SamBkamp/c-server) I wrote. Though it can be easily changed in the code as the arduino does very little processing of the data.


Hardware wiring diagram:
![hardware diagram](circuit_image.png)

Wiring connections from LCD to arduino:

|  LCD pins     | Arduino pins  |
| ------------- | ------------- |
| E             | TX1/D1        |
| RS            | RX0/D0        |
| D0            | D2            |
| D1            | D3            |
| D2            | D4            |
| D3            | D5            |
| D4            | D6            |
| D5            | D7            |
| D6            | D8            |
| D7            | D9            |

You can change the LCD/Arduino pin mappings at the top of `screen_io.c`. There is also an optional WiFi LED connected to Arduino pin D10, this light signals wether or not the WiFi functionality is currently enabled.

### Power

I am running this circuit at 7.5v. In my experience, 5v and even 6v supplies risk a brownout when powering both the LCD and the WiFi functionality. I'm not an engineer but I bet you could get away with 5/6v if you stick a capacitor between Vdd and Vss.