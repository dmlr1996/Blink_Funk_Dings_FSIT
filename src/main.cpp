/* ------------------------------------------
   Blink-Funk-Dings
   Version 1.0, 21.10.2021
   Author: Dominik Magerle
   License: CC BY 4.0 https://creativecommons.org/licenses/by/4.0/
   -------------------------------------------
 */


#include <ESP8266WiFi.h>    // Wifi Library
#include <NeoPixelBus.h>    // LED Library

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
#define RGB_BRIGHTNESS 128
#define LED_PIN    2
#define LED_TYPE   

RgbColor red(RGB_BRIGHTNESS, 0, 0);
RgbColor orange(RGB_BRIGHTNESS, RGB_BRIGHTNESS/2, 0);
RgbColor green(0, RGB_BRIGHTNESS, 0);
RgbColor blue(0, 0, RGB_BRIGHTNESS);
RgbColor white(RGB_BRIGHTNESS);
RgbColor black(0);

// Led Strip Leds
NeoPixelBus <NeoGrbFeature, NeoEsp8266BitBang800KbpsMethod> ledstrip(NUM_LEDS, LED_PIN);
static int counter  = 0;
const int counter_max = 1024;

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
    ledstrip.SetPixelColor(2, red);
    ledstrip.Show();
    Serial.println("Connecting...");
  }

  Serial.println();
  Serial.println("WiFi connected.");
  Serial.println("IP Adress: ");
  Serial.println(WiFi.localIP());
  //Make LED 2 Green, if not Connected.
  ledstrip.SetPixelColor(2, green);
  ledstrip.Show();
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

  ledstrip.SetPixelColor(3, orange);
  ledstrip.Show();

  delay(100);

  ledstrip.SetPixelColor(3, black);
  ledstrip.Show();
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

  setupWifi();

  client.setServer(mqtt_server, mqtt_port);

  client.setCallback(callback);
}
 
void loop() {
  
  handle_button();
  ledstrip.SetPixelColor(0, 255);
  HsbColor pixl1((1.0 * counter) / counter_max, 1.0, 1.0);
  ledstrip.SetPixelColor(1, pixl1);

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
  
  ledstrip.Show();
  counter++;
  counter = counter %1024;
  
  client.loop();

  delay(100);
    
}

  
