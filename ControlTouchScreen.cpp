#include "ControlTouchScreen.h"

#define TFT_DC D2
#define TFT_CS D1
#define TFT_LED D8
#define TOUCH_CS D3
#define TOUCH_IRQ D4

ControlTouchScreen::ControlTouchScreen(ControlOT *controlOT, uint16_t palette[]) {
/*uint16_t palette[] = {ILI9341_BLACK, // 0
                      ILI9341_WHITE, // 1
                      ILI9341_NAVY, // 2
                      ILI9341_DARKCYAN, // 3
                      ILI9341_DARKGREEN, // 4
                      ILI9341_MAROON, // 5
                      ILI9341_PURPLE, // 6
                      ILI9341_OLIVE, // 7
                      ILI9341_LIGHTGREY, // 8
                      ILI9341_DARKGREY, // 9
                      ILI9341_BLUE, // 10
                      ILI9341_GREEN, // 11
                      ILI9341_CYAN, // 12
                      ILI9341_RED, // 13
                      ILI9341_MAGENTA, // 14
                      ILI9341_YELLOW}; // 15  */

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

void ControlTouchScreen::paintPalette(int startX, int startY) {
        int y = startY + LINE_HEIGTH;
        for(int i = 0; i < 16; i++) {
          gfx->setColor(i);
          gfx->drawString(startX, y, "COLOR");
          y = y + LINE_HEIGTH;
        }  
}

void ControlTouchScreen::paintScreen() {
        gfx->fillBuffer(0);
        int x = X_BASE;
        int y = Y_BASE;
        
        gfx->setTextAlignment(TEXT_ALIGN_LEFT);
        gfx->setColor(15);
        gfx->drawString(x, y, "Temperaturas"); 
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

        gfx->commit();  
}

void ControlTouchScreen::calibrationCallback(int16_t x, int16_t y) {
  gfx->setColor(1);
  gfx->fillCircle(x, y, 10);
}
