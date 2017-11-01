//importing libraries 
import oscP5.*;                        //Library which enables oscP5 communication
import netP5.*;
import processing.serial.*;           //Library which enables serial communication

//declaring and initializing global variables 
PFont myFont;
int portnum = 7400;
int i=0;
char val;
float heading;
int new_heading;
Serial myPort;                        //Declares object of type Serial, to send data using serial communication protocol (to Arduino)
OscP5 oscP5;                          //Decalres object of type OscP5 to receive data using OSCP5 communication protocol (from EEG)
OscP5 compass;                        //Decalres object of type OscP5 to receive data using OSCP5 communication protocol (compass ios app)
NetAddress myRemoteLocation;
NetAddress location;                  //compass ios app setting 
char current_state = 'n';             //intializing current state to n (user is performing the neutral mental command)
char previous_state='u';              //intialiazing previous state to u (interface automatically selects forward at the begining)

// boolean variables to switch between BCI and autonomy mode in the interface 
boolean send_bci=false;               
boolean send_auto=false;
boolean after_yes=false;
boolean neutral=true;
char state='1';
boolean autonomy_q=true;
boolean autonomous_state= false;
boolean confirm_command=false; 
boolean prompt_q=false;

//variables to used to later utilize the mental commands 
float command    ;            //Float number to take the intensity of the command from the EEG headset 
float thresh = 0.2;           //Threshold of mental command 

//variables used in the correction when in the autonomous mode
char point;
int orientation=0;


//setup 
void setup() {
  size(1000, 1000);
  myPort = new Serial(this, "COM10",9600);     //myPort sends and reads data using serial communication through port 9600
  oscP5  = new OscP5(this, portnum );          //oscP5 listens to incoming messages at port 7400
  compass= new OscP5(this, 1234);              //compass listens to incoming messages from the compass app through port 1234
  myRemoteLocation = new NetAddress("127.0.0.1", 7400);
  location= new NetAddress("127.0.0.1", 1234);
  myFont = createFont("Arial Black", 32);      //used for formatting the font on the interface 
  textFont(myFont);
}
 
 
 
//serial event (function that is called whenever the Serial port myPort is busy
void serialEvent(Serial myPort) {
 val=myPort.readChar();          //reads the message sent to myPort
if (val =='A'||val =='B'|| val =='C') {  //switch to autonomous mode 
autonomous_state = true;        
point=val;                         
    }
   else if(val=='Z')            //destination is reached (switch to BCI mode) 
  {
   prompt_q=false;
   autonomous_state=false;
   confirm_command=false;
   neutral=false;
   orientation=0;
  }
  else if(val=='0')           //predefine the orientation of the wheelchair within the user's household  
  {
    orientation=230;
    
  }
  else if(val=='1')           //predefine the orientation of the wheelchair within the user's household
  {
    orientation=133;
  }
  else if(val=='2')           //predefine the orientation of the wheelchair within the user's household
  {
    orientation=313;
  }
  else if(val=='3')           //predefine the orientation of the wheelchair within the user's household 
  {
    orientation=50;
  }
  }
 
 
 
 //Incoming OSC messages (port 7400) are forwarded to the oscEvent method and take the name of myMessage 
