#include <AutonomousWheelchair.h>
#include <WheelchairLib.h>
Wheelchair Chair;
char CurrentLocation='n';
byte card1[4] = {55, 11, 27, 43};
byte card2[4] = {55 ,37, 224, 43};
byte card3[4] = {174, 188, 59, 67};
byte card4[4] = {158, 255, 57, 67};
//0.6 per title 
#define atodtime 18000//18000 //31 tiles
#define ctodtime 21000 //37 tiles
#define btodtime 18000 //31 tiles
Reader reader;
float orientation=0;
char Destination = 'n';
Point A(card1,card1,'A');//A for Door
Point B(card2,card2,'B');//B for Door
Point C(card3,card3,'C');//C for Door
Point D(card4,card4,'D');//D for Door
SoftTimer Auto, Autocorrect, sensortime;
///////////////////////////////////////////////////////////////////////////////////////
void Read_Destination(){ 
  if (Serialinput == 'a' && Destination != 'a'&&previousSerialinput!='a'){
  Destination = 'a';
  }
  else if (Serialinput == 'b'&& Destination != 'b'&&previousSerialinput!='b'){
  Destination = 'b';
  }
  else if (Serialinput == 'c' && Destination !='c'&&previousSerialinput!='c'){
  Destination = 'c';
  }
  else if (Serialinput == 'd' && Destination != 'd'&&previousSerialinput!='d'){
  Destination = 'd';
  }
  else{
  Destination = 'n'; 
  }
}
///////////////////////////////////////////////////////////////////
void Sense(){
   if(sensortime.getElapsedTime()>1500){
   
    sensortime.reset();
    }
}
///////////////////////////////////////////////////////////////////
void correction(){
  if(sensortime.getElapsedTime()>150){
  
  headingDegrees = Serialinput;
   
  if(headingDegrees=='i'){
   // Chair.SetState('f','s');
    correctLow.reset();
    correctHigh.reset();
    Autocorrect.reset();
    while(Autocorrect.getElapsedTime()<400) { 
   digitalWrite(Right,LOW);
   digitalWrite(Left,HIGH);
   }
   digitalWrite(Right,HIGH);
  
   
  }
   
  else if(headingDegrees =='w'){
   // Chair.SetState('f','s');
    correctLow.reset();
    correctHigh.reset();
    Autocorrect.reset();

    while(Autocorrect.getElapsedTime()<400) { 
   digitalWrite(Left,LOW);
   digitalWrite(Right,HIGH);
   }
   digitalWrite(Left,HIGH);
  
  }
   sensortime.reset();
  }
  
}
///////////////////////////////////////////////////////////////////

void correctionsensor(){
 // Sense();
  if((cmfr<300&&cmfr>0)||(cmr<80&&cmr>0)){
   //cmfr=300;cmfl=300;cml=300;cmr=300;
   if(cmfr<cmfl||cmr<cml){
   
    Chair.SetState('f','s');
  //////////////////////////////////
   correctLow.reset();
    correctHigh.reset();
   Autocorrect.reset();
   while(Autocorrect.getElapsedTime()<320) { 
   digitalWrite(Left,LOW);
    
   }
   digitalWrite(Left,HIGH);
           }
     }
  else if((cmfl<300&&cmfl>0)||(cml<80&&cml>0)){
     // cmfr=300;cmfl=300;cml=300;cmr=300;
     if(cmfl<cmfr||cml<cmr){
  
    Chair.SetState('f','s');
 /////////////////////////////////
   correctLow.reset();
    correctHigh.reset();
   Autocorrect.reset();
   while(Autocorrect.getElapsedTime()<350){  
   digitalWrite(Right,LOW);
  
   }
    digitalWrite(Right,HIGH);
     }
   }
}
////////////////////////////////////////////////////////////////////////////
void setup() { 
  Chair.Init();
  reader.Init();
  Serial.setTimeout(7);
  Auto.reset();
  sensortime.reset();
  Chair.SetState('s','s');
} 


void UpdateAll(){
   reader.Update();
   Chair.Update();
  
   
  //if(orientation)
   //correction();
   
   if(Chair.movingState=='f')
   correctionsensor();
   
  
   Read_Destination(); 
   D.Update();
   C.Update();
   B.Update();
   A.Update();
   UpdateLocation();  
   
   
  }
  
