#include <Arduino.h>

//ShiftPWM.SetOne(ledNr, value);

void Sunstrips (byte Sun_ON) {
if (Sun_ON == 1) { ShiftPWM.SetOne(15, maxBrightness);}
if (Sun_ON == 10) { ShiftPWM.SetOne(15, 0);}
if (Sun_ON == 2) { ShiftPWM.SetOne(16, maxBrightness); ShiftPWM.SetOne(17, maxBrightness);  }
if (Sun_ON == 20) { ShiftPWM.SetOne(16, 0); ShiftPWM.SetOne(17, 0);  }
if (Sun_ON == 3) { ShiftPWM.SetOne(18, maxBrightness); ShiftPWM.SetOne(19, maxBrightness);  }
if (Sun_ON == 30) { ShiftPWM.SetOne(18, 0); ShiftPWM.SetOne(19, 0);  }
if (Sun_ON == 4) { ShiftPWM.SetOne(20, maxBrightness); ShiftPWM.SetOne(21, maxBrightness);  }
if (Sun_ON == 40) { ShiftPWM.SetOne(20, 0); ShiftPWM.SetOne(21, 0);  }
if (Sun_ON == 5) { ShiftPWM.SetOne(22, maxBrightness); }
if (Sun_ON == 50) { ShiftPWM.SetOne(22, 0);}
if (Sun_ON == 00) {
for( byte led=15;led<23;led++){ ShiftPWM.SetOne(led, maxBrightness); }
}
if (Sun_ON == 99) {
for( byte led=15;led<23;led++){ ShiftPWM.SetOne(led, 0); }
}
//else { ShiftPWM.SetAll(1);}
}

void grow_L1(byte grow_L1) {
// activate L1 Growlight  
if (grow_L1 == 1) { ShiftPWM.SetOne(23, maxBrightness/3);}

if (grow_L1 == 0){ ShiftPWM.SetOne(23, 0);}


}

void moonshine(byte moon) {
// activate L1 Growlight  
if (moon == 1) 
for( byte led=0;led<2;led++){
ShiftPWM.SetHSV(led, 250, 255, 30);}

if (moon == 0) 
for( byte led=0;led<2;led++){
ShiftPWM.SetHSV(led, 250, 255, 0);}
}



//void Sunrise(void){  // Fade in all WHITE outputs
//byte brightW;
//unsigned long fadeTime = 10000;
//unsigned long time = millis()-startTime;
//unsigned long currentStep = time%(fadeTime*2);
//
//brightW = currentStep*maxBrightness/fadeTime; ///fading in
//for(byte led=15;led<23;led++){ ShiftPWM.SetOne(led, brightW); }
////ShiftPWM.SetOne(22,brightW);
//
//  if(currentStep <= fadeTime ){
//    brightW = currentStep*maxBrightness/fadeTime; ///fading in
//  }
//  else{
//    brightW = maxBrightness-(currentStep-fadeTime)*maxBrightness/fadeTime; ///fading out;
//  }
//  }

void lightning (int Ltimer) {
  
//   to do; onweer simulatie timer naar RTC koppelen op willekeurig moment op de dag in een week 
//  met SETONE gaat het nu wel aan maar de leds gaan nog niet uit.. omdat random is gebruikt.
// werkt nog NIET NAAR BEHOREN

int burst; //short burst of light
int big;
byte srumble;
byte brumble;
int mixer;
int diming;
int delaymix;
int dark;
int lightningtimer;
int lightningtime;

//Random lighting storm of bursts and flickers
unsigned long now = millis();
  lightningtime =  Ltimer ; //50000; // duration of lighning 
  lightningtimer = now;  
  
  if (now - lightningtimer < lightningtime)
 {
  burst = random(10,500); //Short burst of lightning value
  big = random(1000,4000);//long burst of lightning value
  srumble = random(25,125);//lower light lighting value
  brumble = random(125,240);//higher light lightning value
  mixer = random(burst,big);//random pick of short or long bursts of lightning
  dark = random(1000,12000);//random pick of time gap for lightning
  
  for(int i = 0; i < mixer; i += diming++) // set random mixer timer (burst of big) + random diming time for i)
  {
  diming = random(1,260); // duration of the burst (== led on)
  ShiftPWM.SetOne(random(15,22),random(srumble,brumble)); //Turn on a random white led for short or long burst
  // analogWrite(ledPin11, random(srumble,brumble));//Turn on LED for short or long burst
  delay(diming);      // delay to remain at set brightness value
  for(byte led=15;led<22;led++){ ShiftPWM.SetOne(led,0); }
}
  //digitalWrite(ledPin11, LOW);    // turn the LED off
delay(dark);
  }
} // end lightning void
