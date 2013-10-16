/* //////////////////////////////////////////////////
    This code is  a project to build a greenhouse
    by
    João Gonçalves
    
*/ //////////////////////////////////////////////////
#include <DHT22.h>
#include <stdio.h> // Only used for sprintf
#include <LiquidCrystal.h>
#include <LCDKeypad.h>
#include <SD.h>
#include <Metro.h>
#include "Wire.h"
#define DS1307_ADDRESS 0x68
#define DHT22_PIN 6
volatile int state = 0;

/*
* SD card attached to SPI bus as follows:
** MOSI - pin 11
** MISO - pin 12
** CLK - pin 13
** CS - pin 8
// Data wire is plugged into port 7 on the Arduino
*/

DHT22_ERROR_t errorCode;
const int chipSelect = 8;
byte zero = 0x00; //workaround for issue #527

int ledPin = 13;
int blacklight = 10;

// Setup a DHT22 instance
DHT22 myDHT22(DHT22_PIN);
// initialize the library with the numbers of the interface pins
LCDKeypad lcd;
//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);  //with normal lcd
char start_msg[] = "GreenHouse v0.1";

int buttonPressed;

  char * item[] = {"Set Relays      ","Set Date-Time   "};
  int str = 1;

boolean PAUSE(int time)
{
  int newstate = state;
  // para compensar o atraso em relação ao outro heart
  boolean exit = false; 
  Metro pause1 = Metro(time); 
  while(!pause1.check())
  {
    if (lcd.button() != KEYPAD_NONE){
      exit = true;
    }
    if(exit)break;
    
  }

return exit;
}

// FUNCTIONS DECLARATION ///////////////////////////////////////

void setDateTime(byte second,byte minute,byte hour,byte weekDay,byte monthDay,byte month,byte year);
byte decToBcd(byte val); //retreive date from RTC
byte bcdToDec(byte val); //retreive date from RTC
void printDate();  // print time/date to serial por

/////////// SETUP ///////////////////////////////////////////////
void setup(void)
{
  Wire.begin();
  Serial.begin(9600);
 
  setDateTime(0,51,23,6,11,10,13);
  /* second =      0; //0-59
   minute =      51; //0-59
   hour =        23; //0-23
   weekDay =     6; //1-7
   monthDay =    11; //1-31
   month =       10; //1-12
   year  =       13; //0-99*/
 //MUST CONFIGURE IN FUNCTION
 
  pinMode(blacklight,OUTPUT);   // blacklight LCD
  digitalWrite(blacklight,HIGH);// blacklight LCD
   // set up the LCD's number of columns and rows: 
  lcd.begin(16,2);
  lcd.setCursor(0, 0);
  lcd.print(start_msg);
  lcd.setCursor(0, 1);
  for (int thisChar = 0; thisChar < 16; thisChar++){
  lcd.setCursor(thisChar, 1);
  lcd.print(".");
  delay(100);}
  lcd.clear();
  lcd.setCursor(0, 0);
  /////////////////////////////////////////////////////
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    lcd.println("Card failed");
    delay(250);
    return;// don't do anything more:
  }
   Serial.println("card initialized.");
   lcd.println("card initialized.");
   delay(500);
   lcd.clear();
   digitalWrite(blacklight,HIGH);
   errorCode = myDHT22.readData();
}

