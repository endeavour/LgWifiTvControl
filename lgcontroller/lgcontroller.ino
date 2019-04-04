/*
Exposes the RS232 serial communication of an LG TV/Projector over MQTT
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>

// Make sure you change these settings to match your config
const int rxPin = D2;
const int txPin = D1;
const char *ssid = "foo";
const char *password = "password";
const char *mqtt_server = "192.168.1.1";
const char *announce_topic = "livingroom/tv/announce";
const char *request_topic = "livingroom/tv/request";
const char *response_topic = "livingroom/tv/response";


WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
const unsigned int MAX_SERIAL_INPUT = 50;
char inputBuffer[MAX_SERIAL_INPUT];
int inputBytesRead = 0;

SoftwareSerial mySerial(rxPin, txPin); // RX, TX

const int led = 2;

void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if (strcmp(topic, request_topic) == 0)
  {
    Serial.print("Writing MQTT payload to serial output");
    for (int i = 0; i < length; i++)
    {
      mySerial.print((char)payload[i]);
    }
    mySerial.print('\r');
  }
}

void reconnect()
{
  // Loop until we're reconnected
  while (!mqttClient.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266LGB8Controller-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (mqttClient.connect(clientId.c_str()))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      mqttClient.publish(announce_topic, "Connected");
      // ... and resubscribe
      mqttClient.subscribe(request_topic);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  pinMode(led, OUTPUT); // Initialize the BUILTIN_LED pin as an output
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  Serial.begin(115200);
  mySerial.begin(9600);
  setup_wifi();
  mqttClient.setServer(mqtt_server, 1883);
  mqttClient.setCallback(callback);
}

void readSerialInput()
{
  while (mySerial.available() > 0) {
    Serial.print("Reading serial data: ");
    byte nextByte = mySerial.read();
    Serial.write(nextByte);
    // LG B8 terminates its responses with an 'x' character.
    if (nextByte == 'x') {    
      Serial.println("Terminator character found, publishing buffer to MQTT");
      mqttClient.publish(response_topic, inputBuffer, inputBytesRead);
      inputBytesRead = 0;
    } else {
      if (inputBytesRead < MAX_SERIAL_INPUT) {
        inputBuffer[inputBytesRead] = nextByte;
        inputBytesRead++;
      } else {
        Serial.println("Input buffer full, discarding.");
        // Just throw the data away if we filled the buffer, something must've gone wrong.
        inputBytesRead = 0;
      }
    }    
  }
}

void loop()
{
  if (!mqttClient.connected())
  {
    reconnect();
  }
  mqttClient.loop();
  readSerialInput();
}
