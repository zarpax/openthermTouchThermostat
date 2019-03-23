#ifndef CTS_H
#define CTS_H
#include <SPI.h>
#include <XPT2046_Touchscreen.h>
#include "TouchControllerWS.h"
#include "MiniGrafx.h" // General graphic library
#include "ILI9341_SPI.h" // Hardware-specific library
#include <FS.h>
#include "ControlOT.h"
#include "common.h"
#include "ArialRounded.h"

class ControlTouchScreen {  
  private:
      ControlOT *controlOT;
      ILI9341_SPI *tft;
      MiniGrafx *gfx;
      XPT2046_Touchscreen *ts;
      TouchControllerWS *touchControllerWS;
      void calibrationCallback(int16_t x, int16_t y);
      CalibrationCallback *calibration;
      simpleDSTadjust *dstAdjusted;
      void drawWifiQuality(int8_t quality);
      
      int X_BASE = 0;
      int Y_BASE = 80;
      int LINE_HEIGTH = 15;  
      int BITS_PER_PIXEL = 4; 

      void paintPalette(int startX, int startY);
  public:
      ControlTouchScreen(ControlOT *controlOT, uint16_t palette[]);
      void fillBuffer();
      void paintScreen(int8_t wifiQuality, float temperature, bool isHotWaterEnabled, bool isFlameOn);
      void drawTime();      
      void commit();
};

#endif
