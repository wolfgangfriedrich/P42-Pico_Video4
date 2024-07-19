// Pico_Video4 header file for 
// (c) Pier42 Electronics Design
//     https://www.tindie.com/stores/pier42/


// written by
// Wolfgang Friedrich
// Rev Start 2023/April/01
// Last change: 2024/June/18

#ifndef P42Pico_Video4_h
#define P42Pico_Video4_h

#define PICO_VIDEO4_VERSION "1.0"

#include <Arduino.h>


const int IN1Pin	= 13;
const int IN2Pin	= 12;
const int IN3Pin	= 11;
const int IN4Pin	= 10;
const int IN5Pin	=  9;
const int IN6Pin	=  8;
const int IN7Pin	=  7;
const int IN8Pin	=  6;

const int OUTPin	=  5;
const int OUTLED	= 27;

const int EN_UART	= 26;

const int TP6 = 29;
const int TP7 = 28;
const int TP8 = 25;
const int TP13 = 2;
const int TP14 = 3;
const int TP15 = 4;

const int GPIO15 = 15;
const int GPIO21 = 21;
const int GPIO22 = 22;

const int QWIIC_SCL = 1;
const int QWIIC_SDA = 0;



#define ON_DEBOUNCE_WAIT  1000		// msec
#define OFF_DEBOUNCE_WAIT  500		// msec

#define IMAGE_SHOW_TIME 2500	// msec

#define CMD_IMAGE_WRITE   'w'     // w[0-8]
#define CMD_IMAGE_CONVERT 'i'     // i[0-8]
#define CMD_VERSION       'v'     // v
#define CMD_CLEARSCREEN   'c'     // c
#define CMD_SHOW          's'     // show a single image s[0-8]
#define CMD_HELP          '?'     // ? 


#define EMPTY_SCREEN_COLOUR 0x00     // 

#define PIN_OFF 0
#define PIN_ON  1

#define s_OFF 0
#define s_ON  1
#define s_DEBOUNCE_OFF 2
#define s_DEBOUNCE_ON  3

#define FALSE 0
#define TRUE  1


// void f_DownloadImage ( unsigned long memory_location );
// void f_DownloadConvertImage ( unsigned long memory_location );

#endif