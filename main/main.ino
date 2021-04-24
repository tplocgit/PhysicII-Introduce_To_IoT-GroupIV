#include <LiquidCrystal_I2C.h>
#include "DHT.h"

#define DHTPIN D4
#define CN07PIN D3
#define DHTTYPE DHT11 // DHT 11
#define TEMPERATURE 0
#define HUMIDITY 1
#define SOUND 2
#define DEGREE 3
#define QUESTION_MARK 0
#define SOUND_DETECTED_MSG "DETECTED"
#define SILENCE_DETECTED_MSG "SILENCE"
#define BUZZER_PIN D5

bool silenceMode = 0;

byte DEG_SYMBOL[] = {
  B00110,
  B01001,
  B01001,
  B00110,
  B00000,
  B00000,
  B00000,
  B00000
};

byte QUESTION_MARK_SYMBOL[] = {
  B01110,
  B10001,
  B10001,
  B00011,
  B00100,
  B00100,
  B00000,
  B00100
};

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  lcd.init();                      // initialize the lcd 
  dht.begin();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.print("Hello, World!!!");
  lcd.createChar(byte(DEGREE), DEG_SYMBOL);
  lcd.createChar(byte(QUESTION_MARK), QUESTION_MARK_SYMBOL);
  pinMode(BUZZER_PIN, OUTPUT);
}

void printSymbolLCD(byte symbol) {
  lcd.write(symbol);
}

void printTemperatureLCD() {
  float c = dht.readTemperature(); //Celcius
  float f = dht.readTemperature(true); //Fahrenheit
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("TEMPERATURE");
  lcd.setCursor(0,1);
  lcd.print(c);
  printSymbolLCD(DEGREE);
  lcd.print("C ");
  lcd.print(f);
  printSymbolLCD(DEGREE);
  lcd.print("F");
}

void printHumidityLCD(){
  float h = dht.readHumidity();
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("HUMIDITY");
  lcd.setCursor(0,1);
  lcd.print(h);
  lcd.print("%RH");
}

bool soundDetected() {
  return !digitalRead(CN07PIN);
}

void printSoundLCD(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("SOUND");
  lcd.setCursor(0,1);
  if (soundDetected()) {
    lcd.print(SOUND_DETECTED_MSG);
    delay(1000);  
  }
  else lcd.print(SILENCE_DETECTED_MSG);
}

void printErrorOptionLCD(){
  lcd.setCursor(0,0);
  lcd.clear();
  lcd.print("ERROR: OCNF");
}

void printLCD(int option){
  switch(option) {
    case TEMPERATURE:
      printTemperatureLCD();
      break;
    case HUMIDITY:
      printHumidityLCD();
      break;
    case SOUND:
      printSoundLCD();
      break;
    default:
      printErrorOptionLCD();
  }
}

void playBuzzer() {
  // put your main code here, to run repeatedly:
  int i = 0;
  while(!soundDetected()){
    float sinVal = (sin(i*(3.1412/180)));
    int toneVal = 2000 + (int(sinVal * 1000)); 
    tone(BUZZER_PIN, toneVal);
    ++i; 
    i %= 180;
  }
}

void loop() {
  playBuzzer();
  if(soundDetected())
    silenceMode = true;
  delay(100);
  lcd.clear();
}
