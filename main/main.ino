#include <LiquidCrystal_I2C.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include "DHT.h"
#include <Time.h>;
#include <NTPClient_Generic.h>
// change next line to use with another board/shield
#include <ESP8266WiFi.h>
//#include <WiFi.h> // for WiFi shield
//#include <WiFi101.h> // for WiFi 101 shield or MKR1000
#include <WiFiUdp.h>
//for LED status
#include <Ticker.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>

// defines
#ifndef LED_BUILTIN
#define LED_BUILTIN 13 // ESP32 DOES NOT DEFINE LED_BUILTIN
#endif

int LED = LED_BUILTIN;

// Pin && CONST
#define DHTPIN D4 
#define CN07PIN D5
#define BUZZER_PIN D6
#define BUTTON_STOP_SOUND_PIN D7
#define BUTTON_NEXT_PIN D8
#define BUTTON_PREV_PIN 3 
#define DHTTYPE DHT11 // DHT 11
#define VN_TIME_OFFSET 25200 // VN lead (+7hour * 60secs) seconds
#define MQTT_SERVER "broker.mqtt-dashboard.com"
#define IN_TOPIC "inTopic4444"
#define TEMPERATURE_CELSIUS_TOPIC "ab2d253a386ab74e2ffe63437d49587d"
#define TEMPERATURE_FAHRENHEIT_TOPIC "10c633befbfb97d88c7cb929a924670c"
#define HUMIDITY_TOPIC "cf10703b86de19ab2c80a82901b41460"
#define SOUND_TOPIC "73b2b6a2eb898ac5a6bb84feb6aebb3a"
#define SET_ALARM_TOPIC "8bd85b302e7a9fc32ff59b51d40e7c4b"
#define MQTT_SERVER_PORT 1883

// Options
#define TEMPERATURE 1
#define HUMIDITY 2
#define SOUND 3
#define TIME 0
#define NUMBER_OF_DISPLAY_OPTION 4

// For LCD display
#define DEGREE 3
#define MUSIC_NODE 4
#define QUESTION_MARK 0
#define SOUND_DETECTED_MSG "DETECTED"
#define SILENCE_DETECTED_MSG "SILENCE"
#define MAX_JSON_BUF 1024

Ticker ticker;

WiFiUDP ntpUDP;

// By default 'pool.ntp.org' is used with 60 seconds update interval and
// no offset
NTPClient timeClient(ntpUDP);

// You can specify the time server pool and the offset, (in seconds)
// additionally you can specify the update interval (in milliseconds).
// NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);
WiFiClient espClient;
PubSubClient client(espClient);

// Reuseable global vars
String formattedDate;
String dayStamp;
String timeStamp;
long lastMsg = 0;
char msg[50];
int value = 0;
float count = 0;
DynamicJsonDocument doc(MAX_JSON_BUF);
unsigned int alarmSeconds = 0;
bool repeat = false;
bool rang = true;
int displayOption = TIME;
unsigned long lastMillis = 0;


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
byte MUSIC_SYMBOL[] = {
  B00000,
  B01111,
  B01001,
  B01111,
  B01001,
  B11011,
  B11011,
  B00000
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
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe(SET_ALARM_TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void sendNum(float value){
  if (!client.connected()) {
    reconnect();
  }
  String pubString = String(value);
  char* buf = new char[pubString.length() + 1];
  pubString.toCharArray(buf, pubString.length()+1);
  Serial.println("Sending: iot/humidity -> " + pubString + "%");
  client.publish(TEMPERATURE_CELSIUS_TOPIC, buf);
  buf = NULL;
  delete buf;
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("Message arrived on topic ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    messageTemp += (char)payload[i];
  }

  if(String(topic) == SET_ALARM_TOPIC) {
      Serial.print("SET ALARM TOPIC -> ");
      Serial.println("Message: ");
      DeserializationError error = deserializeJson(doc, messageTemp);
      repeat = doc["repeat"];
      alarmSeconds = doc["seconds"];
      Serial.println("Time: " + String(alarmSeconds));
      Serial.println("Repeat: " + String(repeat));
  }

  
  Serial.println("Hey you, your're finally awake. You see ...");
  //delay(5000);
}

void MQTTClientSetup(){
  //_________________________________________________

  client.setServer(MQTT_SERVER, MQTT_SERVER_PORT);
  client.subscribe(SET_ALARM_TOPIC);
  client.setCallback(callback);
 
  //delay(1000);
}

void sendMessage(String topic, String msg){
  if (!client.connected()) {
    reconnect();
  }
  char* buf = new char[msg.length() + 1];
  char* topicBuffer = new char[topic.length() + 1];
  msg.toCharArray(buf, msg.length()+1);
  topic.toCharArray(topicBuffer, topic.length() + 1);
  //Serial.println("Sending: " + topic + " -> " + msg);
  client.publish(topicBuffer, buf);
  
  delete buf, topicBuffer;
}

void tick()
{
  //toggle state
  digitalWrite(LED, !digitalRead(LED));     // set pin to the opposite state
}
//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);
}

void lcdSetup(){
  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.print("Hello, World!!!");
  lcd.createChar(byte(DEGREE), DEG_SYMBOL);
  lcd.createChar(byte(QUESTION_MARK), QUESTION_MARK_SYMBOL);
  lcd.createChar(byte(MUSIC_NODE), MUSIC_SYMBOL);
}

void autoConnectWifiSetup(){
  lcd.print("Connecting Wifi ...");
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  // put your setup code here, to run once:
  
  //set led pin as output
  pinMode(LED, OUTPUT);
  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.6, tick);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;
  //reset settings - for testing
  // wm.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wm.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wm.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(1000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Connected!");
  ticker.detach();
  //keep LED on
  digitalWrite(LED, LOW);
}

