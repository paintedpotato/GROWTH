// Humidity has been set to 25%

// Get rid of all extra delays Brian has added
// may affect the count
/* User can't modify it
 *  User can modify light via PWM
 *  and water eventually
 *  
 *  Assuming all lights in a single bath are connected to one 
 *  digital output pin
 */



/*************************************************************
  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Social networks:            http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example shows how to use Arduino with HC-06/HC-05
  Bluetooth 2.0 Serial Port Profile (SPP) module
  to connect your project to Blynk.

  Note: This only works on Android! iOS does not support SPP :(
        You may need to pair the module with your smartphone
        via Bluetooth settings. Default pairing password is 1234

  Feel free to apply it to any other example. It's simple!

  NOTE: Bluetooth support is in beta!

 *************************************************************/

#define BLYNK_USE_DIRECT_CONNECT

// You could use a spare Hardware Serial on boards that have it (like Mega)
#include <SoftwareSerial.h>
SoftwareSerial DebugSerial(0, 1); // RX, TX

#define BLYNK_PRINT DebugSerial
#include <BlynkSimpleSerialBLE.h>
#include <Wire.h>

// GROWTH
#include "Arduino.h"
#include "SI114X.h"
#include <dht.h>
#include <Servo.h>

dht DHT;
SI114X SI1145 = SI114X();

// PIN DEFINITIONS
// No Sunlight sensor pin required. Uses SDA and SCL

// LED pins batchwise
#define LED_1 13          // REMEMBER TO INPUT THESE VALUES
#define LED_2 12
#define LED_3 11


// Soil moisture sensor pins
#define SOIL_1 A0
#define SOIL_2 A1
#define SOIL_3 A2

// Air temp and humidity sensors
#define DHT11_1 48
#define DHT11_2 50
#define DHT11_3 52

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "RDWOEFVykni-X7ZxjVnXTI_CqjMYBrd5";

int sensorValue1;
int sensorValue2;
int sensorValue3;
int IR;
int count;

int stepperWatering1 = 0;
int stepperLighting1 = 0;
int stepperFertilizer1 = 0;
int stepperDehumidifier1 = 0;

int stepperWatering2 = 0;
int stepperLighting2 = 0;
int stepperFertilizer2 = 0;
int stepperDehumidifier2 = 0;

int stepperWatering3 = 0;
int stepperLighting3 = 0;
int stepperFertilizer3 = 0;
int stepperDehumidifier3 = 0;


// IRRIGATION CODE

// Dehumidifier
#define dehumidRelay 31
#define humidVal1 25 //       OPTIMAL VALUES FOR HUMIDITY
#define humidVal2 25
#define humidVal3 25
#define humidMin1 25
#define humidMin2 25
#define humidMin3 25

int batchON = 0;


// Servos
#define servoB1pin 3
#define servoB2pin 4
#define servoB3pin 13

#define batch1ang 60
#define batch2ang 70
#define batch3ang 55

Servo servoB1;
Servo servoB2;
Servo servoB3;


//IRRIGATION SECTION

// Water sensor Reading; The dryer the higher up value; bone dry 600; flooded is 270;
int waterLevel_1=0;   // DON'T USE NULL VALUES
int waterLevel_2=0;  
int waterLevel_3=0;  

// minimum water moisture
int setWaterLevel_1 = 0;
int setWaterLevel_2 = 0;
int setWaterLevel_3 = 0;

// Time between each fetiliser session in s
long fertiTime1 =1*60; 
long fertiTime2 =1*60*5; 
long fertiTime3 =1*60*5; 

// Time btw water session in s
long waterTime1 = 5;
long waterTime2 = 1*60*5;
long waterTime3 = 1*60*5;


//Record of last execution time variables in ms
unsigned long lastTime1 = 0;
unsigned long lastTime2 = 0;
unsigned long lastTime3 = 0;
unsigned long lastTime4 = 0;
unsigned long lastTime5 = 0;
unsigned long lastTime6 = 0;

//set pin up. a is for water valve. b is for fetilezer 
int pin_1a = 1;       
int pin_1b = 2;
int pin_2a = 3;
int pin_2b = 4;
int pin_3a = 5;
int pin_3b = 6;
int pin_water_pump = 7;
int pin_ferti_pump = 8;


