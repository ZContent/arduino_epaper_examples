/***************************************************
 * ePaper Color Calendar Demo
 * For use with Adafruit tricolor e-Paper displays
 * Dan Cogliano
 * DanTheGeek.com
 * 
 * Notes: Uncomment the display type type you are using below. Also, you MUST
 * comment out the "#define USE_EXTERNAL_SRAM" in the Adafruit_EPD.h file 
 * (currently line 29 in the file). This is required in order to use DanTheGeek's 
 * Custom E-Paper Featherwing since it does not include wires to the on-board
 * memory or SD card on the e-paper displays.
 */
 
#include <Adafruit_GFX.h>    // Core graphics library
#include <globalfonts.h>
#include "Adafruit_EPD.h"
#include <iotdate.h>         // https://github.com/zcontent/arduino_iot_calendar

long tzoffset = -5*60; // Timezone offset in minutes

String wifi_ssid="ssid";
String wifi_password="password";


#if defined(ESP8266)
  // Feather HUZZAH 8266
  #define EPD_RESET      15
  #define EPD_DC          2
  #define EPD_CS          0
  #define EPD_BUSY        4
  #define BUTTON0         12
  #define BUTTON1         5
  #define LEDPIN          0
  #define LEDPINON        LOW
  #define LEDPINOFF       HIGH
#else
  // Feather HUZZAH 32 (same physical locations as 8266 but different IO pins)
  #define EPD_RESET       33
  #define EPD_DC          14
  #define EPD_CS          15
  #define EPD_BUSY        23
  #define BUTTON0         12
  #define BUTTON1         22 
  #define LEDPIN          13
  #define LEDPINON        HIGH
  #define LEDPINOFF       LOW
#endif

/* Uncomment the following line if you are using 1.54" tricolor EPD */
//Adafruit_IL0373 gfx(152, 152 ,EPD_DC, EPD_RESET, EPD_CS, EPD_BUSY);

/* Uncomment the following line if you are using 2.15" tricolor EPD */
Adafruit_IL0373 gfx(212, 104 ,EPD_DC, EPD_RESET, EPD_CS, EPD_BUSY);

/* Uncomment the following line if you are using 2.7" tricolor EPD */
//Adafruit_IL91874 gfx(264, 176 ,EPD_DC, EPD_RESET, EPD_CS, EPD_BUSY);

void drawCalendar(int x, int y)
{
  iotDate iotd(-300);
  // draw calendar
  String stryear = String(iotd.getYear());
  Serial.println("printing year " + stryear);
  gfx.setTextColor(EPD_BLACK);
  gfx.setFont();
  gfx.setCursor(8,4);
  gfx.print(stryear);
  
  String strmonth = iotd.getMonthString(iotd.getMonth());
  Serial.println("printing month " + strmonth);
  int dow = iotd.getWDay();
  int today = iotd.getDay();
  int daysinmonth = iotd.getDaysInMonth(iotd.getMonth(),iotd.getYear());
  gfx.setTextColor(EPD_BLACK);
  gfx.setFont(&FreeSansBold9pt7b);
  int16_t fx, fy;
  uint16_t w, h;
  gfx.getTextBounds((char *)strmonth.c_str(), 0,0, &fx, &fy, &w, &h);

  gfx.setCursor((gfx.width() - w)/2,12);
  gfx.print(strmonth);        
  
  Serial.println("day of week is " + String(dow));
  
  int curday = today - dow;
  while(curday > 1)
    curday -= 7;

  x = 0;
  y = 24;
  for(int i = 0 ; i < 7; i++)
  {
    x = 4 + i * (gfx.width()-8)/7;
    gfx.setTextColor(EPD_BLACK);
    gfx.setFont();
    gfx.getTextBounds(String(iotd.getShortDOWString(i)).substring(0,1), 0,0, &fx, &fy, &w, &h);
    gfx.setCursor(x + (gfx.width()/7-w)/2, y-8);
    gfx.print(String(iotd.getShortDOWString(i)).substring(0,1));
  }
  gfx.drawLine(0,24,gfx.width(),24,EPD_BLACK);
  y = 32;
  while(curday < daysinmonth)
  {
    for(int i = 0; i < 7; i++)
    {
      x = 4 + i * (gfx.width()-8)/7;
      if(curday >= 1 && curday <= daysinmonth)
      {
          gfx.setCursor(x,y);
          gfx.setTextColor(EPD_BLACK);
          gfx.setFont(&FreeSans9pt7b);
          //if(iotd.isWeekday(i))
          //  gfx.setFont(&FreeSansBold9pt7b);          
          int16_t fx, fy;
          uint16_t w, h;
          String strday = String(curday);
          if(curday == today)
          {

            gfx.setFont(&FreeSansBold9pt7b);           
            //gfx.fillCircle(x + (gfx.width()-8)/7/2, y, 8, EPD_RED);
            gfx.setTextColor(EPD_RED);

          }
          gfx.getTextBounds(strday.c_str(), 0,0, &fx, &fy, &w, &h);
          gfx.setCursor(x+(gfx.width()-8)/7/2-w/2-fx,y+h/2);
          gfx.print(curday);
      }
      curday++;
    }
    y += 16;
  }
}

void setup() {
  Serial.begin(115200);
  //while(!Serial);

  pinMode(BUTTON0, INPUT_PULLUP);
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, HIGH);
  
  Serial.print("Connecting to WiFi ");
  WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("connected");
  
  gfx.begin();
  Serial.println("ePaper display initialized");
  
  gfx.clearBuffer();
  drawCalendar(0,0);
  gfx.display();
  Serial.println("done, going to sleep...");
  // shutdown microcontroller, wake up after specified time
  //ESP.deepSleep(3600e6); // 60 minutes
  ESP.deepSleep(7200e6); // 2 hours
  //ESP.deepSleep(86400e6); // 24 hours
}

void loop() {
  // put your main code here, to run repeatedly:

}
