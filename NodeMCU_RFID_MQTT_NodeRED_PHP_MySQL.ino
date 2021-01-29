/*
===================RFID NODEMCU NODE-RED PHP MYSQL========================
=================AUTHOR: AANG RAPE'I, S.Pd., S.Tr.Kom.====================
=============================23-01-2021===================================

============== KONEKSI NODEMCU DAN RFID ==============
 *  NodeMCU---MFRC522
 *  D2      -> SDA (SS)
 *  D5      -> SCK
 *  D7      -> MOSI
 *  D6      -> MISO (SCL)
 *             IRQ
 *  GND     -> GND
 *  D1      -> RST
 *  3V      -> 3.3V
*/
#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h> //Library ESP8266
#include <PubSubClient.h> //Library MQTT

#define SS_PIN D2  
#define RST_PIN D1 

MFRC522 mfrc522(SS_PIN, RST_PIN); 


const char *ssid = "XXX"; // Nama SSID Wifi
const char *pass = "XXX"; //Password Wifi

const char *mqtt_server = "broker.hivemq.com"; // Server MQTT
const int mqtt_port = 1883; //Port MQTT
const char *mqttuser = ""; //Username MQTT
const char *mqttpass = ""; //Password MQTT



//==========KONFIGURASI MQTT CLIENT=================
WiFiClient espclient; 
PubSubClient client(mqtt_server,mqtt_port,espclient);

void setup() {
  Serial.begin(115200); //memulai Serial Monitor
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  
  //========MEMULAI KONEKSI WIFI =======
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) { //menunggu sampai menyambung
    delay(1000);
    Serial.println("Menyambungkan Ke..");
  }
  Serial.println("Tersambung ke Wifi ");
  client.setServer(mqtt_server, mqtt_port); //mengeset sambungan server 
}

void reconnect() { //koneksi ulang
  while (!client.connected()) {
    Serial.println();
    Serial.println("Menyambungkan ke MQTT ");
    if (client.connect("ESP8266Client", mqttuser, mqttpass )) {
      Serial.println("tersambung");
    } else {
      Serial.print("Gagal ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void loop(){

  if (! mfrc522.PICC_IsNewCardPresent()){
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()){
    return;
  }
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++){
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  
  if (!client.connected()) {  //jika client tidak tersambung sambungkan ulang
    reconnect();
  }

client.publish("dari_alat", String(content.substring(1)).c_str(), false); //Publish ke MQTT Broker
client.loop(); //ulangi
delay(200);
}
