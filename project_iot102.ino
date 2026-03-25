#include <SoftwareSerial.h>
#include <DHT.h>
#include <Servo.h>

// --- ĐỊNH NGHĨA CHÂN ---
#define DHTPIN 3
#define DHTTYPE DHT11
#define MQ2 A0
#define FLAME 2
#define SERVO_PIN 5
#define RELAY_PIN 12
#define BUZZER 13
#define LED_GREEN 9
#define LED_YELLOW 10
#define LED_RED 11

DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial sim800(7, 8); 
Servo door;
const String PHONE_NUMBER = "0xxxxxxxxx"; // Thay số điện thoại của bạn
unsigned long lastCallTime = 0;

void setup() {
  Serial.begin(9600);    
  sim800.begin(9600);   
  dht.begin();
  
  pinMode(FLAME, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  
  digitalWrite(RELAY_PIN, HIGH); 
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
  digitalWrite(BUZZER, LOW);
  
  door.attach(SERVO_PIN);
  door.write(0); 

  digitalWrite(BUZZER, HIGH);
  delay(150);
  digitalWrite(BUZZER, LOW);
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int smoke = analogRead(MQ2);
  int fire = digitalRead(FLAME);

  if (fire == LOW || t > 45 || smoke > 400) { 
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, HIGH);
    digitalWrite(BUZZER, HIGH);
    
    digitalWrite(RELAY_PIN, LOW);
    door.write(90);
    
    if ((fire == LOW || smoke > 400 || t > 45) && (millis() - lastCallTime > 60000)) {
      sim800.println("ATD" + PHONE_NUMBER + ";");
      lastCallTime = millis();
    }
  } 
  else if (smoke > 250 || t > 35) { 
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, HIGH);
    digitalWrite(LED_RED, LOW);
    digitalWrite(BUZZER, LOW);
    digitalWrite(RELAY_PIN, HIGH);
  } 
  else { 
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);
    digitalWrite(BUZZER, LOW);
    digitalWrite(RELAY_PIN, HIGH);
    door.write(0);
  }

  Serial.print("T"); Serial.print(t);
  Serial.print("H"); Serial.print(h);
  Serial.print("S"); Serial.println(smoke);

  if (Serial.available() > 0) {
    char cmd = Serial.read();
    if (cmd == 'O') door.write(90);
    if (cmd == 'C') door.write(0);
  }

  delay(1000); 
}