void loop() {
  UpdateAll();
  States();//whenever auto

    
}
/////////////////////////////////////////////////////////////////////////////////////////
void UpdateLocation(){
  if(A.Detected){
    A.Detected=false;
    A.ClearCard();
    CurrentLocation='a';
    Serial.write(A.Location);
    }
   if(B.Detected){
    B.Detected=false;
    B.ClearCard();
    CurrentLocation='b';
    Serial.write(B.Location);
    }
   if(C.Detected){
    C.Detected=false;
    C.ClearCard();
    CurrentLocation='c';
    Serial.write(C.Location);
    }
    if(D.Detected){
    D.Detected=false;
    D.ClearCard();
    CurrentLocation='d';
    Serial.write(D.Location);
    }
  
  }
////////////////////////////////////////////////////////////////////////////////////  
void a2d(){
   Auto.reset();
   Chair.SetState('f','s');  
   while(Auto.getElapsedTime()<atodtime){
     UpdateAll();
      //correction();
    }
    Auto.reset();
}
void c2d(){ 
    Auto.reset();
    Chair.SetState('f','s'); 
    while(Auto.getElapsedTime()<ctodtime){
      
    UpdateAll();
   // correction();
    } 
    Auto.reset();  
  }
void b2d(){ 
    Auto.reset();
    Chair.SetState('f','s'); 
    while(Auto.getElapsedTime()<btodtime){
  
    UpdateAll();
    
    } 
    Auto.reset();  
  }
////////////////////////////////////////////////////////////////////////////  
void States(){
  switch(Destination){
////////////////////////////////////////////////////
  case 'c':
  if(CurrentLocation=='a'){//starting a going to c
  orientation=230;
  Serial.write('0');//for 230 isra
  a2d();
  CurrentLocation='d'; 
  }
  ////
  if(CurrentLocation=='b'){ 
  orientation=133;
  Serial.write('1');//for 133 isra
  b2d();  
  c2d();
  CurrentLocation='c'; 
  }
  ////
  if(CurrentLocation=='d'){
   orientation=133;
   Serial.write('1');//for 133 isra
   Chair.SetState('l','s'); 
   while(Chair.movingState=='l')
   Chair.Update();
   Chair.Stop(); 
   c2d();    
   CurrentLocation='c'; 
  }
  ////
  if(CurrentLocation=='c'){
   orientation=0;
   Serial.write('Z');
    Destination = 'n';
   Chair.SetState('s','s');  
   Auto.reset();
  }
  break; 
////////////////////////////////////////////////////  
  case 'b':
  if(CurrentLocation=='a'){//starting a going to c
    orientation=230;
    Serial.write('0');//for 230 isra
   a2d();
   CurrentLocation='d'; 
  }
  ////
  if(CurrentLocation=='d'){
    orientation=313;
   Serial.write('2');//for 313 isra
   Chair.SetState('r','s'); 
   while(Chair.movingState=='r')
   Chair.Update();
   Chair.Stop(); 
   b2d();    
   CurrentLocation='b'; 
  }
  ////
  if(CurrentLocation=='c'){
     orientation=313;
     Serial.write('2');//for 313 isra
  c2d();
  b2d(); 
  CurrentLocation='b'; 
  }
  ////
  if(CurrentLocation=='b'){ 
     orientation=0;
    Serial.write('Z');
    Destination = 'n';
    Chair.SetState('s','s');  
    Auto.reset();
  }
  ////
  break; 

////////////////////////////////////////////////////  
  case 'a':
  if(CurrentLocation=='b'){//starting a going to c
   orientation=133;
    Serial.write('1');//for 133 isra
   b2d();
   Chair.SetState('l','s'); 
   while(Chair.movingState=='l')
   Chair.Update();
   Chair.Stop(); 
   CurrentLocation='d'; 
  }
  ////
  if(CurrentLocation=='c'){
    orientation=313;
    Serial.write('2');//for 313 isra
   c2d();
   Chair.SetState('r','s'); 
   while(Chair.movingState=='r')
   Chair.Update();
   Chair.Stop(); 
   CurrentLocation='d'; 
  }
  ////
   if(CurrentLocation=='d'){
   orientation=50;
   Serial.write('3');//for 50 isra
   a2d();    
   CurrentLocation='a'; 
  }
  ////
  if(CurrentLocation=='a'){ 
    orientation=0;
    Serial.write('Z');
    Destination = 'n';
    Chair.SetState('s','s');  
    Auto.reset();
  }
  ////
  break; 

////////////////////////////////////////////////////  
  }
} 


