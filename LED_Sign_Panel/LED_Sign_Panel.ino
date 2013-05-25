#include "MatrixDisplay.h"
#include "DisplayToolbox.h"
#include "font.h"

// Easy to use function
#define setMaster(dispNum, CSPin) initDisplay(dispNum,CSPin,true)
#define setSlave(dispNum, CSPin) initDisplay(dispNum,CSPin,false)

// 4 = Number of displays
// Data = 10
// WR == 11
// False - we dont need a shadow buffer for this example. saves 50% memory!

// Init Matrix
MatrixDisplay disp(4,11,10, true);
// Pass a copy of the display into the toolbox
DisplayToolbox toolbox(&disp);
String charLookup  = " 0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*(),-.?></\\|[]_=+:'\"{}";

// Prepare boundaries
uint8_t X_MAX = 0;
uint8_t Y_MAX = 0;

// Memory test
extern int __bss_end;
extern int *__brkval;

// Text placeholder
char text[128];

void setup() {
  Serial.begin(9600); 

  // Fetch bounds
  X_MAX = disp.getDisplayCount() * disp.getDisplayWidth();
  Y_MAX = disp.getDisplayHeight();
  
  // Prepare displays
  // The first number represents how the buffer/display is stored in memory. Could be useful for reorganising the displays or matching the physical layout
  // The number is a array index and is sequential from 0. You can't use 4-8. You must use the numbers 0-4
// The second number represents the "CS" pin (ie: CS1, CS2, CS3, CS4) this controls which panel is enabled at any one time. 
  disp.setMaster(0,4);
  disp.setSlave(1,5);
  disp.setSlave(2,6);
  disp.setSlave(3,7);
  // Write directly to the display
  for(int y=0; y < Y_MAX; ++y)	{
    for(int x = 0; x< X_MAX; ++x) {
      toolbox.setPixel(x, y, 1, true); // Lets write straight to the display.
    }
  }
  delay(2000); // Wait two seconds
}
//
void loop()
{

 char digits[64];

// snprintf(digits, sizeof(digits) - 1, "X/Y: %d/%d", X_MAX, Y_MAX);
// scrollText(digits, false);
// delay(2000); 
	
   if(Serial.available()){
     int i = 0;
       delay(100);
       while( Serial.available() && i< sizeof(text)-1) {
          text[i++] = Serial.read();
       }
       text[i-1]='\0';
    }
  snprintf(text, sizeof(text) - 1, "Seed & Feed @ Spoleto 2013!");
  scrollText(text, true);
  snprintf(text, sizeof(text) - 1, "Next Gig: 12:00 Sat 24th @ Marion Square");
  scrollText(text, true);
  //snprintf(digits, sizeof(digits) - 1, "Free Memory: %d", get_free_memory());
  //scrollText(digits, false);
  delay(2000);
  
}

void scrollText(char* text, int pastEnd) {
  int y=1;
  int endPos = 0;
  if(pastEnd)
    endPos =  -(strlen(text) * 7);

  for(int Xpos = X_MAX; Xpos > endPos; Xpos--){
    disp.clear();
    drawString(Xpos,y,text); 
    disp.syncDisplays();
    delay(1);
  }
  
//  EXPERIMENTAL IMPLEMENTATION! NOT WORKING!
//  disp.clear();
//  drawString(X_MAX-100,y,text);
//  disp.syncDisplays();
//  int pos = 0;
//  while (pos > endPos) {
//    disp.shiftLeft();
//    disp.syncDisplays();
//    pos--;
//    delay(5);
//  }
  delay(50);
}

// Write a line of static (non-scrolling) text to the display
void fixedText(char* text){
  int y = 1;
  int x = 0;
  disp.clear();
  drawString(x,y,text);
  disp.syncDisplays(); 
}


// Output a string to the display
void drawString(int x, uint8_t y, char* c){
  for(char i=0; i< strlen(c); i++){
    drawChar(x, y, c[i]);
    x+=6; // Width of each glyph
  }
}


// Output a single character to the display
void drawChar(int x, int y, char c){
  int dots;
  
  c = charLookup.indexOf(c);
  
  for (char col=0; col< 5; col++) {
    if((x+col+1)>0 && x < X_MAX){ // dont write to the display buffer if the location is out of range
      dots = pgm_read_byte_near(&myfont[c][col]);
      for (char row=0; row < 7; row++) {
        if (dots & (64>>row))   	     // only 7 rows.
          toolbox.setPixel(x+col, y+row, 1);
        else 
          toolbox.setPixel(x+col, y+row, 0);
      }
    }
  }
}

// Memory check
int get_free_memory(){
  int free_memory;
  if((int)__brkval == 0)
    free_memory = ((int)&free_memory) - ((int)&__bss_end);
  else
    free_memory = ((int)&free_memory) - ((int)__brkval);
    //Serial.println(free_memory);
  return free_memory;
}