void loop(void)
{ 
  String dataString = "";// new string at each loop
  // The sensor can only be read from every 1-2s, and requires a minimum
  // 2s warm-up after power-on.
  //delay(2000);

 /* buttonPressed=waitButton();
  waitReleaseButton();
  if (buttonPressed==KEYPAD_SELECT){
   digitalWrite(blacklight,LOW);
   delay(1000);}
  */
if(lcd.button() !=KEYPAD_NONE ){
  if(lcd.button() == KEYPAD_UP && state == 0){
     delay(120);
     if(lcd.button() == KEYPAD_UP && state == 0) str++;}
  if(lcd.button() == KEYPAD_DOWN && state == 0){
    delay(120);
    if(lcd.button() == KEYPAD_DOWN && state == 0) str--;}
  if (str >2 || str == 1) {
    str= 1;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(item[str-1]);
    PAUSE(500);}
  if (str <1 || str == 2){
    str = 2;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(item[str-1]);
    PAUSE(500);}
    if(lcd.button() == KEYPAD_SELECT){
     delay(120);
     if(lcd.button() == KEYPAD_SELECT)state = str;}
}
  switch(state)
  {
    case 0:
    
              if (PAUSE(2000))
                 break;
              else{
                        errorCode = myDHT22.readData();
                        if(errorCode == DHT_ERROR_NONE){
                                            lcd.setCursor(0, 0);
                                            lcd.print("TEMP: ");
                                            lcd.print(myDHT22.getTemperatureC());
                                            lcd.print(" C ");
                                            lcd.setCursor(0, 1);
                                            lcd.print("HUM : ");
                                            lcd.print(myDHT22.getHumidity());
                                            lcd.print(" % ");}
                        else{
                          lcd.clear();
                          lcd.setCursor(0, 0);
                          lcd.print("DHT22 ERROR");}
              }
    break;
    case 1:  //Set relays
              ////// Set Relay Number////////////////////////////////////
              
              int relay_number;
              int k;
              boolean aux2;
             aux2 =  false;
              k=0;
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Relay Number ?");
              lcd.setCursor(0, 1);
              waitButton();

              while(!aux2){
                 if(lcd.button() == KEYPAD_SELECT){
                 delay(100);
                 if(lcd.button() == KEYPAD_SELECT) aux2=true;}
                if(lcd.button() == KEYPAD_UP){
                 delay(100);
                 if(lcd.button() == KEYPAD_UP) k++;}
                if(lcd.button() == KEYPAD_DOWN){
                 delay(100);
                 if(lcd.button() == KEYPAD_DOWN) k--;}
                if(k >2) k =2;
                if (k <1) k = 1;
                lcd.setCursor(0,1);
                lcd.print("  ");
                lcd.print(k);}
                relay_number = k;
              
    break;
    case 2:  //Set Date And Time
              byte second =      0; //0-59
              byte minute =      00; //0-59
              byte hour =        00; //0-23
              byte weekDay =     1; //1-7
              byte monthDay =    1; //1-31
              byte month =       1; //1-12
              byte year  =       13; //0-99
              ////// WEEKDAY ////////////////////////////////////
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("WeekDay (1-7) ?");
              lcd.setCursor(0, 1);
              waitButton();
              int i=0;
              boolean aux = false;
              while(!aux){
                 if(lcd.button() == KEYPAD_SELECT){
                 delay(100);
                 if(lcd.button() == KEYPAD_SELECT) aux=true;}
                if(lcd.button() == KEYPAD_UP){
                 delay(100);
                 if(lcd.button() == KEYPAD_UP) i++;}
                if(lcd.button() == KEYPAD_DOWN){
                 delay(100);
                 if(lcd.button() == KEYPAD_DOWN) i--;}
                if(i >7) i =7;
                if (i <1) i = 1;
                lcd.setCursor(0,1);
                lcd.print("  ");
                lcd.print(i);}
                weekDay = i;
                
              ////// DAY ////////////////////////////////////
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Day (1-31) ?");
              lcd.setCursor(0, 1);
              waitButton();
              i=0;
              aux = false;
              while(!aux){
                 if(lcd.button() == KEYPAD_SELECT){
                 delay(100);
                 if(lcd.button() == KEYPAD_SELECT) aux=true;}
                if(lcd.button() == KEYPAD_UP){
                 delay(100);
                 if(lcd.button() == KEYPAD_UP) i++;}
                if(lcd.button() == KEYPAD_DOWN){
                 delay(100);
                 if(lcd.button() == KEYPAD_DOWN) i--;}
                if(i >31) i =31;
                if (i <1) i = 1;
                lcd.setCursor(0,1);
                lcd.print("  ");
                lcd.print(i);}
                monthDay=i;
                
               ////  Month  ///////////////////////////////////////////////////////
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Mounth (1-12) ?");
              lcd.setCursor(0, 1);
              waitButton();
               i=0;
              aux = false;
              while(!aux){
                 if(lcd.button() == KEYPAD_SELECT){
                 delay(100);
                 if(lcd.button() == KEYPAD_SELECT) aux=true;}
                if(lcd.button() == KEYPAD_UP){
                 delay(100);
                 if(lcd.button() == KEYPAD_UP) i++;}
                if(lcd.button() == KEYPAD_DOWN){
                 delay(100);
                 if(lcd.button() == KEYPAD_DOWN) i--;}
                if(i >12) i =12;
                if (i <1) i = 1;
                lcd.setCursor(0,1);
                lcd.print("  ");
                lcd.print(i);}
                month=i;
                
              //// YEAR////////////////////////////////////////////////////////
                                           lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Year (0-99) ?");
              lcd.setCursor(0, 1);
              waitButton();
              i=0;
              aux = false;
              while(!aux){
                 if(lcd.button() == KEYPAD_SELECT){
                 delay(100);
                 if(lcd.button() == KEYPAD_SELECT) aux=true;}
                if(lcd.button() == KEYPAD_UP){
                 delay(100);
                 if(lcd.button() == KEYPAD_UP) i++;}
                if(lcd.button() == KEYPAD_DOWN){
                 delay(100);
                 if(lcd.button() == KEYPAD_DOWN) i--;}
                if(i >99) i =99;
                if (i <0) i = 0;
                lcd.setCursor(0,1);
                lcd.print("  ");
                lcd.print(i);}
                year = i;
                
                /////////////////////// HOUR ////////////////////////////////////////////
               lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("HOUR ?");
              lcd.setCursor(0, 1);
              waitButton();
             i=0;
              aux = false;
              while(!aux){
                 if(lcd.button() == KEYPAD_SELECT){
                 delay(100);
                 if(lcd.button() == KEYPAD_SELECT) aux=true;}
                if(lcd.button() == KEYPAD_UP){
                 delay(100);
                 if(lcd.button() == KEYPAD_UP) i++;}
                if(lcd.button() == KEYPAD_DOWN){
                 delay(100);
                 if(lcd.button() == KEYPAD_DOWN) i--;}
                if(i >23) i = 23;
                if (i <0) i = 0;
                lcd.setCursor(0,1);
                lcd.print("  ");
                lcd.print(i);}
                hour = i;
                
               ////Minutes
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Minutes ?");
              lcd.setCursor(0, 1);
              waitButton();
               i=0;
              aux = false;
              while(!aux){
                 if(lcd.button() == KEYPAD_SELECT){
                 delay(100);
                 if(lcd.button() == KEYPAD_SELECT) aux=true;}
                if(lcd.button() == KEYPAD_UP){
                 delay(100);
                 if(lcd.button() == KEYPAD_UP) i++;}
                if(lcd.button() == KEYPAD_DOWN){
                 delay(100);
                 if(lcd.button() == KEYPAD_DOWN) i--;}
                if(i >59) i =59;
                if (i <0) i = 0;
                lcd.setCursor(0,1);
                lcd.print("  ");
                lcd.print(i);}
                minute = i;
              
              //// SECONDS
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("SECONDS ?");
              lcd.setCursor(0, 1);
              waitButton();
              i=0;
              aux = false;
              while(!aux){
                 if(lcd.button() == KEYPAD_SELECT){
                 delay(100);
                 if(lcd.button() == KEYPAD_SELECT) aux=true;}
                if(lcd.button() == KEYPAD_UP){
                 delay(100);
                 if(lcd.button() == KEYPAD_UP) i++;}
                if(lcd.button() == KEYPAD_DOWN){
                 delay(100);
                 if(lcd.button() == KEYPAD_DOWN) i--;}
                if(i >59) i =59;
                if (i <0) i = 0;
                lcd.setCursor(0,1);
                lcd.print("  ");
                lcd.print(i);}
                second = i; 
                setDateTime(second,minute,hour,weekDay,monthDay,month,year);
                lcd.clear();
                lcd.setCursor(0, 0); 
                lcd.print("SETTINGS SAVED");
                delay(2000);
                lcd.clear();
                lcd.setCursor(0, 0); 
                lcd.print(String((int)hour)+"h "+String((int)minute)+"min "+String((int)second)+"sec"); 
                lcd.setCursor(0, 1); 
                lcd.print(String((int)monthDay)+"-"+String((int)month)+"-"+String((int)year)); 
                delay(3000);
                state = 0;
    break;
 
  }


  
      // Reset the register pointer
      Wire.beginTransmission(DS1307_ADDRESS);
      Wire.write(zero);
      Wire.endTransmission();
      Wire.requestFrom(DS1307_ADDRESS,7 );
      int second = bcdToDec(Wire.read());
      int minute = bcdToDec(Wire.read());
      int hour = bcdToDec(Wire.read() & 0b111111); //24 hour time
      int weekDay = bcdToDec(Wire.read()); //0-6 -> sunday - Saturday
      int monthDay = bcdToDec(Wire.read());
      int month = bcdToDec(Wire.read());
      int year = bcdToDec(Wire.read());
      dataString += "d";
      dataString += String((int)monthDay);
      dataString += "m";
      dataString += String((int)month);
      dataString += "y";
      dataString += String((int)year);
      dataString += "-";
      dataString += String((int)hour);
      dataString += ":";
      dataString += String((int)minute);
      dataString += "t";
      dataString += String((int) myDHT22.getTemperatureC());
      dataString += "h";
      dataString += String((int)myDHT22.getHumidity());
      
      ///////  DATA LOG  ////////////////////////////////////////////////
      // open the file. note that only one file can be open at a time,
      // so you have to close this one before opening another.
      File dataFile = SD.open("datalog.txt", FILE_WRITE);
      if (dataFile) {               // if the file is available, write to it:
        dataFile.println(dataString);
        dataFile.close();          
        Serial.println(dataString); // print to the serial port too:
      }  
      else {
        Serial.println("Error opening File");// if the file isn't open, pop up an error:
      }

}


