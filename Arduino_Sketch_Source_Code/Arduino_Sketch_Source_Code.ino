//
// David Panamski - TUES - 8V
//
// Primary Idea based on: https://www.electronicshub.org/arduino-joystick-interface-control-servo/
// Primary Idea based on: Arduino Joystick Interface – Control Servo using Arduino and Joystick
// Primary Idea based on: July 5, 2018, By Administrator
// Sound ISR based on: //code write by Moz for YouTube changel LogMaker360, 17-9-2015
// Sound ISR based on: Arduino KY-006 simple interrupt on a buzzer. 
// Sound ISR based on: Code belongs to this video: https://www.youtube.com/watch?v=cB1lc2RR4cQ

#include <Servo.h>

// Arduino MEGA2560 Hardware PIN Connection
const int led1GreenPin = 22;
const int led1RedPin = 23;
const int led2GreenPin = 24;
const int led2RedPin = 25;
const int buzzerPin = 26;             
const int x_Analog_Joystick_Pin = A8;                                               
const int y_Analog_Joystick_Pin = A9;
const int switch_Joystick_Pin = 18;
const int switch_Rotary_Pin = 19;
const int Servo_x_Control_Pin = 50;
const int Servo_y_Control_Pin = 51; 

//General Constants
const int Time_Servo = 50;
const int Increment_Servo = 2;
const int X_pos_center = 90;
const int Y_pos_center = 90;

int xValue_Joystick;   //Analog Read from Joystick [0..1023]
int yValue_Joystick;   //Analog Read from Joystick [0..1023]
int X_pos = 90;        //Servo Angle Rotation [0..180] degree
int Y_pos = 90;        //Servo Angle Rotation [0..180] degree

int unsigned long button_time = 0;
int unsigned long last_button_time = 0;
int unsigned long button_time_Rotary = 0;
int unsigned long last_button_time_Rotary = 0;

volatile byte flag_Button_Toggle = 1;		//volatile is because its value can be changed somewhere
volatile byte flag_Button_Toggle_Rotary = 1;
volatile byte Increment_Servo_Multiplier = 1;
volatile byte flag_Sound_1 = 1;
volatile byte flag_Sound_2 = 0;
volatile byte flag_Mode = 1; // 0 - program mode  // 1 - manual interactive mode

 
Servo Servo_X;
Servo Servo_Y;

void setup() 
{
  pinMode (x_Analog_Joystick_Pin, INPUT) ;                     
  pinMode (y_Analog_Joystick_Pin, INPUT) ;
  pinMode(led1GreenPin, OUTPUT);
  pinMode(led1RedPin, OUTPUT);
  pinMode(led2GreenPin, OUTPUT);
  pinMode(led2RedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  
  pinMode(switch_Joystick_Pin, INPUT_PULLUP);
  pinMode(switch_Rotary_Pin, INPUT_PULLUP);
  Servo_X.attach(Servo_x_Control_Pin ); 
  Servo_Y.attach(Servo_y_Control_Pin); 
  Servo_X.write(X_pos_center);
  Servo_Y.write(Y_pos_center);
  Increment_Servo_Multiplier = 5;
  digitalWrite(led1GreenPin, HIGH);
  digitalWrite(led2GreenPin, LOW);
  digitalWrite(buzzerPin, LOW);
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(switch_Joystick_Pin), switchISR_Joystick, CHANGE); //CHANGE //FALLING
  attachInterrupt(digitalPinToInterrupt(switch_Rotary_Pin), switchISR_Rotary, CHANGE); //CHANGE //FALLING
}

void loop() 
{
  sound();
  digitalWrite(buzzerPin, LOW); // sound OFF
  if(flag_Mode==1)  // 1 - manual interactive mode
  {
    manual_move();   
  }  
  
  if(flag_Mode==0)  // 0 - program mode
  {
    program_move();   
  }  

 
}

void switchISR_Joystick()
{
    button_time = millis();
    if(button_time-last_button_time > 200)
    {
      if (flag_Button_Toggle==1) {
        flag_Button_Toggle=0;
       //Serial.println("Set Sound Off"); 
       Increment_Servo_Multiplier = 1;
       digitalWrite(led1GreenPin, LOW);
       //digitalWrite(buzzerPin, LOW);
       flag_Sound_1=1; //start while loop
        // flag_Sound_1=0; //end  while loop
      }
      else {
        flag_Button_Toggle=1;  
        flag_Sound_1=1; //start while loop
        //Serial.println("Set Sound On"); 
        Increment_Servo_Multiplier = 5;
        digitalWrite(led1GreenPin, HIGH);
      }
      
      last_button_time = button_time;
    }

}

