// TODO: Start Config system - ConfigAssist-ESP32-ESP8266
// TODO: read config ini - <IniFile.h>
// TODO: choose a unique device name (DEVICE_NAME) https://www.home-assistant.io/integrations/http/#sensor

// DO: Connected to Home Assistant MQTT
/* /config/configuration.yaml
mqtt:
  sensor:
    - state_topic: "home/fan"
      name: "MQTT Fan"
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#ifndef STASSID
#define STASSID "da-home-2G"
#define STAPSK "kakoyhochesh"
#endif

#define MOSFET 5  // GPOO5 или D1

const char* ssid = STASSID;
const char* password = STAPSK;
const char* mqtt_server = "192.168.0.139";
const int mqtt_port = 1883;


WiFiClient espClient;
PubSubClient client(espClient);

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
      client.subscribe("home/");
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
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

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

  Serial.println("Fan: ");
  
  client.publish("home/fan", "Плавное появление");
  Serial.println("Плавное появление");
  // плавное включение
  for (int i = 0; i <= 1023; i++) {
    analogWrite(MOSFET, i);
    //Serial.println(i);
    delay(10);
  }

  delay(5000);

  client.publish("home/fan", "Плавное затухание");
  Serial.println("Плавное затухание");
  //плавное выключение
  for (int i = 1023; i >= 0; i--) {
    analogWrite(MOSFET, i);
    //Serial.println(i);
    delay(10);
  }

  //analogWrite(MOSFET, 1023);

}
