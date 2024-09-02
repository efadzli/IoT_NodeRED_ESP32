#include <WiFi.h>
#include <MQTT.h>
#include <DHT.h>

// DTH11 parameters
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
int led1_pin= 13;
float temperature = 0;
float humidity = 0;

#define WIFI_SSID             "ENTER_YOUR_SSID_HERE"
#define WIFI_PASSWORD         "ENTER_YOUR_WIFI_KEY_HERE"
#define MQTT_HOST             "broker.hivemq.com"
#define MQTT_PREFIX_TOPIC     "ENTER_YOUR_PHONE_NO_HERE/esp32"
#define MQTT_PUBLISH_TOPIC1    "/temp"
#define MQTT_PUBLISH_TOPIC2    "/humi"
#define MQTT_SUBSCRIBE_TOPIC1  "/led1"

WiFiClient net;
MQTTClient mqtt(1024);
unsigned long lastMillis = 0;

void connectToWiFi() {
  Serial.print("Connecting to Wi-Fi '" + String(WIFI_SSID) + "' ...");

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // while wifi not connected yet, print '.'
  // then after it connected, get out of the loop
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
  }
  //print a new line, then print WiFi connected and the IP address
  Serial.println("");
  Serial.println("WiFi connected");
  // Print the IP address
  Serial.println(WiFi.localIP());
}

void messageReceived(String topic, String payload) {
  Serial.println("Incoming Status from topic " + topic + " -> " + payload);
  
  // check is topic equals MQTT_SUBSCRIBE_TOPIC1
  if(topic==(String(MQTT_PREFIX_TOPIC) + String(MQTT_SUBSCRIBE_TOPIC1))) {
    if(payload=="1"){
      digitalWrite(led1_pin, HIGH);
      Serial.println("LED1 turned ON");
    } else if(payload=="0"){
      digitalWrite(led1_pin, LOW);
      Serial.println("LED1 turned OFF");
    }
  }else{
    Serial.println("Command not match.");
  }
  
}

void connectToMqttBroker(){
  Serial.print("Connecting to '" + String(WIFI_SSID) + "' ...");
  
  mqtt.begin(MQTT_HOST, net);
  mqtt.onMessage(messageReceived);

  String uniqueString = String(WIFI_SSID) + "-" + String(random(1, 98)) + String(random(99, 999));
  char uniqueClientID[uniqueString.length() + 1];
  
  uniqueString.toCharArray(uniqueClientID, uniqueString.length() + 1);
  
  while (!mqtt.connect(uniqueClientID)) {
    Serial.print(".");
    delay(500);
  }

  Serial.println(" connected!");

  Serial.println("Subscribe to: " + String(MQTT_PREFIX_TOPIC) + String(MQTT_SUBSCRIBE_TOPIC1));
  mqtt.subscribe(String(MQTT_PREFIX_TOPIC) + String(MQTT_SUBSCRIBE_TOPIC1));
}


void setup(void)
{ 
  Serial.begin(115200);
  pinMode(led1_pin, OUTPUT);
  digitalWrite(led1_pin, LOW);

  Serial.println("Starting DHT11 Reading..");
  dht.begin();

  connectToWiFi();
  connectToMqttBroker();
  Serial.println();
}

void loop() {  

  mqtt.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }

  if (!mqtt.connected()) {
    connectToMqttBroker();
  }

  if (millis() - lastMillis > 5000) { // publish every 5 seconds
    lastMillis = millis();
      
    // Read DHT11 sensors
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print("%  Temperature: ");
    Serial.print(temperature);
    Serial.println("°C ");

    // Convert sensor reading to String and publish to broker
    String strTemp = String(temperature);
    Serial.println("Publish to topic: " + String(MQTT_PREFIX_TOPIC) + String(MQTT_PUBLISH_TOPIC1));
    Serial.println("Data: " + strTemp);
    mqtt.publish(String(MQTT_PREFIX_TOPIC) + String(MQTT_PUBLISH_TOPIC1), strTemp);

    // Convert sensor reading to String and publish to broker
    String strHumi = String(humidity);
    Serial.println("Publish to topic: " + String(MQTT_PREFIX_TOPIC) + String(MQTT_PUBLISH_TOPIC2));
    Serial.println("Data: " + strHumi);
    mqtt.publish(String(MQTT_PREFIX_TOPIC) + String(MQTT_PUBLISH_TOPIC2), strHumi);

  }
  Serial.println("..");
  delay(500);  
}

