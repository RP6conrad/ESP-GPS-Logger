#ifndef _GxBitmapExamples_H_
#define _GxBitmapExamples_H_

#if defined(ESP8266) || defined(ESP32)
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif

// 'D_watermark_Surfbuddie logo zwart', 48x48px
const unsigned char Surfbuddie_logoS_zwart[304] PROGMEM = {
	0xff, 0xfc, 0x00, 0x07, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 
	0x1f, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x03, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0xff, 0xe0, 0x00, 
	0x00, 0x00, 0x00, 0x3f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x0f, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x00, 0x00, 
	0x00, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x1f, 0xff, 0xfe, 0x00, 0x07, 0xe0, 0x0f, 
	0xff, 0xff, 0x00, 0x07, 0xe0, 0x0f, 0xff, 0xff, 0x80, 0x07, 0xf0, 0x00, 0x00, 0x1f, 0xc0, 0x07, 
	0xf0, 0x00, 0x00, 0x0f, 0xc0, 0x07, 0xf0, 0x00, 0x00, 0x1f, 0xe0, 0x07, 0xf8, 0x03, 0xff, 0xff, 
	0xe0, 0x07, 0xf8, 0x01, 0xff, 0xff, 0xf0, 0x07, 0xfc, 0x01, 0xff, 0xff, 0xe0, 0x07, 0xfc, 0x00, 
	0xfc, 0x00, 0x00, 0x07, 0xfe, 0x00, 0xfc, 0x00, 0x00, 0x07, 0xfe, 0x00, 0x7f, 0xff, 0xfe, 0x07, 
	0xff, 0x00, 0x7f, 0xff, 0xff, 0x07, 0xff, 0x00, 0x3f, 0xff, 0xff, 0x07, 0xff, 0x80, 0x07, 0xff, 
	0xff, 0x07, 0xff, 0x80, 0x00, 0x00, 0x00, 0x07, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x07, 0xff, 0xe0, 
	0x00, 0x00, 0x00, 0x07, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x07, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x0f, 
	0xff, 0xf8, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xfc, 0x00, 0x00, 
	0x00, 0x1f, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 
	0x80, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x7f, 
	0xff, 0xff, 0xf0, 0x00, 0x00, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0xff, 0xff, 0xff, 0xfc, 0x00, 
	0x01, 0xff, 0xff, 0xff, 0xff, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0x80, 0x07, 0xff, 0xff, 0xff, 
	0xff, 0xe0, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff
};

// 'D_watermark_GA_sails_triangle', 48x48px
const unsigned char GAsails_logoS_zwart[304] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3f, 0xff, 0xff, 0xff, 0xff, 
	0xfe, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x9f, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xdf, 0xff, 0xff, 
	0xff, 0xff, 0xfa, 0x4f, 0xff, 0xff, 0xff, 0xff, 0xf2, 0x2f, 0xff, 0xff, 0xff, 0xff, 0xf4, 0x27, 
	0xff, 0xff, 0xff, 0xff, 0xe4, 0x17, 0xff, 0xff, 0xff, 0xff, 0xe8, 0x1b, 0xff, 0xff, 0xff, 0xff, 
	0xd0, 0x09, 0xff, 0xff, 0xff, 0xff, 0x90, 0x0d, 0xff, 0xff, 0xff, 0xff, 0xa0, 0x04, 0xff, 0xff, 
	0xff, 0xff, 0x21, 0x82, 0xff, 0xff, 0xff, 0xff, 0x43, 0xc2, 0x7f, 0xff, 0xff, 0xfe, 0x43, 0xc1, 
	0x7f, 0xff, 0xff, 0xfe, 0x87, 0xe1, 0x3f, 0xff, 0xff, 0xfd, 0x07, 0xe0, 0x9f, 0xff, 0xff, 0xf9, 
	0x0f, 0xf0, 0xdf, 0xff, 0xff, 0xfa, 0x1f, 0xf8, 0x4f, 0xff, 0xff, 0xf2, 0x1f, 0xf8, 0x6f, 0xff, 
	0xff, 0xf4, 0x3f, 0xfc, 0x27, 0xff, 0xff, 0xe8, 0x3f, 0xfc, 0x17, 0xff, 0xff, 0xe8, 0x00, 0x00, 
	0x13, 0xff, 0xff, 0xd0, 0x00, 0x00, 0x0b, 0xff, 0xff, 0x90, 0x20, 0x3f, 0x0d, 0xff, 0xff, 0xa0, 
	0x30, 0x7f, 0x84, 0xff, 0xff, 0x20, 0x70, 0xe3, 0x86, 0xff, 0xff, 0x40, 0xf8, 0xc1, 0xc2, 0x7f, 
	0xfe, 0x80, 0xfc, 0xf0, 0xc1, 0x7f, 0xfc, 0x81, 0xcd, 0xf8, 0xc1, 0x3f, 0xfd, 0x01, 0x8f, 0xf9, 
	0xc0, 0xbf, 0xf9, 0x03, 0x87, 0xf3, 0xc0, 0x9f, 0xfa, 0x07, 0xff, 0xff, 0x80, 0x5f, 0xf6, 0x07, 
	0xff, 0xbf, 0x00, 0x6f, 0xf4, 0x00, 0x00, 0x08, 0x00, 0x27, 0xec, 0x00, 0x00, 0x08, 0x00, 0x17, 
	0xc8, 0x00, 0x00, 0x00, 0x00, 0x13, 0xd0, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x90, 0x00, 0x00, 0x00, 
	0x00, 0x09, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xfd, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

