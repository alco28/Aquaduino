// script for IR comando's from receiver. see lib: (http://github.com/shirriff/Arduino-IRremote) for examples

#include <Arduino.h>

/*
up	down	off	on
FF906F	FFB847	FFF807	FFB04F
red	green	blue	white
FF9867	FFD827	FF8877	FFA857
orange	Lightgreen	lightblue	flash
FFE817	FF48B7	FF6897	FFB24D
brown		purple	strobe
FF02FD	FF32CD	FF20DF	FF00FF
Lorange			fade
FF50AF	FF7887	FF708F	FF58A7
yellow	marine	grow	smooth
FF38C7	FF28D7	FFF00F	FF30CF
*/


void IRswitching (){

if(irrecv.decode(&results)) {;
	IRswitchP = results.value;
		if (millis() - lastIR > 100) { // ensures one IR signal to proces in 125 ms
	switch (IRswitchP) {
	case 0xFF906F:
        #if DEBUG 
	Serial.println("UP");
        #endif
	break;
	case 0xFFB847:
	#if DEBUG 
	Serial.println("DOWN");
        #endif
	break;
	case 0xFFF807:
        #if DEBUG 
	Serial.println("OFF");
        #endif
        ShiftPWM.SetAll(0);
      	break;
	case 0xFFB04F:
        #if DEBUG 
	Serial.println("ON");
        #endif
	 on = !on;
        Sunstrips(on ? 00 : 99);
	break;
	case 0xFF9867:
        #if DEBUG 
	Serial.println("Red");
        #endif
	on = !on;
	Sunstrips(on ? 1 : 10); 
	break;
	case 0xFFD827:
        #if DEBUG 
	Serial.println("Green");
        #endif
	on = !on;
	Sunstrips(on ? 2 : 20); 
	break;
	case 0xFF8877:
        #if DEBUG 
	Serial.println("Blue");
        #endif
        on = !on;
	Sunstrips(on ? 3 : 30); 
	break;
	case 0xFFA857:
        #if DEBUG 
	Serial.println("White");
        #endif
        on = !on;
	Sunstrips(on ? 4 : 40); 
        break;
        case 0xFFE817:
        #if DEBUG 
	Serial.println("Orange");
        #endif
        on = !on;
	Sunstrips(on ? 5 : 50); 
        break;
        case 0xFFF00F:
        #if DEBUG 
	Serial.println("grow_purple");
        #endif
        on = !on;
        grow_L1(on ? 1: 0);
	break; 
        case 0xFF28D7:
        #if DEBUG 
	Serial.println("marine_night");
        #endif
        on = !on;
        moonshine(on ? 1: 0);
	break; 
        case 0xFF58A7:
        #if DEBUG 
	Serial.println("sunrise_demo");
        #endif
        //Sunrise();
        break;
        case 0xFF38C7: //yellow
        #if DEBUG 
	Serial.println("time_serial");
        #endif
        RTC_time();
        break;
         case 0xFF50AF: //yellow
        #if DEBUG 
	Serial.println("lightning storm");
        #endif
        lightning(20000);
        break;
     
        
		  } //end case


 lastIR = millis();      
 irrecv.resume(); // Receive the next value
} 
 else Serial.println(results.value, HEX);
}
 }//void end

