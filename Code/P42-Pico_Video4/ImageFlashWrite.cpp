// Sketch for P42 PicoVideo4 Demo
// FLASH write  a BMP Picture
// (c) Pier42 Electronics Design
//     https://www.tindie.com/stores/pier42/

// written by
// Wolfgang Friedrich
// Last change: 2020/Mar/21


// *** Enable new functionality to write 255 color BMP image data to FLASH address IMAGE1_ADDR
//     this is only tested with UART at 19200 baud!
#define IMAGEWRITE

// inlude the SPI library:
#include <Arduino.h>
#include <SPI.h>
// #include <Wire.h>

#include <avr/pgmspace.h>
// #include "ball_frames.h"
#include "VS23S040D.h"
#include "P42-Pico_Video4.h"
#include "ImageFlashWrite.h"

// const int slaveSelectPin  = 10;
// const int MemSelectPin    = 9;
// const int nHOLDPin        = 8;
// const int nWPPin          = 7;

// const int CHAR_MAP_ADDR		= 0x0000;
// const int BOINGBALL_ADDR	= 0x1000;
// const int IMAGE1_ADDR		= 0x3000;
// const int IMAGE2_ADDR		= 0x5000;

byte ba_Image_Palette [1024];



// --------------------------------------------------------------------------------------
// Hexdump Flash memory to UART
// --------------------------------------------------------------------------------------
void f_SPImemdump (unsigned long address, unsigned int bytes) {

	unsigned long i = 0;
	byte result =0;
  String ASCIIout = "12345678";//[9];
	
	digitalWrite(MemSelectPin,LOW);
	SPI.transfer( 0x03 );					// read data
	SPI.transfer( (address >>16) &0xFF );	// start address
	SPI.transfer( (address >> 8) &0xFF );
	SPI.transfer( (address     ) &0xFF );
	for (i = 0; i < bytes; i++) {
		if ((i % 8) == 0) {
			if (i != 0) {
  			Serial.print(" ");
  			Serial.print(ASCIIout);
				Serial.println (";");
      }
			Serial.print(" 0x");
			Serial.print(address + i,HEX);
			Serial.print(": ");
		}
		result = SPI.transfer( 0x00 );
		Serial.print(" 0x");
		if (result <0x10)
			Serial.print ("0");
		Serial.print(result,HEX);
    if ((result > 0x1F) && (result != 128) && (result != 255))
      ASCIIout[i%8] = result; // readable char
    else
      ASCIIout[i%8] = '.';
    
    ASCIIout[(i%8)+1] = 0;
	}
    digitalWrite(MemSelectPin,HIGH); 

	Serial.print(" ");
	Serial.print(ASCIIout);
	Serial.println("<");
  

}



// --------------------------------------------------------------------------------------
// As the function says
// --------------------------------------------------------------------------------------
void SPImemSectorErase ( unsigned long mem_addr ) {

  byte StatusReg = 0x01;
	
	digitalWrite(MemSelectPin,LOW);
	SPI.transfer( 0x06 );               // WEN
	digitalWrite(MemSelectPin,HIGH); 
	//delay(1);
	digitalWrite(MemSelectPin,LOW);
	SPI.transfer( 0x20 );               // Sector erase
	SPI.transfer( (mem_addr >>16) & 0xFF );        // sector address (16 blocks are reserved for font)
	SPI.transfer( (mem_addr >> 8) & 0xFF );
	SPI.transfer( 0x00 );
	digitalWrite(MemSelectPin,HIGH); 

//	Serial.print( mem_addr + 0x10, HEX );
//	Serial.print( " " );

	//delay(1000);
    StatusReg = 1;
	while (StatusReg & 0x01 == 1) {
		Serial.print(StatusReg,HEX);
		digitalWrite(MemSelectPin,LOW);
		SPI.transfer( 0x05 );
		StatusReg = SPI.transfer(0x00);
		digitalWrite(MemSelectPin,HIGH); 
		delay(1);
	};
	Serial.print(StatusReg, HEX);
//	Serial.println();

}

