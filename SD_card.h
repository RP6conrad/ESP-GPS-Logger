//JsonConfigFile.ino for reading config file !!!
//Changed next file for compiling with Arduino IDE 2.02 (SD(esp32) to SD)
//C:\Users\andre\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.6\libraries\SD\library.properties

#ifndef SD_CARD_H
#define SD_CARD_H
#include "Ublox.h"
#include "GPS_data.h"
#include <SD.h>
#include "ArduinoJson.h"

 
extern struct tm tmstruct ;
extern int Time_Set_OK;
extern int NTP_time_set;
extern int Gps_time_set;
extern int first_fix_GPS;
extern int wifi_search;
extern int sdTrouble;
extern int start_logging_millis;
extern bool sdOK,button,LITTLEFS_OK;
extern bool GPS_logging;
extern float Mean_heading,heading_SD;
extern float RTC_calibration_bat;
extern float calibration_speed;
//extern int time_out_nav_pvt;
extern int next_gpy_full_frame;
extern byte mac[6];
extern const char SW_version[16];
extern char Ublox_type[20];
extern char RTC_Sleep_txt[32];
extern char TimeZone[64];
extern GPS_speed M100;
extern GPS_speed M250;
extern GPS_speed M1852;
extern GPS_time S2;
extern GPS_time s2;
extern GPS_time S10;
extern GPS_time s10;
extern Alfa_speed A250;
extern GPS_data Ublox; // create an object storing GPS_data, definition in RTOS
extern GPS_SAT_info Ublox_Sat;//create an object storing GPS_SAT info !
extern int nav_pvt_message; 
extern int nav_sat_message;
extern int RTC_Board_Logo;
extern int RTC_Sail_Logo;
extern int RTC_bat_choice;
extern int RTC_SLEEP_screen;
extern int RTC_OFF_screen;

struct Config {
  float cal_bat=1.74;//calibration for read out bat voltage
  float cal_speed=3.6;//conversion m/s to km/h, for knots use 1.944
  int sample_rate=5;//gps_rate in Hz, 1, 5 or 10Hz !!!
  int gnss=3;//default setting 2 GNSS, GPS & GLONAS
  int field=1;//choice for first field in speed screen !!!
  int field_actual=1;//actual choice in speed screen
  int speed_large_font=1;//fonts on the first line are bigger, actual speed font is smaller
  int dynamic_model=0;//choice for dynamic model "Sea",if 0 model "portable" is used !!
  float timezone=1;//choice for timedifference in hours with UTC, for Belgium 1 or 2 (summertime)
  bool timezone_DST=1;//auto switch to summertime (daylightsaving)
  int Stat_screens=123;//choice for stats field when no speed, here stat_screen 1, 2 and 3 will be active
  int Stat_screens_time=4;//time between switching stat_screens
  int GPIO12_screens=54;//choice for stats field when gpio12 is activated (pull-up high, low = active)
  int Stat_screens_persist=123;//choice for stats field when no speed, here stat_screen 1, 2 and 3 will be active / for resave the config
  int GPIO12_screens_persist=54;//choice for stats field when gpio12 is activated (pull-up high, low = active) / for resave the config
  int Board_Logo=1;
  int Sail_Logo=1;
  int stat_screen[10];//which stat_screen you want to see ?
  int gpio12_screen[10];//which stat_screen when gpio 12 toggles ?
  int speed_screen[10];//which speed fields are selected ?
  int screen_count=0;
  int gpio12_count=0;
  int speed_count=0;
  int sleep_off_screen=11;
  int stat_speed=1;//max speed in m/s for showing Stat screens
  int start_logging_speed=1;
  int bar_length=1852;//choice for bar indicator for length of run in m (nautical mile)
  int archive_days=10; //how many days files will be moved to the "Archive" dir
  bool bat_choice=1;//choice for voltage in % or voltage
  bool logTXT=1;// switchinf off .txt files
  bool logUBX=1;//log to .ubx
  bool logUBX_nav_sat=0;// log nav sat msg to .ubx
  bool logSBP=1;//log to .sbp
  bool logGPY=1;//log to .gps
  bool logGPX=0;//log to .gpx
  int file_date_time=2;//type of filenaming, with MAC adress or datetime
  char UBXfile[32]="My_ESP_GPS";//your preferred filename
  char Sleep_info[32]="Your ID";//your preferred sleep text
  char ssid[32]="My_SSID";//your SSID
  char password[32]="password";//your password
  char ssid2[32]="ESP_GPS";//your SSID
  char password2[32]="password2";//your password
  int config_fail=0;
  int ublox_type=0;
  int cpu_freq = 40;
  } ;
extern Config config;
void AddString();
void logERR( const char * message);
void Open_files(void);
void Close_files(void);
void Flush_files(void);
void Log_to_SD(void); 
void loadConfiguration(const char *filename, const char *filename_backup, Config &config) ;
void Model_info(int model);
void printFile(const char *filename);
void Session_info(GPS_data G);
void Session_results_M(GPS_speed M);
void Session_results_S(GPS_time S);
void Session_results_Alfa(Alfa_speed A,GPS_speed M);
void TimeZone_env (float timezone);
/*
void log_header_SBP(void);
void log_SBP(void);
*/
#endif
