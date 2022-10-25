#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <AccelStepper.h>

const char* ssid = "SSID";
const char* password = "WPAKEY";

int prevValue = 0;
bool highAlert = false;

#define greenled 13 // D7
#define redled   15 // D8

const int stepsPerRevolution = 2048;  // change this to fit the number of steps per revolution

// ULN2003 Motor Driver Pins
#define IN1 5
#define IN2 4
#define IN3 14
#define IN4 12

AccelStepper stepper(AccelStepper::HALF4WIRE, IN1, IN3, IN2, IN4);

String serverName = "http://192.168.1.190:1880/data.php";

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

  pinMode(greenled, OUTPUT);  
  pinMode(redled, OUTPUT);  

  stepper.setMaxSpeed(2000);
  stepper.setAcceleration(1000);
  stepper.moveTo(0);
}

void setDisplayColor()
{
  if (highAlert == false)
  {
    digitalWrite(greenled, LOW);  
    digitalWrite(redled, HIGH);  
  }  
  else
  {
    digitalWrite(redled, LOW);  
    digitalWrite(greenled, HIGH);  
  }  
  
}

long calcPosition(float val)
{
  float tmp;
  if (val <= 300) {tmp = val * (map(val, 0, 300, 0, 130) / 100.0); highAlert = false;}
  else if (val <= 400 && val > 300 ) {tmp = val * (map(val, 300, 400, 130, 187) / 100.0); highAlert = false;}
  else if (val <= 600 && val > 400 ) {tmp = val * (map(val, 400, 600, 187, 180) / 100.0); highAlert = false;}
  else if (val <= 800 && val > 600 ) {tmp = val * (map(val, 600, 800, 180, 175) / 100.0); highAlert = false;}
  else if (val <= 1000 && val > 800 ) {tmp = val * (map(val, 800, 1000, 175, 176) / 100.0); highAlert = false;}
  else if (val <= 1500 && val > 1000 ) {tmp = val * (map(val, 1000, 1500, 176, 140) / 100.0); highAlert = false;}
  else if (val <= 2000 && val > 1500 ) {tmp = val * (map(val, 1500, 2000, 140, 120) / 100.0); highAlert = false;}
  else if (val <= 3000 && val > 2000 ) {tmp = val * (map(val, 2000, 3000, 120, 90)  / 100.0); highAlert = false;}
  else if (val <= 4000 && val > 3000 ) {tmp = val * (map(val, 3000, 4000, 90, 77) / 100.0); highAlert = true;}
  else if (val <= 5000 && val > 4000 ) {tmp = val * (map(val, 4000, 5000, 77, 69) / 100.0); highAlert = true;}
  else if (val > 5000 && val <= 6000) {tmp = val * 0.625; highAlert = true;}
  else if (val > 6000) {tmp = 3750; highAlert = true;}
  return long(round(tmp));  
}

void loop() {
  if (stepper.distanceToGo() != 0)
  {
        stepper.run();
  }
  else if(WiFi.status()== WL_CONNECTED){
    stepper.disableOutputs();
    WiFiClient client;
    HTTPClient http;
    String serverPath = serverName;
    // Your Domain name with URL path or IP address with path
    http.begin(client, serverPath.c_str());
    
    // Send HTTP GET request
    int httpResponseCode = http.GET();
    
    if (httpResponseCode>0) {
      String payload = http.getString();

      Serial.print("payload: ");
      Serial.println(payload.toInt());
  
      if (abs(payload.toInt() - prevValue) > 0)
      {
        stepper.enableOutputs();  
        Serial.print("MoveTo: ");
        Serial.println(calcPosition(payload.toInt()));
        stepper.moveTo(calcPosition(payload.toInt()));
        setDisplayColor();
        prevValue = payload.toInt();
      }
   }
 else 
   {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
   }
   // Free resources
   http.end();
   delay(500);
 }
else 
 {
    Serial.println("WiFi Disconnected");
 }
}
