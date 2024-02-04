/*Weather monitoring system with the New Blynk app
   Home Page
*/

#define BLYNK_TEMPLATE_ID "TMPL6BUm0uhjn"
#define BLYNK_TEMPLATE_NAME "Weather Monitoring System"
#define BLYNK_AUTH_TOKEN "BR2bXQCXGv1wNnZymHhpvK48XHEfJKI-"

//Include the library files
#include <LiquidCrystal_I2C.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <SFE_BMP180.h>

//Initialize the LCD display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Create an object for the BMP180 sensor
SFE_BMP180 bmp;

char auth[] = BLYNK_AUTH_TOKEN;//Enter your Auth token
char ssid[] = "White House";//Enter your WIFI name
char pass[] = "12345678";//Enter your WIFI password

DHT dht(D3, DHT11);//(DHT sensor pin,sensor type)
BlynkTimer timer;

//Define Rain and LDR pins
#define rain A0
#define light D0

//Create three variables for pressure
double T, P;
char status;

//=====================Flame======================================
int flame_sensor_pin = D7;
int buzzer_pin = D6;
int red_light = D5;
int green_led = D8;
int flame_pin = HIGH;
//=====================Flame======================================

void setup() {
  lcd.clear();
  Serial.begin(9600);
  bmp.begin();
  lcd.backlight();
  lcd.begin(16,2);
  lcd.setContrast(0);
  pinMode(light, INPUT);
  //===================Flame=====================================
  pinMode(red_light, OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(flame_sensor_pin, INPUT);
  pinMode(buzzer_pin, OUTPUT);
  //===================Flame=====================================
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  dht.begin();

  lcd.setCursor(0, 0);
  lcd.print("Weather Monitor");
  lcd.setCursor(4, 1);
  lcd.print("System");
  delay(4000);
  lcd.clear();

  //Call the functions
  timer.setInterval(20L, DHT11sensor);
  timer.setInterval(20L, rainSensor);
  timer.setInterval(20L, pressure);
  timer.setInterval(20L, LDRsensor);
}

//Get the DHT11 sensor values
void DHT11sensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(t);

  lcd.setCursor(8, 0);
  lcd.print("H:");
  lcd.print(h);

}

//Get the rain sensor values
void rainSensor() {
  int value = analogRead(rain);
  value = map(value, 0, 1024, 0, 100);
  Blynk.virtualWrite(V2, value);

  lcd.setCursor(0, 1);
  lcd.print("R:");
  lcd.print(value);
  lcd.print(" ");
}

//Get the pressure values
void pressure() {
  status =  bmp.startTemperature();
  if (status != 0) {
    delay(status);
    status = bmp.getTemperature(T);

    status = bmp.startPressure(3);// 0 to 3
    if (status != 0) {
      delay(status);
      status = bmp.getPressure(P, T);
      if (status != 0) {

      }
    }
  }
  Blynk.virtualWrite(V3, P);
  lcd.setCursor(8, 1);
  lcd.print("P:");
  lcd.print(P);
}

//Get the LDR sensor values
void LDRsensor() {
  bool value = digitalRead(light);
  if (value == 0) {
    WidgetLED LED(V4);
    LED.on();
  } else {
    WidgetLED LED(V4);
    LED.off();
  }

}

void loop() {
  Blynk.run();//Run the Blynk library
  //=====================Flame==================================================
  Blynk.virtualWrite(V10, LOW);
  flame_pin = digitalRead(flame_sensor_pin);
  if (flame_pin == HIGH)
  {
    Serial.println("FLAME!, FLAME!, FLAME!");
    
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Flame!  Flame!");
    digitalWrite(green_led, LOW);
    Blynk.virtualWrite(V8, LOW);
    red_light_warning();
  }
  else
  {
    Serial.println("No Flame");
    digitalWrite(red_light, LOW);
    digitalWrite(green_led, HIGH);
    Blynk.virtualWrite(V8, HIGH);
    digitalWrite(buzzer_pin, LOW);
    Blynk.virtualWrite(V9, LOW);
    Blynk.virtualWrite(V10, LOW);
  }
//=======================Flame==================================================
  timer.run();//Run the Blynk timer
}

void red_light_warning()
{
  digitalWrite(red_light, HIGH);
  Blynk.virtualWrite(V7, HIGH);
  digitalWrite(buzzer_pin, LOW);
  Blynk.virtualWrite(V9, LOW);
  Blynk.virtualWrite(V10, HIGH);
  delay(200);

  digitalWrite(red_light, LOW);
  Blynk.virtualWrite(V7, LOW);
  digitalWrite(buzzer_pin, HIGH);
  Blynk.virtualWrite(V9, HIGH);
  Blynk.virtualWrite(V10, LOW);
  delay(200);
}