// TODO: Startup Config system - ConfigAssist-ESP32-ESP8266
// TODO: read config ini - <IniFile.h> or #include <ini.h>
// TODO: choose a unique device name (DEVICE_NAME) https://www.home-assistant.io/integrations/http/#sensor

#include "config.h"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

// Device settings

// MQTT: topics
constexpr const char* MQTT_LIGHT_STATE_TOPIC = "home/led/light2/status";
constexpr const char* MQTT_LIGHT_COMMAND_TOPIC = "home/led/light2/switch";
constexpr const char* MQTT_LIGHT_BRIGHTNESS_TOPIC = "home/led/light2/brightness/set";

// payloads by default (on/off)
const char* LIGHT_ON = "ON";
const char* LIGHT_OFF = "OFF";

boolean m_light_state = false; // light is turned off by default

uint8_t updateInterval = 1000; // Interval in milliseconds
char buffer[4]; // Buffer to store the sensor value
int brightness = 255;

// buffer used to send/receive data with MQTT
const uint8_t MSG_BUFFER_SIZE = 20;
char m_msg_buffer[MSG_BUFFER_SIZE]; 

// Correcting non-linear light changes
float gradualBrightness(float x){
  return 0.0000333252 * pow(x, 3) - 0.00772894 * pow(x, 2) + 0.803827 * x - 0.72969;
}


void setBrightness(){
  int correctBrightness = gradualBrightness(brightness);
  analogWrite(MOSFET, brightness);
}

void publishBrightness() {
  char cstr[16];
  itoa(brightness, cstr, 10);
  snprintf(m_msg_buffer, MSG_BUFFER_SIZE, "%d", brightness);
  client.publish(MQTT_LIGHT_STATE_TOPIC, cstr, true);
}

// Function called to turn on/off the light
void setLightState() {
  if (m_light_state) {
    digitalWrite(MOSFET, HIGH);
  } else {
    digitalWrite(MOSFET, LOW);
  }
}

// Function called to publish the state of the light (on/off)
void publishLightState() {
  if (m_light_state) {
    client.publish(MQTT_LIGHT_STATE_TOPIC, LIGHT_ON, true);
  } else {
    client.publish(MQTT_LIGHT_STATE_TOPIC, LIGHT_OFF, true);
  }
}

/*=============== Callback MQTT ===============*/

void callbackMqtt(char* p_topic, byte* p_payload, unsigned int p_length) {
  // concat the payload into a string
  String payload;
  for (uint8_t i = 0; i < p_length; i++) {
    payload.concat((char)p_payload[i]);
  }

  Serial.print("payload: ");
  Serial.println(payload);
  Serial.print("p_topic: ");
  Serial.println(p_topic);

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
  } else if (String(MQTT_LIGHT_BRIGHTNESS_TOPIC).equals(p_topic)) {
    brightness = payload.toInt();
    if (brightness < 0 || brightness > 255) {
      // do nothing...
      return;
    } else {
      setBrightness();
      publishBrightness();
    }
  }  
}

/*=============== Reconnect MQTT ===============*/

void reconnectMqtt() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(MQTT_LIGHT_COMMAND_TOPIC, LIGHT_ON, true);
      //client.publish("home/status", "on");
      // ... and resubscribe
      client.subscribe(MQTT_LIGHT_STATE_TOPIC);
      client.subscribe(MQTT_LIGHT_COMMAND_TOPIC);
      client.subscribe(MQTT_LIGHT_BRIGHTNESS_TOPIC);
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
}
