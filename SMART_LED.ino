// TODO: Startup Config system - ConfigAssist-ESP32-ESP8266
// TODO: read config ini - <IniFile.h> or #include <ini.h>
// TODO: choose a unique device name (DEVICE_NAME) https://www.home-assistant.io/integrations/http/#sensor

#include "config.h"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// #define MOSFET 5  // GPOO5 или D1
#define MOSFET 4  // GPOO4 или D2

WiFiClient espClient;
PubSubClient client(espClient);

// Device settings
const char* mainTopic = "home/led"; // MQTT topic where values are published

// MQTT: topics
const char* MQTT_LIGHT_STATE_TOPIC = "home/led/light/status";
const char* MQTT_LIGHT_COMMAND_TOPIC = "home/led/light/switch";

// payloads by default (on/off)
const char* LIGHT_ON = "ON";
const char* LIGHT_OFF = "OFF";

boolean m_light_state = false; // light is turned off by default

int updateInterval = 1000; // Interval in milliseconds
char buffer[4]; // Buffer to store the sensor value
int light = 255;

float calcLight(float x){
  return 0.0000333252 * pow(x, 3) - 0.00772894 * pow(x, 2) + 0.803827 * x - 0.72969;
}

// function called to publish the state of the light (on/off)
void publishLightState() {
  if (m_light_state) {
    client.publish(MQTT_LIGHT_STATE_TOPIC, LIGHT_ON, true);
  } else {
    client.publish(MQTT_LIGHT_STATE_TOPIC, LIGHT_OFF, true);
  }
}

// function called to turn on/off the light
void setLightState() {
  if (m_light_state) {
    digitalWrite(MOSFET, HIGH);
  } else {
    digitalWrite(MOSFET, LOW);
  }
}

/*=============== Callback MQTT ===============*/

void callbackMqtt(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  Serial.print("Message: ");

  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];  //Conver *byte to String
  }
  Serial.print(message);
  /*
  if(message == "0") {digitalWrite(relay,LOW);}
  if(message == "1") {digitalWrite(relay,HIGH);}
  */
  Serial.println();
  
  if(!strcmp(topic, MQTT_LIGHT_COMMAND_TOPIC)){
    light = calcLight(message.toInt());
    //light = message.toInt();
    Serial.println(light);
  }else{
  }
/*
  // concat the payload into a string
  String payload;
  for (uint8_t i = 0; i < length; i++) {
    payload.concat((char)length[i]);
  }
  
  // handle message topic
  if (String(MQTT_LIGHT_COMMAND_TOPIC).equals(p_topic)) {
    // test if the payload is equal to "ON" or "OFF"
    if (payload.equals(String(LIGHT_ON))) {
      if (m_light_state != true) {
        m_light_state = true;
        setLightState();
        publishLightState();
      }
    } else if (payload.equals(String(LIGHT_OFF))) {
      if (m_light_state != false) {
        m_light_state = false;
        setLightState();
        publishLightState();
      }
    }
  }
  */
  
}

/*=============== Reconnect MQTT ===============*/

void reconnectMqtt() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),"mqttdevices", "aDk1T3P$81XT")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("home/status", "on");
      // ... and resubscribe
      client.subscribe(MQTT_LIGHT_COMMAND_TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

/*=============== Setup WIFI ===============*/

void setupWifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

/*=============== Setup ===============*/

void setup() {
  Serial.begin(115200);
  
  setupWifi();

  client.setServer(mqtt_server,mqtt_port);
  client.setCallback(callbackMqtt);

  pinMode(MOSFET, OUTPUT);
  
}

/*=============== LOOP ===============*/

void loop() {

   if (!client.connected()) {
    reconnectMqtt();
  }
  client.loop();

  analogWrite(MOSFET, light);

  //delay(updateInterval);

}