// --------------------------------------------------------------------------------------
// write a single Byte to flash
// --------------------------------------------------------------------------------------
void SPIwritebyte ( unsigned long mem_addr, byte data ) {
	
	byte StatusReg = 0x01;

	digitalWrite(MemSelectPin,LOW);
	SPI.transfer( 0x06 );               // WEN
	digitalWrite(MemSelectPin,HIGH); 
//	delay(1);
	digitalWrite(MemSelectPin,LOW);
	SPI.transfer( 0x02 );               // Page program
    SPI.transfer( (mem_addr >>16) & 0xFF );		// start address
    SPI.transfer( (mem_addr >> 8) & 0xFF );		// 
    SPI.transfer( (mem_addr     ) & 0xFF );		// 
	SPI.transfer( data);
	digitalWrite(MemSelectPin,HIGH); 
	
	StatusReg = 1;
	while (StatusReg & 0x01 == 1) {
//		Serial.print("*");
		digitalWrite(MemSelectPin,LOW);
		SPI.transfer( 0x05 );
		StatusReg = SPI.transfer(0x00);
		digitalWrite(MemSelectPin,HIGH); 
	};
//	Serial.println(StatusReg, HEX);

}


// --------------------------------------------------------------------------------------
// write a single Byte to flash
// --------------------------------------------------------------------------------------
byte SPIreadbyte ( unsigned long mem_addr ) {

	byte result = 0;


 digitalWrite(MemSelectPin,LOW);
  SPI.transfer( 0x03 );               // read data
  SPI.transfer( (mem_addr >>16) & 0xFF );		// start address
  SPI.transfer( (mem_addr >> 8) & 0xFF );		// 
  SPI.transfer( (mem_addr     ) & 0xFF );		// 
  result = SPI.transfer( 0x00 );
  digitalWrite(MemSelectPin,HIGH); 

return (result);
}


void f_DownloadImage ( unsigned long memory_location ) {

	word result = 0;

  byte Capacity = 0;
  byte address = 0x90;
  byte incomingByte = 0;   // for incoming serial data
  byte StatusReg = 0x01;
  unsigned long i,j = 0;
  byte blockwrite_len = 0;
  
  unsigned long ul_ImageAddr = 0;
  
//*

//  Serial.println(F("!!!10 seconds wait... !!!"));
//  delay(10000);
  Serial.println(F("!!!Use a proper terminal SW (e.g. Teraterm) and binary transfer!!!"));
  Serial.println(F("Any key to start Image mem erase!"));
  
  while (Serial.available() == 0) {};
  incomingByte = Serial.read();

  ul_ImageAddr = memory_location;// * 128 * 1024 ; // max image size is 128 KByte = 1 Mbit memory size of video controller

// --------------------------------------------------------------------------------------
// Receive image data over UART and write to Flash
// Currently supported 8bit BMP x-size must be multiple of 4
// --------------------------------------------------------------------------------------

	{
	
	unsigned long FileSize = 54;		// set to header size
	byte inbyte = 0;
	unsigned long counter = 0;
	unsigned long ulongtemp = 0;		// 

	f_SPImemdump ( ul_ImageAddr, 0x08 );

	for (i=ul_ImageAddr; i<(ul_ImageAddr+131071); i=i+256)	// # of 256byte blocks loops for an image max 16k+255 SIZE
	{
		Serial.print(i,HEX);
		SPImemSectorErase ( i );
		Serial.print("             \r");
	}
	Serial.println(F("\r\n SectorErase for image done!"));
	f_SPImemdump ( ul_ImageAddr, 8 );

	Serial.setTimeout(10000);
	Serial.println(F("Please send BMP image now. (10sec timeout)"));
	// file read while loop
	while ( counter < FileSize ) {
		
		while ( !Serial.available() ) {}	// wait for next byte; timeout default is 1000msec
		inbyte = Serial.read();
//Serial.print("(");
//Serial.print(counter,HEX);
//Serial.print(")");

if (counter < 54) {
  Serial.print(inbyte,HEX);
  Serial.print(".");
}
		switch ( counter ) {
			case 0: {
				if (inbyte != 'B') {
					Serial.println(F("B0: Not a BMP"));
					counter = FileSize;						// to stop while loop
				}
				break;
			}
			case 1: {
				if (inbyte != 'M') {
					Serial.println(F("B1: Not a BMP"));
					counter = FileSize;						// to stop while loop
				}
				break;
			}
			case 2:{							// 2-5 file size in bytes
				ulongtemp = inbyte;
				break;
			}
			case 3:{
				ulongtemp += (inbyte<<8);
				break;
			}
			case 4:{
				ulongtemp += (inbyte<<16);
				break;
			}
			case 5:{
				FileSize = ulongtemp + (inbyte<<24);
				Serial.println(FileSize,HEX);
				break;
			}
			default: {
				break;
			}
		}	// switch
		SPIwritebyte ( ul_ImageAddr+counter, inbyte );
		counter ++; // this is the last instruction in the file read while loop
	}		// while
	Serial.println (F("Image done."));

	f_SPImemdump ( ul_ImageAddr, 0x8 );

	delay(1);
	}
	
	// f_SPImemdump ( 0, 16 );
	// f_SPImemdump ( 0x1000, 16 );

	
	Serial.println(F("Image write to memory done. "));

}


