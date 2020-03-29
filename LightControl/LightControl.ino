#include <Wire.h>
#include <SeeedOLED.h>
#include "DHT.h" //Required for temperature/humidity sensor

#define DHTPIN A0     // data pin for temperature/humidity sensor (DHT)
#define DHTTYPE DHT11   // DHT 11 

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  //Setup required for (at least) the display.
  Wire.begin();
  SeeedOled.init();  //initialze SEEED OLED display
  DDRB |= 0x21;
  PORTB |= 0x21;

  //setup for temperature/humidity sensor
  dht.begin();

  //Debugging setup
  Serial.begin(9600);

  //Show something on the display to show it's working.
  //This can be deleted at a later point.
  SeeedOled.clearDisplay();          //clear the screen and set start position to top left corner
  SeeedOled.setNormalDisplay();      //Set display to normal mode (i.e non-inverse mode)
  SeeedOled.setPageMode();           //Set addressing mode to Page Mode
  SeeedOled.setTextXY(0, 0);         //Set the cursor to Xth Page, Yth Column
  SeeedOled.putString("Screen on"); //Print the String
}

void loop() {
  displayTemperature(getTemperature());
  getHumidity();
}

//Get the Temperature from the DHT and send it to the OLED display
void displayTemperature(float t)
{
  //SeeedOled.clearDisplay();          //clear the screen and set start position to top left corner
  //SeeedOled.setNormalDisplay();      //Set display to normal mode (i.e non-inverse mode)
  SeeedOled.setPageMode();           //Set addressing mode to Page Mode
  SeeedOled.setTextXY(0, 0);         //Set the cursor to Xth Page, Yth Column
  SeeedOled.putString("Temperature: "); //Print the String
  SeeedOled.putNumber(t);
}

//Get the temperature from the DHT, send it to the serial monitor, and return the value
float getTemperature()
{
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float t = dht.readTemperature();

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t))
  {
    Serial.println("Failed to read from DHT");
  }
  else
  {
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println(" *C");
  }
  return t;
}

//Get the humidity from the DHT, send it to the serial monitor, and return the value
float getHumidity()
{
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(h))
  {
    Serial.println("Failed to read from DHT");
  }
  else
  {
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.println(" %");
  }
  return h;
}
