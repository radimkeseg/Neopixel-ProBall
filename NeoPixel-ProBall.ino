/**The MIT License (MIT)
Copyright (c) 2017 by Radim Keseg
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

// Helps with connecting to internet
#include <WiFiManager.h>

#include <Adafruit_NeoPixel.h>

// check settings.h for adapting to your needs
#include "settings.h"
#include "embHTML.h"
#include "ITimer.h"
#include "Clock.h"

#include "EfXRainbow.h"
#include "EfXSpike.h"
#include "EfXSpiral.h"
#include "EfXAlarm.h"

// HOSTNAME for OTA update
#define HOSTNAME "WSC-ESP8266-"

//WiFiManager
//Local intialization. Once its business is done, there is no need to keep it around
WiFiManager wifiManager;
ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(12, NEOPIXEL_DATA_IN_PIN, NEO_GRB + NEO_KHZ800);

CustomSettings cs; 

Clock clock(&strip, 0);
EfxRainbow efx_rainbow(&strip);
EfxSpike efx_spike(&strip);
EfxSpiral efx_spiral(&strip);
EfxAlarm efx_alarm(&strip);

/*prototypes*/
void updateData();
void configModeCallback (WiFiManager *myWiFiManager);

/* webserver handlers */
void handle_root(){
  String content = FPSTR(PAGE_INDEX);

  content.replace("{timeoffset}", String(cs.settings.UTC_OFFSET).c_str() );
  
  if (cs.settings.DST) content.replace("{dst}", "checked='checked'");
  else    content.replace("{dst}", "");
  content.replace("{brightness}", String(cs.settings.brightness).c_str());
  content.replace("{brightness_night}", String(cs.settings.brightness_night).c_str());

  content.replace("{val-color-hours}", cs.settings.color_hours);
  content.replace("{val-color-quarters}", cs.settings.color_quarters);
  if (cs.settings.animate) content.replace("{animate}", "checked='checked'");
  else    content.replace("{animate}", "");

  if (cs.settings.ALARM_SWITCH) content.replace("{alarm}", "checked='checked'");
  else    content.replace("{alarm}", "");

  content.replace("{alarmHour}", String(cs.settings.alarmHour).c_str());
  content.replace("{alarmMins}", String(cs.settings.alarmMins).c_str()); 
  content.replace("{val-color-alarm}", cs.settings.color_alarm);
  
  server.send(200, "text/html", content);
}

static bool forceUpdateData = false;
void handle_store_settings(){
  if(server.arg("_dst")==NULL && server.arg("_timeoffset")==NULL ){
    Serial.println("setting page refreshed only, no params");      
  }else{
    Serial.println("settings changed");  
    cs.settings.UTC_OFFSET = atof(server.arg("_timeoffset").c_str());
    cs.settings.DST = server.arg("_dst").length()>0;

    cs.settings.brightness = atoi(server.arg("_brightness").c_str());
    cs.settings.brightness_night = atoi(server.arg("_brightness_night").c_str());

    strncpy(cs.settings.color_hours, server.arg("_input-color-hours").c_str(), 8);
    strncpy(cs.settings.color_quarters, server.arg("_input-color-quarters").c_str(), 8);
    cs.settings.animate = server.arg("_animate").length()>0;

    cs.settings.ALARM_SWITCH = server.arg("_alarm").length()>0;
    cs.settings.alarmHour = atoi(server.arg("_alarmHour").c_str());
    cs.settings.alarmMins = atoi(server.arg("_alarmMins").c_str()); 
    strncpy(cs.settings.color_alarm, server.arg("_input-color-alarm").c_str(), 8);

    cs.print();          
    cs.write();
    
    updateData();
    forceUpdateData = true;
  }
  clock.SetTimeOffset(cs.settings.UTC_OFFSET+cs.settings.DST);
  server.send(200, "text/html", "OK");
}

/**/

