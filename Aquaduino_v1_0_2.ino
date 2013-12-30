
// Development edition

//INTIALISATION

#define SERIAL    1    // set to 1 to also report readings on the serial port
#define DEBUG     1    // set to 1 to display 0;

// ------------------Libaries ------------------------------------
//#include <EEPROM.h>       // EEPROM write/read for LED patterns
#include <MemoryFree.h>    // to test freememory left in SRAM = debuging

#include <avr/pgmspace.h>    // EEPROM progmem read&write for long serial prints
char p_buffer[100];          // buffersize voor N characters
#define P(str) (strcpy_P(p_buffer, PSTR(str)), p_buffer)

// ------------------Infrared module------------------------------------
#include <IRremote.h>      // for receiving IR signals from the remote


byte RECV_PIN = 7;               // pin for IR remote signal
byte on = 0 ;			 // for status change
unsigned int IRswitchP = 0 ;     // (holds IR code at switch case) / unsigned long = default

IRrecv irrecv(RECV_PIN);           // init the IR reciever
decode_results results;            // make buffer for the IRcodes
unsigned long lastIR = millis();   // set timer for IR signals 

unsigned int Sinput = 0; // serial keyboard input case statement for debuging

// ------------------RTC clock module------------------------------------
#include <Wire.h>        // I2C wire (real-time-clock module) SDA - pin 4 SCL = pin 5
#include "RTClib.h"      // RTClib

RTC_DS1307 RTC;

#include <TimeLord.h>    // Timelord lib for season,sunset,moon functions
// what is our longitude (west values negative) and latitude (south values negative)
// used for sunrize,sunset and moonphase function. (get it from google.com/maps)
float const LATITUDE  = 52.19;
float const LONGITUDE = 6.00;


// Delclare of ints for notation
int Hours, Minutes, Seconds;

// ----------------------------SHIFTPWM controller-------------------------------
// libary by Elco Jacobs
// Look at: http://www.elcojacobs.com/shiftpwm/ for libary to easy use Pulse-Wide-Modulation (PWM) for driving leds.

// Clock and data pins are pins from the hardware SPI, you cannot choose them yourself.
// Data pin is MOSI (Uno and earlier: 11        - brown wire)
// Clock pin is SCK Uno and earlier: 13         - green wire)
const int ShiftPWM_latchPin=9; // yellow wire 

// If your LED's turn on if the pin is low, set this to true, otherwise set it to false.
const bool ShiftPWM_invertOutputs = false;

// You can enable the option below to shift the PWM phase of each shift register by 8 compared to the previous.
// This will slightly increase the interrupt load, but will prevent all PWM signals from becoming high at the same time.
// This will be a bit easier on your power supply, because the current peaks are distributed. ONLY POSIBLE if you have 
// MaxBrightness at 255 
const bool ShiftPWM_balanceLoad = false;

#include <ShiftPWM.h>   // include ShiftPWM.h after setting the pins!

// Here you set the number of brightness levels, the update frequency and the number of shift registers.
// These values affect the load of ShiftPWM.
// Choose them wisely and use the PrintInterruptLoad() function to verify your load.
byte maxBrightness = 255; 
byte pwmFrequency = 75;
byte numRegisters = 3; // = 74HC595 IC'S on the PWM board
unsigned int numOutputs = numRegisters*8; // each IC has 8 outputs
byte numRGBLeds = 5; // each RGB led uses 3 outputs (Red,Green,Blue pins)

// ----------------------- LED timers Named Values ----------------------- 

// These are just used to make the lookup code more easily readable:
#define START_H 0        // start hour
#define START_M 1        // start minute
#define END_H 2          // end hour
#define END_M 3          // end minute
#define PARAMETER 4      //  brightness/hue/sat parameter

