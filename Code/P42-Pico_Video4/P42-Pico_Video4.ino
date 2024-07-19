// Sketch for P42 Pico Video4
// (c) Pier42 Electronics Design
//     https://www.tindie.com/stores/pier42/

//  Showing uploaded BMP images on composite video out
//  depending on input triggers
// written by
// Wolfgang Friedrich
// Rev Start:   2024/Feb/29
// Last change: 2024/June/18

// Resources:
// https://hackaday.io/project/196770-rpi-pico-video4
// https://github.com/wolfgangfriedrich/Pico-Video4

// supported resolutions:
//  NTSC 320x200x8bit 
//  NTSC 426x200x8bit 

//  PAL  300x240x8bit
//  PAL  500x240 8bit

#include <Arduino.h>
#include <SPI.h>

// *** Set Video mode in this header file ***
#include "VS23S040D.h"
#include "P42-Pico_Video4.h"
#include "ImageFlashWrite.h"
P42Display P42Display; 

 	int     InputPin[]          = { IN1Pin, IN2Pin, IN3Pin, IN4Pin, IN5Pin, IN6Pin, IN7Pin, IN8Pin};
	u_int32 PinTimeActivated[8];
	u_int32 PinDebounceStart[8];
	byte    ba_PinState[8];			// actual hi or lo state of input pin
	byte    ba_PinStatus[8];
	byte    b_OutputEnable;
	byte    b_LastInputOn;      // shows last and highest priority input that got turned on, if 0 input display already got processed
  bool    b_DoClearScreen = FALSE;

	u_int32 ImageDisplayCounter = 0;
	u_int32 ImageDisplayStart = 0;
	byte b_ImageDisplayIndex;
	byte b_LastImageDisplayIndex;

	u_int32 start_time;
	u_int32 current_time;



