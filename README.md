# SMART LED
Smart LED 12V strip with WiFi control and Home Assistant. 
Simple circuit assembly with convenient settings.
Development is carried out according to the method of simplification for the end user.



## Hardware
 - LOLIN D1 Mini V4.0.0-WEMOS (ESP8266)
 - MOS FET (XY-MOS)
 - SAMIORE DD4012SA 5V
 - WAVGAT AC-DC 12V 3A (XPJ-02C)
 - BTF-FCOB LED STRIP 528 12V LED

## Software
 - Home Assistant 2023.1.0
 - Mosquitto 6.1.3 (MQTT 3.1.1)

## Configuration System
Create file "config.h":
```cpp
#ifndef CONFIG
#define CONFIG

const char* wifi_ssid = "wifi_ssid";
const char* wifi_pass = "wifi_pass";

const char* mqtt_server = "mqtt_server";
const char* mqtt_user = "mqtt_user";
const char* mqtt_pass = "mqtt_pass";
const int mqtt_port = 1883;

String clientId = "NAME-";

#endif
```

## Configuration HA
Edit file "configuration.yaml":
```yaml
mqtt:
  - light:
      name: 'Smart LED'
      state_topic: 'home/led/light/status'
      command_topic: 'home/led/light/switch'
      brightness_state_topic: 'home/led/light/brightness'
      brightness_command_topic: 'home/led/light/brightness/set'
      qos: 0
      payload_on: "ON"
      payload_off: "OFF"
      optimistic: false
```

## Plan
- [x] Connecting to Home Assistant MQTT
- [x] Setup integrations MQTT Light
- [ ] Startup Config system
- [ ] Read config ini form SD
- [ ] Get a unique device name from Home Assistant
- [ ] Wiring diagram
- [ ] Automatic update over the air 
- [ ] Printing the device body on a 3D printer
