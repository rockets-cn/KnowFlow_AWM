#include <Wire.h>

/*********************************************************************
* WaterMonitor.ino
*
* Copyright (C)    2017   [DFRobot](http://www.dfrobot.com)
* GitHub Link :https://github.com/DFRobot/watermonitor
* This Library is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Description:
* This sample code is mainly used to monitor water quality
* including ph, temperature, dissolved oxygen, ec and orp,etc.
* 
* Software Environment: Arduino IDE 1.8.2 
* Software download link: https://www.arduino.cc/en/Main/Software
* 
* Install the library file：
* Copy the files from the github repository folder libraries to the libraries
* in the Arduino IDE 1.8.2 installation directory
*
* Hardware platform   : Arduino M0 Or Arduino Mega2560
* Sensor pin: 
* EC  : A1
* PH  : A2
* ORP : A3
* RTC : I2C
* DO  : Serial port Rx(0),Tx(1)
* temperature:D5
*
* SD card attached to SPI bus as follows:
* Mega:  MOSI - pin 51, MISO - pin 50, CLK - pin 52, CS - pin 53
* and pin #53 (SS) must be an output
* M0:   Onboard SPI pin,CS - pin 4 (CS pin can be changed)
*
* author  :  Jason(jason.ling@dfrobot.com)
* version :  V1.0
* date    :  2017-04-06
**********************************************************************/

#include <SPI.h>
#include <Wire.h>
//#include <SD.h>
#include <Arduino.h>
#include "GravitySensorHub.h"
#include <stdlib.h>
//#include "GravityRtc.h"
#include "OneWire.h"
// #include "Debug.h"
#include "Obloq.h"
#include "ArduinoJson.h"
#include <SoftwareSerial.h>

//MQTT连接相关参数
 //const String ssid = "DFRobot-guest";
 //const String password = "dfrobot@2017";
const String ssid = "Rockets.cn";
const String password = "hellorockets!";
const String client_id = "b52b137510ff72fb";
const String iot_id = "BkfNcRGdW";
const String iot_pwd = "SyxfN5CzOW";
//const String topic = "BJpHJt1VW";

SoftwareSerial softSerial(10, 11);         // RX, TX
bool sendFlag = true;
bool connectMqtt = false;


void handleRaw(const String& data)
{
	Serial.println(data);   //串口打印返回的数据
}
void handleJson(JsonObject& data)
{
	if(data["mqttState"] == 1)
	{      
		connectMqtt = true; 
		return;
	}
}

Obloq olq(softSerial, ssid, password);

//时钟模块
//GravityRtc rtc;

//传感器监视器
GravitySensorHub sensorHub;

void setup() {
	softSerial.begin(9600);
	Serial.begin(9600);
	// rtc.setup();
	sensorHub.setup();
	olq.setHandleRaw(handleRaw);
	olq.setHandleJson(handleJson);
	//Serial.println("begin");

}


//********************************************************************************************
// 函数名称: sensorHub.getValueBySensorNumber(0)
// 函数说明：获取传感器的数值，不同的参数代表获取不同的传感器数据     
// 参    数: 0  ph值  
// 参    数: 1  温度值    
// 参    数: 2  溶解氧
// 参    数: 3  电导率
// 参    数: 4  氧化还原电位
// 返 回 值: 返回一个double类型的数据 
//********************************************************************************************

unsigned long updateTime = 0;
unsigned long printTime = 0;

void loop() {
	olq.update();
	sensorHub.update();
	if (sendFlag && olq.getWifiState() == 2)
	{
		sendFlag = false;
		olq.connect(client_id, iot_id, iot_pwd);
		return;
	}
	if(connectMqtt && millis() - updateTime > 10000)
	{
		updateTime = millis();
		olq.publish("r1xrc0fO-",(String)(sensorHub.getValueBySensorNumber(0)));
		olq.publish("rJ2U9RGdW",(String)(sensorHub.getValueBySensorNumber(3)));
		olq.publish("rJQv9AfuZ",(String)(sensorHub.getValueBySensorNumber(1)));
    olq.publish("BkH6IlQuW",(String)(sensorHub.getValueBySensorNumber(4)));
	}
	// if(millis() - printTime > 1000)
	// {
	// 	printTime = millis();
	// 	Serial.println("hello world");
	// }
	//rtc.update();
	
}


	//*************************串口调试******************
	/*Serial.print(F("ph= "));
	Serial.print(sensorHub.getValueBySensorNumber(0));
	Serial.print(F("  Temp= "));
	Serial.print(sensorHub.getValueBySensorNumber(1));
	Serial.print(F("  Do= "));
	Serial.print(sensorHub.getValueBySensorNumber(2));
	Serial.print(F("  Ec= "));
	Serial.println(sensorHub.getValueBySensorNumber(3));
	Serial.print(F("  Orp= "));
	Serial.println(sensorHub.getValueBySensorNumber(4));*/
