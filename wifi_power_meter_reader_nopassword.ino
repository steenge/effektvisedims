#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char* ssid = "dit_ssid";
const char* password = "din_wpa_kode";

String serverName = "http://192.168.1.190:1880/storepower1.php";

unsigned long pwr;
bool newData;
float prevMillis;
float currentMillis;


void ICACHE_RAM_ATTR ISR() {
  currentMillis = millis();
  newData = true;  
}


void setup() {
  Serial.begin(115200); 

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");

   pinMode(14, INPUT);
   pinMode(BUILTIN_LED, OUTPUT);  

  attachInterrupt(digitalPinToInterrupt(14), ISR, FALLING);

}

void loop() {
  // Send an HTTP POST request depending on timerDelay
  if (newData) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;

      digitalWrite(BUILTIN_LED, LOW); 

      Serial.print("time = ");
      Serial.println(currentMillis-prevMillis);

      Serial.print("3600/(currentMillis-prevMillis) = ");
      Serial.println(3600/(currentMillis-prevMillis));
      //pwr = currentMillis-prevMillis;

      pwr = long(1000*(3600/(currentMillis-prevMillis)));
  
      Serial.print("pwr = ");
      Serial.println(pwr);
      

      prevMillis = currentMillis;

      // t= tid siden sidste puls i sekunder
      String serverPath = serverName + "?t=" + String(pwr);

      Serial.print("serverPath = ");
      Serial.println(serverPath);

     
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverPath.c_str());
      
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    newData = false;
    digitalWrite(BUILTIN_LED, HIGH); 

  }
}