void setDateTime(byte second,byte minute,byte hour,byte weekDay,byte monthDay,byte month,byte year){
  /* second =      0; //0-59
   minute =      51; //0-59
   hour =        23; //0-23
   weekDay =     6; //1-7
   monthDay =    11; //1-31
   month =       10; //1-12
   year  =       13; //0-99*/
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero); //stop Oscillator
  
  Wire.write(decToBcd(second));
  Wire.write(decToBcd(minute));
  Wire.write(decToBcd(hour));
  Wire.write(decToBcd(weekDay));
  Wire.write(decToBcd(monthDay));
  Wire.write(decToBcd(month));
  Wire.write(decToBcd(year));
  Wire.write(zero); //start 
  Wire.endTransmission();

}

byte decToBcd(byte val){
// Convert normal decimal numbers to binary coded decimal
  return ( (val/10*16) + (val%10) );
}

byte bcdToDec(byte val)  {
// Convert binary coded decimal to normal decimal numbers
  return ( (val/16*10) + (val%16) );
}

void printDate(){

  // Reset the register pointer
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero);
  Wire.endTransmission();

  Wire.requestFrom(DS1307_ADDRESS, 7);

  int second = bcdToDec(Wire.read());
  int minute = bcdToDec(Wire.read());
  int hour = bcdToDec(Wire.read() & 0b111111); //24 hour time
  int weekDay = bcdToDec(Wire.read()); //0-6 -> sunday - Saturday
  int monthDay = bcdToDec(Wire.read());
  int month = bcdToDec(Wire.read());
  int year = bcdToDec(Wire.read());

  //print the date EG   3/1/11 23:59:59
   Serial.println("Data Set");
  Serial.print(month);
  Serial.print("/");
  Serial.print(monthDay);
  Serial.print("/");
  Serial.print(year);
  Serial.print(" ");
  Serial.print(hour);
  Serial.print(":");
  Serial.print(minute);
  Serial.print(":");
  Serial.println(second);
  Serial.println("");

}

int waitButton()
{
  int buttonPressed; 
  waitReleaseButton;
  lcd.blink();
  while((buttonPressed=lcd.button())==KEYPAD_NONE)
  {
  }
  delay(50);  
  lcd.noBlink();
  return buttonPressed;
}

void waitReleaseButton()
{
  delay(50);
  while(lcd.button()!=KEYPAD_NONE)
  {
  }
  delay(50);
}/*
boolean debounce(int time, char button){
boolean aux;
    if (lcd.button() == button)
    delay(time);
    if (lcd.button() == button) aux == true;
    return aux;
}*/
