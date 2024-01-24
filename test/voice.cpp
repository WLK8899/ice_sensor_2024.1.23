/*
Arduino Uno 驱动HC-SR04 超声波测距传感器模块
 
Created 2014
by 太极创客
http://www.taichi-maker.com
 
使用Arduino Uno驱动HC-SR04超声波测距传感器模块
程序运行后，传感器将感应到的距离信息通过Arduino IDE的串口监视器显示。
 
接线方法:
HC-SR04 引脚 VCC 连接到 Arduino 引脚 +5VDC
HC-SR04 引脚 Trig 连接到  Arduino 引脚 11
HC-SR04 引脚 Echo 连接到  Arduino 引脚 12
HC-SR04 引脚 GND 连接到  Arduino 引脚 GND
 
获得HC-SR04 超声波测距传感器模块和Arduino的更多信息
请参阅太极创客网站：http://www.taichi-maker.com
 
This example code is in the public domain.
 
 */
#include<Arduino.h>

int trigPin = 18;    //Trig
int echoPin = 23;    //Echo
long duration, cm, inches;
 
void setup() {
  //Serial Port begin
  Serial.begin (115200);
  //Define inputs and outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}
 
void loop()
{
 
 
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(20);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(20);
  digitalWrite(trigPin, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  duration = pulseIn(echoPin, HIGH);
 
  // convert the time into a distance
  cm = (duration/2) / 29.1;
  inches = (duration/2) / 74; 
  
  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  
  delay(10);
}