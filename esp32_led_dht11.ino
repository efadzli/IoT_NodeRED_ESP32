#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

int led_pin= 13;

void setup(void)
{ 
  Serial.begin(115200);
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);

  Serial.println("Starting DHT11 Reading..");
  dht.begin();
}

void loop() {  

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print("%  Temperature: ");
  Serial.print(t);
  Serial.println("°C ");

  if(digitalRead(led_pin)==0)
    digitalWrite(led_pin, HIGH);
  else
    digitalWrite(led_pin, LOW);

  delay(2000);
}