void setup()
{
  Serial.begin(115200);
  lcdSetup();
  autoConnectWifiSetup();
  MQTTClientSetup();
  timeClient.begin();
  timeClient.setTimeOffset(VN_TIME_OFFSET);
  dht.begin();
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_STOP_SOUND_PIN,INPUT);
  
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

void printTimeLCD(){
  timeClient.update();
  int curYear = timeClient.getYear();
  String curMonth = timeClient.getMonthStr();
  int curDay = timeClient.getDay();
  String curDoW = timeClient.getDoW();
  String curHours = timeClient.getStrHours();
  String curMinutes = timeClient.getStrMinutes();
  String curSeconds = timeClient.getStrSeconds();
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(curDoW);
  lcd.print(" ");
  lcd.print(curMonth);
  lcd.print(" ");
  lcd.print(curDay);
  lcd.print(" ");
  lcd.print(curYear);

  lcd.setCursor(4,1);
  lcd.print(curHours);
  lcd.print(":");
  lcd.print(curMinutes);
  lcd.print(":");
  lcd.print(curSeconds);
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
    case TIME:
      printTimeLCD();
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
  //if(silenceMode) return;
  int dot = 0;
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {
    Serial.println(silenceMode);
    lcd.setCursor(0,0);
    lcd.print("Playing song");
    lcd.setCursor(8,1);
    for(int i = 0; i <= dot; ++i)
      printSymbolLCD(MUSIC_NODE);
    dot++;
    dot %= 8;
    if(buttonPressed(BUTTON_STOP_SOUND_PIN)) {
        silenceMode = !silenceMode;
        break;
    }
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
    lcd.clear();
  } 
}

bool buttonPressed(int btnPin) {
  return digitalRead(btnPin) == HIGH;
}

void sendSensorsDataMessage() {
  sendMessage(TEMPERATURE_CELSIUS_TOPIC, String(dht.readTemperature()));
  sendMessage(TEMPERATURE_FAHRENHEIT_TOPIC, String(dht.readTemperature(true)));
  sendMessage(HUMIDITY_TOPIC, String(dht.readHumidity()));
  sendMessage(SOUND_TOPIC, String(soundDetected() ? 1 : 0));
}

void runAlarm() {
    if(timeClient.getEpochTime() == alarmSeconds) {
      if (repeat)
        alarmSeconds += 24*3600;
      
      Serial.print("Wakeup");
      playSong();
  }
}

void handleDisplayOptionChange() {
  int modDOption = displayOption % NUMBER_OF_DISPLAY_OPTION;
  displayOption = displayOption >= 0 ? modDOption : modDOption + NUMBER_OF_DISPLAY_OPTION;
}

void buttonPressedHandle() {
  if(buttonPressed(BUTTON_NEXT_PIN)) {
    Serial.println(String(BUTTON_NEXT_PIN) + " Pressed");
    ++displayOption;
  }
  if(buttonPressed(BUTTON_PREV_PIN)) { 
    Serial.println(String(BUTTON_PREV_PIN) + " Pressed");
    --displayOption;
  }
  
  handleDisplayOptionChange();
}

void handleLCDDisplay(){
  
    printLCD(displayOption);
 
}

void loop() {
  if (!client.connected())
    reconnect();
  client.loop();
  buttonPressedHandle();
  handleLCDDisplay();
  runAlarm();
  sendSensorsDataMessage();
  delay(220);
}
