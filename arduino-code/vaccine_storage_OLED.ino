#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Define temperature threshold values
#define TEMP_THRESHOLD_HIGH 30.0  // Set your high temperature threshold here
#define TEMP_THRESHOLD_LOW  10.0  // Set your low temperature threshold here

// Data wire is connect to the Arduino digital pin 4
#define ONE_WIRE_BUS 4

int peltier = 3; //The N-Channel MOSFET is on digital pin 3
int power = 0; //Power level fro 0 to 99%
int peltier_level = map(power, 0, 99, 0, 255); //This is a value from 0 to 255 that actually controls the MOSFET

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

int buzzer = 8; //digital pin 8 for buzzer
int PinButton1 = 2; //digital pin 2 for increasing temperature button
int PinButton2 = 7; //digital pin 7 for decreasing temperature button
int temp_decrease =0;
int temp_increase =0;

void setup() {
  Serial.begin(115200);

  sensors.begin();
  pinMode(PinButton1, INPUT);
  pinMode(PinButton2, INPUT);
  pinMode(buzzer,OUTPUT);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);
}

void loop() {
  delay(5000);

  //read temperature
  sensors.requestTemperatures(); 
  float t = sensors.getTempCByIndex(0);
  Serial.print(sensors.getTempCByIndex(0));
  temp_decrease=digitalRead(PinButton2);
  temp_increase=digitalRead(PinButton1);
 
  if (isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
  }

  //clear display
  display.clearDisplay();

  // display temperature
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Temperature: ");
  display.setTextSize(2);
  display.setCursor(0,10);
  display.print(t);
  display.print(" ");
  display.setTextSize(1);
  display.cp437(true);
  display.write(167);
  display.setTextSize(2);
  display.print("C");
  
  if (t > TEMP_THRESHOLD_HIGH) {  //function unit including the buzzer and OLED display
    display.setTextSize(1);
    display.setCursor(0, 40);
    display.println("High temperature warning!");
    tone(buzzer, 1000); // tone() is the main function to use with a buzzer, it takes 2 or 3 parameteres (buzzer pin, sound frequency, duration)
    delay(1000);
    tone(buzzer, 1000); // You can also use noTone() to stop the sound it takes 1 parametere which is the buzzer pin
    delay(1000);
  } else if (t < TEMP_THRESHOLD_LOW) {
    display.setTextSize(1);
    display.setCursor(0, 40);
    display.println("Low temperature warning!");
    tone(buzzer, 1000); // tone() is the main function to use with a buzzer, it takes 2 or 3 parameteres (buzzer pin, sound frequency, duration)
    delay(1000);
    tone(buzzer, 2000); // You can also use noTone() to stop the sound it takes 1 parametere which is the buzzer pin
    delay(1000);
  }

   if(temp_increase == HIGH) //button is pressed
     power += 5;
  else if(temp_decrease == HIGH)  //peltier control unit with button for control
    power -= 5;

  if(power > 99) power = 99;
  if(power < 0) power = 0;
    peltier_level = map(power, 0, 99, 0, 255);
    
  analogWrite(peltier, peltier_level); //Write this new value out to the port  
  
  display.display(); 
}