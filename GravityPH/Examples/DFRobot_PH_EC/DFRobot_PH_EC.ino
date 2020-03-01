/*
 * file DFRobot_PH_EC.ino
 * @ https://github.com/DFRobot/GravityPH
 *
 * This is the sample code for The Mixed use of two sensors: 
 *       1、Gravity: Analog pH Sensor / Meter Kit V2, SKU:SEN0161-V2
 *       2、Analog Electrical Conductivity Sensor / Meter Kit V2 (K=1.0), SKU: DFR0300.
 * In order to guarantee precision, a temperature sensor such as DS18B20 is needed, to execute automatic temperature compensation.
 * You can send commands in the serial monitor to execute the calibration.
 * Serial Commands:
 *
 *  PH Calibration：
 *   enterph -> enter the PH calibration mode
 *   calph   -> calibrate with the standard buffer solution, two buffer solutions(4.0 and 7.0) will be automaticlly recognized
 *   exitph  -> save the calibrated parameters and exit from PH calibration mode
 *
 *  EC Calibration：
 *   enterec -> enter the EC calibration mode
 *   calec   -> calibrate with the standard buffer solution, two buffer solutions(1413us/cm and 12.88ms/cm) will be automaticlly recognized
 *   exitec  -> save the calibrated parameters and exit from EC calibration mode
 *
 * Copyright   [DFRobot](http://www.dfrobot.com), 2018
 * Copyright   GNU Lesser General Public License
 *
 * version  V1.0
 * date  2018-04
 */

#include "GravityPH.h"
#include "GravityTDS.h"
#include <EEPROM.h>

#define PIN_PH A1
#define EC_PIN A2
float  phVoltage,voltageEC,phValue,ecValueRAW,temperature = 25;
GravityPH ph;
GravityTDS ec;

void setup()
{
    Serial.begin(115200);  
    ph.begin();
    ec.begin();
}

void loop()
{
    char cmd[10];
    static unsigned long timepoint = millis();
    if(millis()-timepoint>1000U){                            //time interval: 1s
        timepoint = millis();
        //temperature = readTemperature();                   // read your temperature sensor to execute temperature compensation
        phVoltage = analogRead(PIN_PH)/1024.0*5000;          // read the ph voltage
        phValue    = ph.calculatePH(phVoltage,temperature);       // convert voltage to pH with temperature compensation
        Serial.print("pH:");
        Serial.print(phValue,2);
        voltageEC = analogRead(EC_PIN)/1024.0*5000;
        ecValueRAW    = ec.readEC(voltageEC,temperature);       // convert voltage to EC with temperature compensation
        Serial.print(", EC:");
        Serial.print(ecValueRAW,2);
        Serial.println("ms/cm");
    }
    if(readSerial(cmd)){
        strupr(cmd);
        if(strstr(cmd,"PH")){
            ph.calibration(phVoltage,temperature,cmd);       //PH calibration process by Serail CMD
        }
        if(strstr(cmd,"EC")){
            ec.calibration(voltageEC,temperature,cmd);       //EC calibration process by Serail CMD
        }
    }
}

int i = 0;
bool readSerial(char result[]){
    while(Serial.available() > 0){
        char inChar = Serial.read();
        if(inChar == '\n'){
             result[i] = '\0';
             Serial.flush();
             i=0;
             return true;
        }
        if(inChar != '\r'){
             result[i] = inChar;
             i++;
        }
        delay(1);
    }
    return false;
}

float measureTemperature()
{
  //add your code here to get the temperature from your temperature sensor
}