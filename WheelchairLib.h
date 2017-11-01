#ifndef Wheelchair_h
#define Wheelchair_h
#include "Arduino.h"
#include <SoftTimers.h>
//this code is to create a set of functions to use for the BCI Wheelchair
#define ControlSig1 3 //control signal 1 to controller Forward Backward joystick which is the first relay IN1
#define ControlSig2 4 //control signal 2 to controller Left Right joystick
//defining movement relays
#define Left 8 
#define Right 7
#define Forward 5 
#define Backward 6
#define RightTurnTime 1350
#define LeftTurnTime 1200
#define RightUTurnTime 2820
#define LeftUTurnTime 2500
#define CorrectTimeHigh 230
#define CorrectTimeLow 270
#define trigfr 49// attach pin 28 to Triag forward right
#define echofr 48//attach pin 29 to Echo forward right
#define trigfl 30 // attach pin 26 to Trig forward left 
#define echofl 28//attach pin 27 to Echo forward left 
#define trigl  24// attach pin 24 to Trig left
#define echol  26 //attach pin 25 to Echo left
#define trigr  45// attach pin 31 to Trig right
#define echor  44 //attach pin 30 to Echo right
SoftTimer counter,correctHigh,correctLow;
long duration, cmr=300, cml=300, cmfr=300, cmfl=300;
char Serialinput='s';
char previousSerialinput='s';
unsigned long previousMillisfr = 0;unsigned long previousMillisfl = 0;unsigned long  previousMillisr = 0;unsigned long  previousMillisl = 0;
int trigStatefr = LOW;int trigStatefl = LOW;int trigStater = LOW;int trigStatel = LOW;

////////////////////////////////////////////////////////sensor time
long microsecondsToCentimeters(long microseconds){
return microseconds / 29 / 2;
}
////////////////////////////////////////////////////////end sensor 
class Wheelchair{
  public:
  bool moving;
  bool readInput;
  long RightTime = RightTurnTime;//how much time moving right
  long LeftTime =  LeftTurnTime;//how much time moving left
  long ForwardTime = 90000;//how much time moving forward
  long BackwardTime = 90000;//how much time moving backward
  char movingState;//tell how the wheelchair is moving
  void StartJOYLR();//start taking comands from Joystick Left Right
  void StopJOYLR();//stop taking comands from Joystick Left Right
  void StartJOYFB();//start taking comands from Joystick Forward Backward
  void StopJOYFB();//stop taking comands from Joystick Forward Backward
  void CorrectForward();
  void Init();//initailize put this in setup
  void Stop();//stops the wheelchair
  void Movement(int Direction);
  void MovementTime();
  void Serialmovement();
  void ReadSensorInput(char sensor);// read the value from the sensors
  bool isMoving();//check if the wheelchair is moving
  void Update();//run this in loop
  void SetState(char state,char oldstate);//sets the state
  void ChangeTime(int Direction,long NewTime);//changes the default time to move
  };
////////////////////////////////////////////////////////////Startfunctions    
void Wheelchair::Init(){//Initial startup
    for (int i = 3; i <= 8; i++) {
    pinMode(i, INPUT_PULLUP);
    pinMode(i, OUTPUT); // defaults HIGH (relays off)
  }
   correctLow.setTimeOutTime(CorrectTimeLow);
   correctHigh.setTimeOutTime(CorrectTimeHigh); 
   correctLow.reset();
   correctHigh.reset();
   pinMode(trigfl, OUTPUT);
   pinMode(trigfr, OUTPUT);
   pinMode(trigl, OUTPUT);
   pinMode(trigr, OUTPUT);
   pinMode(echofl, INPUT);
   pinMode(echofr, INPUT);
   pinMode(echol, INPUT);
   pinMode(echor, INPUT);
   Serial.begin(9600);//start serial
  }
////////////////////////////////////////////////////////////    
void Wheelchair::CorrectForward(){
  if(movingState=='f'){
  digitalWrite(Forward, LOW);  
   
  if (!correctLow.hasTimedOut()){ 
    correctHigh.reset();
	}
  else if (correctLow.hasTimedOut() && !correctHigh.hasTimedOut()){
    digitalWrite(Right, LOW);  
	}
  else if (correctLow.hasTimedOut() && correctHigh.hasTimedOut()){
    digitalWrite(Right, HIGH); 
	ReadSensorInput('f');
	ReadSensorInput('F');
	ReadSensorInput('r');
	ReadSensorInput('l');
    correctLow.reset();
    correctHigh.reset();
	}
  }
  else
  Stop();
  }
//////////////////////////////////////////////////////////// 
void Wheelchair::ReadSensorInput(char sensor){
  if(sensor == 'f'){//'f' for forward right
  digitalWrite(trigfr, LOW);
  delayMicroseconds(2);
  digitalWrite(trigfr, HIGH);
  delayMicroseconds(5);
  digitalWrite(trigfr, LOW);
  pinMode(echofr,INPUT);
  duration = pulseIn(echofr, HIGH);
  cmfr = microsecondsToCentimeters(duration);
  
  
  }

  if(sensor == 'F'){//'z' for forward left
  digitalWrite(trigfl, LOW);
  delayMicroseconds(2);
  digitalWrite(trigfl, HIGH);
  delayMicroseconds(5);
  digitalWrite(trigfl, LOW);
  pinMode(echofl,INPUT);
  duration = pulseIn(echofl, HIGH);
  cmfl = microsecondsToCentimeters(duration);   
  }
  
  if(sensor == 'r'){//'r' for right
  digitalWrite(trigr, LOW);
  delayMicroseconds(2);
  digitalWrite(trigr, HIGH);
  delayMicroseconds(5);
  digitalWrite(trigr, LOW);
  pinMode(echor,INPUT);
  duration = pulseIn(echor, HIGH);
  cmr = microsecondsToCentimeters(duration);
  }
  
  if(sensor == 'l'){//'l' for left
  digitalWrite(trigl, LOW);
  delayMicroseconds(2);
  digitalWrite(trigl, HIGH);
  delayMicroseconds(5);
  digitalWrite(trigl, LOW);
  pinMode(echol,INPUT);
  duration = pulseIn(echol, HIGH);
  cml = microsecondsToCentimeters(duration);
  }
 
 
  }
