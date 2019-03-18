#ifndef CTS_H
#define CTS_H
#include <SPI.h>
#include <XPT2046_Touchscreen.h>
#include "TouchControllerWS.h"
#include "MiniGrafx.h" // General graphic library
#include "ILI9341_SPI.h" // Hardware-specific library
#include <FS.h>



class ControlTouchScreen {  
  private:
      ILI9341_SPI *tft;
      MiniGrafx *gfx;
      XPT2046_Touchscreen *ts;
      TouchControllerWS *touchControllerWS;
      void calibrationCallback(int16_t x, int16_t y);
      CalibrationCallback *calibration;

      int X_BASE = 0;
      int Y_BASE = 0;
      int LINE_HEIGTH = 15;  
      int BITS_PER_PIXEL = 4; 
  public:
      ControlTouchScreen();
      void paintScreen();
};


#endif