// 'D_watermark_starboard_SVG_logo', 48x48px
const unsigned char Starboard_logoS_zwart[304] PROGMEM = {
	0xff, 0xff, 0xff, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 
	0xff, 0xff, 0xff, 0xff, 0xf4, 0x9f, 0xff, 0xff, 0xff, 0xff, 0xe5, 0x9f, 0xff, 0xff, 0xff, 0xff, 
	0xe5, 0x97, 0xff, 0xff, 0xff, 0xff, 0xe5, 0x93, 0xff, 0xff, 0xff, 0xff, 0xe7, 0x9b, 0xff, 0xff, 
	0xff, 0xff, 0xcf, 0x99, 0xff, 0xff, 0xff, 0xff, 0x80, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x8f, 0xfd, 0xff, 0xff, 0xff, 0xff, 0x91, 0xc5, 0xff, 0xff, 0xff, 0xff, 
	0x8d, 0xbd, 0xff, 0xff, 0xff, 0xfe, 0x48, 0xb8, 0x3f, 0xff, 0xff, 0xfe, 0xd1, 0xbb, 0x7f, 0xff, 
	0xff, 0xfe, 0xdf, 0xbb, 0x7f, 0xff, 0xff, 0xff, 0x6f, 0xb3, 0x7f, 0xff, 0xff, 0xff, 0x2d, 0xb6, 
	0xff, 0xff, 0xff, 0xff, 0xf7, 0xe7, 0xff, 0xff, 0xff, 0xe0, 0x73, 0xee, 0x07, 0xff, 0xff, 0xe7, 
	0x3b, 0xcc, 0x07, 0xff, 0xff, 0xef, 0x89, 0x99, 0xf7, 0xff, 0xff, 0xef, 0xc4, 0x33, 0xf7, 0xff, 
	0xff, 0xef, 0xf7, 0xef, 0xf7, 0xff, 0xff, 0xef, 0xff, 0xff, 0xf7, 0xff, 0xff, 0xe0, 0xff, 0xff, 
	0x07, 0xff, 0xff, 0xf0, 0x7f, 0xfe, 0x07, 0xff, 0xff, 0xff, 0x3f, 0xfc, 0xff, 0xff, 0xff, 0xff, 
	0x9f, 0xf8, 0xff, 0xff, 0xff, 0xff, 0x9f, 0xf9, 0xff, 0xff, 0xff, 0xff, 0x9f, 0xf9, 0xff, 0xff, 
	0xff, 0xff, 0x9f, 0xfc, 0xff, 0xff, 0xff, 0xff, 0x9f, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0x9f, 0xfe, 
	0x7f, 0xff, 0xff, 0xff, 0xcf, 0xfc, 0xff, 0xff, 0xff, 0xff, 0x9f, 0xf9, 0xff, 0xff, 0xff, 0xff, 
	0x03, 0xf3, 0xff, 0xff, 0xff, 0xff, 0x81, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xe0, 0x0e, 0x00, 0x07, 0xff, 0xff, 0xcf, 0xff, 0xff, 0xf7, 0xff, 0xff, 0xcf, 0xf0, 0x3f, 
	0xf7, 0xff, 0xff, 0xcf, 0xe7, 0xff, 0xf7, 0xff, 0xff, 0xcf, 0xe7, 0xef, 0xf7, 0xff, 0xff, 0xcf, 
	0xe7, 0xef, 0xf7, 0xff, 0xff, 0xe0, 0x07, 0xe0, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xef, 0xff
};

// 'D_watermark_Zfins_logo_zwart', 48x48px
const unsigned char Zfin_logoS_zwart[304] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x0f, 0xff, 
	0xff, 0xf0, 0x00, 0x00, 0x07, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x07, 0xff, 0xff, 0xf3, 0xff, 0xff, 
	0xe7, 0xff, 0xff, 0xf3, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xf3, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xf3, 
	0xff, 0xff, 0xe7, 0xff, 0xff, 0xe1, 0xff, 0x00, 0x00, 0x01, 0xff, 0xc0, 0xff, 0x80, 0x00, 0x03, 
	0xff, 0x8c, 0x7f, 0xc7, 0xff, 0xc7, 0xff, 0x1e, 0x3f, 0xe7, 0xff, 0x8f, 0xfe, 0x3f, 0x1f, 0xf3, 
	0xff, 0x9f, 0xfc, 0x7f, 0x8f, 0xf9, 0xfe, 0x3f, 0xf8, 0xff, 0xc7, 0xfc, 0xfe, 0x7f, 0xf1, 0xff, 
	0xe3, 0xfe, 0x78, 0xff, 0xe1, 0xff, 0xf1, 0xff, 0x39, 0xff, 0xc0, 0x00, 0x00, 0xff, 0x83, 0xff, 
	0x80, 0x00, 0x00, 0xff, 0xc7, 0xff, 0xff, 0xf3, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xf3, 0xff, 0xff, 
	0xe7, 0xff, 0xff, 0xf3, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xf3, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xf3, 
	0xff, 0xff, 0xc7, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x07, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x07, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xf5, 0xf7, 
	0xc7, 0xff, 0xff, 0xf1, 0xe1, 0xf3, 0xc7, 0xff, 0xff, 0xf1, 0xe1, 0xf3, 0xe7, 0xff, 0xff, 0xf3, 
	0xe1, 0xf3, 0xc7, 0xff, 0xff, 0xe7, 0xe1, 0xf3, 0xe7, 0xff, 0xff, 0xf3, 0xe5, 0xf3, 0xe7, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

