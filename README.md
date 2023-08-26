## Hardware
 - LOLIN D1 Mini V4.0.0-WEMOS (ESP8266)
 - MOS FET (XY-MOS)
 - SAMIORE DD4012SA 5V
 - WAVGAT AC-DC 12V 3A (XPJ-02C)

## Configuration System
Create file "config.h":
```cpp
#ifndef CONFIG
#define CONFIG

const char* wifi_ssid = "wifi_ssid";
const char* wifi_pass = "wifi_pass";
const char* mqtt_server = "mqtt_server";
const int mqtt_port = 1883;

#endif
```

## Configuration HA
Edit file "configuration.yaml":
```yaml
mqtt:
  sensor:
    - state_topic: "home/light"
      name: "MQTT Light"
```

## Plan
- [x] Connecting to Home Assistant MQTT
- [ ] Setup integrations MQTT Light
- [ ] Startup Config system
- [ ] Read config ini form SD
- [ ] Get a unique device name from Home Assistant
