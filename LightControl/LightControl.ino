#include <Wire.h>
#include <SeeedOLED.h>
#include "DHT.h" //Required for temperature/humidity sensor
#include <Encoder.h> //For the encoder/dial
#include <TimerOne.h> //For the encoder/dial

#define DHTPIN A0     // data pin for temperature/humidity sensor (DHT)
#define DHTTYPE DHT11   // Type of DHT sensor. The company who makes the sensor has at least three models
//Encoder pins are 2 and 3
#define BUTTONPIN 4

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  //Setup required for (at least) the display.
  Wire.begin();
  SeeedOled.init();  //initialze SEEED OLED display
  DDRB |= 0x21;
  PORTB |= 0x21;

  //setup for temperature/humidity sensor
  dht.begin();

  //setup for the encoder/dial
  encoder.Timer_init();

  //setup for the button
  pinMode(BUTTONPIN, INPUT);
  
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
  checkEncoderRotation();
  getHumidity();
  checkEncoderRotation();
  getButtonState();
}

//Returns whether the button is currently pressed or not
//Returns LOW (0) if not pressed
//returns HIGH (1) if pressed
int getButtonState()
{
  //Could get rid of the temp variable if no serial output needed
  Serial.print("Button State: ");
  int buttonState = digitalRead(BUTTONPIN);
  Serial.println(buttonState);
  return buttonState;
}

//Checks for rotation, and returns a number based on the result:
//Return 0 - when no rotation
//Return -1 - when last rotation was anti-clockwise
//Return 1 - when last rotation was clockwise
int checkEncoderRotation()
{
  if (encoder.rotate_flag == 1)
  {
    encoder.rotate_flag = 0;
    if (encoder.direct == 0)
    {
      Serial.println("counter-clockwise rotation detected");
      return -1;
    }
    else
    {
      Serial.println("clockwise rotation detected");
      return 1;
    }
  }
  return 0;
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
