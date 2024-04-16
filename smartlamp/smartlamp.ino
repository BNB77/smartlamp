#include <NTPClient.h>                  //NTP клиент 
#include <ESP8266WiFi.h>                //Предоставляет специальные процедуры Wi-Fi для ESP8266, которые мы вызываем для подключения к сети
#include <WiFiUdp.h>                    //Обрабатывает отправку и получение пакетов UDP
#include <OLED_I2C.h>                   //Библиотека дисплея
#include <DNSServer.h>                  //Локальный DNS сервер для перенаправления всех запросов на страницу конфигурации
#include <ESP8266WebServer.h>           //Локальный веб сервер для страници конфигурации WiFi
#include <WiFiManager.h>                //Библиотека для удобного подключения к WiFi
#include <ESP8266HTTPClient.h>          //HTTP клиент
#include <ArduinoJson.h>
#include <microDS3231.h>

#define SCLK D5  
#define RCLK D0     
#define DIO D6  

MicroDS3231 rtc;

const byte hhOn = 23;                   
const byte hhOff = 7;                   
const byte devMode = 0;                 
const String lat = "55.75";             
const String lon = "37.62";            


byte digitBuffer[4];

                                        //API ключ для openweathermap.org
const String appid = "456c55c0551e981fa796501825f45044"; 

OLED  myOLED(4, 5);                   

extern uint8_t TinyFont[];
extern uint8_t SmallFont[];        
extern uint8_t BigNumbers[];

WiFiUDP ntpUDP;

NTPClient timeClient(ntpUDP, "pool.ntp.org");

String arr_days[]={"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};

WiFiManager wifiManager;
HTTPClient http;
DynamicJsonDocument doc(1500); 

byte hh, mm, ss;                        
                                       
unsigned long timing, rndTiming, LostWiFiMillis, lastUpd; 

String timeStr;                         

int curScr = 1;                        
bool LostWiFi = false;                  
int temp, temp_min, temp_max, wID;      
byte humidity, clouds;                  
String location, weather, description;  
float wind;                             
long timeOffset;                        
byte httpErrCount = 0;                  

int nightX, nightY; 

#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL4JyJ-rnPx"
#define BLYNK_TEMPLATE_NAME "alisa"
#define BLYNK_AUTH_TOKEN "XVAEJfYB5ouS4jwxngOwMXoIpu6gM5md"

char ssid[] = "MTS_GPON_945d00"; 
char pass[] = "G8h4e7f3"; 

#include <BlynkSimpleEsp8266.h>
#define relay LED_BUILTIN 
#define switchPin V0 

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> 
#endif
#define LED_PIN     D3
#define LED_COUNT  98
#define BRIGHTNESS 255 
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);




bool relay1 = 1; // 1 реле с обратной логикой 0 реле с прямой логикой 
bool switchState = 1; // Изначальное состояние выключателя
bool relar = 1;

void setup() {
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, relay1);
  pinMode(switchPin, INPUT_PULLUP);
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  
  strip.begin();           
  strip.show();            
  strip.setBrightness(BRIGHTNESS);
  Serial.begin(9600);
  pinMode(RCLK, OUTPUT);
  pinMode(SCLK, OUTPUT);
  pinMode(DIO, OUTPUT);

  unsigned long timer = millis();     
  myOLED.begin(SSD1306_128X64);         
  bootScr("Starting Serial", 0);        
  Serial.begin(115200);                 

  pinMode(D5, INPUT_PULLUP);
  pinMode(D6, INPUT_PULLUP);
  pinMode(D4, OUTPUT);
  pinMode(D0, OUTPUT);
  digitalWrite(D0, LOW);

  wifiManager.setDebugOutput(devMode);  
                                        
  bootScr("autoConnect", 25);
  wifiManager.autoConnect("Connect-WIFI", "PASSWORD");
  
  while (WiFi.status() != WL_CONNECTED){}

  bootScr("Updating weather", 50);      
  int code = weatherUpdate();           
  if ( code != 200){                    
      myOLED.clrScr();                  
      myOLED.print("Could not Access", CENTER, 16);
      myOLED.print("OpenWeatherMap", CENTER, 24);
      myOLED.print("Code:" + String( code ), CENTER, 32);
      myOLED.update();
      delay(1000);
      myOLED.invert(1);
      myOLED.update();
      delay(1000);
      myOLED.invert(0);
      myOLED.print("Reseting...", CENTER, 42);
      myOLED.update();
      delay(1000);
      ESP.reset();                     
  }
  lastUpd = millis();

  bootScr("Starting NTPClient", 75);  
  timeClient.begin();                   
  timeClient.setTimeOffset(timeOffset);
  randomSeed(millis());            

                                       
  bootScr("Done in " + String( millis() - timer ) + "ms", 100);
  delay(700);
  myOLED.clrScr();
  timing = millis();                  
  rndTiming = 0;
}

BLYNK_WRITE(V0) //Витуальный пин для Blynk
{
  relay1 = param.asInt();
  digitalWrite(relay, relay1);
}