//END OF IRRIGATION SECTION

BlynkTimer timer;



void myTimerEvent()
{
  int sensorData = analogRead(A0);
  Blynk.virtualWrite(V5, millis()/1000);
}

void printTempHumid(int pinDHT11)
{
  int chk = DHT.read11(pinDHT11);
  Serial.print("Temperature = ");
  Serial.println(DHT.temperature);
  Serial.print("Humidity = ");
  Serial.println(DHT.humidity);
}

void printSoilMoisture(int pinSoil)
{
  // read the input on analog pin 0:
  int sensorValue = analogRead(pinSoil);
  // print out the value you read:
  Serial.print("Analogue soil sensor reading = ");
  Serial.println(sensorValue);
  Serial.println(); 
}


void printSunlightInfo()
{
  Serial.print("Vis: "); Serial.println(SI1145.ReadVisible());
  Serial.print("IR: "); Serial.println(SI1145.ReadIR());
  //the real UV value must be div 100 from the reg value , datasheet for more information.
  Serial.print("UV: ");  Serial.println((float)SI1145.ReadUV()/100);
}

BLYNK_CONNECTED(){
  Blynk.syncVirtual(V1);
  Blynk.syncVirtual(V2);
  Blynk.syncVirtual(V3);
  Blynk.syncVirtual(V4);

  Blynk.syncVirtual(V6);
  Blynk.syncVirtual(V7);
  Blynk.syncVirtual(V8);
  Blynk.syncVirtual(V9);

  Blynk.syncVirtual(V10);
  Blynk.syncVirtual(V11);
  Blynk.syncVirtual(V12);
  Blynk.syncVirtual(V13);
}

BLYNK_WRITE(V1){
  stepperWatering1 = param.asInt();
}
BLYNK_WRITE(V2){
  stepperLighting1 = param.asInt();
}
BLYNK_WRITE(V3){
  stepperFertilizer1 = param.asInt();
}
BLYNK_WRITE(V4){
  stepperDehumidifier1 = param.asInt();
}

BLYNK_WRITE(V6){
  stepperWatering2 = param.asInt();
}
BLYNK_WRITE(V7){
  stepperLighting2 = param.asInt();
}
BLYNK_WRITE(V8){
  stepperFertilizer2 = param.asInt();
}
BLYNK_WRITE(V9){
  stepperDehumidifier2 = param.asInt();
}


BLYNK_WRITE(V10){
  stepperWatering3 = param.asInt();
}
BLYNK_WRITE(V11){
  stepperLighting3 = param.asInt();
}
BLYNK_WRITE(V12){
  stepperFertilizer3 = param.asInt();
}
BLYNK_WRITE(V13){
  stepperDehumidifier3 = param.asInt();
}

// Moisture Sensor Batch 1
BLYNK_READ(V5) // Widget in the app READs Virtal Pin V5 with the certain frequency
{
  // This command writes Arduino's uptime in seconds to Virtual Pin V5
  Blynk.virtualWrite(SOIL_1, millis() / 1000);
}

// Humidity Sensor  Batch 1
BLYNK_READ(V14) // Widget in the app READs Virtal Pin V5 with the certain frequency
{
  // This command writes Arduino's uptime in seconds to Virtual Pin V5
  int chk = DHT.read11(DHT11_1);
  Blynk.virtualWrite(DHT.humidity, millis() / 1000);
}

// Sunlight Sensor
BLYNK_READ(V15) // Widget in the app READs Virtal Pin V5 with the certain frequency
{
  // This command writes Arduino's uptime in seconds to Virtual Pin V5
  Blynk.virtualWrite(SI1145.ReadIR(), millis() / 1000);
}