// READ CAREFULLY THIS FOR SETTING THE TIME MATRIX!!
// Any row ending with 24:00 will be considered last.
// Not all periods must be filled with meaningfull values for each channel.
// Periods must always be chronological and without overlaps.
// There must always be rows defined for the max number of periods, but not all have to be valid
// Example: {{0, 0, 24, 0, 255},{0, 0, 0, 0, 0},{0, 0, 0, 0, 0},{0, 0, 0, 0, 0}}
// Explanation: this channel is ALWAYS FULLY ON. 
// As the first period never ends, the remaining rows are only there to fill out the necessary space. 

// Add more timing tables if more channels are added
#define CHANNELS 14		// Number of channels,  8 sun channels,1 grow, 5 rgb = 14
#define MAXPERIODS 5	        // Max number of time periods, change if more are required

//----------------- ARRAYS with timers

byte lightMatrix[CHANNELS][MAXPERIODS][5] =
{
//      timer 1           timer 2                  timer 3                  timer 4                timer 5        
{	{0,0,10,0,0},	{12,30,20,0,255},	{21,30,24,0,0},		{0,0,0,0,0},		{0,0,0,0,0}		},		//#0	Sun_1	15
{	{0,0,9,30,0},	{12,30,20,0,255},	{22,0,24,0,0},		{0,0,0,0,0},		{0,0,0,0,0}		},		//#1	Sun_L2 16
{	{0,0,9,0,0},	{12,30,20,0,255},	{22,30,24,0,0},		{0,0,0,0,0},		{0,0,0,0,0}		},		//#2	Sun_L3 18
{	{0,0,8,30,0},	{12,30,20,0,255},	{23,0,24,0,0},		{0,0,0,0,0},		{0,0,0,0,0}		},		//#3	Sun_L4	20
{	{0,0,9,0,0},	{12,30,20,0,255},	{22,0,24,0,0},		{0,0,0,0,0},		{0,0,0,0,0}		},		//#4	Sun_R2 17
{	{0,0,8,30,0},	{12,30,20,0,255},	{22,30,24,0,0},		{0,0,0,0,0},		{0,0,0,0,0}		},		//#5	Sun_R3 19
{	{0,0,8,0,0},	{12,30,20,0,255},	{23,0,24,0,0},		{0,0,0,0,0},		{0,0,0,0,0}		},		//#6	Sun_R4 21
{	{0,0,11,30,0},	{12,30,20,0,255},	{23,30,24,0,0},		{0,0,0,0,0},		{0,0,0,0,0}		},		//#7	Sun_5 22
			
{	{0,0,7,45,0},	{8,0,8,45,60},		{9,0,14,30,0},		{15,0,18,0,255},	{19,30,24,0,0},	        },		//#8	Growlight Red

{	{0,0,9,00,15},	{9,30,11,0,125},	{12,30,20,30,255},	{22,30,23,30,55},	{23,30,24,0,25}	        },		//#9	RGB_L1/2
{	{0,0,1,0,25},	{1,10,9,0,0},		{10,30,11,0,150},	{12,30,21,0,255},	{23,0,24,0,25}	        },     	        //#10	RGB_3_Left
{	{0,0,1,0,25},	{1,10,8,30,0},		{10,30,11,0,150},	{12,30,21,0,255},	{23,0,24,0,25}	        },     	        //#11	RGB_3_Right
{	{0,0,1,0,25},	{1,10,8,30,0},		{10,0,10,30,125},	{12,30,21,30,255},	{23,30,24,0,35}	        },    	        //#12	RGB_4_Left/R5
{	{0,0,1,0,25},	{1,10,8,0,0},		{9,0,9,30,125},	        {12,30,21,30,255},	{23,30,24,0,35}	        }    	        //#13	RGB_L4_Right
                                                                                                        
}; // matrix array       