void loop() {

     timeClient.update();             
  
  if (millis() - lastUpd > 120000){   
      weatherUpdate();
      lastUpd = millis();
  }

  if (WiFi.status() != WL_CONNECTED) {
    if (LostWiFi == 0){
      LostWiFi = 1;
      LostWiFi = 1;                   
      LostWiFiMillis = millis();
      } else if(millis() - LostWiFiMillis > 180000) {
        
                                     
      myOLED.clrScr();                
      myOLED.print("Wifi connection", CENTER, 16);
      myOLED.print("lost", CENTER, 24);
      myOLED.update();
      delay(1000);
      myOLED.invert(1);
      myOLED.update();
      delay(1000);
      myOLED.invert(0);
      myOLED.print("Reseting...", CENTER, 42);
      myOLED.update();
      delay(1000);
      ESP.reset();                   
    }
     
      
    
  }
  
  
  hh = timeClient.getHours();
  mm = timeClient.getMinutes();      
  ss = timeClient.getSeconds();     
  
  updScr();

  Blynk.run();

  // Считываем состояние выключателя
  int newSwitchState = digitalRead(switchPin);

  // Если состояние изменилось (кнопка нажата)
  if (newSwitchState != switchState) {
    switchState = newSwitchState;
    
    // Инвертируем значение реле
    relay1 = !relay1;
    Serial.print(1);
    digitalWrite(relay, relay1);
    
    // Отправляем обновленное значение в Blynk
    Blynk.virtualWrite(V0, relay1);
  }
  if (relay1 == relar){
    relar = 0;
    colorWipe(strip.Color(  255,   0,   255), 0); // True white (not RGB white)
  }
  else{
     colorOFF(strip.Color(0,   0,   0)     , 0);

    }

   timeClient.update();                
  if (millis() - lastUpd > 120000){   
      weatherUpdate();
      lastUpd = millis();
  }

  if (WiFi.status() != WL_CONNECTED) {
    if (LostWiFi == 0){
      LostWiFi = 1;
      LostWiFi = 1;                  
      LostWiFiMillis = millis();
      } else if(millis() - LostWiFiMillis > 180000) {
        
                                    
      myOLED.clrScr();                
      myOLED.print("Wifi connection", CENTER, 16);
      myOLED.print("lost", CENTER, 24);
      myOLED.update();
      delay(1000);
      myOLED.invert(1);
      myOLED.update();
      delay(1000);
      myOLED.invert(0);
      myOLED.print("Reseting...", CENTER, 42);
      myOLED.update();
      delay(1000);
      ESP.reset();                   
    }
  
  }
  
  
  hh = timeClient.getHours();
  mm = timeClient.getMinutes();     
  ss = timeClient.getSeconds();    
  
  updScr();
  
}

void colorOFF(uint32_t color, int wait){
    for(int i=0; i<strip.numPixels(); i++) { 
    strip.setPixelColor(i, color);         
    strip.show();                         
    delay(wait);                         
  }
  
  
  }
void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { 
    strip.setPixelColor(i, color);         
    strip.show();                          
    delay(wait);                         
  }
}


int weatherUpdate() {                  
  if (WiFi.status() == WL_CONNECTED) { 
    
    logIf("Updating weather");                                    
    String httpStr = String("http://api.openweathermap.org/data/2.5/weather") + String("?lat=") + String(lat) + String("&lon=") + String(lon) + String("&appid=") + String(appid) + String("&units=metric&lang=en");
    http.begin(httpStr);
    
    logIf("Accessing: " + httpStr);
    
    int httpCode = http.GET();        
    String json = http.getString(); 
    logIf("Http Code: " + String(httpCode) );
    logIf("Got JSON: " + json);
    http.end();

    if(httpCode != 200) {             
      httpErrCount++;                 
      logIf( "httpErrCount: " + String(httpErrCount) );
      return httpCode;                
    }
                                     
    DeserializationError error = deserializeJson(doc, json);
    
    if (error) {                      
      if (devMode) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
      }
      return httpCode;               
    }
    
    temp = doc["main"]["temp"];        
    temp_min = doc["main"]["temp_min"];
    temp_max = doc["main"]["temp_max"];
    wind = doc["wind"]["speed"];
    description = doc["weather"][0]["description"].as<String>();
    weather = doc["weather"][0]["main"].as<String>();
    humidity = doc["main"]["humidity"];
    clouds = doc["clouds"]["all"];
    location = doc["name"].as<String>();
    timeOffset = doc["timezone"];
    wID = doc["weather"][0]["id"];
    
    httpErrCount = 0;                  
    
    return httpCode;                  
  }
}


                                         
void logIf(String msg){if(devMode){Serial.println(msg);}}



void bootScr(String str, byte percent) { 
  myOLED.setFont(SmallFont);             
  myOLED.clrScr();  
  myOLED.print(">:" + str, LEFT, 56);    
  String percentBar = "[";               
  for ( byte i = 1; i <= (percent / 10); i++){
    percentBar = percentBar + "=";
  }
  for ( byte i = 0; i < (10 - (percent / 10) ); i++){
    percentBar = percentBar + "-";
  }                                      
  percentBar = percentBar + "] | " + String(percent) + "%";
  myOLED.print(percentBar, CENTER, 48);
  myOLED.update();                       
}



