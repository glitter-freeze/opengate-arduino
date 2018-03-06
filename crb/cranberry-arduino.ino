/*
    ╔═╗╔═╗╔═╗╔╗╔╔═╗╔═╗╔╦╗╔═╗   
    ║ ║╠═╝║╣ ║║║║ ╦╠═╣ ║ ║╣    
    ╚═╝╩  ╚═╝╝╚╝╚═╝╩ ╩ ╩ ╚═╝ 

    OPENGATE CRANBERRY v2.0 alpha 
    free gate control software by @glitter-freeze.
    
    This is ARDUINO sketch.
    
*/
#include <SPI.h>                // for RC522
#include <MFRC522.h>            // RC522 library
#include <Wire.h>               // for LCD connection via i2c
#include <LiquidCrystal_I2C.h>  // LCD 16x2 i2c connection library

#define SS_PIN 10
#define RST_PIN 9
#define buz 7                   // buzzer
#define led 3                   
#define led2 4

byte success[] = {              // ok symbol
  B00000,
  B00000,
  B00001,
  B00010,
  B10100,
  B01000,
  B00000,
  B00000
};
byte wrong[] = {                // wrong symbol
  B00000,
  B10001,
  B01010,
  B00100,
  B01010,
  B10001,
  B00000,
  B00000
};

MFRC522 mfrc522(SS_PIN, RST_PIN);

unsigned long uidDec, uidDecTemp; 
const char ogVersion[ ]=" v2.0a  ";
boolean printLcdStat=true;                      // for print "Wait for card..."


unsigned long ogcard[] = {834145942, 2313595510};   // here your card UID
boolean ogcardstat=false;
int c = sizeof(ogcard)/sizeof(*ogcard);             // num of ogcard[] elements

LiquidCrystal_I2C lcd(0x3F,16,2);                   // your LCD params (now sketch works only with 16x2 LCD)

void tonesuccess(){
  tone(buz, 1000, 200);
}
void tonewrong(){
  tone(buz, 500, 100);
  delay(100);
  tone(buz, 500, 100);
  delay(100);
}

void setup() {
  pinMode(led, OUTPUT);
  pinMode(led2, OUTPUT);
  digitalWrite(led,HIGH);
  Serial.begin(9600);
  Serial.println("--- OPENGATE cranberry v2.0 a ---");
  //------------------------------------------------------------------------------------
  lcd.init(); 
  lcd.backlight();
  lcd.createChar(1, success);
  lcd.createChar(0, wrong);
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("OPENGATE crb");  
  lcd.setCursor(1,1);
  lcd.print("v2.0 a");
  Serial.println("\nCards in memory:");
  Serial.println(c);
  Serial.println("\n");
  
  delay(1000);
  //------------------------------------------------------------------------------------
  Serial.println("Waiting for card...");
  SPI.begin();  
  mfrc522.PCD_Init();  
  tonesuccess();
  digitalWrite(led,LOW);
  //------------------------------------------------------------------------------------
}

void loop() {
  if(printLcdStat==true){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Wait for card..."); 
  }
  printLcdStat=false;
  
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  uidDec = 0;

  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    uidDecTemp = mfrc522.uid.uidByte[i];
    uidDec = uidDec * 256 + uidDecTemp;
  }
  Serial.println("OG Card UID: ");
  Serial.println(uidDec); 
  ogcardstat=false;
  for(int i=0; i<c; i++){
    unsigned long stat = ogcard[i];
    if (uidDec == stat) 
    {
      ogcardstat=true;
      break;
    }
    else{
      ogcardstat=false;
    }
  }
  if(ogcardstat==true){
    digitalWrite(led,HIGH);
    digitalWrite(led2,HIGH);
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.write(1);
    lcd.setCursor(3,0);
    lcd.print("Success");
    Serial.println("Success\n");
    tonesuccess();
    printLcdStat=true;
    delay(1000);
    digitalWrite(led,LOW);
    digitalWrite(led2,LOW);
  }
  else{
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.write(0);
      lcd.setCursor(3,0);
      lcd.print("Wrong card!");
      Serial.println("Wrong card!\n");
      tonewrong();
      printLcdStat=true;
      delay(1000);
      exit;
    }
  ogcardstat=false;
}
