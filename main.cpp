#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

//Wifi credentials
const char* ssid = "Priscom Net-Tech";
const char *password = "Prisdika@123";

//ThingSpeak configuration
const char *thingsSpeakAPIKey = "N6XTZ2ADS1VKYF07";

//Uno Communication
HardwareSerial UnoSerial(2);

float temperature= 0.0;
float humidity = 0.0;
float distance = 0.0;
int motion = 0;
bool hasNewData = false;

void connectWiFi(){
    Serial.println("Connecting to Wifi....");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }

    Serial.println("WiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

void setup(){
    Serial.begin(115200);
    
    //UART2 communication with Arduino Uno
    UnoSerial.begin(9600, SERIAL_8N1, 16, 17);

    Serial.println("========================");
    Serial.println("Smart Room ESP32 Gateway");
    Serial.println("========================");

    connectWiFi();
}

void loop(){  
    if(UnoSerial.available()){
        String data = UnoSerial.readStringUntil('\n');
        data.trim();

        if(data.length() > 0){
            Serial.print("Recieved from UNO: ");
            Serial.println(data);
        
            int firstComma = data.indexOf(',');
            int secondComma = data.indexOf(',', firstComma + 1);
            int thirdComma = data.indexOf(',', secondComma + 1);

            if(firstComma > 0 && secondComma > firstComma && thirdComma > secondComma){
                temperature= data.substring(0, firstComma).toFloat();
                humidity = data.substring(firstComma + 1, secondComma).toFloat();
                distance = data.substring(secondComma + 1, thirdComma).toFloat();
                motion = data.substring(thirdComma + 1).toInt();

                hasNewData = true;
                Serial.printf("Parsed -> Temp: %.2f C | Hum: %.2f %% | Dist: %.2f cm | Motion: %d\n",
                              temperature, humidity, distance, motion);
            }else{
                Serial.print("Raw unparsed serial output from Uno: ");
                Serial.println(data);
            }
        }
    }

    if (hasNewData){

        if(WiFi.status() == WL_CONNECTED){
           HTTPClient http;
           
            String url = "https://api.thingspeak.com/update?api_key=" +String(thingsSpeakAPIKey);
            url += "&field1=" + String(temperature);
            url += "&field2=" + String(humidity);
            url += "&field3=" + String(distance);
            url += "&field4=" + String(motion);

            http.begin(url);
            int httpResponseCode = http.GET();

            if(httpResponseCode > 0){c
                String response = http.getString();
                Serial.printf("ThingSpeak Response: %d | Entry ID: %s\n", httpResponseCode, response.c_str());
            }else{ 
                Serial.printf("ThingSpeak HTTP Error: %s\n", http.errorToString(httpResponseCode).c_str());
            }
            http.end();
            hasNewData = false;
        }else{
            Serial.println("WiFi disconnected. Reconnecting.....");
            connectWiFi();
        }
    }
}