void scr1() {                          
                                       
  String ForDate = timeClient.getFormattedDate();                   
  String justDate = ForDate.substring(0 , ForDate.indexOf( "T" ) ); 
  
  myOLED.clrScr();                      
  
  myOLED.setFont(BigNumbers);
  myOLED.print(timeStr , CENTER, 8 );   

  myOLED.setFont(SmallFont);
  myOLED.print(justDate, CENTER, 40);       
  
  if (httpErrCount < 10){               
    myOLED.print(String(temp) + " C", LEFT, 56);
  }                      
  
  myOLED.update();                      
}



void scr2(){                           
  myOLED.clrScr();                    
  myOLED.setFont(SmallFont);
  myOLED.print(timeStr, CENTER, 4);    
  myOLED.print("Max: " + String(temp_max) + " C", LEFT, 14);
  myOLED.print("Min: " + String(temp_min) + " C", LEFT, 24);
  myOLED.print("Wind: " + String(wind) + "m/s", LEFT, 56);
  
  myOLED.print("C", 120, 14);
  
  myOLED.setFont(BigNumbers);

  if(temp <= -10){
    myOLED.print(String(temp), 70, 14); 
  }
  if ((temp > -1) && (temp < -10) ) {  
    myOLED.print(String(temp), 89, 14);
  }
  if ((temp < 10) && (temp > -1)){    
    myOLED.print(String(temp), 102, 14);
  }
  if (temp >= 10){                     
    myOLED.print(String(temp), 89, 14);
  }
  if (httpErrCount < 10){             
    myOLED.update();                   
  } else {
    myOLED.clrScr();                  
    myOLED.setFont(SmallFont);
    myOLED.print("Could not Access", CENTER, 16);
    myOLED.print("OpenWeatherMap", CENTER, 24);
    myOLED.update();                    
  }
}



void scr3(){                                    
  myOLED.clrScr();                             
  myOLED.setFont(SmallFont);
  myOLED.print(timeStr, CENTER, 4);             
  
  if (wID > 800) {
    myOLED.print(weather + ":" + String(clouds) + "%", LEFT, 14);
  } else {
    myOLED.print(weather, LEFT, 14);
  }
  myOLED.print("%", 120, 14);

  if (description.length() <= 21){              
    myOLED.print(description, LEFT, 56);
  } else {                                      
    myOLED.setFont(TinyFont);                   
    myOLED.print(description, LEFT, 56);
  }
  
  myOLED.setFont(BigNumbers);
  
  if (humidity == 100){                         
    myOLED.print(String(humidity), 75, 14);
  } 
  if ((humidity >= 10) && (humidity != 100)) { 
    myOLED.print(String(humidity), 89, 14);
  } 
  if (humidity < 10) {                          
    myOLED.print(String(humidity), 102, 14);
  }
  
  if (httpErrCount < 10){                       
    myOLED.update();                           
  } else {
    myOLED.clrScr();                           
    myOLED.setFont(SmallFont);
    myOLED.print("Could not Access", CENTER, 16);
    myOLED.print("OpenWeatherMap", CENTER, 24);
    myOLED.update();                          
  }
}



void nightScr() {                            
  //Размер надписи 48 x 8
  if (millis() - rndTiming > 60000){          
    nightX = random(0, 80);                 
    nightY = random(0, 56);
    rndTiming = millis();
  }
  myOLED.setBrightness(1);                   
  myOLED.clrScr();
  myOLED.setFont(SmallFont);                  
  myOLED.print(timeStr, nightX, nightY);      
  myOLED.update();                            
}



void updScr() {                       
   
  timeStr = "";                       
  if (hh <= 9){                       
    timeStr = timeStr + "0" + String(hh) + ".";
  } else {
    timeStr = timeStr + String(hh) + ".";
  }
  if (mm <= 9){
    timeStr = timeStr + "0" + String(mm) + ".";
  } else {
    timeStr = timeStr + String(mm) + ".";
  }
  if (ss <= 9){
    timeStr = timeStr + "0" + String(ss);
  } else {
    timeStr = timeStr + String(ss);
  }     
  
  if ((hh >= hhOn) || (hh < hhOff)){  
    nightScr();                       
    return;
  } else {
    myOLED.setBrightness(255);       
  }
 
  switch(curScr) {                    
    case 1:                           
      if (millis() - timing > 12000){ 
        timing = millis(); 
        curScr = 2;                   
        scr2();                       
      }else{
        scr1();                       
        break;
      }
    break;

    case 2:                           
      if (millis() - timing > 12000){  
        timing = millis(); 
        curScr = 3;                   
        scr3();                       
      }else{
        scr2();                       
        break;
      }
    break;

    case 3:                           
      if (millis() - timing > 12000){  
        timing = millis(); 
        curScr = 1;                   
        scr1();                       
      }else{
        scr3();                       
        break;
      }
    break;
  }
}
