#include <Servo.h> 
#include <SoftwareSerial.h>

#define RXp 11
#define TXp 10
#define MOTOR_PIN 7
#define RELAY_PIN 8
#define SOIL_SENSOR A4
#define DELAY_TIME 10000
#define MOISTURE_THRESHOLD 600

SoftwareSerial mySerial(RXp, TXp);
Servo myservo;

int CDS_Pins[] = {A0, A1, A2, A3}; 
int CDS_Values[4] = {0};
char command;

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600);
  
  for(int i = 0; i < 4; i++) {
    pinMode(CDS_Pins[i], INPUT);
  }
  pinMode(SOIL_SENSOR, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  
  myservo.attach(MOTOR_PIN);
  myservo.write(0);
  digitalWrite(RELAY_PIN, HIGH);
}

void loop() {
  if (mySerial.available() > 0) {
    command = mySerial.read();
    //Serial.println(command);
    handleCommand(command);
  } else {
    static unsigned long lastMillis = 0;
    if (millis() - lastMillis >= DELAY_TIME) {
      autoPump();
      motorPhotoControl();
      mySerial.println("=====================");
      Serial.println("=====================");
      lastMillis = millis();
    }
  }
}

void handleCommand(char command) {
  switch (command) {
    case 'L':
      Serial.println("Pump start!");
      digitalWrite(RELAY_PIN, LOW);
      break;
    case 'H':
      Serial.println("Pump stop!");
      digitalWrite(RELAY_PIN, HIGH);
      break;
    case 'A':
      myservo.write(90);
      Serial.println("90");
      break;
    case 'B':
      myservo.write(135);
      Serial.println("135");
      break;        
    case 'C':
      myservo.write(180);
      Serial.println("180");
      break;
    case 'D':
      myservo.write(0);
      Serial.println("0");
      break;
  }
  delay(15);
}

void autoPump() {
  int soilVal = analogRead(SOIL_SENSOR);
  mySerial.print("soil");
  mySerial.print(": ");
  mySerial.println(soilVal);
  Serial.print("soil");
  Serial.print(": ");
  Serial.println(soilVal);
  if (soilVal < MOISTURE_THRESHOLD) {
     mySerial.println("Pump stop!");
     Serial.println("Pump stop!");
    digitalWrite(RELAY_PIN, HIGH);
  } else {
    digitalWrite(RELAY_PIN, LOW);
    mySerial.println("Pump start!");
    Serial.println("Pump start!");
  }
  delay(1);
}

void motorPhotoControl() {
  for (int i = 0; i < 4; i++) {
    CDS_Values[i] = analogRead(CDS_Pins[i]);
    mySerial.print("Pho");
    mySerial.print(i + 1);
    mySerial.print(": ");
    mySerial.println(CDS_Values[i]);
    Serial.print("Pho");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.println(CDS_Values[i]);
    delay(500);
  }
  
  if (CDS_Values[2] < min(CDS_Values[0], min(CDS_Values[1], CDS_Values[3]))) {
    myservo.write(90);
  } else if (CDS_Values[1] < min(CDS_Values[0], min(CDS_Values[2], CDS_Values[3]))) {
    myservo.write(135);
  } else if (CDS_Values[3] < min(CDS_Values[0], min(CDS_Values[2], CDS_Values[1]))) {
    myservo.write(45);
  } else if (CDS_Values[4] < min(CDS_Values[1], min(CDS_Values[2], CDS_Values[3]))) {
    myservo.write(180);
  }
}

