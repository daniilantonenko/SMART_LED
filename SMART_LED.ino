/*
MQTT 
192.168.0.139:8123
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

// Use WiFiClient class to create TCP connections

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(MQTTcallback);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP8266", "daniilantonenko", "ghbdtngjrf")) {
      Serial.println("connected");
    } else {
      Serial.print("failed with state ");
      Serial.println(client.state());
      delay(2000);
    }
  }
  client.subscribe("homeassistant/da-smart/led1/");

  pinMode(MOSFET, OUTPUT);  // Подключение ШИМ реле
}

void MQTTcallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print(message);

  /*
  if (message == "on") {
    digitalWrite(LED, HIGH);
  } else if (message == "off") {
    digitalWrite(LED, LOW);
  }
  Serial.println();
  Serial.println("-----------------------");
  */

  client.publish("homeassistant/da-smart/led1/", "Led 1 Activated");
}

void loop() {
  
  client.publish("homeassistant/da-smart/led1/", "Плавное появление");
  // плавное включение
  for (int i = 0; i <= 1023; i++) {
    analogWrite(MOSFET, i);
    //Serial.println(i);
    delay(10);
  }

  client.publish("homeassistant/da-smart/led1/", "Плавное затухание");
  //плавное выключение
  for (int i = 1023; i >= 0; i--) {
    analogWrite(MOSFET, i);
    //Serial.println(i);
    delay(10);
  }

  //analogWrite(MOSFET, 1023);


/*
  for (int dutyCycle = 0; dutyCycle < 1023; dutyCycle++) {
    // изменение яркости светодиода с помощью ШИМ
    analogWrite(MOSFET, dutyCycle);
    delay(1);
  }

  // уменьшиние яркости светодиода
  for (int dutyCycle = 1023; dutyCycle > 0; dutyCycle--) {
    // изменение яркости светодиода с помощью ШИМ
    analogWrite(MOSFET, dutyCycle);
    delay(1);
  }
  
*/
/*
  int i;
  int CurrentPhaze=0; //фаза
  float sine [1000]; //массив 1000 отсчетов
  for (i=0; i<1000; i++){
    sine[i]=sin(2*3.1415*0.001*(i+CurrentPhaze));
    //printf("%f\n", sine[i]);
    analogWrite(MOSFET, sine[i]);
    Serial.println(sine[i]);
    delay(5);
  }
  */
  //delay(100);

  client.loop();
}