// Current light PARAMETER table
// Number of values should match number of channels (i.e. {0,0,0} for 3 channels)
byte lightValue[CHANNELS] = {0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0}; // 8 sun channels,1 grow, 5 rgb = 14
// ----------------------- Functions ----------------------- 
void UpdateLights(long seconds)		// Update light PARAMETER values
{
	for(byte channel = 0; channel < CHANNELS; channel++) {  		// For each Channel
		for(byte period = 0; period < MAXPERIODS; period++) {    // For each possible period
			long pEnd = GetSeconds(lightMatrix[channel][period][END_H], lightMatrix[channel][period][END_M], 0);	// Get period end time in seconds
			if(pEnd >= seconds) {				// Period is currently happening
				lightValue[channel] = lightMatrix[channel][period][PARAMETER];	// Set light to defined value
				break;		// Found correct period, don't check the rest

			} else {		// Period has not yet happened
				long pNextStart = GetSeconds(lightMatrix[channel][period+1][START_H], lightMatrix[channel][period+1][START_M], 0); // Get next period start time in seconds
				if(pNextStart > seconds) {					// Currently in between periods
					int preVal = lightMatrix[channel][period][PARAMETER];		// Old light value
					int postVal = lightMatrix[channel][period+1][PARAMETER];	// New light value
					int tDur = pNextStart - pEnd; 			// Transition duration
					int PARAMETERDiff = postVal - preVal;	// Difference in light intesity between periods
					if(PARAMETERDiff > 0) {		// PARAMETER increasing
						lightValue[channel] = (int)((seconds - pEnd) * ((float)PARAMETERDiff / tDur));		// Light value
					} else {					// PARAMETER decreasing
						lightValue[channel] = preVal - (int)((seconds - pEnd) * ((float)(0-PARAMETERDiff) / tDur));	// Light value
					}
					break;	// Found correct period, don't check the rest
				}
			}
		}
	}
}

long GetSeconds(int hours, int minutes, int seconds)	// Convert HH:mm:ss -> Seconds since midnight
{
  	return ((long)hours * 60 * 60) + (minutes * 60) + seconds ;       
}
//---------------------------------HUE MATRIX--------------------------------
#define HUES 5		    //  Number of channels, 5 RGB channels
#define H_MAXPERIODS 5	    // Max number of periods, change if more are required


byte HUEMatrix[HUES][H_MAXPERIODS][5] = 
{
//      timer 1           timer 2                  timer 3                  timer 4                timer 5      
{	{0,0,10,0,255},	{10,0,10,30,25},	{11,0,20,30,60},	{21,30,22,0,25},	{22,0,24,0,255}		},                        //#9	RGB_L1/2
{	{0,0,9,0,255},	{9,0,10,0,25},		{11,0,21,0,60},	        {22,0,22,30,25},	{22,30,24,0,255}  	},                        //#10	RGB_3_Left 
{	{0,0,8,30,255},	{8,30,9,0,25},		{10,30,21,0,60},	{22,30,23,0,25},	{23,0,24,0,255}  	},                        //#11	RGB_3_Right  
{	{0,0,8,30,255},	{8,30,10,0,25},		{11,0,21,30,60},	{22,0,23,30,25},	{23,30,24,0,255}  	},                        //#12	RGB_4_Left/R5   
{	{0,0,8,0,255},	{8,0,9,0,25},		{10,30,21,30,60},	{22,30,23,0,25},	{23,0,24,0,255}	        }                         //#13	RGB_L4_Right	  
}; // matrix array

byte HueValue[HUES] = {0, 0, 0, 0, 0}; //  5 rgb 

