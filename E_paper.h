#ifndef E_PAPER_H
#define E_PAPER_H
#include <GxEPD.h>
#include "Ublox.h"
#include "GPS_data.h"
//! There are four versions of the 2.13 screen,
//  if you are not sure which version, please test each one,
//  if it is successful then it belongs to the model of the file name
// The 2.66 screen has other SPI pins, and more pixels. Partof the pixels are now unused.
#include <GxDEPG0213BN/GxDEPG0213BN.h>  //Huidige schermen, 212*104
//#include <GxGDEH0213B73/GxGDEH0213B73.h>  //Eerste schermen, 212*104
//#include <GxGDEM0213B74/GxGDEM0213B74.h>  // 2.13", 212*104 
//#include <GxDEPG0266BN/GxDEPG0266BN.h>  //2.66 screen for rollz...
//#include <GxGDEW0213M21/GxGDEW0213M21.h>  //nog een andere versie, resolutie NIET OK vanwege resolutie !!!
//#include <GxGDEP015OC1/GxGDEP015OC1.h>    //  GDEH015OC1 1.54" screen for Jeff Turner !!!

/* https://oleddisplay.squix.ch/#/home font-converter online, used for 48pt font !!
Canvas size(s):   48 x 48                                
Background color: White 
Brightness / alpha threshold: zelf instelllen zodat het voorbeeld er het mooiste uitziet in de preview eronder
Scaling: scale to fit, keeping proportions
Center: horizontally & vertically
Flip: horizontally 
*/
// https://tchapi.github.io/Adafruit-GFX-Font-Customiser/ used for extracting only needed digits and decimal point !!
// FreeFonts from Adafruit_GFX
#include "Fonts/FreeSansBold6pt7b.h"//gebruikt
#include "Fonts/FreeMonoBold8pt7b.h"//gebruikt
#include "Fonts/FreeMonoBold9pt7b.h"//gebruikt
#include "Fonts/FreeMonoBold12pt7b.h"//gebruikt

#include "Fonts/FreeSansBold9pt7b.h"//gebruikt
#include "Fonts/FreeSansBold12pt7b.h"//gebruikt
#include "Fonts/FreeSansBold18pt7b.h"//gebruikt
#include "Fonts/FreeSansBold24pt7b.h"//
#include "Fonts/FreeSansBold30pt7b.h"//gebruikt
#include "Fonts/FreeSansBold75pt7b.h"//gebruikt
#include "Fonts/SansSerif_bold_46_nr.h"//bijgevoegd in lib
#include "Fonts/SansSerif_bold_84_nr.h"//bijgevoegd in lib
#include "Fonts/SansSerif_bold_96_nr.h"//bijgevoegd in lib
#include "Fonts/Sea_Dog_2001_Italic9pt7b.h"//simon
#include "Fonts/Sea_Dog_2001_Italic12pt7b.h"//simon
#include "Fonts/Sea_Dog_2001_Italic16pt7b.h"//simon
#include "Fonts/SF_Distant_Galaxy7pt7b.h"//surfbuddies
#include "Fonts/SF_Distant_Galaxy9pt7b.h"//surfbuddies
#include "Fonts/SF_Distant_Galaxy12pt7b.h"//surfbuddies
#include "Fonts/SF_Distant_Galaxy16pt7b.h"//surfbuddies
#include "Fonts/SPINC___12pt7b.h"//buddies on boards
#include "Fonts/BitmapSurfbuddies.h"
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>

#define ELINK_SS 5
#define ELINK_BUSY 4
#define ELINK_RESET 16
#define ELINK_DC 17

//#define BUTTON_PIN 39
#define BOOT_SCREEN 0
#define STATS1 49
#define STATS2 50
#define STATS3 51
#define STATS4 52
#define STATS5 53
#define STATS6 54    //simon stat screen
#define STATS7 55    //bar graph screen 10s run
#define STATS8 56
#define STATS9 57
#define STATSA 65  //asci code for A
#define STATSB 66  //asci code for B
#define STATSC 67  //asci code for C
#define STATSD 68  //asci code for D
#define SPEED1 49
#define SPEED2 50
#define SPEED3 51
#define SPEED4 52
#define SPEED5 53
#define SPEED6 54
#define SPEED7 55
#define SPEED8 56
#define SPEED9 57


#define SPEED 10
#define WIFI_ON 11
#define WIFI_STATION 12
#define WIFI_SOFT_AP 13
//#define SPEED2 14
#define TROUBLE 15
#define GPS_INIT_SCREEN 16


extern GxEPD_Class display;
extern int sdTrouble,gps_speed,S10_previous_run;
extern int wifi_search;
extern int start_logging_millis;
extern bool sdOK,LITTLEFS_OK;
extern bool Wifi_on;
extern bool SoftAP_connection;
extern bool GPS_Signal_OK;
extern bool Shut_down_Save_session;
extern float RTC_voltage_bat,alfa_window;
extern float RTC_minimum_voltage_bat;
extern  double delta_heading,ref_heading;
extern int bootCount,run_count,stat_count,GPIO12_screen;
extern int ftpStatus;
extern String IP_adress;
extern String actual_ssid;
extern const char SW_version[16];
extern UBXMessage ubxMessage;
static int update_epaper = 2;
extern int freeSpace;
/*
extern float Afstand_lijn;
extern float Afstand_lijn2;
extern float Afstand_sec;
extern float Afstand_sec2;
extern float Afstand_gps;
*/
extern RTC_DATA_ATTR int offset;
extern RTC_DATA_ATTR float RTC_distance;
extern RTC_DATA_ATTR float RTC_avg_10s;
extern RTC_DATA_ATTR float RTC_max_2s;
//Simon
extern RTC_DATA_ATTR short RTC_year;
extern RTC_DATA_ATTR short RTC_month;
extern RTC_DATA_ATTR short RTC_day;
extern RTC_DATA_ATTR short RTC_hour;
extern RTC_DATA_ATTR short RTC_min;
extern RTC_DATA_ATTR float RTC_alp;
extern RTC_DATA_ATTR float RTC_500m;
extern RTC_DATA_ATTR float RTC_1h;
extern RTC_DATA_ATTR float RTC_mile;
extern RTC_DATA_ATTR float RTC_R1_10s;
extern RTC_DATA_ATTR float RTC_R2_10s;
extern RTC_DATA_ATTR float RTC_R3_10s;
extern RTC_DATA_ATTR float RTC_R4_10s;
extern RTC_DATA_ATTR float RTC_R5_10s;
extern RTC_DATA_ATTR int RTC_counter;
//Simon
extern GPS_speed M100;
extern GPS_speed M250;
extern GPS_speed M500;
extern Alfa_speed A250;
extern Alfa_speed A500;
extern Alfa_speed a500;
extern GPS_time S1800;
extern GPS_time S3600;
extern GPS_Track M_500;
void Boot_screen(void);
void Off_screen(int choice);
void Sleep_screen(int choice);
void Update_screen(int screen);
void Bat_level(int X_offset,int Y_offset);
void Bat_level_Simon(int offset);
void Sat_level(int offset);
void time_print(int time);
void Stats_4lines(String Message1, String Message2, String Message3, String Message4, float Value1, float Value2, float Value3, float Value4);
#endif