// Called if WiFi has not been configured yet
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Wifi Manager");
  Serial.println("Please connect to AP");
  Serial.println(myWiFiManager->getConfigPortalSSID());
  Serial.println("To setup Wifi Configuration");
}
 
void setup() {
  pinMode(NEOPIXEL_DATA_IN_PIN, OUTPUT);
  Serial.begin(9600);
    
  // Uncomment for testing wifi manager
  //wifiManager.resetSettings();
  wifiManager.setAPCallback(configModeCallback);
  //or use this for auto generated name ESP + ChipID


  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  wifiManager.setTimeout(180);

  String hostname = String("Neopixel-ProBall") + String("-") + String(ESP.getChipId(), HEX);
  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if(!wifiManager.autoConnect( hostname.c_str() ) ){
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }   

  //Manual Wifi
  //WiFi.begin(WIFI_SSID, WIFI_PWD);

  WiFi.hostname(hostname);
  
  cs.init();
  cs.read();   
  cs.print();

  clock.SetTimeOffset(cs.settings.UTC_OFFSET+cs.settings.DST);   

  MDNS.begin( (hostname+"-webupdate").c_str() );
  httpUpdater.setup(&server, "/firmware", update_username, update_password );


  //user setting handling
  server.on("/", handle_root);
  server.on("/offset", handle_store_settings);
  server.begin(); 
  Serial.println("HTTP server started");

  MDNS.addService("http", "tcp", 80);
  
  strip.begin();
  strip.clear();
  strip.show();
  
  updateData();
}

long lastDrew = 0;
long lastUpdate = 0;

long stamp = 0;
bool fast = false;
void loop() {
  // Handle web server
  server.handleClient();
  fast=false;
  clock.Clear(); 
  
  if(clock.getHourInt()>=22 || clock.getHourInt()<6) strip.setBrightness(cs.settings.brightness_night);
  else strip.setBrightness(cs.settings.brightness);

  //animate
  if( cs.settings.animate ) { efx_rainbow.Show(); delay(10); fast=true; } 

  //effect by alarm
  if(cs.settings.ALARM_SWITCH)
  if(clock.getHourInt() == cs.settings.alarmHour && clock.getMinsInt() == cs.settings.alarmMins){
    efx_alarm.Show(); delay(10); fast=true;
  }else{
    efx_alarm.Reset();
  }
  
  
  // effect by the full hour
  if( clock.getMinsInt() == 0 && clock.getSecsInt()<20){
    efx_spike.Show(); delay(10); fast=true;
  // effect by the guarters
  }else{
    efx_spike.Reset();
    if( clock.getMinsInt() == 15 && clock.getSecsInt()<2 || clock.getMinsInt() == 30 && clock.getSecsInt()<4 || clock.getMinsInt() == 45 && clock.getSecsInt()<6){
      efx_spiral.Show(); delay(10); fast=true;
    }else{
      efx_spiral.Reset();
    }
  }  
  strip.show();

/*  
  // show clock
  stamp = millis();
  if (stamp - lastDrew > 500 || stamp < lastDrew ) {

    //show clock hands
    clock.Show(); delay(10);
    lastDrew = stamp;
  }
*/



  if(stamp - lastUpdate > UPDATE_INTERVAL_SECS * 1000 || stamp < lastUpdate || forceUpdateData){
    updateData();
    lastUpdate = stamp;  
  }

  delay(fast?50:500);
}


void updateData(){
  efx_spike.SetUp(ITimer::hex2rgb( cs.settings.color_hours) );
  efx_spiral.SetUp(ITimer::hex2rgb( cs.settings.color_quarters) );
  efx_alarm.SetUp(ITimer::hex2rgb( cs.settings.color_alarm) );
  
  clock.SetTimeOffset(cs.settings.UTC_OFFSET+cs.settings.DST);  
  if(forceUpdateData) forceUpdateData=false;
}
