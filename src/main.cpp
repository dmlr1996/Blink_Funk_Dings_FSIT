/* ------------------------------------------
   Blink-Funk-Dings
   Version 1.0, 21.10.2021
   Author: Dominik Magerle
   License: CC BY 4.0 https://creativecommons.org/licenses/by/4.0/
   -------------------------------------------
 */


#include <ESP8266WiFi.h> // Wifi Library
#include "FastLED.h"     // LED Library

//You need to make a File in your src Folder.
//There the const char* ssid = <SSID> and 
//const char* pwd = <PASSWORD> needs to be defined!
#include <WifiCredentials.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

#define MSG_BUFFER_SIZE 50


#define BUTTON_PIN 0 
#define NUM_LEDS   4 
#define BRIGHTNESS 128
#define LED_PIN    2
#define LED_TYPE   

// Led Strip Leds
CRGB ledstrip[NUM_LEDS]={{0}};
static int counter  = 0;

const char* mqtt_server = "mqtt.fachschaft-it.de";
const int mqtt_port = 4325;

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;

char msg[MSG_BUFFER_SIZE];
int value = 0;

//Button state for toggle button
unsigned int button_state = LOW;

String clientId = "DMA_Client";

//Function for setting up WiFi Connection
void setupWifi(){
  Serial.println();
  Serial.print("Connecting to WIFI Network: ");
  Serial.println(ssid);

  //Start WiFi Connection
  WiFi.mode(WIFI_STA);
  WiFi.hostname("Blinkfunkdings_DMa");
  WiFi.begin(ssid, pwd);

  //Monitor Connectrion Establishment
  while (WiFi.status() != WL_CONNECTED){
    delay(100);
    //Make LED 2 Red, if not Connected.
    ledstrip[2] = CRGB::Red;
    FastLED.show();
    Serial.println("Connecting...");
  }

  Serial.println();
  Serial.println("WiFi connected.");
  Serial.println("IP Adress: ");
  Serial.println(WiFi.localIP());
  //Make LED 2 Green, if not Connected.
  ledstrip[2] = CRGB::Green;
  FastLED.show();
}

//Check for button presses. This was implemented a bit lazily.
void handle_button()
{
  static double last_push = 0;
  if(digitalRead(BUTTON_PIN) == LOW && millis() - last_push > 200){
    Serial.println("Push Button");
    last_push = millis();
    button_state = !button_state;

    client.publish("Dominik", "Hallo FSIT!");
  }
}

//Establish MQTT Connection
void mqttConnect(){
  Serial.println("Connecting to MQTT Server...");

  //Loop until connection
  while (!client.connected()){
    Serial.println("Connecting...");

    //Random Client ID
    clientId += String(random(0xffff), HEX);

    //Try to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("Connected!");

      //Once connected, publish Announcment
      client.publish("Dominik", "Is now Online!");

      //Resubscribe
      //client.subscribe("Julius");
    } else {
      Serial.println("Failed. Reconnect...");
      Serial.println(client.state());
      Serial.println("Try again in 5 seconds");
      delay(5000);
    }
  }
}

//Callback, when Message recieved
void callback(char* topic, byte* payload, unsigned int length) {
  
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");

  ledstrip[3] = CRGB::Orange;
  FastLED.show();
  delay(100);
  ledstrip[3] = CRGB::Black;
  FastLED.show();
}


// Set up the pins, WLAN and LEDS 
void setup() {

  //Randomizer Init
  randomSeed(micros());
  Serial.begin(115200);
  Serial.println();
  Serial.println("Welcome to Blink-Funk-Dings-OS");

  // enable button
  pinMode(BUTTON_PIN, INPUT);

  FastLED.addLeds<NEOPIXEL, LED_PIN>(ledstrip, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  setupWifi();

  client.setServer(mqtt_server, mqtt_port);

  client.setCallback(callback);
}
 
void loop() {
  
  handle_button();
  ledstrip[0] = 255;
  ledstrip[1] = CHSV(counter % 255,255,255);


  // Check WLAN connection (no need to do this as often as we do here! This is just a demo

  //if ((WiFi.status() != WL_CONNECTED) && (counter % 2 == 1)) {
  //  ledstrip[2] = CRGB::Red;
  //} else {
  //  ledstrip[2] = CRGB::Green;
  //}
  
  if (!client.connected()) {
    mqttConnect();
    client.subscribe("Julius");
  }
  
  FastLED.show();
  counter = counter +1  % 1024;
  
  client.loop();

  delay(100);
    
}

  