void setup() {

	u_int16 P42_Display_ID = 0;
	byte incomingByte = 0;   // for incoming serial data






	// while (!Serial) ;

	Serial.begin(19200);
delay(1000);    // wait for serial port to be configured

	Serial.println("");
	Serial.print(F("P42 Pico Video4 "));
	Serial.println(PICO_VIDEO4_VERSION);
#ifdef NTSC320x200 
		Serial.println(F("Resolution: NTSC 320x200 8bit"));
#endif
#ifdef NTSC426x200 
		Serial.println(F("Resolution: NTSC 426x200 8bit"));
#endif
#ifdef PAL300x240 
		Serial.println(F("Resolution: PAL 300x240 8bit"));
#endif
#ifdef PAL500x240 
		Serial.println(F("Resolution: PAL 500x240 8bit"));
#endif

	// Config pins
	pinMode(nWPPin, OUTPUT);
	digitalWrite(nWPPin, HIGH);
	pinMode(nHOLDPin, OUTPUT);
	digitalWrite(nHOLDPin, HIGH);

	// Disable pins that are used for SPI on Uno.
#ifdef MEGA
	pinMode(11, INPUT);
	pinMode(12, INPUT);
	pinMode(13, INPUT);
#endif	

	pinMode( IN1Pin, INPUT);
	pinMode( IN2Pin, INPUT);
	pinMode( IN3Pin, INPUT);
	pinMode( IN4Pin, INPUT);
	pinMode( IN5Pin, INPUT);
	pinMode( IN6Pin, INPUT);
	pinMode( IN7Pin, INPUT);
	pinMode( IN8Pin, INPUT);
	
	pinMode( OUTPin, OUTPUT);
	pinMode( OUTLED, OUTPUT);

	pinMode( TP6, INPUT_PULLUP);
  pinMode( TP7, INPUT_PULLUP);
  pinMode( TP8, INPUT_PULLUP);
  pinMode( TP13, INPUT_PULLUP);
  pinMode( TP14, INPUT_PULLUP);
  pinMode( TP15, INPUT_PULLUP);

  pinMode( GPIO15, INPUT_PULLUP);
  pinMode( GPIO21, INPUT_PULLUP);
  pinMode( GPIO22, INPUT_PULLUP);

	// Config SPI interface
	pinMode(slaveSelectPin, OUTPUT);
	digitalWrite(slaveSelectPin, HIGH);
	pinMode(MemSelectPin, OUTPUT);
	digitalWrite(MemSelectPin, HIGH);
	SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
  SPI.begin();
	// SPI.setClockDivider(SPI_CLOCK_DIV2);
	// SPI.setDataMode(SPI_MODE0);
	// SPI.setBitOrder(MSBFIRST) ; 
	
	P42_Display_ID = P42Display.Config( CH0 );
	// for max performance the unused video outputs are not configured
	// P42_Display_ID = P42Display.Config( CH1 );
	// P42_Display_ID = P42Display.Config( CH2 );
	// P42_Display_ID = P42Display.Config( CH3 );

	// Serial.println(F("P42 Pico Video4 Rev 1.1"));
	
	Serial.print(F("On Debounce  : "));
	Serial.print( ON_DEBOUNCE_WAIT );
	Serial.println(F(" msec"));
	
	Serial.print(F("Off Debounce : "));
	Serial.print( OFF_DEBOUNCE_WAIT );
	Serial.println(F(" msec"));
	
	// Serial.print(F("Show Image   : "));
	// Serial.print( IMAGE_SHOW_TIME );
	// Serial.println(F(" msec"));
	
	// Serial.println(F("Display Image [press key]") );
	// while (Serial.available() == 0) {};
	// incomingByte = Serial.read();
	// P42Display.ClearScreen ( CH0, EMPTY_SCREEN_COLOUR );

	// Serial.println(F("Clear Screen [press key]"));
	// while (Serial.available() == 0) {};
	// incomingByte = Serial.read();

	// u_int32 start_time = 0;
	// u_int32 current_time = 0;
	start_time = millis();

	P42Display.ClearScreen ( CH0, 13 );
	// P42Display.ClearScreen ( CH1, EMPTY_SCREEN_COLOUR );
	// P42Display.ClearScreen ( CH2, EMPTY_SCREEN_COLOUR );
	// P42Display.ClearScreen ( CH3, EMPTY_SCREEN_COLOUR );

	// P42Display.PrintString ( CH0, "P42 Pico Video4 V1.0",  XPIXELS/2 - 40, YPIXELS/2 - 4, 0x05);

	current_time = millis();
	Serial.print (F("Clearscreen duration [msec] : "));
	Serial.println ( current_time - start_time );

	start_time = millis();
	P42Display.DisplayBMPFromFlash ( CH0, 0x3000, 86, 36 );
	// P42Display.DisplayBMPFromFlash ( CH1, 0x3000, 20, 20 );
	// P42Display.DisplayBMPFromFlash ( CH2, 0x3000, 30, 30 );
	// P42Display.DisplayBMPFromFlash ( CH3, 0x3000, 40, 40 );
	current_time = millis();
	Serial.print (F("Logo [msec] : "));
	Serial.println ( current_time - start_time );

	// P42Display.PrintString ( CH0, "P42 Pico Video4 V1.0",  XPIXELS/2 - 40, YPIXELS/2 - 4, 0x05);

  // Initial configuration
  for (int i = 0; i <= ((sizeof(InputPin)/sizeof(InputPin[0])) - 1); i++) {
    ba_PinStatus[i] = s_OFF;
    PinDebounceStart[i] = millis();
    PinTimeActivated[i] = millis();
  }

  b_LastInputOn = 8;    // input channels are 0-7

	Serial.print ( F("Config done. READY.\n\r>") );

  delay(1000);    // for demo only

  b_ImageDisplayIndex = 8;
  b_LastImageDisplayIndex = 8;
  ImageDisplayStart = millis();
}


// void f_DownloadImage ( byte memory_location ) {
	// return;	