void switchISR_Rotary()
{
    button_time_Rotary = millis();
    if(button_time_Rotary-last_button_time_Rotary > 200)
    {
      if (flag_Button_Toggle_Rotary==1) {
       flag_Button_Toggle_Rotary=0;
       //Serial.println("switchISR_Rotary 0 - program mode"); 
       flag_Mode = 0;    // 0 - program mode
       digitalWrite(led2GreenPin, HIGH);
       flag_Sound_2=1; //start while loop
       // flag_Sound_1=0; //end  while loop
      }
      else {
        flag_Button_Toggle_Rotary=1;  
         flag_Sound_2=1; //start while loop
        //Serial.println("switchISR_Rotary 1 - manual interactive mode"); 
        flag_Mode = 1;  // 1 - manual interactive mode
        digitalWrite(led2GreenPin, LOW);
      }
      
      last_button_time_Rotary = button_time_Rotary;
    }

}

void sound(){

  while (flag_Sound_1==1)
  {
      for(int i=0; i<20; i++)
      {
        digitalWrite(buzzerPin, HIGH); //voice
        delay (1); //delay 2ms
        digitalWrite(buzzerPin, LOW); //no voice
        delay (1); //delay 2ms
      }
      flag_Sound_1=0;
  }   //while Sound_1

   while (flag_Sound_2==1)
  {
      for(int i=0; i<10; i++)
      {
        digitalWrite(buzzerPin, HIGH); //voice
        delay (2); //delay 1ms
        digitalWrite(buzzerPin, LOW); //no voice
        delay (1); //delay 1ms
      }
      delay (50); //delay 20ms
      for(int i=0; i<10; i++)
      {
        digitalWrite(buzzerPin, HIGH); //voice
        delay (2); //delay 1ms
        digitalWrite(buzzerPin, LOW); //no voice
        delay (1); //delay 1ms
      }
      flag_Sound_2=0;
  }   //while Sound_2

  digitalWrite(buzzerPin, LOW);
  
}  // sound


void manual_move(){


  xValue_Joystick = analogRead(x_Analog_Joystick_Pin);  
  yValue_Joystick = analogRead(y_Analog_Joystick_Pin);
// For X-Axis Servo
  if (xValue_Joystick < 300)
  {
    if (X_pos < 10) 
    {
      // Do Nothing 
    } 
    else
    {
      X_pos = X_pos - (Increment_Servo_Multiplier*Increment_Servo);
      Servo_X.write(X_pos);
      delay(Time_Servo); 
    } 
  } 
  
  if (xValue_Joystick > 700)
  {
    if (X_pos > 160)
    {
      // Do Nothing  
    }
    else
    {
      X_pos = X_pos + (Increment_Servo_Multiplier*Increment_Servo);
      Servo_X.write(X_pos);
      delay(Time_Servo);
    }
  }
// For Y-Axis Servo
if (yValue_Joystick < 300)
  {
    if (Y_pos < 10) 
    {
      // Do Nothing 
    } 
    else
    {
      Y_pos = Y_pos - (Increment_Servo_Multiplier*Increment_Servo);
      Servo_Y.write(Y_pos);
      delay(Time_Servo); 
    } 
  } 
  
  if (yValue_Joystick > 700)
  {
    if (Y_pos > 160)
    {
      // Do Nothing  
    }
    else
    {
      Y_pos = Y_pos + (Increment_Servo_Multiplier*Increment_Servo);
      Servo_Y.write(Y_pos);
      delay(Time_Servo);
    }
  }
  
  
}  // manual_move

void program_move(){
int i,j;

  sound();
  Servo_X.write(0); 
  Servo_Y.write(0);
  for(i=0;i<=180;i++){
      Servo_X.write(i);
      for(j=0;j<=180;j++){
        Servo_Y.write(j);
        delay(Time_Servo/5);
        if(flag_Mode==1) return;  // 1 - manual interactive mode
      }
      //delay(Time_Servo/5); 
  }

}
