#ifndef E_PAPER_H
#define E_PAPER_H
#include <GxEPD.h>
#include "Ublox.h"
#include "GPS_data.h"
//! There are four versions of the 2.13 screen,
//  if you are not sure which version, please test each one,
//  if it is successful then it belongs to the model of the file name
// The 2.66 screen has other SPI pins, and more pixels. Partof the pixels are now unused.
//#include <GxDEPG0213BN/GxDEPG0213BN.h>  //Huidige schermen
//#include <GxGDEH0213B73/GxGDEH0213B73.h>  //Eerste schermen
#include <GxGDEM0213B74/GxGDEM0213B74.h>  // 2.13", opgepast hier rst en busy aanpassen 
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
#include "Fonts/FreeMonoBold9pt7b.h"//gebruikt
#include "Fonts/FreeMonoBold12pt7b.h"//gebruikt
#include "Fonts/FreeSansBold6pt7b.h"//gebruikt
#include "Fonts/FreeSansBold9pt7b.h"//gebruikt
#include "Fonts/FreeSansBold12pt7b.h"//gebruikt
#include "Fonts/FreeSansBold18pt7b.h"//gebruikt
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
#define BOOT_SCREEN 0
#define STATS1 1
#define STATS2 2
#define STATS3 3
#define STATS4 4
#define STATS5 5
#define STATS6 6    //simon stat screen
#define STATS7 7    //bar graph screen 10s run
#define STATS8 8
#define AVG_10S 9
#define SPEED 10
#define WIFI_ON 11
#define WIFI_STATION 12
#define WIFI_SOFT_AP 13

extern GxEPD_Class display;
extern int sdTrouble,wifi_search,gps_speed;
extern int start_logging_millis;
extern bool sdOK;
extern bool Wifi_on;
extern bool SoftAP_connection;
extern bool GPS_Signal_OK;
extern float RTC_voltage_bat,alfa_window;
extern  double delta_heading,ref_heading;
extern int bootCount,run_count,stat_count;
extern int ftpStatus;
extern String IP_adress;
extern const char SW_version[16];
//xtern int logo_choice[10];
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
//Simon
extern GPS_speed M100;
extern GPS_speed M250;
extern GPS_speed M500;
extern Alfa_speed A250;
extern Alfa_speed A500;
extern Alfa_speed a500;
extern GPS_time S1800;
extern GPS_time S3600;
void Boot_screen(void);
void Off_screen(int choice);
void sleep_screen(void);
void Sleep_screen(int choice);
void Update_screen(int screen);
void Bat_level(int offset);
void Bat_level_Simon(int offset);
void Sat_level(int offset);
class Button_push{
            public:
            Button_push(int GPIO_pin,int push_time,int long_pulse_time,int max_count);//constructor
            boolean Button_pushed(void);//return true if button is pushed longer then push_time
            boolean long_pulse;
            int button_count;
            private:
            boolean button_status, old_button_status,return_value;
            int Input_pin;
            int push_millis; 
            int time_out_millis; 
            int millis_10s;
            int max_pulse_time; 
            int max_button_count;        
};
#endif
