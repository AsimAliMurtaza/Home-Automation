#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>

// WiFi
const char *ssid = "asimali"; // Enter your Wi-Fi name
const char *password = "33103310";  // Enter Wi-Fi password

// MQTT Broker
const char *mqtt_broker = "test.mosquitto.org";
const char *topic1 = "fan1/control";
const char *topic2 = "fan2/speed";
const char *topic3 = "light1/control";
const char *topic4 = "light2/control";
const char *topic5 = "light3/control";
const char *topic6 = "LDRValue";
int LED1Status;
int LED2Status;
int LED3Status;
int fanStatus;
int fanSpeed;
int ldrValue;
int receivedValue = 0;
int count = 0;

const char *mqtt_username = "";
const char *mqtt_password = "";
const int mqtt_port = 1883;
const long channelId = 2390484;
const char* apiKey = "II2KTL4CZTQK4MH0";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
    // Set software serial baud to 115200;
    Serial.begin(115200);
    Serial2.begin(9600, SERIAL_8N1, 16, 17); // RX2=16, TX2=17 on ESP32
    // Connecting to a WiFi network
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the Wi-Fi network");
    //connecting to a mqtt broker
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    while (!client.connected()) {
        String client_id = "esp32-client-";
        client_id += String(WiFi.macAddress());
        Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Public EMQX MQTT broker connected");
        } else {
            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }
    // Publish and subscribe
    client.subscribe(topic1);
    client.subscribe(topic2);
    client.subscribe(topic3);
    client.subscribe(topic4);
    client.subscribe(topic5);
}
void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message: ");
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    String receivedValueStr = "";  // String to accumulate characters
    int receivedValue = 0;         // Integer to store the final value

      for (int i = 0; i < length; i++) {
          receivedValueStr += (char)payload[i];
      }

    // Convert the accumulated string to an integer
    receivedValue = atoi(receivedValueStr.c_str());
    Serial.println();
    Serial.print("Received value as integer: ");
    Serial.println(receivedValue);
    Serial.println("-----------------------");

    // Send the received character to ATmega328P through UART
    if (length > 0) {
        switch (receivedValue) {
            case 1:
                Serial.println("Sending '1' to ATmega328P");
                Serial2.write(1);
                fanStatus=1;
                break;
            case 2:
                Serial.println("Sending '0' to ATmega328P");
                Serial2.write(2);
                fanStatus=0;
                break;
            case 3:
                Serial.println("Turning Light 1 on");
                Serial2.write(3);
                LED1Status = 1;
                break;
            case 4:
                Serial.println("Turning Light 1 off");
                Serial2.write(4);
                LED1Status = 0;
                break;
            case 5:
                Serial.println("Turning Light 2 on");
                Serial2.write(5);
                LED2Status = 1;
                break;
            case 6:
                Serial.println("Turning Light 2 off");
                Serial2.write(6);
                LED2Status = 0;
                break;
            case 7:
                Serial.println("Turning Light 3 on");
                Serial2.write(7);
                LED3Status = 1;
                break;
            case 8:
                Serial.println("Turning Light 3 off");
                Serial2.write(8);
                LED3Status = 0;
                break;
            default:
                Serial2.write(receivedValue);
                fanSpeed = receivedValue;
                Serial.println("donee");
                break;
        }
    }
}

void loop() {
    client.loop();
    delay(1000);
    if(count>=10){
      if(Serial2.available()){
      Serial.print("val received by Atmega: ");
      receivedValue = Serial2.read();
      Serial.println(receivedValue);
      ldrValue = receivedValue;
      }
      postRequest();
      updateValue();
      count = 0;
    }
    count++;
}
void updateValue(){
  char payload[200];       // Adjust the size based on the maximum size of your value
      snprintf(payload, sizeof(payload), "%d", ldrValue);
      // Publish the payload to the MQTT topic
      client.publish(topic6, payload);
}
void postRequest(){

  // Create the URL for ThingSpeak API
  String url = "http://api.thingspeak.com/update?api_key=" + String(apiKey) +
               "&field1=" + String(LED1Status) +
               "&field2=" + String(LED2Status) +
               "&field3=" + String(LED3Status) +
               "&field4=" + String(fanStatus) + 
               "&field5=" + String(fanSpeed) +
               "&field6=" + String(ldrValue);

  // Send HTTP POST request
  HTTPClient http;
  http.begin(url);

  int httpResponseCode = http.POST("");

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("HTTP POST request failed. Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}