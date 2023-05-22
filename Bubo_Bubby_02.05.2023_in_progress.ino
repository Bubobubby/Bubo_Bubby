
#include <LiquidCrystal_I2C.h>  // activates the LCD I2C library
#include <RTClib.h> 
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <Wire.h>                   // deals with I2C connections

LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS3231 rtc;
SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
int LED = 4;
int LED2 = 8;
int PIR = 2;

//const int volumePin = A0;  // linijka od sterowania głośnością do usunięcia
//int volumeState = 0;      // linijka od sterowania głośnością do usunięcia
volatile boolean moveDetected = false;
volatile boolean musicAllowed = true;
DateTime interruptTime;

//void printDetail(uint8_t type, int value);  // linijka od sterowania głośnością do usunięcia
 

void setup() {
  //Inicjalizacja
  lcd.init();
  lcd.backlight();

  rtc.begin();
  mySoftwareSerial.begin(9600);
  Serial.begin(115200);
  Serial.println("Start");  // 

  // pinMode(PIR, INPUT);
  pinMode(LED, OUTPUT);

  myDFPlayer.begin(mySoftwareSerial);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  myDFPlayer.setTimeOut(500);
  //Set volume value (From 0 to 30)
  myDFPlayer.volume(30);




  //Wyświetlanie tekstu początkowego
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Zaczynamy!!!"); 
  Serial.println("Zaczynamy!!!");
  
  delay(1000); 
  lcd.clear();
  lcd.setCursor(8,0);
  lcd.print("3");
  Serial.println("3");
  
  delay(1000);
  lcd.clear();
  lcd.setCursor(8,0);
  lcd.print("2"); 
  Serial.println("2");
  
  delay(1000);
  lcd.clear();
  lcd.setCursor(8,0);
  lcd.print("1");
  Serial.println("1");
  
  delay(1000);  

  //Inicjalizacja przerwania dla PIR

  pinMode(PIR, INPUT_PULLUP);
  attachInterrupt(0, pirAction, RISING);
}

void pirAction() {
  moveDetected = true;
}

int updateRTC() {
  DateTime rtcTime = rtc.now();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Godzina: ");
  lcd.print(rtcTime.hour());
  lcd.setCursor(0,1);           
  lcd.print("Minuta: ");
  lcd.print(rtcTime.minute());
  lcd.print(" ");
  lcd.print(rtcTime.second());
}


void log(String text) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(text);
  Serial.println(text);
}

void log(String text, String text2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(text);
  Serial.println(text);
  lcd.setCursor(0,1);
  lcd.print(text2);  
  Serial.println(text2);   
}

int selectTrack(int dayNumber) {
  int trackCount = myDFPlayer.readFileCountsInFolder(dayNumber);
  long value = millis();
  int trackNumber = (value % trackCount) + 1;
  Serial.println(trackCount);
  Serial.println(myDFPlayer.readFileCountsInFolder(dayNumber));
  return trackNumber;
}

int selectFolder(DateTime rtcTime) {
  // to_do: implementation

  int dayOfWeek = rtcTime.dayOfTheWeek(); // 0 (Sunday) to 6 (Saturday).
  int dayOfMonth = rtcTime.day();


  log("Dzien: ", (String) rtcTime.day());
  delay(50);
  log("Dzien tygodnia: ", (String) rtcTime.dayOfTheWeek());
  delay(50);
  log("Miesiac: ", (String) rtcTime.month());
  delay(50);
  log("Rok: ", (String) rtcTime.year());
  delay(50);
  
  
//  int dayOfMonth = 11;
 // int dayOfWeek = 3;

  int weekNumber = (dayOfMonth - 1) / 7;
  weekNumber = weekNumber % 4;
  int folderNumber = dayOfWeek + weekNumber * 7; 

  log("Folder: ", (String) folderNumber);
  delay(50);

  return folderNumber;
  // return trackNumber;

}

void loop() {
  
  updateRTC();
  delay(50);
  
  DateTime rtcTime = rtc.now();
  if( rtcTime.hour() >=8 &&  rtcTime.hour() <=20){
  if (musicAllowed && moveDetected) {
//    updateRTC();
    interruptTime = (rtcTime + TimeSpan(0, 0,5,0 )); // (dzień, godzina, minuta, sekunda)
    int folderNumber = selectFolder(rtcTime);
    int trackNumber = selectTrack(folderNumber);
    log("Wybrany utwor", (String) trackNumber);
    delay(1000);
    

    digitalWrite(LED, LOW);
    digitalWrite(LED2, HIGH); 
     //Set volume from poti
    
//    const int volumePin = A0;                    // linijka od sterowania głośnością do usunięcia
//    byte volumeState = 254;  
//
//     int read = analogRead (volumePin);
//     byte state = map (read, 0, 1023, 30, 5);
//
//    if (state < volumeState - 1 || state > volumeState + 1) {
//           volumeState = state;
//           myDFPlayer.volume(volumeState);
//           myDFPlayer.volume(volumeState);
//           Serial.print (volumePin);
//           Serial.print ("volume");
//           Serial.println (volumeState);
//           Serial.println(myDFPlayer.readVolume());                   // linijka od sterowania głośnością do usunięcia
//       }
    myDFPlayer.playFolder(folderNumber,trackNumber);   //trackNumber
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Odtwarzam");
    Serial.println("Odtwarzam");
    delay(500);
    moveDetected = false;
    musicAllowed = false;
  }

  if ((rtcTime > interruptTime) && !musicAllowed) {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Czujnik aktywny");     // Chciałbym by napis pozostał aż do aktywacji
    Serial.println("Czujnik aktywny");
    digitalWrite(LED, HIGH);
    delay(2000);
    musicAllowed = true;
    moveDetected = false;
  }
else{
       digitalWrite(LED, HIGH);
       digitalWrite(LED2, LOW);
}
  }
}


  // DateTime rtcTime = rtc.now();
  // if( rtcTime.hour() >=0 || rtcTime.hour() <=0) {
  //    //using the digitalRead function we will read the signal of the sensor
  //   int value = digitalRead(PIR);
  //   //if its high or if an any object is detected it will activate the LED and Buzzer
  //   lcd.clear();
  //   lcd.setCursor(0, 0);
  //   lcd.print("Wartosc PIR: ");
  //   lcd.print(value);
  //   if (value == HIGH){
  //     lcd.setCursor(0, 1);
  //     lcd.print("Odtwarzam");
  //     DateTime rtcTime = rtc.now();
  //     int dayNumber = rtcTime.hour();
  //     int trackNumber = (dayNumber % 5) +1;
      
  //     myDFPlayer.play(trackNumber);
  //     delay(10000);
  //   } else {
  //      digitalWrite(LED, HIGH);
  //   } 
  // }
