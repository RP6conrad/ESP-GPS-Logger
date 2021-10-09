//JsonConfigFile.ino for reading config file !!!
#ifndef SD_CARD_H
#define SD_CARD_H
#include "Ublox.h"
#include "GPS_data.h"
#include <SD.h>
#include "ArduinoJson.h"
 
extern struct tm tmstruct ;
extern int Time_Set_OK;
extern int first_fix_GPS;
extern byte checksumA;
extern byte checksumB;
extern int sdTrouble;
extern bool sdOK,button;
extern bool logCSV;
extern bool logUBX;
extern bool logOAO;
extern bool GPS_logging;
extern float Mean_heading,heading_SD;
extern float calibration_bat;
extern float calibration_speed;
extern int time_out_nav_pvt;
extern byte mac[6];
extern char SW_version[32];
extern GPS_speed M100;
extern GPS_speed M250;
extern GPS_speed M1852;
extern GPS_time S2;
extern GPS_time S10;
//extern GPS_time S600;
extern Alfa_speed A250;
extern GPS_data Ublox; // create an object storing GPS_data, definition in RTOS
extern int nav_pvt_message_nr; 

struct Config {
  float cal_bat=1.74;
  float cal_speed=3.6;
  int sample_rate=1;//gps_rate in Hz, 1, 5 or 10Hz !!!
  int field=1;//choice for first field in speed screen !!!
  int dynamic_model=1;//choice for dynamic model "Sea",if 0 model "portable" is used !!
  int timezone=2;//choice for timedifference in hours with UTC, for Belgium 1 or 2 (summertime)
  int stat_field=6;//choice for stats field when no speed
  bool logCSV=0;
  bool logUBX=1;
  bool logOAO=1;
  char UBXfile[32]="/BN220A000.ubx";
  char ssid[32];
  char password[32];
  } ;
extern Config config;
extern void AddString();
void logERR( const char * message);
void Open_files(void);
void Close_files(void);
void Flush_files(void);
void Log_to_SD(void); 
void loadConfiguration(const char *filename, Config &config) ;
void Model_info(int model);
void printFile(const char *filename);
void Session_info(GPS_data G);
void Session_results_M(GPS_speed M);
void Session_results_S(GPS_time S);
void Session_results_Alfa(Alfa_speed A,GPS_speed M);
void log_header_OAO(void);
void log_OAO(void);
#endif
