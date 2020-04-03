#include <Wire.h>
#include <SeeedOLED.h>
#include "DHT.h" //Required for temperature/humidity sensor
#include <Encoder.h> //For the encoder/dial
#include <TimerOne.h> //For the encoder/dial

#define TRUE 1
#define FALSE 0
#define DHTPIN A0     // data pin for temperature/humidity sensor (DHT)
#define DHTTYPE DHT11   // Type of DHT sensor. The company who makes the sensor has at least three models
//Encoder pins are 2 and 3. The following are used here, but the pins seem to be set in the encoder.cpp file:
#define EncoderPin1     2
#define EncoderPin2     3
#define BUTTONPIN 4

/*enum EncoderDir
  {
    Anticlockwise = 0,
    Clockwise     = 1,
  };
  typedef enum EncoderDir EncodedirStatus;
  EncodedirStatus EncoderRotateDir;*/

//For the modes section, the defines could be replaced with Enums (Though not sure, since it's set outside this file)
//Number of available modes
#define NUMBER_OF_MAIN_OPTIONS 3
#define NUMBER_OF_COLORS_OPTIONS 5
#define NUMBER_OF_SEQUENCE_OPTIONS 1

//Main modes
#define MAIN_MODE 0
#define COLORS_MODE 1
#define SEQUENCES_MODE 2
#define LIGHT_OFF 3

//Color modes
#define RED_MODE 0
#define GREEN_MODE 1
#define BLUE_MODE 2
#define BRIGHTNESS_MODE 3
#define BACK_FROM_COLORS 4

//Sequence modes
#define BACK_FROM_SEQUENCES 0

DHT dht(DHTPIN, DHTTYPE);

char textLine[16]; //The OLED screen is 8 lines tall, 16 characters wide.
//char nextLine[53];
//sprintf(nextLine,"%s = %i", "id=1&type=moisture&unit=%&value=", buffer);
//Serial.println(nextLine);

int menuItem = MAIN_MODE;
int secondaryMenuItem = 0;

int buttonWasPushed = FALSE;
int updateMenu = TRUE;

void setup() {
  //Setup required for (at least) the display.
  Wire.begin();
  SeeedOled.init();  //initialze SEEED OLED display
  DDRB |= 0x21;
  PORTB |= 0x21;

  //setup for temperature/humidity sensor
  dht.begin();

  //setup for the encoder/dial
  pinMode(EncoderPin1, INPUT);
  pinMode(EncoderPin2, INPUT);
  attachInterrupt(digitalPinToInterrupt(EncoderPin1), checkEncoderRotation, FALLING);
  encoder.Timer_init();

  //setup for the button
  pinMode(BUTTONPIN, INPUT);
  //It'd be nice to attach an interrupt, but the encoder seems to need one for each direction, and there's only two available on an Arduino UNO
  //attachInterrupt(digitalPinToInterrupt(BUTTONPIN), buttonPushed, RISING);

  //Debugging setup
  Serial.begin(9600);

  SeeedOled.clearDisplay();          //clear the screen and set start position to top left corner
  SeeedOled.setNormalDisplay();      //Set display to normal mode (i.e non-inverse mode)
  SeeedOled.setPageMode();           //Set addressing mode to Page Mode
}

void loop() {
  //Determine which portion of the menu system the user is in
  //Modes are setup in setup
  switch (menuItem) {
    case MAIN_MODE:
      modeMainMenu();
      break;
    case COLORS_MODE:
      //modeColors();
      break;
    case SEQUENCES_MODE:
      //modeSequences();
      break;
    case LIGHT_OFF:
      //lightOff();
      break;
    default:
      break;
  }
}

void modeMainMenu()
{
  //Instead of having a specific "displayMainMenu()", if I could have some sort of structure to pass, the method could be generalized.
  if (updateMenu == TRUE)
  {
    displayMainMenu();
    updateMenu = FALSE;
  }
}

void displayMainMenu()
{
  //SeeedOled.clearDisplay();
  SeeedOled.setTextXY(0, 2);
  SeeedOled.putString("Colors Mode");
  SeeedOled.setTextXY(1, 2);
  SeeedOled.putString("Sequences Mode");
  SeeedOled.setTextXY(2, 2);
  SeeedOled.putString("Turn Light Off");
  //Display something to show the current menu choice
  //First clear off any previous markers (could clearDisplay here also, but this seems less disruptive)
  for(int i = 0; i < NUMBER_OF_MAIN_OPTIONS; i++)
  {
    SeeedOled.setTextXY(i, 0);
    SeeedOled.putString(" ");
  }
  SeeedOled.setTextXY(secondaryMenuItem, 0);
  SeeedOled.putString(">"); //Print the String
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

void buttonPushed()
{
  Serial.println("Button Pushed");
  //delay(10) //Maybe add this if buttons being twitchy. Or possibly even longer. Not sure how quickly humans respond to seeing the screen change.
  buttonWasPushed = TRUE;
}

//When the interrupt triggers, say to update the menu, and adjust options as appropriate
void checkEncoderRotation()
{
  updateMenu = TRUE;
  if (encoder.rotate_flag == 1)
  {
    encoder.rotate_flag = 0;
    if (encoder.direct == 0)
    {
      Serial.println("counter-clockwise rotation detected");
      //Set secondaryMenuItem, depending on which menu
      secondaryMenuItem--;
      Serial.print("Secondary Menu Item # before switch: ");
      Serial.print(secondaryMenuItem);
      switch (menuItem) {
        case MAIN_MODE:
          if (secondaryMenuItem < 0)
            secondaryMenuItem = NUMBER_OF_MAIN_OPTIONS - 1;
          break;
        case COLORS_MODE:
          if (secondaryMenuItem < 0)
            secondaryMenuItem = NUMBER_OF_COLORS_OPTIONS - 1;
          break;
        case SEQUENCES_MODE:
          secondaryMenuItem = NUMBER_OF_SEQUENCE_OPTIONS - 1;
          break;
        default:
          secondaryMenuItem = 0;
          break;
      }
      Serial.print(" after switch: ");
      Serial.println(secondaryMenuItem);
      return;
    }
    else
    {
      Serial.println("clockwise rotation detected");
      //Set secondaryMenuItem, depending on which menu
      secondaryMenuItem++;
      switch (menuItem) {
        case MAIN_MODE:
          if (secondaryMenuItem >= NUMBER_OF_MAIN_OPTIONS)
            secondaryMenuItem =  0;
          break;
        case COLORS_MODE:
          if (secondaryMenuItem >= NUMBER_OF_COLORS_OPTIONS)
            secondaryMenuItem = 0;
          break;
        case SEQUENCES_MODE:
          if(secondaryMenuItem >= NUMBER_OF_SEQUENCE_OPTIONS)
            secondaryMenuItem = 0;
          break;
        default:
          secondaryMenuItem = 0;
          break;
      }
      return;
    }
  }
  return;// 0;
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
