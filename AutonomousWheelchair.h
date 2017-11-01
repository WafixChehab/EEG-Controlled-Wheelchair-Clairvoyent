#ifndef AutonomousWheelchair_h
#define AutonomousWheelchair_h
#include "Arduino.h"
/*
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno           Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <HMC5883L.h>
#include <SoftTimers.h>
#define SS_PIN 53
#define RST_PIN 22

byte CardRead[4];
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key; 
int error = 0;
float headingDegrees;
HMC5883L compass;
 ////////////////////////////////reader
class Reader{
  public:
  void  Init();
   void Readcard();
   void Readcompass();
   void Update();
   
  };
void Reader::Init(){
  Wire.begin(); // Start the I2C interface.
  SPI.begin(); // Init SPI bus

  rfid.PCD_Init(); // Init MFRC522 
  for (byte i = 0; i < 6; i++) {
  key.keyByte[i] = 0xFF;
  }
  /*
  Wire.begin(); // Start the I2C interface.

  compass = HMC5883L(); // Construct a new HMC5883 compass.
    

  error = compass.SetScale(1.3); // Set the scale of the compass.
  if(error != 0) ;// If there is an error, print it out.
   // Serial.println(compass.GetErrorText(error));
  
 // Serial.println("Setting measurement mode to continous.");
  error = compass.SetMeasurementMode(Measurement_Continuous); // Set the measurement mode to Continuous
  if(error != 0); // If there is an error, print it out.
   // Serial.println(compass.GetErrorText(error));
   */
}
void Reader::Readcard(){
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

//  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  //Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    return;
  }
  for(int i =0;i<4;i++)
  CardRead[i]=rfid.uid.uidByte[i];
  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}
/////////////////////////////////////////////////////////////////
void Reader::Readcompass(){
   MagnetometerRaw raw = compass.ReadRawAxis();
 
  MagnetometerScaled scaled = compass.ReadScaledAxis();
  
 
  int MilliGauss_OnThe_XAxis = scaled.XAxis;// (or YAxis, or ZAxis)

  
  float heading = atan2(scaled.YAxis, scaled.XAxis);
  
  float declinationAngle = 0.0457;
  heading += declinationAngle;
  
  // Correct for when signs are reversed.
  if(heading < 0)
    heading += 2*PI;
    
  // Check for wrap due to addition of declination.
  if(heading > 2*PI)
    heading -= 2*PI;
   
  // Convert radians to degrees for readability.
  headingDegrees = heading * 180/M_PI; 


}
void Reader::Update(){
  Readcard();
  //Readcompass();
  }
//////////////////////////////////////////////////////////////////end reader functions
 /////////////////////////////////////Point
class Point{
  public:
   byte card1[4];
   byte card2[4];
   char Location;
   bool Detected = false;
   ///////////functions
   void Update();
   void Detect();
   void ClearCard();
   Point(byte Card1[],byte Card2[], char location);   
   
  
};
///////////////////////////////////////////////////////////////////
Point::Point(byte Card1[],byte Card2[], char location){
  for(int i = 0 ; i<4 ; i++){
  card1[i] = Card1[i];
  card2[i] = Card2[i];
  }
  Location = location;
}

///////////////////////////////////////////////////////////////////
void Point::ClearCard(){
  for(int i=0;i<4;i++)
  CardRead[i]=0;
  
  }
///////////////////////////////////////////////////////////////////
void Point::Update(){
  Detect();
}
void Point::Detect(){
////////////////////////////////////////////////////////////////////////detect card 1
  int i =-1;
  do{
   i++;
   //Serial.println(i);
   //Serial.println(CardRead[i]);
   delay(5);
  }while(CardRead[i]==card1[i]&&i<=3);
  if(i == 4){
  Detected = true;
  return;
  }
  else
  Detected = false;
 
///////////////////////////////////////////////////////////////////////////////detect card 2
  i = -1;
  do{
   i++;
   //Serial.println(i);
   //Serial.println(CardRead[i]);
   delay(5);
   }while(CardRead[i]==card2[i]&&i<=3);
  if(i == 4){
  Detected = true;
  return;
  }
  else
   Detected = false;
}
/////////////////////////////////////////////////////////////////////////////end point functions
#endif