// 'Fanatic_logoS_zwart', 48x48px
const unsigned char Fanatic_logoS_zwart[304] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0x9f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x87, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0x83, 0xff, 0xff, 0xff, 0xff, 0xff, 0x81, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x7f, 0xff, 0xff, 0xff, 0xff, 0x80, 0x3f, 0xff, 
	0xff, 0xff, 0xff, 0x80, 0x1f, 0xff, 0xff, 0xff, 0xff, 0x80, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xc0, 
	0x07, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x03, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x01, 0xff, 0xff, 0xff, 
	0xff, 0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x3f, 
	0xff, 0xff, 0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xbf, 0x80, 0x0f, 0xff, 0xff, 0xff, 0x9f, 
	0xc0, 0x07, 0xff, 0xff, 0xff, 0x8f, 0xe0, 0x03, 0xff, 0xff, 0xff, 0x87, 0xf0, 0x01, 0xff, 0xff, 
	0xff, 0x83, 0xf8, 0x00, 0xff, 0xff, 0xff, 0x81, 0xfc, 0x00, 0x7f, 0xff, 0xff, 0x80, 0xfe, 0x00, 
	0x3f, 0xff, 0xff, 0x80, 0x7f, 0x00, 0x1f, 0xff, 0xff, 0x80, 0x3f, 0x80, 0x0f, 0xff, 0xff, 0x80, 
	0x1f, 0xc0, 0x07, 0xff, 0xff, 0x80, 0x0f, 0xe0, 0x07, 0xff, 0xff, 0xc0, 0x07, 0xf0, 0x07, 0xff, 
	0xff, 0xe0, 0x03, 0xf8, 0x0f, 0xff, 0xff, 0xf0, 0x01, 0xfc, 0x1f, 0xff, 0xff, 0xf8, 0x00, 0xfe, 
	0x3f, 0xff, 0xff, 0xfc, 0x00, 0x7f, 0x7f, 0xff, 0xff, 0xfe, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 
	0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0x80, 0x0f, 0xff, 0xff, 0xff, 0xff, 0x80, 0x07, 0xff, 0xff, 
	0xff, 0xff, 0x80, 0x0f, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x3f, 
	0xff, 0xff, 0xff, 0xfc, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xff, 0xff, 0xf0, 
	0x01, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

// 'NoveNove_logoS_zwart', 48x48px
const unsigned char NoveNove_logoS_zwart[304] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x0f, 
	0xff, 0xff, 0xff, 0xff, 0xe0, 0x03, 0xff, 0xff, 0xff, 0xff, 0x80, 0x01, 0xff, 0xff, 0xff, 0xff, 
	0x00, 0x00, 0x7f, 0xff, 0xff, 0xfc, 0x03, 0xe0, 0x1f, 0xff, 0xff, 0xf0, 0x0f, 0xf0, 0x07, 0xff, 
	0xff, 0xc0, 0x1f, 0xfc, 0x03, 0xff, 0xff, 0x80, 0x7f, 0xff, 0x00, 0xff, 0xfe, 0x01, 0xfe, 0x3f, 
	0xc0, 0x3f, 0xfc, 0x07, 0xf8, 0x0f, 0xf0, 0x1f, 0xf8, 0x0f, 0xe0, 0x07, 0xf8, 0x0f, 0xf8, 0x3f, 
	0xc0, 0x01, 0xfe, 0x0f, 0xf8, 0x7f, 0x01, 0x80, 0x7f, 0x0f, 0xf8, 0x7c, 0x03, 0xe0, 0x1f, 0x0f, 
	0xf8, 0xf8, 0x0f, 0xf8, 0x0f, 0x0f, 0xf8, 0xf0, 0x3f, 0xfe, 0x0f, 0x0f, 0xf8, 0xf0, 0x7f, 0xff, 
	0x0f, 0x0f, 0xf8, 0xf0, 0x7f, 0xfe, 0x0f, 0x0f, 0xf8, 0xf0, 0x1f, 0xf8, 0x0f, 0x0f, 0xf8, 0xf0, 
	0x07, 0xf0, 0x0f, 0x0f, 0xf8, 0xf0, 0x01, 0xc0, 0x3f, 0x0f, 0xf8, 0xf0, 0x00, 0x00, 0xff, 0x0f, 
	0xf8, 0xf0, 0x40, 0x01, 0xfc, 0x0f, 0xf8, 0xf0, 0xf0, 0x07, 0xf8, 0x0f, 0xf8, 0xf0, 0xfc, 0x1f, 
	0xe0, 0x1f, 0xf8, 0xf0, 0x7f, 0xff, 0x80, 0x7f, 0xf8, 0xf0, 0x3f, 0xfe, 0x00, 0xff, 0xf8, 0xf0, 
	0x1f, 0xfc, 0x03, 0xff, 0xf8, 0xf8, 0x07, 0xf0, 0x0f, 0xff, 0xf8, 0x7c, 0x01, 0xc0, 0x3f, 0xcf, 
	0xf8, 0x7f, 0x00, 0x00, 0x7f, 0x0f, 0xf8, 0x3f, 0xc0, 0x01, 0xfe, 0x0f, 0xf8, 0x0f, 0xf0, 0x07, 
	0xf8, 0x0f, 0xfc, 0x03, 0xf8, 0x1f, 0xe0, 0x1f, 0xfe, 0x01, 0xff, 0x7f, 0x80, 0x3f, 0xff, 0x80, 
	0x7f, 0xff, 0x00, 0xff, 0xff, 0xe0, 0x1f, 0xfc, 0x03, 0xff, 0xff, 0xf0, 0x0f, 0xf0, 0x0f, 0xff, 
	0xff, 0xfc, 0x03, 0xc0, 0x1f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xc0, 0x01, 
	0xff, 0xff, 0xff, 0xff, 0xe0, 0x07, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x0f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

