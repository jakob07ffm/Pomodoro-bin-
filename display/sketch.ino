#include <TM1637Display.h>


#define CLK 9  
#define DIO 10 


TM1637Display disp(CLK, DIO);

int counter = 0;  
bool colonState = false; 

void setup() {

  disp.setBrightness(7);  
  disp.clear();          

  for (int i = 0; i < 4; i++) {
    disp.showNumberDec(8888); 
    delay(200);               
    disp.clear();             
    delay(200);               
  }
}

void loop() {
  
  disp.showNumberDec(counter, true, 4, 0, 0);

  
  colonState = !colonState;
  disp.showNumberDecEx(counter, colonState ? 0x80 : 0, true);

  
  int brightness = map(counter % 10, 0, 9, 0, 7);
  disp.setBrightness(brightness);

  
  counter++;
  
  
  if (counter > 9999) {
    counter = 0;
  }

  
  int delayTime = map(counter, 0, 9999, 100, 500);
  delay(delayTime);
}
