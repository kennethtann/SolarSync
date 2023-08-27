#define BLYNK_TEMPLATE_ID "TMPL6Kk6nehxD"
#define BLYNK_TEMPLATE_NAME "Sun Tracking Solar Panel"
#define BLYNK_FIRMWARE_VERSION "0.1.0"
#define BLYNK_PRINT Serial
#include "BlynkEdgent.h"

//Servo
#include <ESP32Servo.h>

//LED Matrix
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

//Temp Hum DHT11
#include "DHT.h"
#define DHTPIN 17
#define DHTTYPE DHT11




Servo servo1;
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4

#define CLK_PIN 12   // or SCK
#define DATA_PIN 11  // or MOSI
#define CS_PIN 10    // or SS

//MD_Parola Display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
MD_Parola Display = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

int initial_servopos = 90;
int LDR1 = 15;
int LDR2 = 16;
int error = 5;
int servopin = 7;

DHT dht(DHTPIN, DHTTYPE);
float t, h;

//Voltage Reading
const int Analog_channel_pin = 18;
int ADC_VALUE = 0;
int voltage_value = 0;

//Blynk Manual Servo Control
BLYNK_WRITE(V0) {
  int s0 = param.asInt();
  servo1.write(s0);
  Blynk.virtualWrite(V1, s0);
  delay(2000);
}

//Blynk DHT11 Values
void sendSensor() {
  h = dht.readHumidity();
  t = dht.readTemperature();
  Blynk.virtualWrite(V2, h);
  Blynk.virtualWrite(V3, t);
}

void sendVoltage() {
  ADC_VALUE = analogRead(Analog_channel_pin);
  voltage_value = (ADC_VALUE * 3.3) / (4095);
  Blynk.virtualWrite(V4, voltage_value);
}


void setup() {
  //Blynk
  BlynkEdgent.begin();
  //LED Matrix
  Display.begin();
  Display.setIntensity(0);
  Display.displayClear();
  Display.displayScroll("solarsync", PA_RIGHT, PA_SCROLL_LEFT, 30);

  //Servo LDR
  servo1.attach(servopin);
  pinMode(LDR1, INPUT);
  pinMode(LDR2, INPUT);
  servo1.write(initial_servopos);
  delay(2000);

  dht.begin();

  timer.setInterval(1000L, sendSensor);
  timer.setInterval(1000L, sendVoltage);
}


void loop() {

  int R1 = analogRead(LDR1);  //read LDR 1 value
  int R2 = analogRead(LDR2);  //read LDR 2 value
  int a = abs(R1 - R2);       //calculate difference btwn LDRs
  int b = abs(R2 - R1);       //calculate difference btwn LDRs

  if ((a <= error) || (b <= error)) {

  } else {
    if (R1 > R2) {
      initial_servopos = --initial_servopos;
      if(initial_servopos <= 0){
        initial_servopos = 0;
      }
    }
    if (R1 < R2) {
      initial_servopos = ++initial_servopos;
      if(initial_servopos >= 180){
        initial_servopos = 180;
      }
    }
  }
  servo1.write(initial_servopos);



  delay(100);

  if (Display.displayAnimate()) {
    Display.displayReset();
  }

  BlynkEdgent.run();
  timer.run();
}
