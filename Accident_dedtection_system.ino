#include "TinyGPS++.h"
#include "SoftwareSerial.h"

SoftwareSerial GPRS(2,3);
SoftwareSerial serial_connection(8, 9); // RX=pin 8, TX=pin 9 for GPS
TinyGPSPlus gps; // GPS object to handle NMEA data

float latitude = 0; 
float longitude = 0;                       
int Contrast = 20;

#define x A1
#define y A2
#define z A3
int xsample = 0;
int ysample = 0;
int zsample = 0;
#define samples 10
#define minVal -50
#define MaxVal 50

void setup() {
  Serial.begin(9600); // Serial monitor
  serial_connection.begin(9600); // GPS module
  GPRS.begin(9600); // SIM900 module
  Serial.println("Successfully Initialized...");
  Serial.println("GPS Start");

  for(int i = 0; i < samples; i++) {
    xsample += analogRead(x);
    ysample += analogRead(y);
    zsample += analogRead(z);
  }

  xsample /= samples;
  ysample /= samples;
  zsample /= samples;
  
  delay(1000);
}

void loop() {
  while(serial_connection.available()) {
    gps.encode(serial_connection.read()); // Read GPS data
  }

  if(gps.location.isUpdated()) {
    latitude = gps.location.lat();
    longitude = gps.location.lng();
    Serial.print("Latitude: "); 
    Serial.println(latitude, 6);
    Serial.print("Longitude: ");
    Serial.println(longitude, 6);
    delay(1000);
  }

  int value1 = analogRead(x);
  int value2 = analogRead(y);
  int value3 = analogRead(z);

  int xValue = xsample - value1;
  int yValue = ysample - value2;
  int zValue = zsample - value3;

  Serial.print("x = ");
  Serial.println(xValue);
  Serial.print("y = ");
  Serial.println(yValue);
  Serial.print("z = ");
  Serial.println(zValue);
  delay(1000);

  if (xValue < minVal || xValue > MaxVal || yValue < minVal || yValue > MaxVal || zValue < minVal || zValue > MaxVal) {
    for (int i = 0; i < 5; i++) {
      // Make a call
      Serial.println("Calling...");
      GPRS.print("ATD+Replace with emergency number;\r"); // Replace with emergency contact number
      Serial.println("Dialing...");
      delay(10000); // Wait for call connection
      Serial.println("Disconnecting...");
      GPRS.print("ATH\r"); // End call

      // Send SMS with location
      sendAlert();
    }
  }
}

void sendAlert() {
  String sms_data;
  sms_data = "Accident Alert!!\r";
  sms_data += "http://maps.google.com/maps?q=loc:";
  sms_data += String(latitude, 6) + "," + String(longitude, 6);

  GPRS.println("AT+CMGF=1"); // Set SMS mode to text
  delay(1000);
  GPRS.print("AT+CMGS=\"+91............\"\r"); // Replace with the emergency contact number
  delay(1000);
  GPRS.print(sms_data);
  GPRS.write(26); // ASCII code for Ctrl+Z to send the SMS
  delay(1000);
}
