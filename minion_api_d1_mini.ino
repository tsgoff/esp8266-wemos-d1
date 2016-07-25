/* WeMos D1 mini Monitoring Status

*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
extern "C" {
#include "user_interface.h"
}

// Listen for HTTP requests on standard port 80
ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

// include WiFi network from config.h
// const char* ssid     = "SSID";
// const char* password = "******************";
#include "config.h"


void handle_root() {
  String DOCUMENTROOT = "<html><head><title>Minion</title></head>";
  DOCUMENTROOT += "<body style='background-color:F5E050'>";
  DOCUMENTROOT += "<span style='display:block; width:100%; font-size:2em; font-family:Verdana; text-align:center'>Minion API</span><br/>";
  DOCUMENTROOT += "<a href='/v1/beedoo'>/v1/beedoo/auth/token</a><br/>";
  DOCUMENTROOT += "<a href='/v1/megafon'>/v1/megafon/auth/token</a><br/>";
  DOCUMENTROOT += "<a href='/v1/right'>/v1/right/auth/token</a><br/>";
  DOCUMENTROOT += "<br/>";
  DOCUMENTROOT += "<a href='/update'>Update Firmware. v1.0</a><br/>";
  DOCUMENTROOT += "</body>";
  DOCUMENTROOT += "</html>";
  server.send(200, "text/html", DOCUMENTROOT);
  delay(100);
}

void WIFI_Connect()
{
  digitalWrite(2, 1);
  WiFi.disconnect();
  Serial.println("Booting Sketch...");
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  // Wait for connection
  for (int i = 0; i < 25; i++)
  {
    if ( WiFi.status() != WL_CONNECTED ) {
      delay ( 250 );
      digitalWrite(2, 0);
      Serial.print ( "." );
      delay ( 250 );
      digitalWrite(2, 1);
    }
  }
  digitalWrite(2, 0);
}

void setup(void)
{
  wifi_station_set_hostname("minion");
  pinMode(D1, OUTPUT); //megafon
  pinMode(D2, OUTPUT); //beedoo
  pinMode(D5, OUTPUT); //right hand

  // Open the Arduino IDE Serial Monitor to see what the code is doing
  Serial.begin(9600);

  Serial.println("Banana?");
  Serial.println("");

  // Connect to your WiFi network
  //WiFi.begin(ssid, password);
  //Serial.print("Connecting");
  WIFI_Connect();

  // Wait for successful connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to: ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");

  // Handle http requests
  server.on("/", handle_root);

  server.on("/v1/beedoo/auth/KXxAcJDMhGpWOVP0gWM9", []() {
    char response[50];
    snprintf(response, 50, "beedoo");
    digitalWrite(D2, HIGH);
    delay(100);
    digitalWrite(D2, LOW);
    server.send(200, "text/plain", response);
  });

  server.on("/v1/megafon/auth/KXxAcJDMhGpWOVP0gWM9", []() {
    char response[50];
    snprintf(response, 50, "megafon");
    digitalWrite(D1, HIGH);
    delay(100);
    digitalWrite(D1, LOW);
    server.send(200, "text/plain", response);
  });

  server.on("/v1/right/auth/KXxAcJDMhGpWOVP0gWM9", []() {
    char response[50];
    snprintf(response, 50, "right hand");
    digitalWrite(D5, HIGH);
    delay(100);
    digitalWrite(D5, LOW);
    server.send(200, "text/plain", response);
  });

  // Start the web server
  httpUpdater.setup(&server);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void)
{
  if (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(2, 1);
    WIFI_Connect();
    Serial.println("wifi reconnect");
  } else {
    digitalWrite(2, 0);
  }
  yield();
  delay(10);
  // Listen for http requests
  server.handleClient();
}