void oscEvent (OscMessage myMessage) {                         
//////////////////////////////////////// check the following conditions in BCI mode 
if (autonomous_state==false)                         
{
   
if(myMessage.checkAddrPattern("/COG/NEUTRAL") == true){
  current_state='n';
  confirm_command=false;
  send_bci=true;
  myPort.clear();
  myPort.write('s');
}
else if (myMessage.checkAddrPattern("/COG/LEFT") == true) {         //Checks if the osc message has the pattern "/COG/LEFT"
    confirm_command=false;
    command = myMessage.get(0).floatValue();                        //Retrieves the intensity of the left command
    if ((command>thresh) && (current_state=='n'))                   //declares a proper state based on teh previous information 
    {
       if (previous_state == 'l')
      {current_state = 'u';}
      else if (previous_state == 'u')
      {current_state = 'r';} 
      else if (previous_state == 'r')
      {current_state = 'l';} 
    }
  } else if (myMessage.checkAddrPattern("/COG/PUSH") == true) {    //Checks if the osc message has the pattern "/COG/RIGHT"
    command = myMessage.get(0).floatValue();                       //Retrieves the intensity of the right command
    if ((command>thresh) && current_state=='n')                    //Checks where the interface selection is to send a specific character to the Arduino and move the wheelchair 
    {
      confirm_command=true;
       
    if ( previous_state=='s' && send_bci==true)                //STATE = STOP
  {
     myPort.clear();
     myPort.write('s');
     send_bci=false;
    }
  
   else if (previous_state=='u' && send_bci==true)            //STATE = UP
  {
    
       myPort.clear();
       myPort.write('f');
       send_bci=false;
    }
   
   else if (previous_state=='r' && send_bci==true)          //STATE = RIGHT
  {
       myPort.clear();
       myPort.write('r');
       send_bci=false;
    }
  
   else if (previous_state=='l' && send_bci==true)          //STATE = LEFT
  {
       myPort.clear();
       myPort.write('l');
       send_bci=false;
     
  }
       
    }
    else
    {
      confirm_command=false;
    }
  }
 }
 
 
/////////////////////////////// check the following conditions in Autonmous mode 
else 
 { 

   if (myMessage.checkAddrPattern("/compass/heading") == true) {            //Checks if the osc message has the pattern 
   heading = myMessage.get(0).floatValue();                                 //continuously received the compass heading from the app
   new_heading = round(heading);
   myPort.clear();
   myPort.write(new_heading);
   //println(orientation);
  if(orientation!=0)                                    //corrects the wheelchair's orientation by comparing the predefined orientation 
                                                        //in serila event and the new compass heading received from the app
  {
    if(new_heading<orientation-5)
   {
     myPort.write('i');
     println("l");
    // orientation=0;
   }
   else if(new_heading>orientation+5)
   {
     myPort.write('w');
     println("r");
    // orientation=0;
   }
}
   }
  if(myMessage.checkAddrPattern("/COG/NEUTRAL") == true){                   //also check for the following conditions in Autonomous mode 
  confirm_command=false;
  neutral=true;
  send_auto=true;
   }
   
  if(neutral==true)                                                      //change states to change the interface in Autonomous mode based on
                                                                         //the user's current location 
  {
    if (myMessage.checkAddrPattern("/COG/LEFT") == true)
  {
    confirm_command=false;
    command = myMessage.get(0).floatValue();     
    if(command>thresh)
    {
      if(state=='1'){
        state='2';
        neutral=false;
      }
      else if(state=='2')
      {
        state='1';
        neutral=false;
          }
        }
       }
      else if (myMessage.checkAddrPattern("/COG/PUSH") == true)
  {
    command = myMessage.get(0).floatValue();     
    if(command>thresh)
    {
      confirm_command=true;
        if (point=='A' && send_auto==true && after_yes==true)
  {
    if(state=='1')
  { 
     myPort.clear();
     myPort.write('b');
     send_auto=false;
      //i++;
      //println(i);
  }
  else if(state=='2')
  {

     myPort.clear();
     myPort.write('c');
    send_auto=false;
  }
  }
  
  else if(point=='B' && send_auto==true && after_yes==true)
  {
    if(state=='1')
  { 
  
    myPort.clear();
    myPort.write('a');
    send_auto=false;
  }
  else if(state=='2')
  {
  
     myPort.clear();
     myPort.write('c');
     send_auto=false;
  }
  }
  else if(point=='C' && send_auto==true && after_yes==true)
  {
    if(state=='1')
  { 
     myPort.clear();
     myPort.write('a');
     send_auto=false;
  }
  else if(state=='2')
  {
  
     myPort.clear();
       myPort.write('b');
       send_auto=false;
  }
  }
  
        }
        else{
          confirm_command=false;
        }
       }
          
        }
      }
     }



/////////////////////////////////////////////////////////////////draw the interface 
void draw(){
  //Background
  background(255);
if(autonomous_state == false) //BCI mode interfacece / based on the state, call the corresponding interface function 
{
   after_yes=false;
   if (current_state == 'u'||(current_state=='n'&& previous_state=='u'))            //STATE = UP
  {
     forward_selected_confirmed(0,200,255);
    previous_state='u';
    if(confirm_command==true)
    {
    
      forward_selected_confirmed(112,128,144);
    }
   
  } else if (current_state == 'r'||(current_state=='n'&& previous_state=='r'))          //STATE = RIGHT
  {
    right_selected_confirmed(0,200,255);
    previous_state='r';
    if(confirm_command==true)
    {
      right_selected_confirmed(112,128,144);
    }
  
  } else if (current_state == 'l'||(current_state=='n'&& previous_state=='l'))          //STATE = LEFT
  {
     left_selected_confirmed(0,200,255);
    previous_state='l';
    if(confirm_command==true)
    {
     left_selected_confirmed(112,128,144);
    }
  }
}

//////////////////////////////////////////////// 
else{                                           //Autonomous state interface /based on the state, call the corresponding function interfacr
if(autonomy_q==true){
  
if( state=='1')
  {
  Auto_sc(0,200,255,0,0,128);
  Auto_qtext();
  if(confirm_command==true)
  {
  Auto_sc(112,128,144,0,0,128);
  Auto_qtext();
  prompt_q=true;
  autonomy_q=false;
  confirm_command=false;
  neutral=false;
  after_yes=true;
  }
  }
  else if(state=='2')
  {
  Auto_sc(0,0,128,0,200,255);
  Auto_qtext();
  if(confirm_command==true)
  {
    Auto_sc(0,0,128,112,128,144);
    Auto_qtext();
    prompt_q=false;
    autonomous_state=false;
    confirm_command=false;
    neutral=false;
    myPort.clear();
    myPort.write('n'); // tell arduino to go back to BCI mode 
   }
}
}
///////////////////////////////////////////////////check points(to ask the user a question based on the user's location)
else{
if(prompt_q)
{
  
  if (point=='A')
  {
    if(state=='1')
  { 
    Auto_sc(0,200,255,0,0,128);
    Auto_atext();
   if(confirm_command==true)
  {
    Auto_sc(112,128,144,0,0,128);
    Auto_atext();
      
  }
  }
  else if(state=='2')
  {
   Auto_sc(0,0,128,0,200,255);
   Auto_atext();
   if(confirm_command==true)
  {
    Auto_sc(0,0,128,112,128,144);
    Auto_atext();

  }
  }
  }
  
  else if(point=='B')
  {
    if(state=='1')
  { 
    Auto_sc(0,200,255,0,0,128);
    Auto_btext();
   if(confirm_command==true)
  {
    Auto_sc(112,128,144,0,0,128);
    Auto_btext();

  }
  }
  else if(state=='2')
  {
   Auto_sc(0,0,128,0,200,255);
   Auto_btext();
   if(confirm_command==true)
  {
    Auto_sc(0,0,128,112,128,144);
    Auto_btext();

  }
  }
  }
  else if(point=='C')
  {
    if(state=='1')
  { 
    Auto_sc(0,200,255,0,0,128);
    Auto_ctext();
   if(confirm_command==true)
  {
    Auto_sc(112,128,144,0,0,128);
    Auto_ctext();

  }
  }
  else if(state=='2')
  {
   Auto_sc(0,0,128,0,200,255);
   Auto_ctext();
   if(confirm_command==true)
  {
    Auto_sc(0,0,128,112,128,144);
    Auto_ctext();
  }

  }
  }
}
}
}
}
 
 
 
 
 
 
 
 

