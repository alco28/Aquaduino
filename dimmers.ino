


void lightning (int Ltimer) {
  
//   to do timer naar RTC koppelen op willekeurig moment op de dag in een week
//  met SETONE gaat het nu wel aan maar de leds gaan nog niet uit.. omdat random is gebruikt.

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

