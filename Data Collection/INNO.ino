#include <PubSubClient.h>
#include <rpcWiFi.h>
#include <TFT_eSPI.h>
#include"LIS3DHTR.h"
#include <Arduino.h>
#include <SensirionI2CSht4x.h>
#include <Wire.h>
 
LIS3DHTR<TwoWire> lis;
SensirionI2CSht4x sht4x;
 
//Required Information
#define WIFISSID "CompSci" // Put your WifiSSID here
#define PASSWORD "C0mputerSc1ence!123" // Put your wifi password here
#define TOKEN "BBFF-Zdjr2K8DbpUNQrwQbHsmAL90FvW8Ot" // Put your Ubidots' TOKEN
#define VARIABLE_LABEL1 "uv" // Assign the variable label
#define VARIABLE_LABEL2 "humidity"
#define VARIABLE_LABEL3 "temperature"
#define VARIABLE_LABEL4 "moisture"
#define VARIABLE_LABEL5 "sound"
#define DEVICE_LABEL "wio-terminal" // Assign the device label
#define MQTT_CLIENT_NAME "6654276949" // MQTT client Name
 
const long interval = 100;
unsigned long previousMillis = 0;
 
TFT_eSPI tft;
 
char mqttBroker[] = "industrial.api.ubidots.com";
 
WiFiClient wifiClient;
PubSubClient client(wifiClient);

int MOIST = A2;
int UV = A5;
 
//sensor values
static int uvValue = 0;
static float humidityValue = 0;
static float temperatureValue = 0;
static int moistureValue = 0;
static int soundValue = 0;
 
// Space to store values to send
static char str_uv[6];
static char str_humidity[6];
static char str_temperature[6];
static char str_moisture[6];
static char str_sound[6];

char payload[700];
char topic[150];
 
void callback(char* topic, byte* payload, unsigned int length){
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
}
 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
 
  // Attempt to connect
  if (client.connect(MQTT_CLIENT_NAME, TOKEN,"")) {
    Serial.println("connected");
  }
  else {
    Serial.print("failed, rc=");
    Serial.print(client.state());
    Serial.println(" try again in 2 seconds");
    // Wait 2 seconds before retrying
    delay(2000);
    }
  }
}
 
//Reading built-in sensor values
void read_builtin()
{
  uvValue = analogRead(UV);
  Serial.print("uv = ");
  Serial.println(uvValue);

  float temperature;
  float humidity;
  sht4x.measureHighPrecision(temperature, humidity);

  humidityValue = humidity;
  Serial.print("humidity = ");
  Serial.println(humidityValue);

  temperatureValue = temperature;
  Serial.print("temperature = ");
  Serial.println(temperatureValue);

  moistureValue = analogRead(MOIST);
  Serial.print("moisture = ");
  Serial.println(moistureValue);

  soundValue = analogRead(WIO_MIC);
  Serial.print("Sound = ");
  Serial.println(soundValue);

  tft.fillScreen(TFT_BLACK);
  String uvString = "UV Rating = ";
  uvString += uvValue;
  tft.drawString(uvString ,20,10);
  String tempString = "Temperature (C) = ";
  tempString += temperatureValue;
  tft.drawString(tempString ,20,40);
  String humidityString = "Humidity (%) = ";
  humidityString += humidityValue;
  tft.drawString(humidityString ,20,70);
  String moistureString = "Moisture = ";
  moistureString += moistureValue;
  tft.drawString(moistureString ,20,100);
}
 
//Sending data to Ubidots
void send_data()
{
  dtostrf(uvValue, 4, 0, str_uv);
  dtostrf(humidityValue, 4, 0, str_humidity);
  dtostrf(temperatureValue, 4, 3, str_temperature);
  dtostrf(moistureValue, 4, 3, str_moisture);
  dtostrf(soundValue, 4, 0, str_sound);
 
  if (!client.connected()) {
    reconnect();
  }
 
  // Builds the topic
  sprintf(topic, "%s", ""); // Cleans the topic content
  sprintf(topic, "%s%s", "/v2.0/devices/", DEVICE_LABEL);
 
  //Builds the payload
  sprintf(payload, "%s", ""); // Cleans the payload
  sprintf(payload, "{\"%s\":", VARIABLE_LABEL1); // Adds the variable label
  sprintf(payload, "%s%s", payload, str_uv); // Adds the value
  sprintf(payload, "%s}", payload); // Closes the dictionary brackets
  client.publish(topic, payload);
  delay(500);

  
 
  sprintf(payload, "%s", ""); // Cleans the payload
  sprintf(payload, "{\"%s\":", VARIABLE_LABEL2); // Adds the variable label
  sprintf(payload, "%s%s", payload, str_humidity); // Adds the value
  sprintf(payload, "%s}", payload); // Closes the dictionary brackets
  client.publish(topic, payload);
  delay(500);
 
  sprintf(payload, "%s", ""); // Cleans the payload
  sprintf(payload, "{\"%s\":", VARIABLE_LABEL3); // Adds the variable label
  sprintf(payload, "%s%s", payload, str_temperature); // Adds the value
  sprintf(payload, "%s}", payload); // Closes the dictionary brackets
  client.publish(topic, payload);
  delay(500);
 
  sprintf(payload, "%s", ""); // Cleans the payload
  sprintf(payload, "{\"%s\":", VARIABLE_LABEL4); // Adds the variable label
  sprintf(payload, "%s%s", payload, str_moisture); // Adds the value
  sprintf(payload, "%s}", payload); // Closes the dictionary brackets
  client.publish(topic, payload);
  delay(500);
 
  sprintf(payload, "%s", ""); // Cleans the payload
  sprintf(payload, "{\"%s\":", VARIABLE_LABEL5); // Adds the variable label
  sprintf(payload, "%s%s", payload, str_sound); // Adds the value
  sprintf(payload, "%s}", payload); // Closes the dictionary brackets
  client.publish(topic, payload);
  delay(500);
 
  client.loop();
}
 
void setup() {
  Serial.begin(115200);
  lis.begin(Wire1);
  pinMode(WIO_MIC, INPUT);

  pinMode(MOIST, INPUT);
  pinMode(UV, INPUT);

  Wire.begin();
  sht4x.begin(Wire);

  tft.begin();
  tft.setRotation(3);
  tft.setTextSize(2);
  tft.fillScreen(TFT_BLACK);
 
  lis.setOutputDataRate(LIS3DHTR_DATARATE_25HZ); //Data output rate
  lis.setFullScaleRange(LIS3DHTR_RANGE_2G);
 
//  while(!Serial);
 
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
 
  tft.drawString("Connecting to WiFi...",20,120);
  WiFi.begin(WIFISSID, PASSWORD);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    WiFi.begin(WIFISSID, PASSWORD);
  }
 
  tft.fillScreen(TFT_BLACK);
  tft.drawString("Connected to the WiFi",20,120);
 
  delay(1000);
  client.setServer(mqttBroker, 1883);
  client.setCallback(callback);
 
}
 
void loop() {
  read_builtin();   //Reading buile-in sensor values
  send_data();   //Sending data to Ubidots
  delay(5000);
}
