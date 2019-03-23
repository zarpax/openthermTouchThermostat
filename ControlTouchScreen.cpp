#include "ControlTouchScreen.h"

#define TFT_DC D2
#define TFT_CS D1
#define TFT_LED D8
#define TOUCH_CS D3
#define TOUCH_IRQ D4

ControlTouchScreen::ControlTouchScreen(ControlOT *controlOT, uint16_t palette[]) {
  this->dstAdjusted = new simpleDSTadjust(StartRule, EndRule);
  this->controlOT = controlOT;                       
  this->tft = new ILI9341_SPI(TFT_CS, TFT_DC);
  this->gfx = new MiniGrafx(tft, BITS_PER_PIXEL, palette);
  this->ts = new XPT2046_Touchscreen(TOUCH_CS, TOUCH_IRQ);
  this->touchControllerWS = new TouchControllerWS(ts);  
  
  //this->calibration = calibrationCallback;

  // Turn on the background LED
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);

  ts->begin();
  gfx->init();
  gfx->fillBuffer(0);
  gfx->commit();  
}

void ControlTouchScreen::fillBuffer() {
  gfx->fillBuffer(0);
  
}

void ControlTouchScreen::commit() {
  gfx->commit();
}

void ControlTouchScreen::paintPalette(int startX, int startY) {
        int y = startY + LINE_HEIGTH;
        for(int i = 0; i < 16; i++) {
          gfx->setColor(i);
          gfx->drawString(startX, y, "COLOR");
          y = y + LINE_HEIGTH;
        }  
}

void ControlTouchScreen::paintScreen(int8_t wifiQuality, float temperature, bool isHotWaterEnabled, bool isFlameOn) {
        drawWifiQuality(wifiQuality);
        
        int x = X_BASE;
        int y = Y_BASE;
        
        gfx->setTextAlignment(TEXT_ALIGN_LEFT);
        gfx->setColor(15);
        //gfx->drawString(x, y, "Temperaturas"); 
        Nodes nodes = controlOT->getTemps();
        int y2 = y + LINE_HEIGTH;
        
        for(int i = 0; i < (sizeof(nodes.nodes) / sizeof(nodes.nodes[0])); i++) {
          String data = String(nodes.nodes[i].temp) + "ºC Humedad ";
          data = data + String(nodes.nodes[i].humidity) + "%";
          //Serial.println("Datos a mostrar:" + data);
          gfx->setColor(15);
          gfx->drawString(x, y2, nodes.nodes[i].name + " " + data); 
          y2 = y2 + LINE_HEIGTH;
        }

        gfx->drawString(x, y2, "Agua Caliente " + String(isHotWaterEnabled? "On" : "Off"));
        y2 = y2 + LINE_HEIGTH;
        gfx->drawString(x, y2, "Llama Encendida " + String(isFlameOn? "On" : "Off"));
        y2 = y2 + LINE_HEIGTH;
        gfx->drawString(x, y2, "Temperatura " + String(temperature));
}

void ControlTouchScreen::calibrationCallback(int16_t x, int16_t y) {
  gfx->setColor(1);
  gfx->fillCircle(x, y, 10);
}

void ControlTouchScreen::drawTime() {
  
  char time_str[11];
  char *dstAbbrev;
  time_t now = dstAdjusted->time(&dstAbbrev);
  struct tm * timeinfo = localtime (&now);

  gfx->setTextAlignment(TEXT_ALIGN_CENTER);
  gfx->setFont(ArialRoundedMTBold_14);
  gfx->setColor(ILI9341_WHITE);
  String date = WDAY_NAMES[timeinfo->tm_wday] + " " + MONTH_NAMES[timeinfo->tm_mon] + " " + String(timeinfo->tm_mday) + " " + String(1900 + timeinfo->tm_year);
  gfx->drawString(120, 6, date);

  gfx->setFont(ArialRoundedMTBold_36);

  if (IS_STYLE_12HR) {
    int hour = (timeinfo->tm_hour+11)%12+1;  // take care of noon and midnight
    sprintf(time_str, "%2d:%02d:%02d\n",hour, timeinfo->tm_min, timeinfo->tm_sec);
  } else {
    sprintf(time_str, "%02d:%02d:%02d\n",timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
  }
  gfx->drawString(120, 20, time_str);

  gfx->setTextAlignment(TEXT_ALIGN_LEFT);
  gfx->setFont(ArialMT_Plain_10);
  gfx->setColor(ILI9341_BLUE);
  if (IS_STYLE_12HR) {
    sprintf(time_str, "%s\n%s", dstAbbrev, timeinfo->tm_hour>=12?"PM":"AM");
  } else {
    sprintf(time_str, "%s", dstAbbrev);
  }
  gfx->drawString(195, 27, time_str);  // Known bug: Cuts off 4th character of timezone abbreviation
  gfx->commit();
}

void ControlTouchScreen::drawWifiQuality(int8_t quality) {
  gfx->setColor(ILI9341_WHITE);
  gfx->setTextAlignment(TEXT_ALIGN_RIGHT);  
  gfx->drawString(228, 9, String(quality) + "%");
  for (int8_t i = 0; i < 4; i++) {
    for (int8_t j = 0; j < 2 * (i + 1); j++) {
      if (quality > i * 25 || j == 0) {
        gfx->setPixel(230 + 2 * i, 18 - j);
      }
    }
  }
}