// ----------------------- Functions ----------------------- 
void UpdateHUE(long seconds)		// Update light PARAMETER values
{
	for(byte channel = 0; channel < HUES; channel++) {  		// For each Channel
		for(byte period = 0; period < H_MAXPERIODS; period++) {    // For each possible period
			long pEnd = GetSeconds(HUEMatrix[channel][period][END_H], HUEMatrix[channel][period][END_M], 0);	// Get period end time in seconds
			if(pEnd >= seconds) {				// Period is currently happening
				HueValue[channel] = HUEMatrix[channel][period][PARAMETER];	// Set light to defined value
				break;		// Found correct period, don't check the rest

			} else {		// Period has not yet happened
				long pNextStart = GetSeconds(HUEMatrix[channel][period+1][START_H], HUEMatrix[channel][period+1][START_M], 0); // Get next period start time in seconds
				if(pNextStart > seconds) {					// Currently in between periods
					int preVal = HUEMatrix[channel][period][PARAMETER];		// Old light value
					int postVal = HUEMatrix[channel][period+1][PARAMETER];	// New light value
					int tDur = pNextStart - pEnd; 			// Transition duration
					int HueDiff = postVal - preVal;	// Difference in light intesity between periods
					if(HueDiff > 0) {		// PARAMETER increasing
						HueValue[channel] = (int)((seconds - pEnd) * ((float)HueDiff / tDur));		// Light value
					} else {					// PARAMETER decreasing
						HueValue[channel] = preVal - (int)((seconds - pEnd) * ((float)(0-HueDiff) / tDur));	// Light value
					}
					break;	// Found correct period, don't check the rest
				}
			}
		}
	}
}

long GetSeconds2(int hours, int minutes, int seconds)	// Convert HH:mm:ss -> Seconds since midnight
{
	return ((long)hours * 60 * 60) + (minutes * 60) + seconds ;
}

//----------------------------SATURATION MAXTRIX---------------------------------------

#define SATS 5		         // Number of channels, 5 RGB channels
#define S_MAXPERIODS 3	        //  Max number of periods, change if more are required

byte SATMatrix[SATS][S_MAXPERIODS][5] = 
{
//      timer 1          timer 2           timer 3                 
{  {0,0,12,30,255},  {12,31,20,30,0},  {20,30,24,0,255}    },                         //#9	RGB_L1/2                   
{  {0,0,12,30,255},  {12,31,21,0,0},  {21,0,24,0,255}    },                         //#10	RGB_3_Left     
{  {0,0,12,30,255},  {12,31,21,0,0},  {21,0,24,0,255}   },                        //#11	RGB_3_Right    
{  {0,0,12,30,255},  {12,31,21,30,0},   {21,30,24,0,255}    },                        //#12	RGB_4_Left/R5          
{  {0,0,12,30,255},  {12,31,21,30,0},  {21,0,24,0,255}   }                         //#13	RGB_L4_Right    
}; // matrix array

byte SatValue[SATS] = {0, 0, 0, 0, 0}; //  5 rgb 

// ----------------------- Functions ----------------------- 
void UpdateSAT(long seconds)		// Update light PARAMETER values
{
	for(byte channel = 0; channel < SATS; channel++) {  		// For each Channel
		for(int period = 0; period < S_MAXPERIODS; period++) {    // For each possible period
			long pEnd = GetSeconds3(SATMatrix[channel][period][END_H], SATMatrix[channel][period][END_M], 0);	// Get period end time in seconds
			if(pEnd >= seconds) {				// Period is currently happening
				SatValue[channel] = SATMatrix[channel][period][PARAMETER];	// Set light to defined value
				break;		// Found correct period, don't check the rest

			} else {  // Period has not yet happened

				long pNextStart = GetSeconds(SATMatrix[channel][period+1][START_H], SATMatrix[channel][period+1][START_M], 0); // Get next period start time in seconds
				if(pNextStart > seconds) {					// Currently in between periods
					int preVal = SATMatrix[channel][period][PARAMETER];		// Old light value
					int postVal = SATMatrix[channel][period+1][PARAMETER];	// New light value
					int tDur = pNextStart - pEnd; 			// Transition duration
					int SATDiff = postVal - preVal;	// Difference in light intesity between periods
					if(SATDiff > 0) {		// PARAMETER increasing
						SatValue[channel] = (int)((seconds - pEnd) * ((float)SATDiff / tDur));		// Light value
					} else {					// PARAMETER decreasing
						SatValue[channel] = preVal - (int)((seconds - pEnd) * ((float)(0-SATDiff) / tDur));	// Light value
					}
					break;	// Found correct period, don't check the rest
				}

			}
		}
	}
}

