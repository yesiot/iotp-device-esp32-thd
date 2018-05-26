/*
 This sketch uses the MQTT client library:
 https://github.com/knolleary/pubsubclient

 DHT sensor library:
 https://github.com/adafruit/DHT-sensor-library
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTTYPE DHT11 

const char* c_dev_name = "esp32";

const char* c_wifi_ssid     = "[ssid]";
const char* c_wifi_password = "[password]";

const char* c_mqtt_server = "[mqtt server name]";
const int   c_mqtt_port = 1883;
const char* c_mqtt_user = "[mqtt user name]";
const char* c_mqtt_password = "[mqtt password]";

// DHT Sensor
const int DHTPin = 17;
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);

WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
    //setup serial
    Serial.begin(115200);
    delay(10);

    setupWifi();

    client.setServer(c_mqtt_server, c_mqtt_port);

    dht.begin();
}

void setupWifi() {
    Serial.printf("\n\nConnecting to %s\n", c_wifi_ssid);

    WiFi.begin(c_wifi_ssid, c_wifi_password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void mqttConnect() {

  while (!client.connected()) {
    
    Serial.print("Attempting MQTT connection...");

    if (client.connect(c_dev_name, c_mqtt_user, c_mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop()
{ 
  if (!client.connected()) {
    mqttConnect();
  }
  client.loop();

  String topic = String(c_dev_name) + "/status";
  client.publish(topic.c_str(), "ALIVE");

  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t) ) {
      Serial.println("Failed to read from DHT sensor!");        
  }
  else {

    Serial.printf("Humidity:    %.1f %%\n", h);
    Serial.printf("Temperature: %.1f *C\n", t);

    String hStr = String(h);
    String tStr = String(t);
   
    topic = String(c_dev_name) + "/humidity";
    client.publish(topic.c_str(), hStr.c_str());
    
    topic = String(c_dev_name) + "/temp";
    client.publish(topic.c_str(), tStr.c_str());
  }

  delay(1000);
}

