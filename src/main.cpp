/* ------------------------------------------
   Blink-Funk-Dings
   Version 1.0, 21.10.2021
   Author: Dominik Magerle
   License: CC BY 4.0 https://creativecommons.org/licenses/by/4.0/
   -------------------------------------------
 */


#include <ESP8266WiFi.h> // Wifi Library
#include "FastLED.h"     // LED Library

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

#define MSG_BUFFER_SIZE 50


#define BUTTON_PIN 0 
#define NUM_LEDS   4 
#define BRIGHTNESS 128
#define LED_PIN    2

// Led Strip Leds
CRGB ledstrip[NUM_LEDS]={{0}};

// Network SSID
//const char* ssid     = "BFD";
//const char* password = "dingsdings";

const char* ssid     = "DOMINIK";
const char* password = "Dominik1996";

const char* mqtt_server = "broker.mqttdashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;

char msg[MSG_BUFFER_SIZE];
int value = 0;




// Button state for toggle button
unsigned int button_state = LOW;

// do what needs to be done when the button fires.
void toggle_state(){
  button_state = !button_state;
}

//Check for button presses. This was implemented a bit lazily.
void handle_button(unsigned int button_pin)
{
  static double last_push = 0;
  if(digitalRead(BUTTON_PIN) == LOW && millis() - last_push > 200){
    Serial.println("Push Button");
    last_push = millis();
    toggle_state(); 
  }
}




// Set up the pins, WLAN and LEDS 
void setup() {
  Serial.begin(115200);
  Serial.println("\n Welcome to Blink-Funk-Dings-OS");

  // enable button
  pinMode(BUTTON_PIN, INPUT);

  FastLED.addLeds<NEOPIXEL, LED_PIN>(ledstrip, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  
  // Connect WiFi
  Serial.println(WiFi.macAddress());
  WiFi.hostname("Blinkfunkdings");
  WiFi.begin(ssid, password);


  // Wait for connection. This blocks everything until connected.
  while (WiFi.status() != WL_CONNECTED) {
    ledstrip[2] = CRGB::Red;
    FastLED.show();
    delay(500);
  }

  // Print the IP address when conneted
  Serial.print("IP address: ");
  Serial.print(WiFi.localIP());
  Serial.print("\n");
  ledstrip[2] = CRGB::Green;
  FastLED.show();
}
 
void loop() {
  static int counter  = 0;
  
  handle_button(0);
  //ledstrip[0] = 255;
  ledstrip[1] = CHSV(counter % 255,255,255);
  ledstrip[3] = (button_state == LOW?CRGB::Black: CRGB(255, 128, 0));


  // Check WLAN connection (no need to do this as often as we do here! This is just a demo

  if ((WiFi.status() != WL_CONNECTED) && (counter % 2 == 1)) {
    ledstrip[2] = CRGB::Red;
  } else {
    ledstrip[2] = CRGB::Green;
  }
  
  FastLED.show();
  counter = counter +1  % 1024;
  delay(100);
    
}

  
