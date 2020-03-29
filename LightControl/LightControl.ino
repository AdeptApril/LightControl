#include <Wire.h>
#include <SeeedOLED.h>

void setup() {
  //Setup required for (at least) the display.
  Wire.begin();
  SeeedOled.init();  //initialze SEEED OLED display
  DDRB|=0x21;        
  PORTB |= 0x21;

  //Show something on the display to show it's working.
  //This can be deleted at a later point.
  SeeedOled.clearDisplay();          //clear the screen and set start position to top left corner
  SeeedOled.setNormalDisplay();      //Set display to normal mode (i.e non-inverse mode)
  SeeedOled.setPageMode();           //Set addressing mode to Page Mode
  SeeedOled.setTextXY(0,0);          //Set the cursor to Xth Page, Yth Column  
  SeeedOled.putString("Screen on"); //Print the String
}

void loop() {
  // put your main code here, to run repeatedly:

}