// 'DuoTone_logoS_zwart', 48x48px
const unsigned char DuoTone_logoS_zwart[304] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xff, 0xff, 0xff, 0x83, 0xff, 0xff, 0xff, 0xff, 
	0xfe, 0x03, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x03, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x03, 0xff, 0xff, 
	0xff, 0xff, 0xc0, 0x03, 0xff, 0xff, 0xff, 0xff, 0x00, 0x03, 0xff, 0x9f, 0xff, 0xfc, 0x00, 0x03, 
	0xfe, 0x1f, 0xff, 0xf8, 0x00, 0x03, 0xf8, 0x1f, 0xff, 0xe0, 0x00, 0x03, 0xe0, 0x1f, 0xff, 0x80, 
	0x00, 0x07, 0x80, 0x1f, 0xfe, 0x00, 0x00, 0x0f, 0x80, 0x1f, 0xfc, 0x00, 0x00, 0x3f, 0x00, 0x1f, 
	0xf0, 0x00, 0x00, 0xff, 0x00, 0x1f, 0xf0, 0x00, 0x03, 0xff, 0x00, 0x1f, 0xe0, 0x00, 0x07, 0xff, 
	0x00, 0x1f, 0xe0, 0x00, 0x1f, 0xff, 0x00, 0x1f, 0xe0, 0x00, 0x7f, 0xff, 0x00, 0x1f, 0xe0, 0x01, 
	0xff, 0xff, 0x00, 0x1f, 0xe0, 0x03, 0xff, 0xff, 0x00, 0x1f, 0xe0, 0x07, 0xff, 0xff, 0x00, 0x1f, 
	0xe0, 0x07, 0xff, 0xff, 0x00, 0x1f, 0xe0, 0x07, 0xff, 0xff, 0x00, 0x1f, 0xe0, 0x03, 0xff, 0xff, 
	0x00, 0x1f, 0xe0, 0x01, 0xff, 0xff, 0x00, 0x1f, 0xe0, 0x00, 0x7f, 0xff, 0x00, 0x1f, 0xe0, 0x00, 
	0x3f, 0xff, 0x00, 0x1f, 0xe0, 0x00, 0x0f, 0xff, 0x00, 0x1f, 0xe0, 0x00, 0x03, 0xff, 0x00, 0x1f, 
	0xf0, 0x00, 0x00, 0xff, 0x00, 0x1f, 0xf8, 0x00, 0x00, 0x3f, 0x00, 0x1f, 0xfe, 0x00, 0x00, 0x1f, 
	0x80, 0x1f, 0xff, 0x80, 0x00, 0x07, 0x80, 0x1f, 0xff, 0xe0, 0x00, 0x03, 0xe0, 0x1f, 0xff, 0xf0, 
	0x00, 0x03, 0xf0, 0x1f, 0xff, 0xfc, 0x00, 0x03, 0xfc, 0x1f, 0xff, 0xff, 0x00, 0x03, 0xff, 0x1f, 
	0xff, 0xff, 0xc0, 0x03, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x03, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x03, 
	0xff, 0xff, 0xff, 0xff, 0xfe, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0x83, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xe3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

// 'JP_logoS_zwart', 48x48px
const unsigned char JP_logoS_zwart[304] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x3f, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x0f, 0xff, 0xff, 0xff, 0xff, 0x80, 0x03, 0xff, 0xff, 0xff, 0xff, 
	0x87, 0x00, 0xff, 0xff, 0xff, 0xff, 0x8f, 0xc0, 0x7c, 0xff, 0xff, 0xff, 0x8f, 0xf0, 0x1c, 0x3f, 
	0xff, 0xff, 0x8f, 0xfc, 0x04, 0x1f, 0xff, 0xff, 0x87, 0x9f, 0x00, 0x07, 0xff, 0xff, 0x87, 0x87, 
	0x80, 0x01, 0xff, 0xff, 0xc3, 0x81, 0xe0, 0x00, 0x7f, 0xff, 0xc3, 0xc0, 0x78, 0x20, 0x7f, 0xff, 
	0xe1, 0xe0, 0x3c, 0x38, 0x7f, 0xff, 0xf0, 0xf0, 0x0c, 0x3c, 0x3f, 0xff, 0xf8, 0x38, 0x02, 0x3c, 
	0x3f, 0xff, 0xfc, 0x1e, 0x00, 0x3c, 0x3f, 0xff, 0xfe, 0x07, 0x80, 0x3e, 0x3f, 0xff, 0xff, 0x03, 
	0xc0, 0x1e, 0x3f, 0xff, 0xff, 0x80, 0xf0, 0x1e, 0x3f, 0xff, 0xff, 0xe0, 0x3c, 0x1e, 0x3f, 0xff, 
	0xff, 0xf0, 0x1e, 0x1e, 0x3f, 0xff, 0xff, 0xfc, 0x1e, 0x0e, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0x0e, 
	0x0f, 0xff, 0xff, 0xff, 0x1f, 0x07, 0x07, 0xff, 0xff, 0xff, 0x0f, 0x07, 0x83, 0xff, 0xff, 0xff, 
	0x07, 0x03, 0x80, 0xff, 0xff, 0xff, 0x01, 0x01, 0xc0, 0x7f, 0xff, 0xff, 0xc0, 0x00, 0xf0, 0x1f, 
	0xff, 0xff, 0xe0, 0x00, 0x38, 0x0f, 0xff, 0xff, 0xf8, 0x08, 0x1e, 0x03, 0xff, 0xff, 0xfe, 0x0c, 
	0x07, 0x83, 0xff, 0xff, 0xff, 0x8f, 0x03, 0xc1, 0xff, 0xff, 0xff, 0xcf, 0x80, 0xe1, 0xff, 0xff, 
	0xff, 0xff, 0xe0, 0x21, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x11, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x01, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0x81, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xe1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