/* Latches for dehumidifier */
void dehumidifierOn(int i){
    if (digitalRead(dehumidRelay) != HIGH) {

      if (i == 1){  
        
        int chk = DHT.read11(DHT11_1); //batch 1
        if (DHT.humidity > humidVal1)
        {
          digitalWrite(dehumidRelay, HIGH);
          servoB1.write(batch1ang); 
          batchON = 1;
        }
      }
      
      if (i == 2){
        
        int chk2 = DHT.read11(DHT11_2); // batch 2
        if (DHT.humidity > humidVal1)
        {
          digitalWrite(dehumidRelay, HIGH);
          servoB1.write(batch2ang); 
          batchON = 2;
        }
      }

      if (i == 3){
        
        int chk3 = DHT.read11(DHT11_3); // batch 3
        if (DHT.humidity > humidVal1)
        {
          digitalWrite(dehumidRelay, HIGH);
          servoB1.write(batch3ang); 
          batchON = 3;
        }
      }
    }
    
    else { //if HIGH

      if (i == 1){        // BATCH ONE
        
        int chk4 = DHT.read11(DHT11_1);
        if (batchON = 1) {
          if (DHT.humidity < humidMin1) {
            digitalWrite(dehumidRelay, LOW);
            servoB1.write(0); 
            batchON = 0;
          }
        }
      }

      if (i == 2){      // BATCH TWO
        
        int chk5 = DHT.read11(DHT11_2);
        if (batchON = 2) {
          if (DHT.humidity < humidMin1) {
            digitalWrite(dehumidRelay, LOW);
            servoB2.write(0); 
            batchON = 0;
          }
        }
      }

      if (i == 3){      // BATCH THREE
        
        int chk6 = DHT.read11(DHT11_3);
        if (batchON = 3) {
          if (DHT.humidity < humidMin1) {
            digitalWrite(dehumidRelay, LOW);
            servoB3.write(0); 
            batchON = 0;
          }
        }
      }
    }
} 



/* Irrigation system */
void waterControlLoop1() {
  //Water Control Loop (Batch 1)
  waterLevel_1 = analogRead(SOIL_1);
  //if(( millis() - lastTime1 )>= waterTime1){           //check time since last watering, execute if enough time has passed
  int temp = count + waterTime1;
  while( count >= temp){
    
    //lastTime1 = millis();   //record time of execution
    //Serial.println(lastTime1);
    
    float deltaWater_1 =float ( waterLevel_1 - setWaterLevel_1)/setWaterLevel_1;     //check relative 

    if(deltaWater_1 <= -0.1){
  
      Serial.println("Batch 1. TOO MUCH WATER! ");
      Serial.println(deltaWater_1);
      // add more code here to inform user in mobile app?
 
    }
    else if(deltaWater_1 < 0.1 && deltaWater_1 >-0.1){
        Serial.println("Batch 1 just enough water; do nothing");
        Serial.println(deltaWater_1);
    }
    else{
      //too little water, open valve and activate pump for a short while
      
      Serial.println("Water batch 1");
      Serial.println(deltaWater_1);
      digitalWrite(pin_1a,LOW);
      digitalWrite(pin_water_pump,LOW);
      //-----------------------------------delay(1000);
      digitalWrite(pin_water_pump,HIGH);
      digitalWrite(pin_1a,HIGH);
      
      }
    }
}

void waterControlLoop2() {
//Water Control Loop (Batch 2)
 waterLevel_2 = analogRead(A1);
  //if(( millis() - lastTime3)>= waterTime2){
  int temp = count + waterTime2;
  while( count >= temp){  
    //lastTime3 = millis();   //record time of execution
    
    float deltaWater_2 =float ( waterLevel_2 - setWaterLevel_2)/setWaterLevel_2;

    if(deltaWater_2 <= -0.1){
  
      //Serial.print("TOO MUCH WATER!");
 
    }
    else if(deltaWater_2 < 0.1 && deltaWater_2 >-0.1){
      // just enough water; do nothing 
    }
    else{
      digitalWrite(pin_2a,LOW);
      digitalWrite(pin_water_pump,LOW);
      //-------------------------------------delay(1000);
      digitalWrite(pin_water_pump,HIGH);
      digitalWrite(pin_2a,HIGH);
    }

  } 
} 


