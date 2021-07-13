/**
   PostHTTPClient.ino

    Created on: 21.11.2016

*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

/* this can be run with an emulated server on host:
        cd esp8266-core-root-dir
        cd tests/host
        make ../../libraries/ESP8266WebServer/examples/PostServer/PostServer
        bin/PostServer/PostServer
   then put your PC's IP address in SERVER_IP below, port 9080 (instead of default 80):
*/
//#define SERVER_IP "10.0.1.7:9080" // PC address with emulation on host
#define SERVER_IP "192.168.1.62:3000"

#ifndef STASSID
#define STASSID "Dexter"
#define STAPSK  "32649971"
#endif
  int httpCode;
  int stateAlarm;
  int SENSOR = 2;
  int ALARMA = 4;
  int DOORS = 14;
  int DESACTIVER = 12;
void setup() {
  pinMode(SENSOR, INPUT_PULLUP);
  pinMode(DESACTIVER, INPUT);
  pinMode(ALARMA, OUTPUT);
  pinMode(DOORS, OUTPUT);

  digitalWrite(DOORS, LOW);
  digitalWrite(ALARMA, LOW);
  
  Serial.begin(115200);
  stateAlarm = 'F';
  Serial.println();
  Serial.println();
  Serial.println();

  WiFi.begin(STASSID, STAPSK);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  // wait for WiFi connectionç
    int pushButtonState;
    int desactiveState;
    pushButtonState = digitalRead(SENSOR);
    desactiveState = digitalRead(DESACTIVER);
    
   if(pushButtonState == LOW){
     Serial.print("Se Activó el pulsador Alarma, enviando mensaje de alerta");
     digitalWrite(ALARMA, HIGH);
     stateAlarm = 'T';
     delay(500);
    }
      if(desactiveState == LOW){
         digitalWrite(ALARMA, LOW);
         digitalWrite(DOORS, LOW);

           stateAlarm = 'F';    
       }
    
      
    
  if ((WiFi.status() == WL_CONNECTED)) {
    WiFiClient client;
    HTTPClient http;
    Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    http.begin(client, "http://" SERVER_IP "/postplain/"); //HTTP
    http.addHeader("Content-Type", "application/json");
    //datasServer = http.GET();
    Serial.print("[HTTP] POST...\n");
    // start connection and send HTTP header and body
    if(digitalRead(4) == HIGH || (stateAlarm == 'T')){
      httpCode = http.POST("{\"Security_room_alarm\":\"Enable\", \"DOORS\": \"CLOSED\"}");
      }else{
        if(digitalRead(DOORS) == HIGH){
           httpCode = http.POST("{\"Security_room_alarm\":\"Disable\", \"DOORS\": \"CLOSED\"}");            
          }else{
           httpCode = http.POST("{\"Security_room_alarm\":\"Disable\", \"DOORS\": \"OPENED\"}");                        
          }
      }
   
    // httpCode will be negative on error
    if (httpCode > 0 && (digitalRead(4) == HIGH)) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        Serial.println("received payload:\n<<");
        Serial.println(payload);   
        if(payload == "CLOSE_DOORS"){
           Serial.print("Cerrando Puertas de seguridad");
           digitalWrite(DOORS, HIGH);
          }
          
        Serial.println(">>");
      }
    } else {
      if(digitalRead(DOORS) == HIGH && (digitalRead(4) == LOW)){
         Serial.print("Security Room is Safe with DOORS Closed but Alarm desactivate!");  
        }
      if(digitalRead(DOORS) == HIGH && (digitalRead(4) == HIGH)){
         Serial.print("Security Room is Safe with DOORS Closed but Alarm ACTIVATE!");  
        }
      //Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    
     

    http.end();
  }

  delay(500);
}