// 'NP_logoS_zwart', 48x48px
const unsigned char NP_logoS_zwart[304] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0x00, 0x00, 0x07, 0xc0, 0x01, 0xff, 0x1e, 0x0f, 0xe3, 0xc7, 0xf8, 0xff, 0xbf, 0xcf, 
	0xf9, 0x87, 0xfc, 0xff, 0x9f, 0xcf, 0xfc, 0x87, 0xfe, 0x7f, 0x9f, 0xe7, 0xfc, 0x17, 0xfe, 0x7f, 
	0xdf, 0xf7, 0xfe, 0x33, 0xfe, 0x7f, 0xcf, 0xf7, 0xfe, 0x33, 0xff, 0x3f, 0xcf, 0xf3, 0xff, 0x3b, 
	0xff, 0x3f, 0xef, 0xfb, 0xff, 0x79, 0xff, 0xbf, 0xe7, 0xf9, 0xfe, 0x79, 0xff, 0x9f, 0xe7, 0xf9, 
	0xfe, 0x7d, 0xff, 0x9f, 0xf3, 0xfd, 0xfe, 0xfc, 0xff, 0xdf, 0xf3, 0xfc, 0xfc, 0xfc, 0xff, 0xcf, 
	0xf3, 0xfc, 0xfc, 0xfe, 0xff, 0xcf, 0xf9, 0xfe, 0xfd, 0xfe, 0x7f, 0xef, 0xfc, 0xfe, 0x79, 0xfe, 
	0x7f, 0xe7, 0xfc, 0x1c, 0x79, 0xff, 0x7f, 0xe7, 0xff, 0x00, 0x7b, 0xff, 0x3f, 0xf7, 0xff, 0xff, 
	0x33, 0xff, 0x3f, 0xf3, 0xff, 0xff, 0x37, 0xfe, 0x1f, 0xf3, 0xff, 0xff, 0xb7, 0xfe, 0x1f, 0xfb, 
	0xff, 0xff, 0x87, 0xfe, 0x9f, 0xf9, 0xff, 0xff, 0x8f, 0xfc, 0xcf, 0xf9, 0xff, 0xff, 0xcf, 0xfc, 
	0xcf, 0xf9, 0xff, 0xff, 0xc0, 0x01, 0xc0, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

// 'Pryde_logoS_zwart', 48x48px
const unsigned char Pryde_logoS_zwart[304] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x03, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 
	0x7f, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x07, 0xff, 0xff, 0xc0, 
	0x00, 0x00, 0x01, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x7f, 
	0xfe, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x1f, 0xfc, 0x00, 0x1f, 0xf8, 0x00, 0x7f, 0xff, 
	0x00, 0x1f, 0xf0, 0x01, 0xff, 0xff, 0x80, 0x0f, 0xf0, 0x03, 0xff, 0xff, 0xc0, 0x07, 0xe0, 0x07, 
	0xff, 0xff, 0xf0, 0x07, 0xe0, 0x0f, 0xff, 0xff, 0xf0, 0x03, 0xe0, 0x1f, 0xff, 0xff, 0xf8, 0x03, 
	0xc0, 0x1f, 0xfc, 0x1f, 0xfc, 0x03, 0xc0, 0x3f, 0xf0, 0x07, 0xfc, 0x01, 0xc0, 0x3f, 0xe0, 0x03, 
	0xfe, 0x01, 0x80, 0x7f, 0xc0, 0x01, 0xfe, 0x01, 0x80, 0x7f, 0x80, 0x01, 0xfe, 0x01, 0x80, 0x7f, 
	0x80, 0x00, 0xff, 0x01, 0x80, 0x7f, 0x80, 0x00, 0xff, 0x00, 0x80, 0x7f, 0x00, 0x00, 0xff, 0x00, 
	0x80, 0x7f, 0x00, 0x00, 0xff, 0x00, 0x80, 0x7f, 0x80, 0x00, 0xff, 0x00, 0x80, 0x7f, 0x80, 0x00, 
	0xff, 0x01, 0x80, 0x7f, 0x80, 0x01, 0xfe, 0x01, 0x80, 0x7f, 0xc0, 0x01, 0xfe, 0x01, 0xc0, 0x3f, 
	0xe0, 0x03, 0xfe, 0x01, 0xc0, 0x3f, 0xf0, 0x07, 0xfc, 0x01, 0xc0, 0x1f, 0xfc, 0x1f, 0xfc, 0x03, 
	0xe0, 0x1f, 0xff, 0xff, 0xf8, 0x03, 0xe0, 0x0f, 0xff, 0xff, 0xf0, 0x03, 0xe0, 0x07, 0xff, 0xff, 
	0xf0, 0x07, 0xf0, 0x03, 0xff, 0xff, 0xe0, 0x07, 0xf0, 0x01, 0xff, 0xff, 0x80, 0x0f, 0xf8, 0x00, 
	0x7f, 0xff, 0x00, 0x1f, 0xfc, 0x00, 0x1f, 0xfc, 0x00, 0x1f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x3f, 
	0xff, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0x80, 0x00, 0x00, 0x00, 0xff, 0xff, 0xc0, 0x00, 0x00, 
	0x01, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x07, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xfe, 
	0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xc0, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};