void waterControlLoop3() {
  //Water Control Loop (Batch 3)
 waterLevel_3 = analogRead(A2);
  
  int temp = count + waterTime3;
  while( count >= temp){
  //if(( millis() - lastTime5 )>= waterTime3){
    
    //lastTime5 = millis();   //record time of execution
    
    float deltaWater_3 =float ( waterLevel_3 - setWaterLevel_3)/setWaterLevel_3;

    if(deltaWater_3 <= -0.1){
  
      //Serial.print("TOO MUCH WATER!");
 
    }
    else if(deltaWater_3 < 0.1 && deltaWater_3 >-0.1){
      // just enough water; do nothing 
    }
    else{
      digitalWrite(pin_3a,LOW);
      digitalWrite(pin_water_pump,LOW);
      //--------------------------------delay(1000);
      digitalWrite(pin_water_pump,HIGH);
      digitalWrite(pin_3a,HIGH);
      }

    }  
}

void fertiliserControlLoop1() {
  //Fertilizer control loop (Batch 1)
  int temp = count + fertiTime1;
  while( count >= temp){
  //if(( millis()- lastTime2 )>= fertiTime1)    //check time since last fertilizing, execute if enough time has passed
 //{

    //lastTime2 = millis();   //record time of execution
    
    digitalWrite(pin_1b,LOW);
    digitalWrite(pin_ferti_pump,LOW);
    //-------------------------------------delay(1000);
    digitalWrite(pin_ferti_pump,HIGH);
    digitalWrite(pin_1b,HIGH);
   
  }
}

void fertiliserControlLoop2() {
    //Fertilizer control loop (Batch 2)
  //if(( millis()- lastTime4 )>= fertiTime2){
  int temp = count + fertiTime2;
  while( count >= temp){
    
    //lastTime4 = millis();   //record time of execution
    
    digitalWrite(pin_2b,LOW);
    digitalWrite(pin_ferti_pump,LOW);
    //---------------------------------------delay(1000);
    digitalWrite(pin_ferti_pump,HIGH);
    digitalWrite(pin_2b,HIGH);
   
  }
}

void fertiliserControlLoop3() {
   //Fertilizer control loop (Batch 3)
   int temp = count + fertiTime3;
    while( count >= temp){

      //lastTime6 = millis();   //record time of execution
    
      digitalWrite(pin_3b,LOW);
      digitalWrite(pin_ferti_pump,LOW);
      //------------------------------------delay(1000);
      digitalWrite(pin_ferti_pump,HIGH);
      digitalWrite(pin_3b,HIGH);
   
    } 
}

void setup()
{
  // Debug console
  DebugSerial.begin(9600);

  DebugSerial.println("Waiting for connections...");

  // Blynk will work through Serial
  // 9600 is for HC-06. For HC-05 default speed is 38400
  // Do not read or write this serial manually in your sketch
  Serial.begin(9600);
  Blynk.begin(Serial, auth);
  timer.setInterval(1000L, myTimerEvent);

  /* LEDs per batch */
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);

  /* SERVOS */
  servoB1.attach(servoB1pin);
  servoB2.attach(servoB2pin);
  servoB3.attach(servoB3pin);

  servoB1.write(0);
  servoB2.write(0);
  servoB3.write(0); 

  /*IRRIGATION SECTION */

  pinMode(pin_1a,OUTPUT);
  pinMode(pin_1b,OUTPUT);
  pinMode(pin_2a,OUTPUT);
  pinMode(pin_2b,OUTPUT);
  pinMode(pin_3a,OUTPUT);
  pinMode(pin_3b,OUTPUT);
  pinMode(pin_water_pump,OUTPUT);
  pinMode(pin_ferti_pump,OUTPUT);

  digitalWrite(pin_1a,HIGH);
  digitalWrite(pin_1b,HIGH);
  digitalWrite(pin_2a,HIGH);
  digitalWrite(pin_2b,HIGH);
  digitalWrite(pin_3a,HIGH);
  digitalWrite(pin_3b,HIGH);
  digitalWrite(pin_water_pump,HIGH);
  digitalWrite(pin_ferti_pump,HIGH);

//Dehumidifier/servo
digitalWrite(dehumidRelay, LOW);

  
}

