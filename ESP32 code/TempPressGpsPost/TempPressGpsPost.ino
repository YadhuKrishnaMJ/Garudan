#include <TinyGPS.h> // Gps Library
#include <WiFi.h> //Wifi Library for local Demo connection
#include <HTTPClient.h>  //Http Module for API connection
#include <WiFiClientSecure.h> //Https module for Secure API Connection
#include <Wire.h>
#include "SparkFunBME280.h"   //Zip Library for GY-BM ME/PM 280
BME280 mySensorA; //Uses default I2C address 0x77
BME280 mySensorB; //Uses I2C address 0x76 (jumper closed)

//Wifi Connection(For Local Demo)
const char* ssid = "psvm";
const char* password = "password";

//GPS and HTTP initialization
TinyGPS gps;  
WiFiClientSecure client;  // Use WiFiClientSecure for HTTPS
HardwareSerial gpsSerial(1); 

void setup()
{
  Serial.begin(115200); //Serial Baud rate

  //GPS and Wifi Ready State
  gpsSerial.begin(9600, SERIAL_8N1, 33, 32);
  WiFi.begin(ssid, password);

  //Wifi Connection procedure
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  //Disable certificates for Testing Purpose
  client.setInsecure();

  //I2C connection for Temperature/Pressure Sensor
  Wire.begin();
  mySensorA.setI2CAddress(0x77); //The default for the SparkFun Environmental Combo board is 0x77 (jumper open).
  //If you close the jumper it is 0x76
  //The I2C address must be set before .begin() otherwise the cal values will fail to load.
  if(mySensorA.beginI2C() == false) Serial.println("Sensor A connect failed");
  mySensorB.setI2CAddress(0x76); //Connect to a second sensor
  if(mySensorB.beginI2C() == false) Serial.println("Sensor B connect failed");
}

void loop()
{ 
  //Global Variables for GPS data
  float latitude, longitude;
  unsigned long fix_age;
  //Gps Data Retrieval
  if (gpsSerial.available()) {
    //Parse and store GPS data
    gps.f_get_position(&latitude, &longitude, &fix_age);
  }
  
  //Store Pressure
  float pressure = mySensorB.readFloatPressure();

  //Store Temp in C and F
  float tempC = mySensorB.readTempC();
  float tempF = mySensorB.readTempF();

  Serial.print(" Pressure: ");
  Serial.print(pressure, 0);
  Serial.print(" Temp: ");
  Serial.print(tempC, 2);
  Serial.print("C ");
  Serial.print(tempF, 2);
  Serial.print("F ");
  Serial.println();

  /*....................................................................................
  HTTP post function to send JSON data to Express Server
  ......................................................................................*/

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    // Use the correct protocol (http:// or https://) based on your server setup
    String serverUrl = "http://192.168.1.100:3000/api/gps-data";
    http.begin(client, serverUrl);  // Begin HTTP connection to server
    http.addHeader("Content-Type", "application/json");  // Set HTTP headers
    http.setTimeout(10000);  // Set timeout to 10 seconds (adjust as needed)

    // Prepare JSON data with latitude, longitude, temperature, and pressure
    String httpRequestData = "{\"latitude\":\"" + String(latitude, 6) + "\",\"longitude\":\"" + String(longitude, 6) + "\",\"temperatureC\":\"" + String(tempC, 2) + "\",\"temperatureF\":\"" + String(tempF, 2) + "\",\"pressure\":\"" + String(pressure, 0) + "\"}";

    // Send HTTP POST request with JSON data
    int httpResponseCode = http.POST(httpRequestData);

    // Check for redirection (if necessary, handle it)
    if (httpResponseCode == HTTP_CODE_TEMPORARY_REDIRECT) {
      String newUrl = http.header("Location");  // Get redirected URL from the 'Location' header
      Serial.print("Redirected to: ");
      Serial.println(newUrl);
      http.end();  // Close current connection

      // Begin new connection to the redirected URL
      http.begin(client, newUrl);
      http.addHeader("Content-Type", "application/json");  // Ensure headers are set for new request
      httpResponseCode = http.POST(httpRequestData);  // Send POST request to new URL
    }

    // Process HTTP response
    if (httpResponseCode > 0) {
      String response = http.getString();  // Get response from server
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);  // Print HTTP response code
      Serial.println(response);  // Print server response
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(http.errorToString(httpResponseCode).c_str());  // Print HTTP error code if POST request fails
    }

    http.end();  // End HTTP connection
    delay(15000); // Delay for 10 seconds before sending the next request
  }
}
