#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
// #include<DHT.h>
// #define DHTPIN D2
// #define DHTTYPE DHT11 
 
// DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "My_Station";    // name of your wifi network!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
const char* password = "myhome@123";     // wifi pasword !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
const char* host = "script.google.com";
const int httpsPort = 443;
// Use WiFiClientSecure class to create TLS connection
WiFiClientSecure client;
// SHA1 fingerprint of the certificate, don't care with your GAS service
const char* fingerprint = "d4ffb06f904c4c1937b518e0dce5d67c736fefe7712fe4cfb8e47b0242e22de6";
String GAS_ID = "AKfycbyQcWjML_yabWGvLz35b9UdZ6JiUQ-cNOIUzt-p-wrg4pEQbvtGD-qnoHGfpIwiGjvZ";   // Replace by your GAS service id           !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
int dis;


const int trigPin = 12;
const int echoPin = 14;
const int vcc = 13;
long duration ;
int distanceCm;
void setup() 
{
  
  // dht.begin();  // sensor
  Serial.begin(115200); //Serial
  Serial.println();
pinMode(trigPin,OUTPUT);
pinMode(vcc,OUTPUT);
pinMode(echoPin,INPUT);

  //connecting to internet
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
 

}

void loop() 
{
 // float h = dht.readHumidity();
 // float t = dht.readTemperature();
  // Serial.print("Temp = ");
  // Serial.print(t);
  // Serial.print(" HUM= ");
  // Serial.println(h);
  // put your main code here, to run repeatedly:
 digitalWrite(vcc,HIGH);

 digitalWrite(trigPin,LOW);
 delayMicroseconds(2);
 digitalWrite(trigPin,HIGH);
 delayMicroseconds(2);
 digitalWrite(trigPin,LOW);

 duration = pulseIn(echoPin,HIGH);
 distanceCm = duration*0.034/2;
 Serial.println(distanceCm);


   dis = (int) distanceCm;
  
   sendData(dis);
 
 delay(3000);
}

// Function for Send data into Google Spreadsheet
void sendData(int dis)
{
  client.setInsecure();
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  // if (client.verify(fingerprint, host)) {
  // Serial.println("certificate matches");
  // } else {
  // Serial.println("certificate doesn't match");
  // }
  String string_distance =  String(dis, DEC); 
  // String string_humidity =  String(hum, DEC); 
  String url = "/macros/s/" + GAS_ID + "/exec?data=" + string_distance;
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Connection: close\r\n\r\n");

  Serial.println("request sent");
  while (client.connected()) {
  String line = client.readStringUntil('\n');
  if (line == "\r") {
    Serial.println("headers received");
    break;
  }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
  Serial.println("esp8266/Arduino CI successfull!");
  } else {
  Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");
} 