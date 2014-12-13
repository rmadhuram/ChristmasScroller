#include "fonts.c"
#include <Colorduino.h>

int mode = 1;
int stepCounter = 0;
int scrollCounter = 0;
int const MAX_STRINGS = 4;
const char *greetings[MAX_STRINGS] = {
  "* Merry Christmas! Drop your presents below. *",
  "For unto us a Child is born - Is 9:6 ",
  "Joy to the world ",
  "Emmanuel, God with us "
};

int currentGreeting = 0;

typedef struct Color {
  int r;
  int g;
  int b;
};

Color current;
Color target;
int colorCounter = 0;
int const MAX_STEPS = 10;

char buffer[8][8];

/********************************************************
Name: ColorFill
Function: Fill the frame with a color
Parameter:R: the value of RED.   Range:RED 0~255
          G: the value of GREEN. Range:RED 0~255
          B: the value of BLUE.  Range:RED 0~255
********************************************************/
void ColorFill(unsigned char R,unsigned char G,unsigned char B)
{
  PixelRGB *p = Colorduino.GetPixel(0,0);
  for (unsigned char y=0;y<ColorduinoScreenWidth;y++) {
    for(unsigned char x=0;x<ColorduinoScreenHeight;x++) {
      p->r = R;
      p->g = G;
      p->b = B;
      p++;
    }
  }
    
}

void setup()
{
  Colorduino.Init();
  unsigned char whiteBalVal[3] = {36,63,63}; // for LEDSEE 6x6cm round matrix
  Colorduino.SetWhiteBal(whiteBalVal);  
  ColorFill(0, 0, 0);
  for (int i=0; i<8; i++) {
    for (int j=0; j<8; j++) {
      buffer[i][j] = 0;
    }
  }  
}

/****
 * Scroller 
 ***/

void scroll_drawPage() {
  ColorFill(0, 0, 0);
  int r, g, b;
  for (int i=0; i<8; i++) { 
    for (int j=0; j<8; j++) {
      
      if (j < 4) {
        r = 240; g = 0; b = 0;
      } else {
        r = 0; g = 240; b = 0;
      }
    
      if (buffer[i][j] == 1) {
        Colorduino.SetPixel(7-i, j, r, g, b);
      }     
    }
  }
  Colorduino.FlipPage();  
}

void scroll_drawText(int count) {
  
  int fontIndex = (int) count / 6;
  unsigned char c = greetings[currentGreeting][fontIndex];
  
  for (int i=0; i<7; i++) {
    for (int j=0; j<8; j++) {
      buffer[i][j] = buffer[i+1][j];
    }  
  }
  
  int idx = count % 6;
  unsigned char code = 0;
  
  if (idx != 5) {
    code = font[c*5 + idx];
  }
  
  for (int j=0; j<8; j++) {
    if (code % 2) {
      buffer[7][j] = 1;
    } else {
      buffer[7][j] = 0;
    }
    code = code >> 1;
  }
  
  scroll_drawPage();
  
}

/**
 * Color changer
 */
 
void cc_fill() {
  float alpha = (float) colorCounter / MAX_STEPS;
  int r = floor((1-alpha)*current.r + alpha*target.r);
  int g = floor((1-alpha)*current.g + alpha*target.g);
  int b = floor((1-alpha)*current.b + alpha*target.b); 

  // mask with an asterisk.
  
  unsigned char mask[8] = {0x00, 0x99, 0x5a, 0x3c, 0xff, 0x3c, 0x5a, 0x99};
  
  PixelRGB *p = Colorduino.GetPixel(0,0);
  for (int i=0; i<8; i++) {
    int row = mask[i];
    for (int j=0; j<8; j++) {
      if (row % 2 == 1) {
        p->r = r; p->g = g; p->b = b;
      } else {
        p->r = 0; p->g = 0; p->b = 0;
      }
      row = row >> 1;
      p++;
    }
  }
  
  Colorduino.FlipPage();  
} 

void loop()
{
  if (mode == 0) {
    scroll_drawText(scrollCounter);
    scrollCounter++;
    if (scrollCounter >= strlen(greetings[currentGreeting]) * 6) {
      scrollCounter = 0;
      currentGreeting = random(MAX_STRINGS);
      stepCounter++;
      if (stepCounter == 2) {
        mode = 1;
        stepCounter = 0;
      }
    }
    delay(120);
  } else if (mode == 1) {
    
    colorCounter++;
    cc_fill();
    if (colorCounter == MAX_STEPS) {
      colorCounter = 0;
      stepCounter++;
      if (stepCounter == 20) {
        mode = 0;
        stepCounter = 0;
      }
      current.r = target.r;
      current.g = target.g;
      current.b = target.b;
      
      target.r = random(256);
      target.g = random(256);
      target.b = random(256);    
    }    
    
    delay(200);
    
  }
}
