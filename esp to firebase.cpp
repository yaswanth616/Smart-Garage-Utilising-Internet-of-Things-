#include <Arduino.h>

#include<bits/stdc++.h>
#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif

//Provide the token generation process info.
#include <addons/TokenHelper.h>

// //Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>
using namespace std;
// namespace std;

void callback(char* topic, byte* payload, unsigned int length);
const char *SSID = "sandeep";
const char *PWD = "sandeep7777";
#define API_KEY "AIzaSyCgtJKQIc8GBtAexFXe7OLCN8_pDt-RotY"

// /* 3. Define the RTDB URL */
 #define DATABASE_URL "rfid-db127-default-rtdb.firebaseio.com"


// //Define Firebase Data object
 FirebaseData fbdo;

 FirebaseAuth auth;
 FirebaseConfig config;
void connectToWiFi() {
  delay(100);
  Serial.print("Connectiog to ");
 
  WiFi.begin(SSID, PWD);
  Serial.println(SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("Connected.");
  
}

#include <PubSubClient.h>
// MQTT client
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient); 
char *mqttServer = "broker.hivemq.com";
int mqttPort = 1883;
void setupMQTT() {
  mqttClient.setServer(mqttServer, mqttPort);
  // set the callback function
  mqttClient.setCallback(callback);
}
//  int trigPin=13;
//  int echoPin=14;
void setup() {
  Serial.begin(9600);
  connectToWiFi();

  setupMQTT();
  //

   Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  config.database_url = DATABASE_URL;



  //////////////////////////////////////////////////////////////////////////////////////////////
  //Please make sure the device free Heap is not lower than 80 k for ESP32 and 10 k for ESP8266,
  //otherwise the SSL connection will fail.
  //////////////////////////////////////////////////////////////////////////////////////////////

  Firebase.begin(DATABASE_URL, API_KEY);
  

  //Comment or pass false value when WiFi reconnection will control by your code or third party library
 // Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);

}

void reconnect() {
  Serial.println("Connecting to MQTT Broker...");
  while (!mqttClient.connected()) {
      Serial.println("Reconnecting to MQTT Broker..");
      String clientId = "ESP32Client-";
      clientId += String(random(0xffff), HEX);
      
      if (mqttClient.connect(clientId.c_str())) {
        Serial.println("Connected.");
        // subscribe to topic
         mqttClient.subscribe("/swa/rfid");
        mqttClient.subscribe("/swa/slots");
        
        
      }
      
  }
}

 char str[100];
 char str2[100];
char s1[]="/swa/slots";
  char s2[]="/swa/rfid";
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Callback - ");
  Serial.print("Message:");
  
  if(!strcmp(topic,s1))
  {
 
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    
    str[i]=(char)payload[i];
  }
  }
  if(!strcmp(topic,s2)){
    for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    
    str2[i]=(char)payload[i];
  }
  }
  
  }
//long last_time=0;
 void loop() {
   if (!mqttClient.connected())
     reconnect();
    mqttClient.loop();

  
  // long now = millis();
  // if (now - last_time > 60000) {
  //   // Send data
  //   float d,r;
  //   digitalWrite(trigPin, LOW);
  //   delayMicroseconds(2);
  //   digitalWrite(trigPin, HIGH);
  //   delayMicroseconds(10);
  //   digitalWrite(trigPin, LOW);
  //   d=pulseIn(echoPin, HIGH);
  //   r=(d/2)/29.1;
  //  // Serial.print(r);
  //   // char* data;
  //   r=10.55;
  //   Serial.println(" cm");
  //   // Publishing data throgh MQTT
  //   char	data[8];
	// 			dtostrf(r,	1,	2,	data);
	// 			Serial.print(data);
  //   //sprintf(data, "%f", r);
  //   //Serial.println(data);
  //   mqttClient.publish("/swa/distance", data);
  //  last_time = now;
  //   }

  if (Firebase.ready()) 
  {
    
    //Firebase.setInt(fbdo, main, 5);
    Firebase.setString(fbdo, "/free_slots", str);
    Firebase.setString(fbdo, "/current_rfid", str2);
    //Firebase.setInt(fbdo, "/login/b", y);
     Firebase.RTDB.getString( &fbdo,"/current_rfid");
     if (fbdo.dataType() == "string") {
  String value = fbdo.stringData();
  Serial.println(value);
  
   String s="/"+value;
 Firebase.RTDB.getString( &fbdo,s);
  if (fbdo.dataType() == "string") {
  String value1 = fbdo.stringData();
  Serial.println(value1);
  }

// else
// {
// Serial.println("not booked");
// }
     }
 

}
     delay(500);
  }