void f_DownloadConvertImage ( unsigned long memory_location ) {

	word result = 0;

  byte Capacity = 0;
  byte address = 0x90;
  byte incomingByte = 0;   // for incoming serial data
  byte StatusReg = 0x01;
  unsigned long i,j,k = 0;
  byte blockwrite_len = 0;
  
  unsigned long ul_ImageAddr = 0;
  unsigned long ul_EndAddr = 0;
  unsigned long ul_ImageBuffer = 0x120000; // 8 * 128 * 1024 + 0x20000 ; one 0x20000 buffer above the highest image
  unsigned long ul_ImageSize = 0;
  unsigned long ul_FileSize = 0;
  byte b_ColourDepth = 0;
  byte ba_PaletteMap[256];
  
//*

//  Serial.println(F("!!!10 seconds wait... !!!"));
//  delay(10000);
  Serial.println(F("!!!Use a proper terminal SW (e.g. Teraterm4) and binary transfer!!!"));
  Serial.println(F("Any key to start buffer mem erase!"));
  
  while (Serial.available() == 0) {};
  incomingByte = Serial.read();

  ul_ImageAddr = memory_location;// * 128 * 1024 ; // max image size is 128 KByte = 1 Mbit memory size of video controller

// --------------------------------------------------------------------------------------
// Receive image data over UART and write to Flash
// Currently supported 8bit BMP  x-size must be multiple of 4
// --------------------------------------------------------------------------------------

	{
	
	unsigned long FileSize = 54;		// set to header size
	unsigned long ul_FileXPixels = 0;		// horizontal width
	byte inbyte = 0;
	unsigned long counter = 0;
	unsigned long ulongtemp = 0;		// 

  byte b_ClosestPaletteEntry = 255;
  u_int32 ul_PaletteDistance = 0; 
  u_int32 ul_ClosestPaletteDistance = 442;	// sqrt(3 * 255^2)

	f_SPImemdump ( ul_ImageBuffer, 0x8 );

	for (i=ul_ImageBuffer; i<(ul_ImageBuffer+131071); i=i+256)	// # of 256byte blocks loops for an image max 16k+255 SIZE
	{
		Serial.print(i,HEX);
		SPImemSectorErase ( i );
		Serial.print("             \r");
	}
	Serial.println(F("\r\n SectorErase for buffer done!"));
	f_SPImemdump ( ul_ImageBuffer, 8 );

	Serial.setTimeout(10000);
	Serial.println(F("Please send BMP image now. (10sec timeout)"));
	// file read while loop
	while ( counter < FileSize ) {
		
		while ( !Serial.available() ) {}	// wait for next byte; timeout default is 1000msec
		inbyte = Serial.read();
    //Serial.print("(");
    //Serial.print(counter,HEX);
    //Serial.print(")");

    if (counter < 54) {
      Serial.print(inbyte,HEX);
      Serial.print(".");
    }
		switch ( counter ) {
			case 0: {
				if (inbyte != 'B') {
					Serial.println(F("B0: Not a BMP"));
					counter = FileSize;						// to stop while loop
				}
				break;
			}
			case 1: {
				if (inbyte != 'M') {
					Serial.println(F("B1: Not a BMP"));
					counter = FileSize;						// to stop while loop
				}
				break;
			}
			case 2:{							// 2-5 file size in bytes
				ulongtemp = inbyte;
				break;
			}
			case 3:{
				ulongtemp += (inbyte<<8);
				break;
			}
			case 4:{
				ulongtemp += (inbyte<<16);
				break;
			}
			case 5:{
				FileSize = ulongtemp + (inbyte<<24);
				Serial.println(FileSize,HEX);
				break;
			}
			case 0x12:{ // horizontal width, only parsing 2 byte, assuming image size < 65535 pixel
				ulongtemp = inbyte;
				break;
			}
			case 0x13:{
				ul_FileXPixels = ulongtemp + (inbyte<<8);
				Serial.println(ul_FileXPixels,HEX);
				break;
			}
			default: {
				break;
			}
		}	// switch
		SPIwritebyte ( ul_ImageBuffer+counter, inbyte );
		counter ++; // this is the last instruction in the file read while loop
	}		// while
	Serial.println (F("Image to temp buffer done."));

	f_SPImemdump ( ul_ImageBuffer, 0x8 );

	// now read the image from the buffer
	// do the colour conversion and write to selected image buffer.

	// erase target image buffer
	f_SPImemdump ( ul_ImageAddr, 0x8 );

  if ( ul_ImageAddr == 0x3000 ) {
    ul_EndAddr = ul_ImageAddr+(131071)-0x3000;
  }
  else {
    ul_EndAddr = ul_ImageAddr+(131071);
  };

	for (i=ul_ImageAddr; i<(ul_EndAddr); i=i+256)	// # of 256byte blocks loops for an image max 16k+255 SIZE in 24 bit colour depth
	{
		Serial.print(i,HEX);
		SPImemSectorErase ( i );
		Serial.print("             \r");
	}
	Serial.println(F("\r\n SectorErase for image done!"));
	f_SPImemdump ( ul_ImageAddr, 8 );
	

// start of image read from buffer > conversion > write to target image

	ul_FileSize = 54; 	// set to header size
	i = 0;
	while ( i < ul_FileSize ) {
		
		
		inbyte = SPIreadbyte( ul_ImageBuffer + i );
//Serial.print("(");
//Serial.print(counter,HEX);
//Serial.print(")");

		if (i < 54) {
		  Serial.print(inbyte,HEX);
		  Serial.print(".");
		}
		switch ( i ) {
			case 0: {
				if (inbyte != 'B') {
					Serial.println(F("B0: Not a BMP"));
					i = ul_FileSize;						// to stop while loop
				}
				break;
			}
			case 1: {
				if (inbyte != 'M') {
					Serial.println(F("B1: Not a BMP"));
					i = ul_FileSize;						// to stop while loop
				}
				break;
			}
			case 2:{							// 2-5 file size in bytes
				ulongtemp = inbyte;
				break;
			}
			case 3:{
				ulongtemp += (inbyte<<8);
				break;
			}
			case 4:{
				ulongtemp += (inbyte<<16);
				break;
			}
			case 5:{
				ul_FileSize = ulongtemp + (inbyte<<24);
				Serial.println(ul_FileSize,HEX);
				break;
			}
			case 0x1C:{
				if (! (inbyte == 8 )){  // or inbyte == 24) ) {
					Serial.println(F("Wrong colour depth"));
					i = ul_FileSize;						// to stop while loop
				}
				else {
					b_ColourDepth = inbyte;
					// Serial.print(F("Filesize: "));
					// Serial.println(ul_FileSize,HEX);
					// Serial.print(F("Colourdepth: "));
					// Serial.println(b_ColourDepth);
				}
				break;
			}
			default: {
				break;
			}
		}	// switch
		
		if  ( i < 54 ) {	// write header
			// wait writing the file length until 8 or 24 bit colour depth is known. 
			SPIwritebyte ( ul_ImageAddr + i, inbyte );
		}
		else if (b_ColourDepth == 8) {
			// read buffer palette and write VS23S040D palette
			if ( i < 54 + 4*256 -1 ) {			// change palette
				ba_Image_Palette[ i - 54] = inbyte;
				inbyte = ba_VS23S040_Palette[ i - 54];
			}
			// match pixel colour and write new palette pointer
			// match the palette entries and have a temporary conversion table for the pointers, much faster...
			else if ( i == 54 + 4*256 - 1 ) {		// last palette byte, calc palette conversion
				// last byte of 256 palette entries
				ba_Image_Palette[ i - 54] = inbyte;
				inbyte = ba_VS23S040_Palette[ i - 54];

				for (j=0; j<=255; j++) {
				// Serial.print(F("\r\nPalette Entry: "));
 				// Serial.print(j);
 				// Serial.println(">");
          // init
					b_ClosestPaletteEntry = 255;
					ul_PaletteDistance = 0; 
					ul_ClosestPaletteDistance = 442;	// sqrt(3 * 255^2)

          // Serial.println("");
					for (k=0; k<=255; k++) {
            Serial.print(k);
            Serial.print("\r");
						// calc colour distance between palette entrys
						ul_PaletteDistance = (unsigned long) sqrt((sq(ba_VS23S040_Palette[k*4]-ba_Image_Palette[j*4])) + 
												  (sq(ba_VS23S040_Palette[k*4+1]-ba_Image_Palette[j*4+1])) +
												  (sq(ba_VS23S040_Palette[k*4+2]-ba_Image_Palette[j*4+2])));
						// compare to smallest distance, store new entry if smaller
						if (ul_PaletteDistance < ul_ClosestPaletteDistance) {
							b_ClosestPaletteEntry = k;
							ul_ClosestPaletteDistance = ul_PaletteDistance;
							if ( ul_PaletteDistance == 0 ) {	// perfect match, end for(k) loop.
        				Serial.print(F("."));
								k=255;
							}
						}
					// 
					}
/*				Serial.print(F(" (CPalDist("));
 				Serial.print(k);
				Serial.print(F("): "));
 				Serial.print(ul_ClosestPaletteDistance);
				Serial.print(F(")(CPalEntry: "));
 				Serial.print(b_ClosestPaletteEntry);
				Serial.println(F(")"));

				Serial.print(F(" (VSPal("));
 				Serial.print(ba_VS23S040_Palette[b_ClosestPaletteEntry*4]);
				Serial.print(F(","));
 				Serial.print(ba_VS23S040_Palette[b_ClosestPaletteEntry*4+1]);
				Serial.print(F(","));
 				Serial.print(ba_VS23S040_Palette[b_ClosestPaletteEntry*4+2]);
				Serial.print(F(")ImagePal("));
 				Serial.print(ba_Image_Palette[j*4]);
				Serial.print(F(","));
 				Serial.print(ba_Image_Palette[j*4+1]);
				Serial.print(F(","));
 				Serial.print(ba_Image_Palette[j*4+2]);
				Serial.println(F(")"));
*/

					ba_PaletteMap[j] = b_ClosestPaletteEntry;

				}
 				Serial.println("<");

			}
			else {								// write pixels, which are palette pointer bytes.
				inbyte = ba_PaletteMap[inbyte];// new palette pointer
			}
		}
		else if (b_ColourDepth == 24) {
			// file size changes if colour depth changes from 24 to 8!!!
			
			// write VS23S040D palette
			
			// read 3 colour values. REMEMBER, they are stored backwards BGR!!!
			// same colour matching routine as 8 bit.
			// write palette pointer
			
		}
		else {
			Serial.println(F("Wrong colour depth"));
		
		}
		
		if ( (i % 1000) == 0 ){
      Serial.print(".");
    }
		
		SPIwritebyte ( ul_ImageAddr + i, inbyte );
		i ++; // this is the last instruction in the file read while loop
	}		// while
	Serial.println (F("\r\nImage conversion done."));

	
	
	
	
	
	
	
	
	
	delay(1);
	}
	
	// f_SPImemdump ( 0, 16 );
	// f_SPImemdump ( 0x1000, 16 );

	
	Serial.println(F("Image convert + write to memory done. "));

}