// }

void(* resetFunc) (void) = 0;//declare reset function at address 0

void loop() {

//	word adc_value = 0;
	byte    b_IncomingByte = 0;   // for incoming serial data
//	byte GPIOControlReg = 0;
	u_int32 i,j,x,y; // Used for counters
	u_int32 ul_pageadr;
	
	

	current_time = millis();

	//
  // Master State Machine
  //
	// Check for Character from serial to initial image download sequence
	if (Serial.available() != 0) {

		b_IncomingByte = Serial.read();
		switch (b_IncomingByte) {

			case CMD_IMAGE_WRITE:
				// statements
				Serial.println(F("w\r\n'w' Write Image"));
				
				// read next byte to know which image location to write to 
				while (Serial.available() == 0) {};
				b_IncomingByte = Serial.read();
				
				// maybe add a 0 case to download a logo
				
				if ( b_IncomingByte >= '1'  and  b_IncomingByte <= '8') {

          Serial.print(F("w"));
          Serial.print(b_IncomingByte-'0', HEX);
          Serial.print(F(": SPI Flash Start Adress [0x"));
          Serial.print((b_IncomingByte - '1') * 128 * 1024 + 0x20000, HEX); // offset of 0x20000 (=128K) to keep room for Logo or char bitmap
          Serial.println(F("]"));

					 f_DownloadImage ( (b_IncomingByte - '1') * 128 * 1024 + 0x20000 );
				}
        else if ( b_IncomingByte == '0' ) {
          f_DownloadImage ( 0x3000 );
        }
			break;

			case CMD_IMAGE_CONVERT:
				// statements
				Serial.println(F("i\r\n'i' Convert Image"));
				
				// read next byte to know which image location to write to 
				while (Serial.available() == 0) {};
				b_IncomingByte = Serial.read();
				
				// maybe add a 0 case to download a logo
				
				if ( b_IncomingByte >= '1'  and  b_IncomingByte <= '8') {

          Serial.print(F("i"));
          Serial.print(b_IncomingByte-'0', HEX);
          Serial.print(F(": SPI Flash Start Adress [0x"));
          Serial.print((b_IncomingByte - '1') * 128 * 1024 + 0x20000, HEX); // offset of 0x20000 (=128K) to keep room for Logo or char bitmap
          Serial.println(F("]"));

					 f_DownloadConvertImage ( (b_IncomingByte - '1') * 128 * 1024 + 0x20000 );
				}
        else if ( b_IncomingByte == '0' ) {
          f_DownloadConvertImage ( 0x3000 );
        }
			break;

			case CMD_VERSION:
				// statements
				Serial.print(F("v\r\nPier42 Pico Video4 (c) "));
				Serial.print(PICO_VIDEO4_VER_DATE);
				Serial.println(PICO_VIDEO4_VERSION);
			break;

			case CMD_CLEARSCREEN:
				// statements
				Serial.println(F("c\r\nClear Screen"));
      	P42Display.ClearScreen ( CH0, EMPTY_SCREEN_COLOUR );
			break;

			case CMD_SHOW:
				// statements
				Serial.print(F("s\r\n's' show Image "));
				
				// read next byte to know which image location to write to 
				while (Serial.available() == 0) {};
				b_IncomingByte = Serial.read();
				
				// 1-8 show images with calculated start address, 0 show logo with fixed adress.
				
				if ( b_IncomingByte >= '1'  and  b_IncomingByte <= '8') {

          Serial.print(F("s"));
          Serial.print(b_IncomingByte-'0', HEX);
          Serial.print(F(": [0x"));
          Serial.print((b_IncomingByte - '1') * 128 * 1024 + 0x20000, HEX); // offset of 0x20000 (=128K) to keep room for Logo or char bitmap
          Serial.print(F("] "));

        	start_time = millis();
          P42Display.DisplayBMPFromFlash ( CH0, (b_IncomingByte - '0') * 0x20000, 0, 0 );   // subtract ASCII value to result in int of channel#
	        current_time = millis();
          Serial.print(F(" [msec] "));
        	Serial.println ( current_time - start_time );

          b_DoClearScreen = FALSE;           // disable clearscreen to keep showing the current image, start again with input action
				}
        else if ( b_IncomingByte == '0' ) {
          Serial.print(F("s0 Logo [0x3000] "));
          // Serial.print(b_IncomingByte-'0', HEX);
        	start_time = millis();
          P42Display.DisplayBMPFromFlash ( CH0, 0x3000, 0, 0 );   // start address of logo
	        current_time = millis();
          Serial.print(F(" [msec] "));
        	Serial.println ( current_time - start_time );

          b_DoClearScreen = FALSE;           // disable clearscreen to keep showing the current image, start again with input action
        }
        else {
          Serial.println(F(" Invalid target."));
        }
			break;

			case CMD_MEM:
				Serial.print(F("m\r\n'm' Show Memory of Image "));
				
				// read next byte to know which image location to write to 
				while (Serial.available() == 0) {};
				b_IncomingByte = Serial.read();
				
				// 1-8 show images with calculated start address, 0 show logo with fixed adress, 9 show temp buffer for image conversion.
				
				if ( b_IncomingByte >= '1'  and  b_IncomingByte <= '9') {

          Serial.print(F("s"));
          Serial.print(b_IncomingByte-'0', HEX);
          Serial.print(F(": [0x"));
          Serial.print((b_IncomingByte - '1') * 128 * 1024 + 0x20000, HEX); // offset of 0x20000 (=128K) to keep room for Logo or char bitmap
          Serial.println(F("] "));

//          P42Display.DisplayBMPFromFlash ( CH0, (b_IncomingByte - '0') * 0x20000, 0, 0 );   // subtract ASCII value to result in int of channel#
          ul_pageadr = (b_IncomingByte - '0') * 0x20000;
          while (b_IncomingByte != 'x' ) {
            f_SPImemdump ( ul_pageadr , 256);

  				  while (Serial.available() == 0) {};
	  			  b_IncomingByte = Serial.read();

            if (b_IncomingByte != 'x' ) {
              ul_pageadr = ul_pageadr + 256;
            }
          }

  //        b_DoClearScreen = FALSE;           // disable clearscreen to keep showing the current image, start again with input action
				}
        else if ( b_IncomingByte == '0' ) {
          Serial.println(F("m0 Logo [0x3000] "));
          // Serial.print(b_IncomingByte-'0', HEX);
//          P42Display.DisplayBMPFromFlash ( CH0, 0x3000, 0, 0 );   // start address of logo
          ul_pageadr = 0x3000;
          while (b_IncomingByte != 'x' ) {
            f_SPImemdump ( ul_pageadr , 256);

  				  while (Serial.available() == 0) {};
	  			  b_IncomingByte = Serial.read();

            if (b_IncomingByte != 'x' ) {
              ul_pageadr = ul_pageadr + 256;
            }
          }

//          b_DoClearScreen = FALSE;           // disable clearscreen to keep showing the current image, start again with input action
        }
        else {
          Serial.println(F(" Invalid target."));
        }

      break;

      case CMD_DELETE:
      // delete an image section
				// statements
				Serial.print(F("d\r\n'd' delete Image "));
				
				// read next byte to know which image location to write to 
				while (Serial.available() == 0) {};
				b_IncomingByte = Serial.read();
				
				// 1-9 show images with calculated start address, 0 show logo with fixed adress.
        ul_pageadr = (b_IncomingByte - '0') * 0x20000;
				
				if ( b_IncomingByte >= '1'  and  b_IncomingByte <= '9') {

          Serial.print(F("d"));
          Serial.print(b_IncomingByte-'0', HEX);
          Serial.print(F(": [0x"));
          Serial.print((b_IncomingByte - '1') * 128 * 1024 + 0x20000, HEX); // offset of 0x20000 (=128K) to keep room for Logo or char bitmap
          Serial.print(F("] "));

          // for mem location calc          P42Display.DisplayBMPFromFlash ( CH0, (b_IncomingByte - '0') * 0x20000, 0, 0 );   // subtract ASCII value to result in int of channel#
          // erase target image buffer
          f_SPImemdump ( ul_pageadr, 0x8 );

          for (i=ul_pageadr; i<(ul_pageadr+(131071)); i=i+256)	// # of 256byte blocks loops for an image max 16k+255 SIZE in 24 bit colour depth
          {
            Serial.print(i,HEX);
            SPImemSectorErase ( i );
            Serial.print("             \r");
          }
          Serial.println(F("\r\n SectorErase for image done!"));
          f_SPImemdump ( ul_pageadr, 8 );
	
				}
        else if ( b_IncomingByte == '0' ) {
          Serial.print(F("d0 Logo [0x3000] "));

          // for mem location calc          P42Display.DisplayBMPFromFlash ( CH0, 0x3000, 0, 0 );   // start address of logo
          ul_pageadr = 0x3000;
          f_SPImemdump ( ul_pageadr, 0x8 );

          for (i=ul_pageadr; i<(ul_pageadr+(131071)-0x3000); i=i+256)	// # of 256byte blocks loops for an image max 16k+255 SIZE in 24 bit colour depth
          {
            Serial.print(i,HEX);
            SPImemSectorErase ( i );
            Serial.print("             \r");
          }
          Serial.println(F("\r\n SectorErase for image done!"));
          f_SPImemdump ( ul_pageadr, 8 );




        }
        else {
          Serial.println(F(" Invalid target."));
        }


      break;


			case CMD_HELP:
				// statements
				Serial.println(F("?\r\nw[1-8] Write Image into Buffer 1-8"));
				Serial.println(F("w0     Write Logo Image into Memory 128x128 px^2"));
				Serial.println(F("i[1-8] Colour convert and write Image into Buffer 1-8"));
				Serial.println(F("i0     Colour convert and write Logo Image into Memory 128x128 px^2"));
				Serial.println(F("s[0-8] Show Image[0-8] on screen"));
				Serial.println(F("m[0-8] Show Memory Dump of Image[0-8] 'x':Exit 'anykey':next page"));
				Serial.println(F("d[0-9] Delete memory section of Image[0-9]"));
				Serial.println(F("c      Clear Screen"));
				Serial.println(F("v      Show Version"));
				Serial.println(F("?      This help command"));
			break;

      case 'q':
        // resetFunc(); //call reset
			default:
				// statements
				Serial.println(F("No Command"));
			break;
		};

		Serial.print(F(">"));
		while (Serial.available()) { Serial.read(); };    // flush serial input

	}


	else {
		// check inputs
      // b_OutputEnable = 0; // !!!!!! might flicker output
      int i = 0;
		  // for (int i = 0; i <= ((sizeof(InputPin)/sizeof(InputPin[0])) - 1); i++) {
		  for (int j = (sizeof(InputPin)/sizeof(InputPin[0])); j > 0; j--) {

        i = j-1;

			  // Update state var
        ba_PinState[i] = digitalRead ( InputPin[i]);

        // update pin status
        switch ( ba_PinStatus[i] ) {
          case s_OFF: {                          // pin is off, waiting for rising edge
            if (ba_PinState[i] == PIN_ON) {
              PinDebounceStart[i] = current_time;
              ba_PinStatus[i] = s_DEBOUNCE_OFF;
              b_DoClearScreen = TRUE;           // enable clearscreen after showing logo at start or 's' command

              Serial.print(i+1);                  // Channel counter is 1-8
              Serial.println(F(": debounce off"));
            } 
            break;
          }
          case s_ON: {                          // pin is on, waiting for rising edge
            if (ba_PinState[i] == PIN_OFF) {
              PinDebounceStart[i] = current_time;
              ba_PinStatus[i] = s_DEBOUNCE_ON;
              b_DoClearScreen = TRUE;           // enable clearscreen after showing logo at start or 's' command

              Serial.print(i+1);                  // Channel counter is 1-8
              Serial.println(F(": debounce on"));
            } 
            
            break;
          }
          case s_DEBOUNCE_OFF: {                  // debouncing when exiting the OFF state
            if (ba_PinState[i] == PIN_OFF) {
              ba_PinStatus[i] = s_OFF;

              Serial.print(i+1);                  // Channel counter is 1-8
              Serial.println(F(": off"));
            } 
            else {
              if ( current_time > (PinDebounceStart[i] + OFF_DEBOUNCE_WAIT)) {
                PinTimeActivated[i] = current_time;
                ba_PinStatus[i] = s_ON;

                P42Display.DisplayBMPFromFlash ( CH0, (i + 1) * 0x20000, 0, 0 );

                Serial.print(i+1);                  // Channel counter is 1-8
                Serial.println(F(": on "));
              }
            }
            
            break;
        }  
          case s_DEBOUNCE_ON: {                  // debouncing when exiting the ON state
            if (ba_PinState[i] == PIN_ON) {
              ba_PinStatus[i] = s_ON;

              Serial.print(i+1);                  // Channel counter is 1-8
              Serial.println(F(": on"));
            } 
            else {
              if ( current_time > (PinDebounceStart[i] + ON_DEBOUNCE_WAIT)) {
                PinTimeActivated[i] = current_time;
                ba_PinStatus[i] = s_OFF;

                Serial.print(i+1);                  // Channel counter is 1-8
                Serial.println(F(": off"));
              }
            }
            
            break;
          }

          default : {

            break;
          };
        };

	  	};
		  // Serial.println(); // end line in case no change happened.
      // if any of the Input pins is on, enable the output
      if ( (ba_PinState[0] == s_ON) || (ba_PinState[1] == s_ON) || (ba_PinState[2] == s_ON) || (ba_PinState[3] == s_ON) || 
           (ba_PinState[4] == s_ON) || (ba_PinState[5] == s_ON) || (ba_PinState[6] == s_ON) || (ba_PinState[7] == s_ON) ) { 
        b_OutputEnable = 1;
      }
      else if ( (ba_PinState[0] == s_OFF) || (ba_PinState[1] == s_OFF) || (ba_PinState[2] == s_OFF) || (ba_PinState[3] == s_OFF) || 
                (ba_PinState[4] == s_OFF) || (ba_PinState[5] == s_OFF) || (ba_PinState[6] == s_OFF) || (ba_PinState[7] == s_OFF) ) {
        b_OutputEnable = 0;
        b_ImageDisplayIndex = 8;
        b_LastImageDisplayIndex = 8;// reset display routine - old: b_ImageDisplayIndex;
        if ( b_DoClearScreen ) {
          P42Display.ClearScreen ( CH0, EMPTY_SCREEN_COLOUR );
        }
        ImageDisplayStart = millis() - IMAGE_SHOW_TIME;     // keep time current but laggging behind for IMAGE_SHOW_TIME msec, when no image is displayed. To display next image immideately
//        Serial.println(F(" Clear Screen (check inputs)"));
      };
      
  		// enable output if at least one image is on
  		if ( b_OutputEnable == 1) {
  			digitalWrite ( OUTPin, PIN_ON );
  			digitalWrite ( OUTLED, PIN_ON );
        // Serial.println(F("Output in ON!"));
	  	}
	  	else {
	  		digitalWrite ( OUTPin, PIN_OFF );
	  		digitalWrite ( OUTLED, PIN_OFF );
// only once or do it in the image display section			P42Display.ClearScreen ( CH0, EMPTY_SCREEN_COLOUR );
	  	}
		// clear screen if no input is active
		
  	};

}
