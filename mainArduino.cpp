#include <Arduino.h>
#include <DHT.h>
#include <SoftwareSerial.h>

//DHT11
#define DHTPIN 2
#define DHTTYPE DHT11

//PIR
#define PIR_PIN 3

//Ultrasonic
#define TRIG_PIN 4
#define ECHO_PIN 5 

//Buzzer
#define BUZZER_PIN 6

//ESP32 Communication
SoftwareSerial espSerial(10, 11);

DHT dht(DHTPIN, DHTTYPE);

float temperature = 0;
float humidity = 0;
float distance = 0;
int motion = 0;

void setup(){
    Serial.begin(9600);
    espSerial.begin(9600);
    dht.begin();

    pinMode(PIR_PIN, INPUT);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);

    digitalWrite(BUZZER_PIN, LOW);
    Serial.println("Smart Room Monitoring Started");
}

float getDistance(){
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH);

    if (duration == 0){
        return 0.0;
}
float distanceCm = (duration * 0.0343) / 2.0;
return distanceCm;
}  

void loop(){
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();
    distance = getDistance();
    motion = digitalRead(PIR_PIN);

    Serial.print(temperature);
    Serial.print(",");

    Serial.print(humidity);
    Serial.print(",");

    Serial.print(distance);
    Serial.print(",");

    Serial.println(motion);

    if(distance < 20 || motion == 1){
        digitalWrite(BUZZER_PIN, HIGH);
    }else{
        digitalWrite(BUZZER_PIN, LOW);
    }

    delay(20000);
}