long GetSeconds3(int hours, int minutes, int seconds)	// Convert HH:mm:ss -> Seconds since midnight
{
	return ((long)hours * 60 * 60) + (minutes * 60) + seconds ;
}


  


//=====================================SETUP===============================================================================================================================================================================

void setup()
{
  // Turn all LED's off.
 // ShiftPWM.SetAll(0);
  
  Wire.begin(); // I2C interface start
  irrecv.enableIRIn(); // Start the IR receiver
  
  #if SERIAL || DEBUG
  Serial.begin(57600);
  Serial.println(P("Aquaduino controller")); 
  #endif 

//------------------------------- SHIFT PWM-------------------------------
// Sets the number of 8-bit registers that are used.
  ShiftPWM.SetAmountOfRegisters(numRegisters);
  ShiftPWM.Start(pwmFrequency,maxBrightness);

//------------------------------------------------------------------------
  

RTC.begin(); // setup Real-time-clock, check with PC systemtime
            DateTime now = RTC.now();
	    DateTime compiled = DateTime(__DATE__, __TIME__);
	    if (now.get() < compiled.get()) {
	    Serial.println("RTC Updating");
	    // following line sets the RTC to the date & time this sketch was compiled
	    RTC.adjust(DateTime(__DATE__, __TIME__));
	  }
//------------------------------- Timelord-------------------------------  
  TimeLord mytime1; 
  mytime1.TimeZone(1 * 60); // tell TimeLord what timezone your RTC is synchronized to. You can ignore DST
  // as long as the RTC never changes back and forth between DST and non-DST
  mytime1.Position(LATITUDE, LONGITUDE); // tell TimeLord where in the world we are
  mytime1.DstRules(3,2,11,1, 60); // second sunday in march thru first sunday in november
 
  // Convert RTClib year INT format to Byte format from Timelord
  byte year ;
  year = now.year() - 2000 ; // only the decimal and year fit into a byte (255).
  byte today[] = {  now.second(), now.minute(), now.hour(), now.day(), now.month(), year   }; // store today's date (at noon) in an array for TimeLord to use
  


  
  
//------------------------------------------------------------------------
  

} //end setup

 float moonphase1() 
 {
    TimeLord mytime1;
    DateTime now = RTC.now();
    mytime1.TimeZone(1 * 60); // tell TimeLord what timezone your RTC is synchronized to. You can ignore DST
  // as long as the RTC never changes back and forth between DST and non-DST
  mytime1.Position(LATITUDE, LONGITUDE); // tell TimeLord where in the world we are
  mytime1.DstRules(3,2,11,1, 60); // second sunday in march thru first sunday in november
 
  // Convert RTClib year INT format to Byte format from Timelord
  byte year ;
  year = now.year() - 2000 ; // only the decimal and year fit into a byte (255).
  byte today[] = {  now.second(), now.minute(), now.hour(), now.day(), now.month(), year   }; // store today's date (at noon) in an array for TimeLord to use
   
  float moon = 0;   
  moon = mytime1.MoonPhase(today); // if the sun will set today (it might not, in the [ant]arctic)
  return moon;
  }


//=====================================LOOP=========================