//////////////////////////////////////////////////////////////////interface features (functions used to draw the interface)
void Auto_sc(int r1, int g1, int b1, int r2, int g2, int b2)
{
  stroke (255);
  myFont = createFont("Arial Black", 32);
  fill(r1,g1,b1);
  rect(250, 400, 150, 150, 7);
 fill(r2,g2,b2);
  rect(600, 400, 150, 150, 7);
  fill(0,0,128);
}

void Auto_qtext()
{
  text("Do you want to switch to Autonomous Mode?",100,300);
  fill (255);
  text("Yes",292,480);
  text("No",650,480);
}

void Auto_atext()
{
  text("Where would you like to go?",255,300);
  fill (255);
  text("B",310,480);
  text("C",660,480);
}

void Auto_btext()
{
  text("Where would you like to go?",255,300);
  fill (255);
  text("A",310,480);
  text("C",660,480);
}

void Auto_ctext()
{
  text("Where would you like to go?",255,300);
  fill (255);
  text("A",310,480);
  text("B",660,480);
}
/*void stop_selected_confirmed(int r, int g, int b)
{
   //TRIANGLES
    stroke(255);
    fill(0, 0, 128);
    triangle(350, 300, 650, 300, 500, 100);
    fill(0, 0, 128);
   triangle(300, 350, 300, 650, 100, 500);
    fill(0, 0, 128);
    triangle(700, 350, 700, 650, 900, 500);
    //Circle
    fill(r, g, b);
    ellipse(500, 500, 250, 250);
    //TEXT
    fill(255);
    text("FORWARD", 415, 250);
   text("STOP", 455, 510);
    text("LEFT", 185, 510);
    text("RIGHT",730, 510);
}*/

void left_selected_confirmed(int r, int g, int b)
{
    //TRIANGLES
    stroke(255);
    fill(0, 0, 128);
    triangle(350, 300, 650, 300, 500, 100);
    fill(r, g, b);
    triangle(300, 350, 300, 650, 100, 500);
    fill(0, 0, 128);
    triangle(700, 350, 700, 650, 900, 500);
    //Circle
    fill(0, 0, 128);
    ellipse(500, 500, 250, 250);
    //TEXT
    fill(255);
    text("FORWARD", 415, 250);
    text("STOP", 455, 510);
    text("LEFT", 185, 510);
    text("RIGHT",730, 510);
}
 
 
void forward_selected_confirmed(int r, int g, int b)
{
      //TRIANGLES
    stroke(255);
    fill(r, g, b);
    triangle(350, 300, 650, 300, 500, 100);
    fill(0, 0, 128);
    triangle(300, 350, 300, 650, 100, 500);
    fill(0, 0, 128);
    triangle(700, 350, 700, 650, 900, 500);
    //Circle
    fill(0, 0, 128);
    ellipse(500, 500, 250, 250);
    //TEXT
   fill(255);
    text("FORWARD", 415, 250);
    text("STOP", 455, 510);
    text("LEFT", 185, 510);
    text("RIGHT",730, 510);
}
 
 
void right_selected_confirmed(int r, int g, int b)
{
  //TRIANGLES
    stroke(255);
    fill(0, 0, 128);
    triangle(350, 300, 650, 300, 500, 100);
    fill(0, 0, 128);
    triangle(300, 350, 300, 650, 100, 500);
    fill(r, g, b);
    triangle(700, 350, 700, 650, 900, 500);
    //Circle
   fill(0, 0, 128);
   ellipse(500, 500, 250, 250);
    //TEXT
    fill(255);
    text("FORWARD", 415, 250);
    text("STOP", 455, 510);
    text("LEFT", 185, 510);
    text("RIGHT",730, 510);
}