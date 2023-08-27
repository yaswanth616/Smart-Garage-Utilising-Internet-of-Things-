#include <Arduino.h>
#include <WiFi.h>
#include<bits/stdc++.h>
using namespace std;

#include <SPI.h>                                 //rfid
#include <MFRC522.h>

void printHex(byte *buffer, byte bufferSize);
void printDec(byte *buffer, byte bufferSize);
void decrement(int);

#define SS_PIN 15
#define RST_PIN 5
 
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key; 

// Init array that will store new NUID 
byte nuidPICC[4];
String UID;
                                                //rfid

const char *SSID = "sk";
const char *PWD = "shashikanth";
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

#include <PubSubClient.h> //hivemq
// MQTT client
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient); 
char *mqttServer = "broker.hivemq.com";
int mqttPort = 1883;
void setupMQTT() {
  mqttClient.setServer(mqttServer, mqttPort);

}
 int trigPin1=13;
 int echoPin1=12;
  //  int trigPin2=32;
  //  int echoPin2=33;
  int slots=10;
  //int a=slots;
void setup() {
  Serial.begin(9600);
  connectToWiFi();

  setupMQTT();
                                                               //ultrasonic
   pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1,INPUT);
    // pinMode(trigPin2,OUTPUT);
    // pinMode(echoPin2, INPUT);
                                                             //ultrasonic
                                                                    //rfid
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 
   

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
 //printDec(key.keyByte, MFRC522::MF_KEY_SIZE);
                                                                    //rfid
}
 void printHex(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}


void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}


void reconnect() {
  Serial.println("Connecting to MQTT Broker...");
  while (!mqttClient.connected()) {
      Serial.println("Reconnecting to MQTT Broker..");
      String clientId = "ESP32Client-";
      clientId += String(random(0xffff), HEX);
      
      if (mqttClient.connect(clientId.c_str())) {
        Serial.println("Connected.");
        
      }
      
  }
}
int count1=1;
//int count2=1;
void decrement(int r1)
{
  
 
  
  if(r1<=10)
  { if(count1==1)
     {slots--;
     count1=2;}
  }
   if(r1>10)
  { 
    //slots++;
    count1=1;
    {

    }
    
  }
  // if(r2<=10)
  // {
  //   if(count2==1)
  //    {slots--;
  //    count1=2;}

  // }
  
 
  // if(r2>10)
  // {
  //   count2=1;
  // }

}




long last_time=0;
 void loop() {
   if (!mqttClient.connected())
     reconnect();
    mqttClient.loop();

  delay(1000);
   
    // Send data

    // ultrasonic1
    float d1,r1;
    
    digitalWrite(trigPin1, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin1, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin1, LOW);
    d1=pulseIn(echoPin1, HIGH);
    r1=(d1/2)/29.1;
    Serial.println(" ");
   Serial.print(r1);
   Serial.println(" :cm");
   decrement(r1);
  //  else slots++;
   // ultrasonic1

   // ultrasonic2
  //  float d2,r2;
    
  //   digitalWrite(trigPin2, LOW);
  //   delayMicroseconds(2);
  //   digitalWrite(trigPin2, HIGH);
  //   delayMicroseconds(10);
  //   digitalWrite(trigPin2, LOW);
  //   d1=pulseIn(echoPin2, HIGH);
  //   r2=(d2/2)/29.1;
  //  Serial.print(r2);
  //  Serial.println(" :cm");
  //  decrement(r1,r2);
   
   // ultrasonic2
    
    
   
    // char* data;
    
    // Publishing data throgh MQTT
    char	data[8];
				dtostrf(slots,	1,	2,	data);
				Serial.print(data);
 // long now = millis();
   //if (now - last_time > 60000) {
    mqttClient.publish("/swa/slots", data);
   //}
   // last_time = now;
    



                  //rfid
    UID="";
    // x=random(0,9);
     // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }
 

  if (rfid.uid.uidByte[0] != nuidPICC[0] || 
    rfid.uid.uidByte[1] != nuidPICC[1] || 
    rfid.uid.uidByte[2] != nuidPICC[2] || 
    rfid.uid.uidByte[3] != nuidPICC[3] ) {
    Serial.println(F("A new card has been detected."));

    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
   
    Serial.println(F("The NUID tag is:"));
    Serial.print(F("In hex: "));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    Serial.print(F("In dec: "));
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();

    Serial.print("UID IS : ");
    Serial.println(UID);
  }
  else Serial.println(F("Card read previously."));

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();



/**
 * Helper routine to dump a byte array as hex values to Serial. 
 */

  for (byte i = 0; i < rfid.uid.size; i++) {
        UID += String(rfid.uid.uidByte[i], HEX);
      }
      char	rdata[100];
      strcpy(rdata,UID.c_str());
				//dtostrf(UID,	1,	2,	rdata);
				Serial.print(UID);
    mqttClient.publish("/swa/rfid", rdata);
  
 }
 