void loop()
{ 
DateTime now = RTC.now();
// some functions for debuging the system  
#ifdef SERIAL || DEBUG
  // serial commands
  if (Serial.available() >0 ){

    
  Sinput = Serial.read(); // read a number from the serial port to set some actions
  Serial.print("input set "); 
  Serial.print(Sinput); 
      Serial.print(": ");
      switch(Sinput){
       case 'f':
       Serial.print("freeMemory test=");
      break;
      case 'l':
       Serial.println(P("Print information about the interrupt frequency, duration and load on your program"));
       break;
      case 't':
      Serial.print("Print RTC time");
      break;
      case 'w':
      Serial.println("white output"); 
      break;
      case 'r':
      Serial.println("rgb output");
      break;
     // default:
     //   Serial.println("Unknown mode!");
     //   break;
            }//end switch
  } // end serial
 
  
  switch(Sinput){
      case 'f':
        Serial.println(freeMemory());
        break;
      case 'l':
       // Print information about the interrupt frequency, duration and load on your program
      ShiftPWM.PrintInterruptLoad();
      Serial.flush();
      break;
      case 't':
      RTC_time (); // show time
      delay (125);
      break;
       case 'm':
      moonphase(); // show time
      Serial.print("moonfunction");
      delay (125);
      break;
      case 'w':
      Serial.print("white 1");   Serial.print(" \t "); Serial.println(lightValue[0]);
      Serial.print("white 2L");  Serial.print(" \t "); Serial.println(lightValue[1]);
      Serial.print("white 2R");  Serial.print(" \t "); Serial.println(lightValue[2]);
      Serial.print("white 3L");  Serial.print(" \t "); Serial.println(lightValue[3]);
      Serial.print("white 3R");  Serial.print(" \t "); Serial.println(lightValue[4]);
      Serial.print("white 4L");  Serial.print(" \t "); Serial.println(lightValue[5]);
      Serial.print("white 4R");  Serial.print(" \t "); Serial.println(lightValue[6]);
      Serial.print("white 5");   Serial.print(" \t "); Serial.println(lightValue[7]);
      Serial.println("---------");
      Serial.print("growlight"); Serial.print(" \t "); Serial.println(lightValue[8]);
      break;
      case 'r':
      Serial.println("RGB 1/2");Serial.print("Hue");Serial.print(" \t ");  Serial.print(HueValue[0]);Serial.println(" ");  Serial.print("SAT");Serial.print(" \t "); Serial.println(SatValue[0]);Serial.print("Lux");Serial.print(" \t "); Serial.println(lightValue[9]); 
      Serial.println("RGB L3");Serial.print("Hue"); Serial.print(" \t ");  Serial.print(HueValue[1]); Serial.println(" ");Serial.print("SAT");Serial.print(" \t "); Serial.println(SatValue[1]);  Serial.print("Lux");Serial.print(" \t "); Serial.println(lightValue[10]); 
      Serial.println("RGB R3");Serial.print("Hue"); Serial.print(" \t ");  Serial.print(HueValue[2]); Serial.println(" ");Serial.print("SAT");Serial.print(" \t "); Serial.println(SatValue[2]);  Serial.print("Lux");;Serial.print(" \t "); Serial.println(lightValue[11]); 
      Serial.println("RGB L4");Serial.print("Hue"); Serial.print(" \t ");  Serial.print(HueValue[3]); Serial.println(" ");Serial.print("SAT");Serial.print(" \t "); Serial.println(SatValue[3]);  Serial.print("Lux");Serial.print(" \t "); Serial.println(lightValue[12]); 
      Serial.println("RGB L5");Serial.print("Hue"); Serial.print(" \t ");  Serial.print(HueValue[4]); Serial.println(" ");Serial.print("SAT");Serial.print(" \t "); Serial.println(SatValue[4]);  Serial.print("Lux");Serial.print(" \t "); Serial.println(lightValue[13]); 
      break;
            }//end switch
#  endif      
  

    
// ====== shiftPWM parameters

IRswitching(); //activate the IR listning commands (tab IR_Signals)
// ========== White Ledstrip commands ============
Ledtimer(); // activate the timer for sunset,sunrise
//lightning(20000); example of use of the ligning void script param=duration of lightning


#ifdef  DEBUG
// output of arrays on Serial port for debuging

#  endif  







} // end loop

  void moonphase() {
  TimeLord mytime1; 
  DateTime now = RTC.now();
  byte year ;
  year = now.year() - 2000 ; // only the decimal and year fit into a byte (255).
  byte today[] = {  now.second(), now.minute(), now.hour(), now.day(), now.month(), year   }; // store today's date (at noon) in an array for TimeLord to use


  float moonphase;
  moonphase = mytime1.MoonPhase(today); // if the sun will set today (it might not, in the [ant]arctic)
 // deze conversie werkt nog niet goed. wil float naar een byte omzetten (via een array).
 
 byte* mp = (byte*) &moonphase;
 
  #if DEBUG
  Serial.println("moonphase: ");
  Serial.print(moonphase);
  Serial.println();  
  Serial.print(mp[2]);Serial.print(" -- "); Serial.print(mp[3]);
  #endif 
  }
  