void loop()
{

  /* Review these parts */
  
  //IRRIGATION SECTION  
  // read soil moisture input on analog pins:
  //waterControlLoop1();  
  //fertiliserControlLoop1();
  //waterControlLoop2();  
  //fertiliserControlLoop2();
  //waterControlLoop3();  
  //fertiliserControlLoop3();

  /*                      */
   // END IRRIGATION SECTION

    // Dehumidifier and servo actuation

//batch1
/*
    if (digitalRead(dehumidRelay) != HIGH) {
      
        int chk = DHT.read11(DHT11_1); //batch 1
      if (DHT.humidity > humidVal1)
      {
        digitalWrite(dehumidRelay, HIGH);
        servoB1.write(batch1ang); 
        batchON = 1;
      }
      
      int chk2 = DHT.read11(DHT11_2); // batch 2
      if (DHT.humidity > humidVal1)
      {
        digitalWrite(dehumidRelay, HIGH);
        servoB1.write(batch2ang); 
        batchON = 2;
      }
      
      int chk3 = DHT.read11(DHT11_3); // batch 3
      if (DHT.humidity > humidVal1)
      {
        digitalWrite(dehumidRelay, HIGH);
        servoB1.write(batch3ang); 
        batchON = 3;
      }
    }
    
    else { //if HIGH
      int chk4 = DHT.read11(DHT11_1);
      if (batchON = 1) {
        if (DHT.humidity < humidMin1) {
        digitalWrite(dehumidRelay, LOW);
        servoB1.write(0); 
        batchON = 0;
        }
      }

      int chk5 = DHT.read11(DHT11_2);
      if (batchON = 2) {
        if (DHT.humidity < humidMin1) {
        digitalWrite(dehumidRelay, LOW);
        servoB2.write(0); 
        batchON = 0;
        }
      }

      int chk6 = DHT.read11(DHT11_3);
      if (batchON = 3) {
        if (DHT.humidity < humidMin1) {
        digitalWrite(dehumidRelay, LOW);
        servoB3.write(0); 
        batchON = 0;
        }
      }
    }*/



  Blynk.run();

  if( millis()%1 == 0)
  {
    count++;
  }

  Serial.println(count);
  Serial.println(stepperWatering1);
  Serial.println(stepperLighting1);
  Serial.println(stepperFertilizer1);
  Serial.println(stepperDehumidifier1);
  
  Serial.println(stepperWatering2);
  Serial.println(stepperLighting2);
  Serial.println(stepperFertilizer2);
  Serial.println(stepperDehumidifier2);
  
  Serial.println(stepperWatering3);
  Serial.println(stepperLighting3);
  Serial.println(stepperFertilizer3);
  Serial.println(stepperDehumidifier3);
  
  for (int i=1;i<=3;i++)
  {
    if(stepperWatering1 == i)
    {
      switch(stepperWatering1)
      {
        case 1: 
        if (count%10 == 0)
        {
          // Do Watering
          waterControlLoop1();
          Serial.println("I watered once in 10s");
        }
        break;
  
        case 2: 
        if (count%5 == 0)
        {
          // Do Watering
          waterControlLoop1();
          Serial.println("I watered twice in 10s");
        }
        break;
  
        case 3: 
        if (count%3 == 0)
        {
          // Do Watering
          waterControlLoop1();
          Serial.println("I watered thrice in 9s");
        }
        break;
       }
    }

    if(stepperLighting1 == i)
    {
      switch(stepperLighting1)
      {
        case 1: 
        if (count%10 == 0)
        {
          // Do Lighting
          analogWrite(LED_1,85);
          Serial.println("I lighted to minimum brightness");
        }
        break;
  
        case 2: 
        if (count%5 == 0)
        {
          // Do Lighting
          analogWrite(LED_1,170);
          Serial.println("I lighting to medium brightness");
        }
        break;
  
        case 3: 
        if (count%3 == 0)
        {
          // Do Lighting
          analogWrite(LED_1,255);
          Serial.println("I lighted to maximum brightness");
        }
        break;
       }
    }

    if(stepperFertilizer1 == i)
    {
      switch(stepperFertilizer1)
      {
        case 1: 
        if (count%10 == 0)
        {
          // Do Fertilizer
          fertiliserControlLoop1();
          Serial.println("I fertilizered once in 10s");
        }
        break;
  
        case 2: 
        if (count%5 == 0)
        {
          // Do Watering
          fertiliserControlLoop1();
          Serial.println("I fertilizered twice in 10s");
        }
        break;
  
        case 3: 
        if (count%3 == 0)
        {
          // Do Fertilizer
          fertiliserControlLoop1();
          Serial.println("I fertilizered thrice in 9s");
        }
        break;
       }
    }

    if(stepperDehumidifier1 == i)
    {
      switch(stepperDehumidifier1)
      {
        case 1: 
        if (count%10 == 0)
        {
          // Do latches
           /*int chk = DHT.read11(DHT11_1);
           if (DHT.humidity > humidVal1)
           {
              digitalWrite(dehumidRelay, HIGH);
              servoB1.write(batch1ang); 
              batchON = 1;
           }*/
           dehumidifierOn(i);
           
          Serial.println("I opened latch once in 10s");
        }
        break;
  
        case 2: 
        if (count%5 == 0)
        {
          // Do dehumidifier
          dehumidifierOn(i);
          Serial.println("I opened latch twice in 10s");
        }
        break;
  
        case 3: 
        if (count%3 == 0)
        {
          // Do dehumidifier
          dehumidifierOn(i);
          Serial.println("I opened latch thrice in 9s");
        }
        break;
       }
    }

    // Batch 2
    if(stepperWatering2 == i)
    {
      switch(stepperWatering2)
      {
        case 1: 
        if (count%10 == 0)
        {
          // Do Watering
          waterControlLoop2();
          Serial.println("I watered once in 10s");
        }
        break;
  
        case 2: 
        if (count%5 == 0)
        {
          // Do Watering
          waterControlLoop2();
          Serial.println("I watered twice in 10s");
        }
        break;
  
        case 3: 
        if (count%3 == 0)
        {
          // Do Watering
          waterControlLoop2();
          Serial.println("I watered thrice in 9s");
        }
        break;
       }
    }

    if(stepperLighting2 == i)
    {
      switch(stepperLighting2)
      {
        case 1: 
        if (count%10 == 0)
        {
          // Do Lighting
          analogWrite(LED_2,85);
          Serial.println("I lighted to minimum brightness");
        }
        break;
  
        case 2: 
        if (count%5 == 0)
        {
          // Do Lighting
          analogWrite(LED_2,170);
          Serial.println("I lighting to medium brightness");
        }
        break;
  
        case 3: 
        if (count%3 == 0)
        {
          // Do Lighting
          analogWrite(LED_2,255);
          Serial.println("I lighted to maximum brightness");
        }
        break;
       }
    }

    if(stepperFertilizer2 == i)
    {
      switch(stepperFertilizer2)
      {
        case 1: 
        if (count%10 == 0)
        {
          // Do Fertilizer
          fertiliserControlLoop2();
          Serial.println("I fertilizered once in 10s");
        }
        break;
  
        case 2: 
        if (count%5 == 0)
        {
          // Do fertilizering
          fertiliserControlLoop2();
          Serial.println("I fertilizered twice in 10s");
        }
        break;
  
        case 3: 
        if (count%3 == 0)
        {
          // Do fertilizering
          fertiliserControlLoop2();
          Serial.println("I fertilizered thrice in 9s");
        }
        break;
       }
    }

    if(stepperDehumidifier2 == i)
    {
      switch(stepperDehumidifier2)
      {
        case 1: 
        if (count%10 == 0)
        {
          // Do latches
          dehumidifierOn(i);
          /*
          int chk2 = DHT.read11(DHT11_2); // batch 2
      if (DHT.humidity > humidVal1)
      {
        digitalWrite(dehumidRelay, HIGH);
        servoB1.write(batch2ang); 
        batchON = 2;
      }*/
          Serial.println("I opened latch once in 10s");
        }
        break;
  
        case 2: 
        if (count%5 == 0)
        {
          // Do opening
          dehumidifierOn(i);
          Serial.println("I opened latch twice in 10s");
        }
        break;
  
        case 3: 
        if (count%3 == 0)
        {
          // Do opening
          dehumidifierOn(i);
          Serial.println("I opened latch thrice in 9s");
        }
        break;
       }
    }

    // Batch 3
    if(stepperWatering3 == i)
    {
      switch(stepperWatering3)
      {
        case 1: 
        if (count%10 == 0)
        {
          // Do Watering
          waterControlLoop3();
          Serial.println("I watered once in 10s");
        }
        break;
  
        case 2: 
        if (count%5 == 0)
        {
          // Do Watering
          waterControlLoop3();
          Serial.println("I watered twice in 10s");
        }
        break;
  
        case 3: 
        if (count%3 == 0)
        {
          // Do Watering
          waterControlLoop3();
          Serial.println("I watered thrice in 9s");
        }
        break;
       }
    }

    if(stepperLighting3 == i)
    {
      switch(stepperLighting3)
      {
        case 1: 
        if (count%10 == 0)
        {
          // Do Lighting
          analogWrite(LED_3,85);
          Serial.println("I lighted minimum brightness");
        }
        break;
  
        case 2: 
        if (count%5 == 0)
        {
          // Do Lighting
          analogWrite(LED_3,170);
          Serial.println("I lighting to medium brightness");
        }
        break;
  
        case 3: 
        if (count%3 == 0)
        {
          // Do Lighting
          analogWrite(LED_3,255);
          Serial.println("I lighted to max brightness");
        }
        break;
       }
    }

    if(stepperFertilizer3 == i)
    {
      switch(stepperFertilizer3)
      {
        case 1: 
        if (count%10 == 0)
        {
          // Do Fertilizer
          fertiliserControlLoop3();
          Serial.println("I fertilizered once in 10s");
        }
        break;
  
        case 2: 
        if (count%5 == 0)
        {
          // Do fertilizering
          fertiliserControlLoop3();
          Serial.println("I fertilizered twice in 10s");
        }
        break;
  
        case 3: 
        if (count%3 == 0)
        {
          // Do fertilizering
          fertiliserControlLoop3();
          Serial.println("I fertilizered thrice in 9s");
        }
        break;
       }
    }

    if(stepperDehumidifier3 == i)
    {
      switch(stepperDehumidifier3)
      {
        case 1: 
        if (count%10 == 0)
        {
          // Do latches
          dehumidifierOn(i);
          /*
      int chk3 = DHT.read11(DHT11_3); // batch 3
      if (DHT.humidity > humidVal1)
      {
        digitalWrite(dehumidRelay, HIGH);
        servoB1.write(batch3ang); 
        batchON = 3;
      }*/
          Serial.println("I opened latch once in 10s");
        }
        break;
  
        case 2: 
        if (count%5 == 0)
        {
          // Do opening
          dehumidifierOn(i);
          Serial.println("I opened latch twice in 10s");
        }
        break;
  
        case 3: 
        if (count%3 == 0)
        {
          // Do opening
          dehumidifierOn(i);
          Serial.println("I opened latch thrice in 9s");
        }
        break;
       }
    }
    
  }//*/
  //timer.run(); // running timer every second                  -------------- this code supposed to control blinking in loop - hampers the serial's
                                                              // continuity of data stream
  //Blynk.virtualWrite(V3, pinData); // virtual LED
  //digitalWrite(51,HIGH);

  //----------------------------------
  //wateringTimes(stepperWatering1);

  
  //Blynk.virtualWrite(V5, value);
  
  // Print temp and humidity data to serial monitor
  //////WOULD LIKE TO MAKE INTO A FOR LOOP
  Serial.println("Sunlight sensor:");
  printSunlightInfo();
  Serial.println();  
  
  Serial.println("First batch");
  printTempHumid(DHT11_1);
  printSoilMoisture(SOIL_1);
  Serial.println();
  /*
  Serial.println("Second batch");
  printTempHumid(DHT11_2);
  printSoilMoisture(SOIL_2);

  Serial.println();
  Serial.println("Third batch");
  printTempHumid(DHT11_3);
  printSoilMoisture(SOIL_3);
  Serial.println();
*/
  // Print temp and humidity data to serial monitor

  //delay(1);        // delay in between reads for stability
  //delay(500);

  
   // OLD - preintegrated era code
  // Here's where we begin the code *********************************************
  /*
   * BATCH ONE
   * 
   * SOIL MOISTURE
   * So a difference of 100 units would be the ideal range of watering since 
   * 1st time I watered: diff = 850 down to 350
   * 2nd time I watered: diff = 350 down to 250 & it steadily climbed up to 301
   * So a value of 50-100 would be ideal
   * 
   * LIGHTS
   * When I did testing the values jumped to roughly IR/Visible light 466/306 so again from 261/254
   * Roughly 150 is range for IR, 25 for Visible Light
   * Therefore relying on dimming blue light, red light and IR
   */

  
  int chk1 = DHT.read11(DHT11_1);
  Serial.print("Temperature = ");
  Serial.println(DHT.temperature);
  Serial.print("Humidity = ");
  Serial.println(DHT.humidity);

  if(DHT.humidity > 25.00)
  {
    // actuate the dehumidifier for 1
  }
  else
  {
    // keep it normally open
  }
  
  
  // read the input on analog pin 0:
  sensorValue1 = analogRead(SOIL_1);
  // print out the value you read:
  Serial.print("Analogue soil sensor reading = ");
  Serial.println(sensorValue1);
  Serial.println(); 

  // when actuated
  //while( abs(sensorValue1 - 200) < 50 ) // the change must be at least 50 ------- do not uncomment the following lines until a logic sequence is established
  {
    // start the water actuation
    
  }
  

  Serial.print("Vis: "); Serial.println(SI1145.ReadVisible());
  Serial.print("IR: "); Serial.println(SI1145.ReadIR());
  //the real UV value must be div 100 from the reg value , datasheet for more information.
  Serial.print("UV: ");  Serial.println((float)SI1145.ReadUV()/100);

  IR = SI1145.ReadIR();
  
  // when actuated ... P.S. One is fine because ------- do not uncomment the following lines until a logic sequence is established
  //while(IR < 300)
  {
    // Continue to turn the lights on
    
  }

  
  //delay(500);
  //  *********************************************
  /*
   * BATCH TWO
   */
  
  int chk2 = DHT.read11(DHT11_2);
  Serial.print("Temperature = ");
  Serial.println(DHT.temperature);
  Serial.print("Humidity = ");
  Serial.println(DHT.humidity);

  if(DHT.humidity > 25.00)
  {
    // actuate the dehumidifier for 2
  }
  else
  {
    // keep it normally open
  }
  
  // read the input on analog pin 0:
  sensorValue2 = analogRead(SOIL_2);
  // print out the value you read:
  Serial.print("Analogue soil sensor reading = ");
  Serial.println(sensorValue2);
  Serial.println(); 

  // when actuated
  //while( abs(sensorValue2 - 200) < 50 ) // the change must be at least 50
  {
    // start the water actuation
    
  }
  
  Serial.print("Vis: "); Serial.println(SI1145.ReadVisible());
  Serial.print("IR: "); Serial.println(SI1145.ReadIR());
  //the real UV value must be div 100 from the reg value , datasheet for more information.
  Serial.print("UV: ");  Serial.println((float)SI1145.ReadUV()/100);

  
  //delay(500);
  //  *********************************************
  /*
   * BATCH THREE
   */
  
  int chk = DHT.read11(DHT11_3);
  Serial.print("Temperature = ");
  Serial.println(DHT.temperature);
  Serial.print("Humidity = ");
  Serial.println(DHT.humidity);

  if(DHT.humidity > 25.00)
  {
    // actuate the dehumidifier for 3
  }
  else
  {
    // keep it normally open
  }

  
  // read the input on analog pin 0:
  sensorValue3 = analogRead(SOIL_3);
  // print out the value you read:
  Serial.print("Analogue soil sensor reading = ");
  Serial.println(sensorValue3);
  Serial.println(); 

  Serial.print("Vis: "); Serial.println(SI1145.ReadVisible());
  Serial.print("IR: "); Serial.println(SI1145.ReadIR());
  //the real UV value must be div 100 from the reg value , datasheet for more information.
  Serial.print("UV: ");  Serial.println((float)SI1145.ReadUV()/100);

  delay(500);//*/

}
