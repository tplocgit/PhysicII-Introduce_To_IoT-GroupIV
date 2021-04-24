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

bool silenceMode = false;

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


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Music stuff
 /* 
  Tetris theme - (Korobeiniki) 
  Connect a piezo buzzer or speaker to pin 11 or select a new pin.
  More songs available at https://github.com/robsoncouto/arduino-songs                                            
                                              
                                              Robson Couto, 2019
*/

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST 0

// change this to make the song slower or faster
int tempo=144; 

// notes of the moledy followed by the duration.
// a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
// !!negative numbers are used to represent dotted notes,
// so -4 means a dotted quarter note, that is, a quarter plus an eighteenth!!
int melody[] = {

  //Based on the arrangement at https://www.flutetunes.com/tunes.php?id=192
  
  NOTE_E5, 4,  NOTE_B4,8,  NOTE_C5,8,  NOTE_D5,4,  NOTE_C5,8,  NOTE_B4,8,
  NOTE_A4, 4,  NOTE_A4,8,  NOTE_C5,8,  NOTE_E5,4,  NOTE_D5,8,  NOTE_C5,8,
  NOTE_B4, -4,  NOTE_C5,8,  NOTE_D5,4,  NOTE_E5,4,
  NOTE_C5, 4,  NOTE_A4,4,  NOTE_A4,8,  NOTE_A4,4,  NOTE_B4,8,  NOTE_C5,8,

  NOTE_D5, -4,  NOTE_F5,8,  NOTE_A5,4,  NOTE_G5,8,  NOTE_F5,8,
  NOTE_E5, -4,  NOTE_C5,8,  NOTE_E5,4,  NOTE_D5,8,  NOTE_C5,8,
  NOTE_B4, 4,  NOTE_B4,8,  NOTE_C5,8,  NOTE_D5,4,  NOTE_E5,4,
  NOTE_C5, 4,  NOTE_A4,4,  NOTE_A4,4, REST, 4,

  NOTE_E5, 4,  NOTE_B4,8,  NOTE_C5,8,  NOTE_D5,4,  NOTE_C5,8,  NOTE_B4,8,
  NOTE_A4, 4,  NOTE_A4,8,  NOTE_C5,8,  NOTE_E5,4,  NOTE_D5,8,  NOTE_C5,8,
  NOTE_B4, -4,  NOTE_C5,8,  NOTE_D5,4,  NOTE_E5,4,
  NOTE_C5, 4,  NOTE_A4,4,  NOTE_A4,8,  NOTE_A4,4,  NOTE_B4,8,  NOTE_C5,8,

  NOTE_D5, -4,  NOTE_F5,8,  NOTE_A5,4,  NOTE_G5,8,  NOTE_F5,8,
  NOTE_E5, -4,  NOTE_C5,8,  NOTE_E5,4,  NOTE_D5,8,  NOTE_C5,8,
  NOTE_B4, 4,  NOTE_B4,8,  NOTE_C5,8,  NOTE_D5,4,  NOTE_E5,4,
  NOTE_C5, 4,  NOTE_A4,4,  NOTE_A4,4, REST, 4,
  

  NOTE_E5,2,  NOTE_C5,2,
  NOTE_D5,2,   NOTE_B4,2,
  NOTE_C5,2,   NOTE_A4,2,
  NOTE_GS4,2,  NOTE_B4,4,  REST,8, 
  NOTE_E5,2,   NOTE_C5,2,
  NOTE_D5,2,   NOTE_B4,2,
  NOTE_C5,4,   NOTE_E5,4,  NOTE_A5,2,
  NOTE_GS5,2,

};

// sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
// there are two values per note (pitch and duration), so for each note there are four bytes
int notes=sizeof(melody)/sizeof(melody[0])/2; 

// this calculates the duration of a whole note in ms (60s/tempo)*4 beats
int wholenote = (60000 * 4) / tempo;

int divider = 0, noteDuration = 0;
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


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
  while(!silenceMode){
    float sinVal = (sin(i*(3.1412/180)));
    int toneVal = 2000 + (int(sinVal * 1000)); 
    tone(BUZZER_PIN, toneVal);
    ++i; 
    i %= 180;
    delay(2);
    if(soundDetected()){
      noTone(BUZZER_PIN);
      silenceMode = true;
      break;  
    }
  }
}

void playSong() {
  // iterate over the notes of the melody. 
  // Remember, the array is twice the number of notes (notes + durations)
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    divider = melody[thisNote + 1];
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(BUZZER_PIN, melody[thisNote], noteDuration*0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);
    
    // stop the waveform generation before the next note.
    noTone(BUZZER_PIN);

     if(soundDetected()){
      noTone(BUZZER_PIN);
      silenceMode = true;
      break;  
    }
  } 
}

void loop() {
  /*if(!silenceMode) {
    //playBuzzer();
    playSong();
  }
  else
    noTone(BUZZER_PIN);
  if(silenceMode && soundDetected())
    silenceMode = false;*/
  printLCD(SOUND);
  delay(100);
  lcd.clear();
}