#define LOGO_WIDHT          48
#define LOGO_HEIGHT         48

#endif

#ifndef _GxBitmapExamples_H_
#define _GxBitmapExamples_H_

#if defined(ESP8266) || defined(ESP32)
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif

// 'D_watermark_Surfbuddie logo zwart', 64x64px
const unsigned char Surfbuddie_logo_zwart[528] PROGMEM = {
	0xff, 0xff, 0xf8, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0xff, 0xff, 0xff, 
	0xff, 0xfc, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 
	0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 
	0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 
	0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 
	0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 
	0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 
	0xf0, 0x01, 0xff, 0xff, 0xff, 0x80, 0x00, 0x0f, 0xf0, 0x00, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x0f, 
	0xf0, 0x00, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x0f, 0xf8, 0x00, 0x7f, 0xff, 0xff, 0xf0, 0x00, 0x0f, 
	0xf8, 0x00, 0x3f, 0xff, 0xff, 0xf8, 0x00, 0x0f, 0xfc, 0x00, 0x00, 0x00, 0x07, 0xf8, 0x00, 0x0f, 
	0xfc, 0x00, 0x00, 0x00, 0x07, 0xfc, 0x00, 0x0f, 0xfc, 0x00, 0x1f, 0xff, 0xff, 0xfc, 0x00, 0x0f, 
	0xfe, 0x00, 0x1f, 0xff, 0xff, 0xfe, 0x00, 0x0f, 0xfe, 0x00, 0x1f, 0xff, 0xff, 0xfe, 0x00, 0x0f, 
	0xff, 0x00, 0x0f, 0xff, 0xff, 0xff, 0x00, 0x0f, 0xff, 0x00, 0x0f, 0xff, 0xff, 0xfe, 0x00, 0x0f, 
	0xff, 0x80, 0x07, 0xf8, 0x00, 0x00, 0x00, 0x0f, 0xff, 0x80, 0x07, 0xf8, 0x00, 0x00, 0x00, 0x0f, 
	0xff, 0x80, 0x03, 0xfe, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xc0, 0x03, 0xff, 0xff, 0xff, 0xf0, 0x0f, 
	0xff, 0xc0, 0x01, 0xff, 0xff, 0xff, 0xf8, 0x0f, 0xff, 0xe0, 0x00, 0xff, 0xff, 0xff, 0xf8, 0x0f, 
	0xff, 0xf0, 0x00, 0x7f, 0xff, 0xff, 0xfc, 0x0f, 0xff, 0xf0, 0x00, 0x07, 0xff, 0xff, 0xf8, 0x0f, 
	0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 
	0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 
	0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 
	0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x3f, 
	0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3f, 
	0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x7f, 
	0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0xff, 
	0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x01, 0xff, 
	0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x03, 0xff, 
	0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x0f, 0xff, 
	0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x1f, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x7f, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xf0, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x03, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

// 'D_watermark_GA_sails_triangle', 64x64px
const unsigned char GAsails_logo_zwart[528] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xfe, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x9f, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xfd, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xcf, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xfb, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf2, 0x27, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xe6, 0x37, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xec, 0x13, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xc8, 0x19, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xd8, 0x0d, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0x90, 0x0c, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xb0, 0x06, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0x60, 0x06, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x60, 0x03, 0x7f, 0xff, 0xff, 
	0xff, 0xff, 0xfe, 0xc0, 0x03, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x81, 0x81, 0x9f, 0xff, 0xff, 
	0xff, 0xff, 0xf9, 0x83, 0xc0, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xf9, 0x03, 0xc0, 0xcf, 0xff, 0xff, 
	0xff, 0xff, 0xf3, 0x07, 0xe0, 0x6f, 0xff, 0xff, 0xff, 0xff, 0xf6, 0x0f, 0xf0, 0x67, 0xff, 0xff, 
	0xff, 0xff, 0xe6, 0x0f, 0xf0, 0x37, 0xff, 0xff, 0xff, 0xff, 0xec, 0x1f, 0xf8, 0x33, 0xff, 0xff, 
	0xff, 0xff, 0xc8, 0x1f, 0xf8, 0x1b, 0xff, 0xff, 0xff, 0xff, 0xd8, 0x3f, 0xfc, 0x19, 0xff, 0xff, 
	0xff, 0xff, 0x90, 0x3f, 0xfc, 0x0c, 0xff, 0xff, 0xff, 0xff, 0x30, 0x7f, 0xfe, 0x0e, 0xff, 0xff, 
	0xff, 0xff, 0x60, 0xff, 0xff, 0x06, 0x7f, 0xff, 0xff, 0xfe, 0x60, 0xff, 0xff, 0x03, 0x7f, 0xff, 
	0xff, 0xfe, 0xc0, 0xff, 0xff, 0x03, 0x3f, 0xff, 0xff, 0xfc, 0x80, 0x00, 0x00, 0x01, 0xbf, 0xff, 
	0xff, 0xf9, 0x80, 0x00, 0x00, 0x01, 0x9f, 0xff, 0xff, 0xfb, 0x00, 0x00, 0x1f, 0x80, 0xdf, 0xff, 
	0xff, 0xf3, 0x00, 0xc0, 0x7f, 0xe0, 0x6f, 0xff, 0xff, 0xf6, 0x01, 0xc0, 0x7f, 0xf0, 0x67, 0xff, 
	0xff, 0xe4, 0x01, 0xe0, 0xf8, 0xf8, 0x37, 0xff, 0xff, 0xec, 0x03, 0xf1, 0xf0, 0x78, 0x33, 0xff, 
	0xff, 0xd8, 0x07, 0xf0, 0x40, 0x38, 0x1b, 0xff, 0xff, 0x98, 0x07, 0xf9, 0xfc, 0x38, 0x19, 0xff, 
	0xff, 0xb0, 0x0f, 0x39, 0xfe, 0x3c, 0x0d, 0xff, 0xff, 0x30, 0x0e, 0x1d, 0xfe, 0x38, 0x06, 0xff, 
	0xff, 0x60, 0x1e, 0x1f, 0xf0, 0x78, 0x06, 0xff, 0xfe, 0x40, 0x3c, 0x1f, 0xf9, 0xf8, 0x03, 0x7f, 
	0xfe, 0xc0, 0x3f, 0xff, 0xff, 0xf0, 0x03, 0x3f, 0xfc, 0x80, 0x7f, 0xff, 0xff, 0xf0, 0x01, 0xbf, 
	0xf9, 0x80, 0x7f, 0xff, 0x1f, 0xc0, 0x01, 0x9f, 0xfb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xdf, 
	0xf3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6f, 0xf6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67, 
	0xec, 0x01, 0x81, 0x80, 0x00, 0x06, 0x00, 0x37, 0xec, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 
	0xd8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1b, 0x9f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 
	0x9f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

// 'D_watermark_starboard_SVG_logo', 64x64px
const unsigned char Starboard_logoM_zwart[528] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xdf, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xfe, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xcf, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xe8, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc8, 0xcf, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xc8, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc8, 0x88, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xc8, 0x8c, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x8c, 0x8c, 0x7f, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0x0f, 0x8e, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x80, 0x3f, 0xff, 0xff, 
	0xff, 0xff, 0xfe, 0x00, 0x40, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xe0, 0x7f, 0xff, 0xff, 
	0xff, 0xff, 0xfe, 0x1f, 0xff, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x1f, 0xff, 0x3f, 0xff, 0xff, 
	0xff, 0xff, 0xfe, 0x01, 0xe0, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x09, 0x8f, 0x3f, 0xff, 0xff, 
	0xff, 0xff, 0xf0, 0x00, 0x1f, 0x07, 0xff, 0xff, 0xff, 0xff, 0xf1, 0x00, 0x9e, 0x07, 0xff, 0xff, 
	0xff, 0xff, 0xf3, 0x21, 0x9e, 0x67, 0xff, 0xff, 0xff, 0xff, 0xf3, 0x3f, 0xde, 0x67, 0xff, 0xff, 
	0xff, 0xff, 0xfb, 0x1f, 0xdc, 0x67, 0xff, 0xff, 0xff, 0xff, 0xf9, 0x9d, 0xdc, 0xcf, 0xff, 0xff, 
	0xff, 0xff, 0xfc, 0x8d, 0xd8, 0x9f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x4c, 0x19, 0x3f, 0xff, 0xff, 
	0xff, 0xfe, 0x03, 0x47, 0xf1, 0x7f, 0xff, 0xff, 0xff, 0xfe, 0x00, 0xe7, 0xe3, 0x80, 0x3f, 0xff, 
	0xff, 0xfc, 0x00, 0x73, 0xe7, 0x00, 0x1f, 0xff, 0xff, 0xfc, 0x7c, 0x31, 0xc6, 0x1f, 0x1f, 0xff, 
	0xff, 0xfc, 0x7f, 0x18, 0x8c, 0x3f, 0x1f, 0xff, 0xff, 0xfc, 0x7f, 0x88, 0x08, 0xff, 0x1f, 0xff, 
	0xff, 0xfc, 0x7f, 0xc6, 0x33, 0xff, 0x1f, 0xff, 0xff, 0xfc, 0x7f, 0xff, 0xff, 0xff, 0x1f, 0xff, 
	0xff, 0xfc, 0x07, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xfc, 0x03, 0xff, 0xff, 0xe0, 0x3f, 0xff, 
	0xff, 0xfe, 0x01, 0xff, 0xff, 0xc0, 0x3f, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0x8f, 0xff, 0xff, 
	0xff, 0xff, 0xf8, 0x7f, 0xff, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x3f, 0xfe, 0x1f, 0xff, 0xff, 
	0xff, 0xff, 0xfe, 0x3f, 0xfe, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0xfe, 0x3f, 0xff, 0xff, 
	0xff, 0xff, 0xfe, 0x3f, 0xfe, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x7f, 0xff, 0x1f, 0xff, 0xff, 
	0xff, 0xff, 0xfc, 0x7f, 0xff, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3f, 0xff, 0x8f, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0x1f, 0xff, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0xff, 0x1f, 0xff, 0xff, 
	0xff, 0xff, 0xfe, 0x3f, 0xfe, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x0f, 0xfc, 0x7f, 0xff, 0xff, 
	0xff, 0xff, 0xf8, 0x00, 0x78, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0xf9, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x0e, 0x00, 0x00, 0x3f, 0xff, 
	0xff, 0xfc, 0x00, 0x1f, 0xc0, 0x00, 0x3f, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xff, 
	0xff, 0xfc, 0xff, 0xe0, 0x1f, 0xff, 0x3f, 0xff, 0xff, 0xfc, 0xff, 0xc0, 0x1f, 0xff, 0x3f, 0xff, 
	0xff, 0xfc, 0xff, 0xc7, 0xf7, 0xff, 0x3f, 0xff, 0xff, 0xfc, 0xff, 0xcf, 0xf7, 0xff, 0x3f, 0xff, 
	0xff, 0xfc, 0x7f, 0xcf, 0xf3, 0xff, 0x3f, 0xff, 0xff, 0xfc, 0x00, 0x0f, 0xf0, 0x00, 0x3f, 0xff, 
	0xff, 0xfe, 0x00, 0x0f, 0xf0, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

// 'D_watermark_Zfins_logo_zwart', 64x64px
const unsigned char Zfin_logo_zwart[528] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 
	0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xfc, 0x7f, 0xff, 
	0xff, 0xff, 0x1f, 0xff, 0xff, 0xfc, 0x7f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xfc, 0x7f, 0xff, 
	0xff, 0xff, 0x1f, 0xff, 0xff, 0xfc, 0x7f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xfc, 0x7f, 0xff, 
	0xff, 0xfe, 0x1f, 0xff, 0xff, 0xf8, 0x00, 0x01, 0xff, 0xfc, 0x0f, 0xff, 0x00, 0x00, 0x00, 0x01, 
	0xff, 0xf8, 0x07, 0xff, 0x80, 0x00, 0x00, 0x03, 0xff, 0xf0, 0x63, 0xff, 0xc1, 0xff, 0xff, 0x07, 
	0xff, 0xe0, 0xf1, 0xff, 0xe1, 0xff, 0xff, 0x0f, 0xff, 0xc1, 0xf8, 0xff, 0xf0, 0xff, 0xfe, 0x1f, 
	0xff, 0x87, 0xfc, 0x7f, 0xf8, 0x7f, 0xfc, 0x3f, 0xff, 0x0f, 0xfe, 0x3f, 0xfc, 0x3f, 0xf8, 0x7f, 
	0xfe, 0x1f, 0xff, 0x1f, 0xfe, 0x1f, 0xf0, 0xff, 0xfc, 0x1f, 0xff, 0x8f, 0xff, 0x0f, 0xe1, 0xff, 
	0xf8, 0x7f, 0xff, 0xc7, 0xff, 0x87, 0xc3, 0xff, 0xf0, 0x7f, 0xff, 0xc3, 0xff, 0xc3, 0x87, 0xff, 
	0xe0, 0x00, 0x00, 0x01, 0xff, 0xe0, 0x0f, 0xff, 0xc0, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x1f, 0xff, 
	0xc0, 0x00, 0x00, 0x01, 0xff, 0xf8, 0x3f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xfc, 0x7f, 0xff, 
	0xff, 0xff, 0x1f, 0xff, 0xff, 0xfc, 0x7f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xfc, 0x7f, 0xff, 
	0xff, 0xff, 0x1f, 0xff, 0xff, 0xfc, 0x7f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xfc, 0x7f, 0xff, 
	0xff, 0xff, 0x1f, 0xff, 0xff, 0xfc, 0x7f, 0xff, 0xff, 0xff, 0x0f, 0xff, 0xff, 0xf8, 0x7f, 0xff, 
	0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xd9, 0xfd, 0xfc, 0x7f, 0xff, 
	0xff, 0xfe, 0x1f, 0xc1, 0xf8, 0xf8, 0x7f, 0xff, 0xff, 0xff, 0x8f, 0xc1, 0xf8, 0xfe, 0x7f, 0xff, 
	0xff, 0xff, 0x1f, 0xc1, 0xf8, 0xfc, 0x7f, 0xff, 0xff, 0xff, 0x1f, 0xc1, 0xf8, 0xf8, 0x7f, 0xff, 
	0xff, 0xfe, 0x3f, 0xc1, 0xf8, 0xfe, 0x7f, 0xff, 0xff, 0xfe, 0x1f, 0xc1, 0xf8, 0xfe, 0x7f, 0xff, 
	0xff, 0xff, 0x1f, 0xc9, 0xfd, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

#define LOGO_WIDHT          64
#define LOGO_HEIGHT         64

#endif