void Ledtimer () {
// Get Time
	DateTime now = RTC.now();

// hier worden de inputs gegeven voor de lighttimers. als je deze now.hour() vervangt door de timelord tijd moet het goed gaan met daylightsavingstime.
	Hours = now.hour();
	Minutes = now.minute();
	Seconds = now.second();
  
	
	UpdateLights(GetSeconds(Hours, Minutes, Seconds));
        UpdateHUE(GetSeconds2(Hours, Minutes, Seconds)); // functie GetSeconds2 en 3 zijn aangemaakt maar waarom dat nu nieuwe moeten zijn geen idee..
        UpdateSAT(GetSeconds3(Hours, Minutes, Seconds));

// ++++++++++++++++++++++++++white & growlight ledstripts +++++++++++++++++++++++++++++++++++++

        // Update actual light PARAMETER to match values defined in lightValue[]
for( byte led=15;led<24;led++) //led loop
  { for(byte lv=0;lv<9;lv++){  // lightvalue loop 

ShiftPWM.SetOne(led, lightValue[lv]);
  } // led loop
    }  //lv loop


//	ShiftPWM.SetOne(15, lightValue[0]);
//	ShiftPWM.SetOne(16, lightValue[1]);
//	ShiftPWM.SetOne(18, lightValue[2]);
//	ShiftPWM.SetOne(20, lightValue[3]);
//	ShiftPWM.SetOne(17, lightValue[4]);
//	ShiftPWM.SetOne(19, lightValue[5]);
//	ShiftPWM.SetOne(21, lightValue[6]);
//	ShiftPWM.SetOne(22, lightValue[7]);
//growlight
// ShiftPWM.SetOne(23, lightValue[8]); //growlight
      


// ++++++++++++++++++++++++++color ledstripts +++++++++++++++++++++++++++++++++++++
 
 for( byte RGB=0;RGB<5;RGB++)       // RGB led loop
  { for(byte HUE=0;HUE<5;HUE++){    // HUE loop 
      for(byte SAT=0;SAT<5;SAT++){  // SAT loop 
        for(byte lv=0;lv<5;lv++){   // LV loop 
 
 ShiftPWM.SetHSV(RGB, HueValue[HUE], SatValue[SAT], lightValue[lv]); // 0 = first bar 5=  last bar
        }}}} // end loops
 
// ShiftPWM.SetHSV(0, HueValue[0], SatValue[0], lightValue[9]); //moonlite on the first bar
// ShiftPWM.SetHSV(1, HueValue[1], SatValue[1], lightValue[10]);
// ShiftPWM.SetHSV(2, HueValue[2], SatValue[2], lightValue[11]);
// ShiftPWM.SetHSV(3, HueValue[3], SatValue[3], lightValue[12]);
// ShiftPWM.SetHSV(4, HueValue[4], SatValue[4], lightValue[13]);
 
 }; // end Ledtimer()
 

void RTC_time () {
  //start RTC module
    
    DateTime now = RTC.now();
    Serial.print("The onboard time:"); Serial.println();    Serial.print(now.year(), DEC);    Serial.print('/');    Serial.print(now.month(), DEC);    Serial.print('/');    Serial.print(now.day(), DEC);
    Serial.print(' ');    Serial.print(now.hour(), DEC);    Serial.print(':');    Serial.print(now.minute(), DEC);    Serial.print(':');    Serial.print(now.second(), DEC);    Serial.println();  
    }
    