////////////////////////////////////////////////////////////    
bool Wheelchair::isMoving(){//check if wheelchair is moving
   if(moving)
   return true;
   else
   return false;
  }
//////////////////////////////////////////////////////////// 
void Wheelchair::Update(){//updates the wheelchair object
  Serialmovement();
  MovementTime();
  //
  }
////////////////////////////////////////////////////////////   
void Wheelchair::ChangeTime(int Direction,long NewTime){//changes movement time
    switch(Direction){
    case Forward:
    ForwardTime = NewTime;
    break;
    case Backward:
    BackwardTime = NewTime;
    break;
    case Right:
    RightTime = NewTime;
    break;
    case Left:
    LeftTime = NewTime;
    break; 
  };
  }
////////////////////////////////////////////////////////////   
void Wheelchair::Stop(){//stops movement
  for (int i = 8; i >= 5; i--){ 
  digitalWrite(i,HIGH);//turn off all relays
  }
  moving = false;//wheelchair not moving
}
////////////////////////////////////////////////////////////  
void Wheelchair::StopJOYLR(){//starts accepting commands from circuit
  digitalWrite(ControlSig2,LOW);
  }
void Wheelchair::StartJOYLR(){
  digitalWrite(ControlSig2,HIGH);
  }
void Wheelchair::StopJOYFB(){
  digitalWrite(ControlSig1,LOW);
  }
void Wheelchair::StartJOYFB(){
  digitalWrite(ControlSig1,HIGH);
  }
////////////////////////////////////////////////////////////  
void Wheelchair::SetState(char state,char oldstate){//sets the state of movement
  switch(state){
    case 'f':
    if(oldstate!=state){
    counter.reset();
    movingState='f';
    //correctLow.reset();
    }
    break;
    case 'b':
    //counter.reset();
    //movingState='b';
    break;
    case 'R'://for forward right
    counter.reset();
    movingState='R';
    break;
    case 'r':
    if(oldstate!=state)
    counter.reset();   
    movingState = 'r';//Lower Caps for 90 degree
    break;
    case 'L'://for forward left
    counter.reset();
    movingState='L';
    break;
    case 'l'://Lower caps for 90 degree
    if(oldstate!=state)
    counter.reset();
    movingState = 'l';
    break;
    case 's':
    movingState = 's';
    break; 
  };
  }
////////////////////////////////////////////////////////////  
void Wheelchair::Movement(int Direction){
    Stop();
 
    digitalWrite(Direction,LOW);
  
 if(movingState=='R'||movingState=='L'){
     digitalWrite(Forward,LOW);
     digitalWrite(Direction,LOW);
    } 
    
    moving = true;//wheelchair is moving
    
 } 
////////////////////////////////////////////////////////////  
 void Wheelchair::MovementTime(){

     switch(movingState){
     case 'r':
     Movement(Right);
    // ReadSensorInput('r');
    
     break;
     case 'R':
     Movement(Right);
    // ReadSensorInput('r');
    
     break;
     case 'l':
    
     Movement(Left);
   // ReadSensorInput('l');
     
     break;
     case 'L':
    
     Movement(Left);
    // ReadSensorInput('l');
     
     break;
     case 'f':
    // Movement(Forward);
     //ReadSensorInput('f');
     //ReadSensorInput('F');
     CorrectForward();
     
     
     break;
     case 'b':
   //  Movement(Backward);
     break;
     case 's':
     Stop();
     break;
     };
     if(movingState=='r'&&counter.getElapsedTime()>RightTime){
     movingState='s';Serialinput='s';}
     else if(movingState=='R'&&counter.getElapsedTime()>RightTime){
     movingState='s';Serialinput='s';}
     else if(movingState=='l'&&counter.getElapsedTime()>LeftTime){
     movingState='s';Serialinput='s';}
     else if(movingState=='L'&&counter.getElapsedTime()>LeftTime){
     movingState='s';Serialinput='s';}
     else if(movingState=='f'&&counter.getElapsedTime()>ForwardTime){
     movingState='s';Serialinput='s';}
  //   else if(movingState=='b'&&counter.getElapsedTime()>BackwardTime){
    // movingState='s';Serialinput='s';}
  } 
////////////////////////////////////////////////////////////  
void Wheelchair::Serialmovement(){//takes serial data from processing
  if(Serial.available()){
	// ReadSensorInput('r');ReadSensorInput('f');ReadSensorInput('F');ReadSensorInput('l');
  StopJOYLR();
  StopJOYFB();
  previousSerialinput =  Serialinput;
  Stop();
  Serialinput = Serial.read();  
  SetState(Serialinput , previousSerialinput);
  }
  
  }
////////////////////////////////////////////////////////////end Functions 
#endif