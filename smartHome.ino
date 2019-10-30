//대림대학교 컴퓨터 정보학부
//Daelim University College dept.Computer Information

//블루투스 정보
//'1' 차고 모터 제어

#include <SoftwareSerial.h>
#include <Wire.h>
#include "DHT.h"
#include <LiquidCrystal_I2C.h>  // Library for LCD
#include "pitches.h"
#include <Servo.h>
LiquidCrystal_I2C TV(0x27, 16, 2);  // I2C LCD -> TV
#define DHTPIN 2 //DHT seonsor pin
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

Servo servo;

void bedTone();


//기본적인 센서 핀
int piezoPin = 8; // Piezo buzzer
int irPin = A3;  // IR sensor
int cdsPin = A2; //cds sensor
int swsv = 7; //Switch for Servomoter
int angle = 0; //servo moter angle

//cds 센서로 작동되는 RGB LED
int led_blue = 9;
int led_green = 10;
int led_red = 11;

//피에조 부저 음계
int C = 1047; // 도
int D = 1175; // 레 
int E = 1319; // 미 
int F = 1397; // 파 
int G = 1568; // 솔 
int A = 1760; // 라 
int B = 1976; // 시
int notes[25] = { G, G, A, A, G, G, E, G, G, E, E, D, G, G, A, A, G, G, E, G, E, D, E, C };
//학교종이 땡땡땡 멜로디

int tempo = 200;

boolean lightOnOff = 0; // 0 = Off, 1 = on
boolean flag = 1;
boolean flag2 = 1;
int numTones = 5;
//int notes[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, 0, NOTE_B3, NOTE_C4};
//int noteDurations[] = {4, 8, 8, 4, 4, 4, 4, 4};
// 가까울 시 울릴 경보의 멜로디 작성

SoftwareSerial BTSerial(4,5);

void setup()
{
  // 시리얼 통신을 위해 통신속도(Baudrate)를 9600으로 설정
  Serial.begin(9600);
  BTSerial.begin(9600); // 블루투스 통신을 위해 통신속도를 9600으로 설정
  
  pinMode(piezoPin, OUTPUT); // 피에조 핀을 출력핀으로 설정
  dht.begin();
  TV.begin(); // I2C LCD의 기본 설정
  
  TV.backlight();  // I2C LCD의 백라이트를 켜줌
  TV.setCursor(0, 0); // I2C LCD의 커서위치를 0, 0으로 설정(첫번째 줄, 첫번째 칸)
  servo.attach(12);
  
  pinMode(swsv, INPUT_PULLUP);
  pinMode(led_red, OUTPUT);
  pinMode(led_green, OUTPUT);
  pinMode(led_blue, OUTPUT);
}

void loop()
{
  
 // data 선언 , 블루투스로 들어오는 통신을 읽어 저장
    
  digitalWrite(led_red, LOW);
  digitalWrite(led_green, LOW);
  digitalWrite(led_blue, LOW);

  //TV.clear();
  int cdsValue = analogRead(cdsPin);
  Serial.print("조도 값 : "); //cds Value index
  Serial.println(cdsValue);
  int IRval = analogRead(irPin);
  float humidity = dht.readHumidity(); //humidity - 습도 측정 val
  float temperature = dht.readTemperature(); //temperature - 온도 측정 val
  if(isnan(humidity) || isnan(temperature) ) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  Serial.print((int)temperature);
  Serial.print("*C, ");
  Serial.print((int)humidity);
  Serial.println(" %");

    if(BTSerial.available()){
    byte data = BTSerial.read();
  if(data == '1' && flag2 == 1)
  {
    flag2 = 0;
    data = '0';
    servo.write(180);
    delay(1000);
  }
  else if(data == '1'&& flag2 == 0)
  {
    flag2 = 1;
    data = '0';
    servo.write(0);
    delay(1000);
  }
}
  if(cdsValue > 300) {
    bedTone();
    lightOnOff = 0;
   
    digitalWrite(led_red,LOW);
    digitalWrite(led_green,LOW);
    digitalWrite(led_blue,LOW);
    delay(300);
    }
    
    
    else if(cdsValue < 300){
    lightOnOff = 1;
    
    digitalWrite(led_red,HIGH);
    digitalWrite(led_blue,HIGH);
    digitalWrite(led_green,HIGH);
    delay(300);
    }


  if (IRval < 100 && flag == 1) {          //TV가 켜져있고, 거리가 가깝다면 TV OFF
    flag = 0;
    TV.clear();
    TV.noBacklight();
    tone(piezoPin, 600, 1000);
    delay(100);
  }
  else if (IRval > 900 && flag == 0) {    //TV가 꺼져있고, 거리가 멀다면 TV ON
    noTone(piezoPin);
    flag = 1;
    TV.backlight();
    TV.print("hum : ");
    TV.print(humidity);
    TV.print(" %");
    TV.setCursor(0,1);
    TV.print("temp : ");
    TV.print(temperature);
    TV.print(" C");
  }
  }


void bedTone() {
  for(int i = 0; i < 12; i++) {
    tone(piezoPin, notes[i], tempo);
    delay(300);
    delay(3);
  }
}
