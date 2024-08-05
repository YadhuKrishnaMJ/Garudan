#include <TinyGPS.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

const char* ssid = "psvm";
const char* password = "password";

TinyGPS gps;
WiFiClientSecure client;  // Use WiFiClientSecure for HTTPS
HardwareSerial gpsSerial(1); 

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600, SERIAL_8N1, 33, 32);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  // Disable certificate verification (for testing only)
  client.setInsecure();
}

void loop() {
  if (gpsSerial.available()) {
    float latitude, longitude;
    unsigned long fix_age;

    gps.f_get_position(&latitude, &longitude, &fix_age);  // Get current GPS position

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;

      // Use the correct protocol (http:// or https://) based on your server setup
      String serverUrl = "https://lemon-ghosts-repair.loca.lt/api/gps-data";
      http.begin(client, serverUrl);  // Begin HTTP connection to server
      http.addHeader("Content-Type", "application/json");  // Set HTTP headers
      http.setTimeout(10000);  // Set timeout to 10 seconds (adjust as needed)

      // Prepare JSON data with latitude and longitude
      String httpRequestData = "{\"latitude\":\"" + String(latitude, 6) + "\",\"longitude\":\"" + String(longitude, 6) + "\"}";

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
}
