#include <SD.h>
#include <sd_defines.h>
#include <sd_diskio.h>

#include <ETH.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WiFiGeneric.h>
#include <WiFiServer.h>
#include <WiFiSTA.h>
#include <WiFiType.h>
#include <WiFiUdp.h>
//#include <HTTPClient.h>//takes 120 kb flash !!!

/*
 * Changes 2/5/2021
 * Alfa calculation with distance smaller<500 m
 * Only logging with speed>1m/s, has influence on calculation speed due
 * my algorithm does not check for lost data points !!!
 * Changes 30/7/2021 sw 4.52
 * Check for time out gps ubx nav msg, after 1.2s restart gps not implemented !!
 * Changes 9/8/2021 sw 4.53 : buffering nav pvt sentences, only writing after 10 messages, not implemented !!
 * CPU freq from 180Mhz to 240 Mhz
 * Set rate ublox bug correction (slow startup)
 * Files stay open after writing, every minute flush, close files by switching off
 * Increase buffer size of Serial2, from 256 to 1024 bytes
 * Changes 12/8/2021 sw 4.54
 * SD_card.CPP function append_file() / read_file()dismissed !!
 * config.field for choosing field1 from speed screen
 * long push for deepsleep 2s -> 1s
 * Update avg during run
 * Add 18Hz sample rate, only possible with GNSS  ??
 * Changes 15/8/2021 sw 4.55
 * Bug fix calculation avg, runs were counted double !!!
 * Field choice now with short push
 * Changes sw 4.56
 * Field choice anti dender
 * Screen change while searching for SSID
 * Best 2s update on the fly
 * Slowest 10s run instead 100m
 * Soft_ap and station mode @ startup
 * If SSID not found, SoftAP active for 100s after reed switch
 * If connected to SoftAP, ftp is possible to change config.txt !!!
 * lib FTPserverJH change, chosing IP depends station or ap
 * Changes sw 4.57
 * Fix bug calaculation avg
 * Fix bug calculation fastest run on the fly
 * Add T5 mac nr to error file + ubx file
 * Try to read unique id from ublox, no success because works only with M8N version 3.0
 * When softAP, station mode is switched off
 * SW-version declared as char array
 * Changes sw 4.58
 * display_max_speed for S1800 fix
 * knots window 1.943 - 1.944
 * sorting speedruns only if new max during this run !!!
 * mac adress added to filenames 
 * no mac adress in ubx file anymore
 * Changes sw 4.59
 * New run detection minimal heading change 40° to 50° 
 * New run delay of 10s after heading change
 * Cleanup Speed update function (sAcc removed)
 * Changes sw 4.60
 * Added 1800S/3600S best speed to the speed screen, speed_field 6 en 7
 * Faster update screen stat1 / stat2
 * statscreen2 : toggling between 1800s / 3600s
 * Changes sw 4.61
 * Added cal_speed in session results
 * Longer view stat2 screen
 * Added timezone to config.txt in hours (+2 for Belgium):"timezone":2,
 * Added time field, toggle between alfa and time
 * Change dynamic model ublox m8n to sea in init
 * Removed parameter "5000" with setting time
 * bug in ftpserver or WinSCP : file time is +2h ??
 * Changes sw 4.62
 * Dynamic model can be choosen from config.txt file, standard = Sea
 * dynamic_model:0-> portable
 * dynamic_model:1-> sea (this is default !)
 * Added choosen dynamic model to results
 * Added distance for M100/M1852... to results
 * Changes sw 5.0
 * Automatic switching to portable with overspeed (>20 m/s)
 * Extra dynamic model Automotive : 2-> automotive
 * for higher accuracy, change of datatypes to double
 * All speeds are now in mm/s (format double)
 * Added STATS3 screen with M100/M500, config.Stat_screens 6
 * Added Rtos5.h for general #define
 * Bugfix in new_run_detection, speed m/s -> mm/s
 * Start logging if speed>MIN_SPEED_START_LOGGING, now 2m/s
 * Added log files in oao format
 * Removed logging to csv files
 * Changes sw 5.1
 * Add distance of run
 * Add configurable bar in "speed" screen, reflects run_distance, default 1852m = full bar
 * Changes sw 5.2
 * Change to proportional font 12pt and 18pt, speed screen now has 2 variables on top
 * Replace sat nr in speed screen with other variable
 * Add a alfa indicator, the actual distance from the start point is shown, has to be < 50m
 * At the same time shows the bar the alfa distance, 250 m = full bar
 * If config.field=1: 
 *        automatic switching to alfa screen after jibe until 300 m 
 *        then Run /. AVG screen.
 * If config.field=2:        
 *        if run distance>1852 m, switch to NM view
 *        else  Run screen !!       
 * Changes sw 5.3       
 * No "window" anymore for alfa, needs further development
 * Some display changes
 * This version goes to GP3S
 * Changes sw 5.4
 * Wifi stays on for 100s, first 10s searching for SSID, then still 90s AP actif if reed switch seen.
 * Station or AP, but not both at the same time !
 * Wifi screen stays as long as connection SSID or AP, no SAT nr in Wifi screen as GPS is not read
 * Changed esp32 lib from 1.0.4 to 1.0.6
 * Changes to ESP32FTP lib, so Android FTP is now possible
 * FTP status visible in Wifi screen, FTP = 2 : waiting for connection, FTP 5 : Connection OK
 * Changes sw 5.42
 * Removed CR in FTP lib for directory listing, as FTP has a problem with MacOS
 * changes sw 5.44
 * Several changes in FTP lib for problem with MacOS
 * Bugfix for e-paper lib BN74
 * Optimization Alfa screen, Window...
 * Test for 1.54" e-paper
 * changes sw 5.45
 * exit distance for alfa = estimation of alfa-circle with current heading
 * changes sw 5.46
 * GPIO 25/26 are RTC, drive capability changed!! Were not working !!
 * extra Button GPIO_Pin12, pullup, give STAT4 screen !!
 * long push GPIO_Pin12 reset STAT4 screen
 * short push toggles next screen
 * changes sw 5.47
 * NM bar start@minimal speed (10 m/s ?)
 * More stats screen over GPIO pin 12
 * Alfa speed screen with Window, avg speed 250m and Exit
 * Sea model does not give a gps-fix if actual height is not on sea-level, default = 0 !!!
 * wifi off screen nr of sats added
 * bugfix bat symbol if bat_voltage>4.2
 * Choice of stat_screens extended : preferred screens are chosen with their nr : Stat_screens = 523 is then stat_screen 5, 2 and 3 !!!
 * changes sw 5.48
 * Added Simon screens
 * RTC offset for sleep screen 
 * RTC txt for sleep screen
 * sleep_screen "speed in km/h" or "knots" depends on calibration
 * nr of stat screens are changed (e_paper.h)
 * low_bat voltage 3.1 -> go to sleep !!!
 * update STAT4 with actual run, added extra display_speed[]
 * choice GPIO12 screens with config
 * choice Logos with config
 * choice sleep-screen, off-screen with config
 * json doc 512 -> 1024 byte
 * boot screen added config fail / ok
 * SW 5.49
 * Alfa calculation , alfa-circle² to avoid sqrt in calculations, factor 111120 -> 111195
 * Bugfix in alfa calculation (pointer + 1)
 * Shutdown : RTC data only written if new GPS-data is available
 * SW 5.50
 * Added bar graph screen which shows the speed from al the runs (max=42)
 * Scale  bar value depends on fastest run, min value = 24
 * Speed km/h or knots
 * SW 5.51
 * new run detection, min speed of 5 m/s
 * max nr of stat screens change to 9
 * bugfix Log_to_SD() before set speed to 0 with bad reception, checksum ubx was not correct with speed 0 !!!
 * Added config.gnss : set for 3 gnss possible, works only with ublox ROM 3.0 !
 * Check for GNSS setting : ubx msg MON_GNSS
 * Add setting GNSS in txt file @ end of session
 * SW 5.52
 * bugfix for bar graph 
 * SW 5.53
 * bugfix for filenaming if logUBX=0 !!!
 * SW 5.54
 * Added extra Board / Sail logo's, design by Hans Scholten
 * Added variable to config.txt : "stat_speed":2,  // speed less then 2 m/s -> stat screens
 * Filenr depends now from existing .txt files !!
 * Shut down screen : add type of E-paper (in case you forgot...)
 * Webserver can now download and delete files from the SD-card !!! 
 * Contribution from Triton_dm on github !!
 * SW5.55
 * Bugfix for choice screens with GPIO39 after boot
 * SW5.56 (Contribution from Triton_dm on github !!)
 * Added an upload section to the webserver
 * Enabled the webserver on AP Usage, prevent from stopping during downloading files
 * Add IP Address of the AP to display
 * Add a special LOGO section > 99 and add a special logo of a beer supplier hier @Achensee/Tirol
 * Change the Webinterface to a responsive design
 * prevent delete on one click
 * SW5.57 (JH)
 * Added file timestamp to webserver !!
 * SW5.58 Changes by tritondm !!
 * fix the download issue with phps extionsion on android chrome
 * add a config section for changing config - thats the base for the next step - config credentials for gps-speedsurfing.com and upload
 * after before upload a config_backup will created
 * SW5.60 Bugfixes + added sbp file format
 * Changed next file for compiling with Arduino IDE 2.02 (SD(esp32) to SD)
 * C:\Users\andre\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.6\libraries\SD\library.properties
 * Bugfix decoding config logo, GPIO12 and stat_screen
 * Range bar_length now from 100 - 10000
 * Removed fileformat .oao
 * Added new fileformat .sbp
 * SW5.61 First draft new open source fileformat
 * Some minor changes for the webserver (triton_dm)
 * Separate Board_logo and Sail_logo config
 * Test with other fonts in speed screen, added "speed_large_font" to the config : Run, Alfa and NM can have a bigger font.
 * Changed order config.field, 1=Auto between Run, Alfa & NM, 2=Run & NM, 3=Alfa, 4=NM, 5= Total distance, 6= 2s/10s, 7= 0.5h, 8= 1h
 * Font 46pt_nr and 84pt_nr added, condensed format only digits and decimal point to save memory 
 * Added Github link to main menu
 * SW5.62
 * Added GPX file format
 * Added GPY file format
 * If no config file : Wifi AP waiting time = 120 s
 * Bugfix config webserver : no switching to km/h if knots was set
 * SW5.63
 * Added support for BN266 e-paper
 * Changed GPS-parser union to struct, every ubx message has its own struct now
 * Added ubx nav dop Msg for extracting HDOP
 * Added ubx mon ver Msf for extracting sw / hw version ublox
 * Changed baudrate ublox to 38400, necessary for 10Hz navDOP + navPVTD
 * Correction timestamp list files with FTP 
 * Added sail logo "Severne" = 10
 * Added board logo "F2" = 10
 * Bugfix for SSID with white space
 * Added watchtdog for WAKE UP screen (hangs sometimes after OTA)
 * Added loadbalance for flushfiles()
 * SW5.64
 * Bugfix garbage frame in gpy / ubx
 * Reboot ESP : ublox_off for 1s !!
 * Added support for ublox M10 (Mateksys maxM10s), define in Rtos.h
 * Time stat screen toggle configurable
 * bdrate back to 19200BD, should be ok for 10*100 (nav_pvt) + 10*26 bytes(nav_dop)
 * Added support for GNSS GPS + GALILEO + BEIDOU for M8 + M10
 * Correction time on sleep-screen with timezone
 * SW5.65
 * Bugfix stat screen time
 * SW 5.66
 * Added SW-version + e-paper type to Wifi-on screen + sleep screen Simon
 * baud rate back to 38400 for nav sat msg
 * Added (configurable) ubx nav sat message to ubx file, Rate = 1/10 of NAV PVT, timeshift 18.05 s for ascending order ubx msg
 * This is if you  want to evaluate the signal quality of your gps (ucenter)
 * Filename with Timestamp in it, configurable
 * .sbp file corrections, problem with uploading to gp3s, header change (0xa0, 0xa2)
 * .sbp file correction for sAcc, hDOP
 * added alfa screen to 0.5h / 1h, bar_length gives time passed 30min /60 min
 * sleep_screen Simon : 500m -> 1h best
 * SW 5.67
 * NAV SAT not with polling, but over cfg msg @ 10% rate of NAV PVT
 * NAV SAT for M10 added @ 10% rate of NAV PVT
 * removed timeshift iTow NAV SAT
 * Added extra option filenaming
 * Some changes to header SBP
 * correction time bug in sleep screen
 * reset 30 min / 60 min bar if speed drops<2m/s for 120s (screen best 0.5h / 1h)
 * SW 5.68
 * Analog bat measurement to function()
 * ESP32 time is now local time, min 5 sats visible !!
 * Add check for nav pvt valid flags time&date before setting the time (thanks Peter!)
 * Check for plausible year when GPS time is set (>=2023)
 * Removed timelib.h, only sys.time.h needed
 * Fix time_bug in different screens
 * Add Ubx ID M8(ROM3 necessary,5 byte ID), saved in error.txt 
 * Add Ubx ID M10(6 byte ID), saved in error.txt 
 * Add Class GPS_NAV_INFO + Evaluation last 16 NAV_SAT msg in log file error.txt
 * Add navDOP msg to ubx file, if nav_Sat is active (for analysing data) 
 * SW 5.69
 * Change baudrate for M10 to 38400 bd, #define ALI_M10 in rtos5.h
 * Bugfix for gpy file : MakeTime -> mktime, offset from 1970 -> 1900, month -1 !!!
 * Option for other wake-up GPIO with #define in rtos5.h
 * Add type e-paper to .txt file
 * Some text in e-paper adapted : Mile ->NM, Alf->Alph, Dis->Dist
 * SW 5.70
 * Again timebug -> e-paper keyword "now" may not be used for getLocalTime() !!
 * Same issue in SD_card.cpp -> open files !!
 * GPS_SAT_INFO buffer 16 -> 10
 * Set GPS_time @ start logging
 * check for year>2023
 * While waiting for Sats / Min speed : Wifi Off with waiting Sats/Speed on screen
 * Total time in .Txt from start logging
 * SW 5.71
 * Again bugfix for time in gpy file (offset year 1970->1900, month->-1)
 * Removed #include timelib.h
 * Removed listing of directorys in webserver
 * Add sortTable() after DOM is loaded : now the files are ordered by date after loading all filenames !
 * Timestamp files with FTP command MLSD is now UTC
 * Add type of ublox to .txt file + e-paper shut down
 * https://www.mischianti.org/2021/03/28/how-to-use-sd-card-with-esp32-2/
 * SW5.72
 * .txt files configurable over webserver, if name_MAC_counter, .txt file is necessary for keeping track counter !!!
 * When Wifi, NTP time is set, if needed, directory "Archive" is created
 * When webserver start, Files older then xx days are copied to directory "Archive", are not visble in the listing anymore.
 * Added link "Archive files" in header : to move files to the Archive dir when older then archive_days
 * Added link "Archive list" in header : to list all the files on the SD, dir "Archive" included
 * Link to overview Board / Sail logo updated
 * STAT1 screen Sat -> last 2s, 3m/s for new value needed
 * STAT6 screen Mile -> NM
 * Sleep screen bat voltage -> bat percent, 4.2=100%, 3.4=0% 
 */
#include "FS.h"
#include "SPI.h"
#include "sys/time.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include "Ublox.h"
#include "E_paper.h"
#include "SD_card.h"
#include "GPS_data.h"
#include "Arduino.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include "ESP32FtpServerJH.h"
#include "OTA_server.h" 
#include <esp_task_wdt.h>
#include <driver/rtc_io.h>
#include <driver/gpio.h>
#include <lwip/apps/sntp.h>
#include <esp32-hal.h>
#include <time.h>
#include "Rtos5.h"

#define SPI_MOSI 23
#define SPI_MISO -1//standaard is MISO GPIO 19 !!!!
#define SPI_CLK 18

#define SDCARD_SS 13
#define SDCARD_CLK 14
#define SDCARD_MOSI 15
#define SDCARD_MISO 2
// Connect the GPS RX/TX to arduino pins 32 and 33
#define RXD2 32 //geel is Tx Ublox, Beitian wit is Tx
#define TXD2 33 //groen is Rx Ublox, Beitian groen is Rx
#define PIN_BAT 35
#define CALIBRATION_BAT_V 1.75 //voor proto 1
#define uS_TO_S_FACTOR 1000000UL /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  4000        /* Time ESP32 will go to sleep (in seconds, max is ?) */
#define WDT_TIMEOUT 60             //60 seconds WDT, opgelet zoeken naar ssid time-out<dan 10s !!!

#define MIN_numSV_FIRST_FIX 5     //alvorens start loggen, changed from 4 to 5 7.1/2023
#define MAX_Sacc_FIRST_FIX 2     //alvorens start loggen
#define MIN_numSV_GPS_SPEED_OK 4  //min aantal satellieten voor berekenen snelheid, anders 
#define MAX_Sacc_GPS_SPEED_OK 1   //max waarde Sacc voor berekenen snelheid, anders 0
#define MAX_GPS_SPEED_OK 40       //max snelheid in m/s voor berekenen snelheid, anders 0

String IP_adress="0.0.0.0";
const char SW_version[16]="SW-ver. 5.72";//Hier staat de software versie !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#if defined(_GxGDEH0213B73_H_) 
const char E_paper_version[16]="E-paper 213B73";
#endif
#if defined(_GxDEPG0213BN_H_) 
const char E_paper_version[16]="E-paper 213BN";
#endif
#if defined(_GxGDEM0213B74_H_) 
const char E_paper_version[16]="E-paper 213B74";
#endif
#if defined(UBLOX_M10) 
const char Ublox_type[20]="Ublox M10 9600bd";
#elif defined(ALI_M10) 
const char Ublox_type[20]="Ublox M10 38400bd";
#else 
const char Ublox_type[20]="Ublox M8 9600bd";
#endif
int sdTrouble=0;
bool sdOK = false;
bool button = false;
bool reed = false;
bool deep_sleep = false;
bool Wifi_on=true;
bool SoftAP_connection = false;
bool GPS_Signal_OK = false;
bool long_push = false;
bool Field_choice = false;
bool NTP_time_set = false;

int analog_bat;
int first_fix_GPS,run_count,old_run_count,stat_count,GPS_delay;
int start_logging_millis;
int wifi_search=10;//was 10
int ftpStatus=0;
int time_out_nav_pvt=1200;
int nav_pvt_message=0;
int old_message=0;
int nav_sat_message=0;
int next_gpy_full_frame=0;
int nav_pvt_message_nr=0;
int msgType;
int stat_screen=5;//keuze stat scherm indien stilstand
int GPIO12_screen=0;//keuze welk scherm
int low_bat_count;
int gps_speed;
float alfa_window;
float analog_mean=2000;
float Mean_heading,heading_SD;

byte mac[6];  //unique mac adress of esp32
IPAddress local_IP(192,168,4,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

RTC_DATA_ATTR float calibration_speed=3.6;
RTC_DATA_ATTR int offset = 0;
RTC_DATA_ATTR float RTC_distance;
RTC_DATA_ATTR float RTC_avg_10s;
RTC_DATA_ATTR float RTC_max_2s;
//Simon
RTC_DATA_ATTR short RTC_year;
RTC_DATA_ATTR short RTC_month;
RTC_DATA_ATTR short RTC_day;
RTC_DATA_ATTR short RTC_hour;
RTC_DATA_ATTR short RTC_min;
RTC_DATA_ATTR float RTC_alp;
RTC_DATA_ATTR float RTC_500m;
RTC_DATA_ATTR float RTC_1h;
RTC_DATA_ATTR float RTC_mile;
RTC_DATA_ATTR float RTC_R1_10s;
RTC_DATA_ATTR float RTC_R2_10s;
RTC_DATA_ATTR float RTC_R3_10s;
RTC_DATA_ATTR float RTC_R4_10s;
RTC_DATA_ATTR float RTC_R5_10s;
RTC_DATA_ATTR char RTC_Sleep_txt[32]="Your ID";
RTC_DATA_ATTR int RTC_Board_Logo;
RTC_DATA_ATTR int RTC_Sail_Logo;
RTC_DATA_ATTR int RTC_SLEEP_screen=0;
RTC_DATA_ATTR int RTC_OFF_screen=0;
//Simon
RTC_DATA_ATTR float calibration_bat=1.75;//bij ontwaken uit deepsleep niet noodzakelijk config file lezen
RTC_DATA_ATTR float RTC_voltage_bat=3.6;
FtpServer ftpSrv;  
GPS_data Ublox; // create an object storing GPS_data !
GPS_SAT_info Ublox_Sat;//create an object storing GPS_SAT info !
GPS_speed M100(100);
GPS_speed M250(250);
GPS_speed M500(500);
GPS_speed M1852(1852);
GPS_time S2(2);
GPS_time s2(2);
GPS_time S10(10);
GPS_time s10(10);//for  stats GPIO_12 screens, reset possible !!
GPS_time S1800(1800);
GPS_time S3600(3600);
Alfa_speed A250(50);
Alfa_speed A500(50);
Alfa_speed a500(50);//for  Alfa stats GPIO_12 screens, reset possible !!
Button_push Short_push12 (12,100,15,1);//GPIO pin 12 is nog vrij, button_count 0 en 1 !!!
Button_push Long_push12 (12,2000,10,4);
Button_push Short_push39 (WAKE_UP_GPIO,100,10,8);//was 39
Button_push Long_push39 (WAKE_UP_GPIO,1500,10,8);//was 39

#if defined(_GxDEPG0266BN_H_) //only for screen BN266, Rolzz... !!!
GxIO_Class io(SPI, /*CS=5*/ ELINK_SS, /*DC=*/ 19, /*RST=*/4);
GxEPD_Class display(io, /*RST=*/4, /*BUSY=*/34);
#else
GxIO_Class io(SPI, /*CS=5*/ ELINK_SS, /*DC=*/ ELINK_DC, /*RST=*/ ELINK_RESET);
GxEPD_Class display(io, /*RST=*/ ELINK_RESET, /*BUSY=*/ ELINK_BUSY);
#endif

SPIClass sdSPI(VSPI);

const char *filename = "/config.txt";
const char *filename_backup = "/config_backup.txt"; 
//Config config;  
  /*
Method to print the reason by which ESP32 has been awaken from sleep
*/
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    RTC_voltage_bat=analog_mean*calibration_bat/1000;
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); reed=1;
                                 rtc_gpio_deinit(GPIO_NUM_xx);//was 39
                                 esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
                                 Boot_screen();
                                 break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); 
                                 break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer");
                                  analog_mean = analogRead(PIN_BAT);
                                  for(int i=0;i<10;i++){
                                        Update_bat();
                                        //analog_bat = analogRead(PIN_BAT);
                                        //analog_mean=analog_bat*0.1+analog_mean*0.9;
                                        }
                                  //RTC_voltage_bat=analog_mean*calibration_bat/1000;
                                  esp_sleep_enable_ext0_wakeup(GPIO_NUM_xx,0); //was 39  1 = High, 0 = Low
                                  Sleep_screen(RTC_SLEEP_screen);
                                  go_to_sleep(3000);//was 4000
                                  break;                               
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); 
                                     break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); 
                                break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); 
              Boot_screen();
              break;
    }
}

void go_to_sleep(int sleep_time){
  deep_sleep=true;
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  Ublox_off();
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);//flash in deepsleep, CS stays HIGH!!
  gpio_deep_sleep_hold_en();
  esp_sleep_enable_timer_wakeup( uS_TO_S_FACTOR*sleep_time);
  Serial.println("Setup ESP32 to sleep for every " + String(sleep_time) + " Seconds");
  Serial.println("Going to sleep now");
  Serial.flush(); 
  delay(3000);
  esp_deep_sleep(uS_TO_S_FACTOR*sleep_time);
}

void Shut_down(void){
        long_push=true;
        Ublox_off();
        GPS_Signal_OK=false;
        GPS_delay=0;
        if(Time_Set_OK){    //Only safe to RTC memory if new GPS data is available !!
            Time_Set_OK=false;
            RTC_distance=Ublox.total_distance/1000000;
            RTC_alp=A500.display_max_speed*calibration_speed;
            RTC_500m=M500.avg_speed[9]*calibration_speed;
            RTC_1h=S3600.display_max_speed*calibration_speed;            
            RTC_mile=M1852.display_max_speed*calibration_speed;
            RTC_max_2s= S2.avg_speed[9]*calibration_speed;
            RTC_avg_10s=S10.avg_5runs*calibration_speed;
            RTC_R1_10s=S10.avg_speed[9]*calibration_speed;
            RTC_R2_10s=S10.avg_speed[8]*calibration_speed;
            RTC_R3_10s=S10.avg_speed[7]*calibration_speed;
            RTC_R4_10s=S10.avg_speed[6]*calibration_speed;
            RTC_R5_10s=S10.avg_speed[5]*calibration_speed;
            if(config.logTXT){
              Session_info(Ublox);
              Session_results_S(S2);
              Session_results_S(S10);
              Session_results_S(S1800);
              Session_results_S(S3600);
              Session_results_M(M100);
              Session_results_M(M500);
              Session_results_M(M1852);
              Session_results_Alfa(A250,M250);
              Session_results_Alfa(A500,M500);
              }
            delay(100);
            Close_files(); 
            RTC_year=(tmstruct.tm_year+1900);//local time is corrected with timezone in close_files() !!
            RTC_month=(tmstruct.tm_mon+1);
            RTC_day=(tmstruct.tm_mday);
            RTC_hour=(tmstruct.tm_hour);
            RTC_min=(tmstruct.tm_min);
            }
        go_to_sleep(10);//got to sleep na 10s     
}
void Update_bat(void){
    analog_bat = analogRead(PIN_BAT);
    analog_mean=analog_bat*0.1+analog_mean*0.9;
    RTC_voltage_bat=analog_mean*calibration_bat/1000;
}
void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.print("NTP Time = ");
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}  
//For RTOS, the watchdog has to be triggered
void feedTheDog_Task0(){
  TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE; // write enable
  TIMERG0.wdt_feed=1;                       // feed dog
  TIMERG0.wdt_wprotect=0;                   // write protect
}
void feedTheDog_Task1(){ 
  TIMERG1.wdt_wprotect=TIMG_WDT_WKEY_VALUE; // write enable
  TIMERG1.wdt_feed=1;                       // feed dog
  TIMERG1.wdt_wprotect=0;                   // write protect
} 

void OnWiFiEvent(WiFiEvent_t event){
  switch (event) {
    case SYSTEM_EVENT_STA_CONNECTED:
      Serial.println("ESP32 Connected to SSID Station mode");
      WiFi.mode(WIFI_MODE_STA);//switch off softAP
      Serial.println("ESP32 Soft AP switched off");
    case SYSTEM_EVENT_STA_GOT_IP://  @this event no IP !!!         SYSTEM_EVENT_STA_CONNECTED:
      Serial.println("ESP32 Connected to WiFi Network");
      IP_adress =  WiFi.localIP().toString();
      break;
    case SYSTEM_EVENT_AP_START:
      WiFi.softAPConfig(local_IP, gateway, subnet);  
      Serial.println("ESP32 soft AP started");
      break;
    case SYSTEM_EVENT_AP_STACONNECTED:
      Serial.println("Station connected to ESP32 soft AP");
      IP_adress =  WiFi.softAPIP().toString();
      SoftAP_connection=true;
      break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
      Serial.println("Station disconnected from ESP32 soft AP");
      SoftAP_connection=false;
      break;
    default: break;
  }
}
static void setTimeZone(long offset, int daylight)
{
    char cst[17] = {0};
    char cdt[17] = "DST";
    char tz[33] = {0};
    if(offset % 3600){
        sprintf(cst, "UTC%ld:%02u:%02u", offset / 3600, abs((offset % 3600) / 60), abs(offset % 60));
    } else {
        sprintf(cst, "UTC%ld", offset / 3600);
    }
    if(daylight != 3600){
        long tz_dst = offset - daylight;
        if(tz_dst % 3600){
            sprintf(cdt, "DST%ld:%02u:%02u", tz_dst / 3600, abs((tz_dst % 3600) / 60), abs(tz_dst % 60));
        } else {
            sprintf(cdt, "DST%ld", tz_dst / 3600);
        }
    }
    sprintf(tz, "%s%s", cst, cdt);
    Serial.println(tz);
    setenv("TZ", tz, 1);
    tzset();
}
void setup() {
  Serial.println("Configuring WDT...");
  esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL); //add current thread to WDT watch
  Serial.begin(115200);
  Serial.println("setup Serial");
  Serial.println("Serial Txd is on pin: "+String(TX));
  Serial.println("Serial Rxd is on pin: "+String(RX));
  
  SPI.begin(SPI_CLK, SPI_MISO, SPI_MOSI, ELINK_SS); //SPI is used for SD-card and for E_paper display !
  print_wakeup_reason(); //Print the wakeup reason for ESP32, go back to sleep is timer is wake-up source !
 //sometimes after OTA hangs here ???
  pinMode(25, OUTPUT);//Power beitian //default drive strength 2, only 2.7V @ ublox gps
  pinMode(26, OUTPUT);//Power beitian
  pinMode(27, OUTPUT);//Power beitian
  rtc_gpio_set_drive_capability(GPIO_NUM_25,GPIO_DRIVE_CAP_3);//see https://www.esp32.com/viewtopic.php?t=5840
  rtc_gpio_set_drive_capability(GPIO_NUM_26,GPIO_DRIVE_CAP_3);//3.0V @ ublox gps current 50 mA
  gpio_set_drive_capability(GPIO_NUM_27,GPIO_DRIVE_CAP_3);//rtc_gpio_ necessary, if not no output on RTC_pins 25 en 26, 13/3/2022
  
  Ublox_on();//beitian bn220 power supply over output 25,26,27
  Serial2.setRxBufferSize(1024); // increasing buffer size ?
  #if defined(ALI_M10)
  Serial2.begin(38400, SERIAL_8N1, RXD2, TXD2); //connection to ublox over serial2
  Serial.println("Serial2 38400bd Txd is on pin: "+String(TXD2));
  Serial.println("Serial2 38400bd Rxd is on pin: "+String(RXD2));
  #else
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2); //connection to ublox over serial2
  Serial.println("Serial2 9600bd Txd is on pin: "+String(TXD2));
  Serial.println("Serial2 9600bd Rxd is on pin: "+String(RXD2));
  #endif
  for(int i=0;i<425;i++){//Startup string van ublox to serial, ca 424 char !!
     while (Serial2.available()) {
              Serial.print(char(Serial2.read()));
              }
     delay(2);   //was delay (1)
     }
  //SPIClass sdSPI = SPIClass(HSPI);  //test JH 25/12/2022
  sdSPI.begin(SDCARD_CLK, SDCARD_MISO, SDCARD_MOSI, SDCARD_SS);//default 20 MHz gezet worden !

  struct timeval tv = { .tv_sec =  0, .tv_usec = 0 };
  settimeofday(&tv, NULL);
  if (!SD.begin(SDCARD_SS, sdSPI)) {//test for faster writing, JH 25/12/2022
        sdOK = false;Serial.println("No SDCard found!");
        } 
        else {
        sdOK = true;Serial.println("SDCard found!");
        uint64_t cardSize = SD.cardSize() / (1024 * 1024);
        Serial.printf("SD Card Size: %lluMB\n", cardSize); 
        Serial.println(F("Loading configuration..."));// Should load default config 
        loadConfiguration(filename, filename_backup, config); // load config file
        Serial.print(F("Print config file...")); 
        printFile(filename); 
        } 
  #if !defined(UBLOX_M10)     
  Init_ublox(); //switch to ubx protocol
  #else
  Init_ubloxM10(); //switch to ubx protocol
  #endif
  Serial.print("SW Ublox=");
  Serial.println(ubxMessage.monVER.swVersion);
  Serial.print ("HW Ublox=");
  Serial.println (ubxMessage.monVER.hwVersion);
  Serial.print ("Extensions Ublox= ");
  for(int i=0;i<6;i++){
    Serial.print (ubxMessage.monVER.ext[i].extension);
    Serial.print (", ");
    }
  Serial.println();  
  Serial.println (ubxMessage.monGNSS.default_Gnss);
  Serial.println (ubxMessage.monGNSS.enabled_Gnss);
  #if !defined(UBLOX_M10)
  Set_rate_ublox(config.sample_rate);//after reading config file !!  
  #else
  Set_rate_ubloxM10(config.sample_rate);//after reading config file !!  
  #endif  
  Update_screen(BOOT_SCREEN);
  
  const char* ssid = config.ssid; //WiFi SSID
  const char* password = config.password; //WiFi Password
  const char *soft_ap_ssid = "ESP32AP"; //accespoint ssid
  const char *soft_ap_password = "password"; //accespoint password
    
  WiFi.onEvent(OnWiFiEvent);
  WiFi.mode(WIFI_STA);
  WiFi.softAP(soft_ap_ssid, soft_ap_password);
  WiFi.begin(ssid, password);
  Serial.print("T5 MAC adress: ");
  WiFi.macAddress(mac);
 
  // Wait for connection during 10s in station mode
  bool ota_notrunning=true;
  while ((WiFi.status() != WL_CONNECTED)&(SoftAP_connection==false)) {
    esp_task_wdt_reset();
    server.handleClient(); // wait for client handle, and update BAT Status, this section should be moved to the loop... 
    Update_bat();         //client counter wait until download is finished to prevent stoping the server during download
    if(digitalRead(WAKE_UP_GPIO)==false){//switch over to AP-mode, search time 100 s
       if(ota_notrunning){
            WiFi.disconnect();
            WiFi.mode(WIFI_AP);
            WiFi.softAP(soft_ap_ssid, soft_ap_password); 
            wifi_search=100;
            IP_adress =  WiFi.softAPIP().toString();
            Serial.println(IP_adress);
            Serial.println("start OTA Server");
            OTA_setup(); //start webserver and ota - for use on AP Mode
            ota_notrunning=false;
          }else{
            wifi_search=100;
            Serial.println("Set AP Counter to 100");
          }
        }
    if(wifi_search<=10) Update_screen(WIFI_STATION);
    else Update_screen(WIFI_SOFT_AP);
    //delay(250);
    Serial.print(".");
    wifi_search--;
    if(wifi_search<=0){
      server.close();
      IP_adress = "0.0.0.0";
      break;
      }
    }
  if((WiFi.status()== WL_CONNECTED)|SoftAP_connection){
      Serial.println("");
      Serial.print("Connected to ");
      Serial.println(ssid);
      Serial.print("IP address: ");
      Serial.println(IP_adress);
      Wifi_on=true;
      ftpSrv.begin("esp32","esp32");    //username, password for ftp
      OTA_setup();  //start webserver for over the air update
      }
  else{
      Serial.println("No Wifi connection !");
      WiFi.disconnect(true);
      WiFi.mode(WIFI_OFF);
      Wifi_on=false;      
  }
  //analog_mean=RTC_voltage_bat/calibration_bat*1000;//RTC_voltage_bat staat in RTC mem !!!
  delay(100);
 
   //Create RTOS task, so logging and e-paper update are separated (update e-paper is blocking, 800 ms !!)
  xTaskCreate(
                    taskOne,          /* Task function. */
                    "TaskOne",        /* String with name of task. */
                    10000,            /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */
 
  xTaskCreate(
                    taskTwo,          /* Task function. */
                    "TaskTwo",        /* String with name of task. */
                    10000,            /* Stack size in bytes was 10000, but stack overflow on task 2 ?????? now 20000. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */
}
 
void loop() {  
  feedTheDog_Task0();
  feedTheDog_Task1();
  esp_task_wdt_reset();
  Update_bat();
  delay(100);  
}

void taskOne( void * parameter )
{
 while(true){ 
   if (Short_push12.Button_pushed())GPIO12_screen++;//toggle screen
   if (GPIO12_screen>config.gpio12_count)GPIO12_screen=0;
   if (Long_push12.Button_pushed()){ s10.Reset_stats(); s2.Reset_stats();a500.Reset_stats();} //resetten stats   
     
   if(Long_push39.Button_pushed()) Shut_down();
   if (Short_push39.Button_pushed()){
      config.field=Short_push39.button_count;
      }
   Field_choice=Short_push39.long_pulse;//10s wachttijd voor menu field keuze....//bug sw 5.54 !!
   
   if((WiFi.status() != WL_CONNECTED)&(Wifi_on==true)&(SoftAP_connection==false)){
        Serial.println("No Wifi connection !");
        server.close();
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        Wifi_on=false; 
        sntp_stop();//no messing around with time synchronization anymore, EPOCH time is set to 0 !!! 
        setTimeZone(0,0);//set TZ to UTC, as mktime returns localtime !!!!
        printLocalTime();
        NTP_time_set=false;
       }   
   if((WiFi.status() == WL_CONNECTED)|SoftAP_connection){
        ftpSrv.handleFTP();        //make sure in loop you call handleFTP()!! 
        server.handleClient();
        ftpStatus=ftpSrv.cmdStatus;//for e-paper
        // Init and get the time if WLAN, create "Archive" dir if not present
        if(WiFi.status() == WL_CONNECTED){
          const char* ntpServer = "pool.ntp.org";
          long  gmtOffset_sec = 3600*config.timezone;//configTime sets Timezone !! mktime use TZ...
          const int daylightOffset_sec = 0;
          if(NTP_time_set==0){
            configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
            setTimeZone(-gmtOffset_sec, daylightOffset_sec);
            printLocalTime();
            //setTimeZone(0,0);
           // printLocalTime();
            NTP_time_set=true;
            }
          if(!SD.exists("/Archive")){
              SD.mkdir("/Archive");
              }
          }
        #if defined (STATIC_DEBUG)
        msgType = processGPS(); //debug purposes
        static int testtime;;
        if((millis()-testtime)>1000){
              testtime=millis();
              Set_GPS_Time(config.timezone);
              config.timezone++;
              }
        #endif
        }
   else msgType = processGPS(); //only decoding if no Wifi connection
          //while (Serial2.available()) {
          //Serial.print(char(Serial2.read()));}  
 if ( msgType == MT_NAV_DOP ) {      //Logging after NAV_DOP, ublox sends first NAV_PVT, and then NAV_DOP.  
        #if defined (STATIC_DEBUG)
        //Serial.print(" DOPiTOW P ");Serial.print (ubxMessage.navPvt.iTOW);
        //Serial.print(" DOPiTOW N ");Serial.println (ubxMessage.navDOP.iTOW);
        #endif     
      
        if((ubxMessage.navPvt.numSV>=MIN_numSV_FIRST_FIX)&((ubxMessage.navPvt.sAcc/1000.0f)<MAX_Sacc_FIRST_FIX)&(ubxMessage.navPvt.valid>=7)&(GPS_Signal_OK==false)){
              GPS_Signal_OK=true;
              first_fix_GPS=millis()/1000;
              }
        if(GPS_Signal_OK==true){
              GPS_delay++;
              }
        if ((Time_Set_OK==false)&(GPS_Signal_OK==true)&(GPS_delay>(TIME_DELAY_FIRST_FIX*config.sample_rate))){//vertraging Time_Set_OK is nu 10 s!!
          int avg_speed=(avg_speed+ubxMessage.navPvt.gSpeed*19)/20; //FIR filter gem. snelheid laatste 20 metingen in mm/s
          if(avg_speed>MIN_SPEED_START_LOGGING){
            if(Set_GPS_Time(config.timezone)){            
                Time_Set_OK=true;
                start_logging_millis=millis();
                Open_files();//only start logging if GPS signal is OK
                }
          }      //    Alleen speed>0 indien snelheid groter is dan 1m/s + sACC<1 + sat<5 + speed>35 m/s !!!
        }
        if ((sdOK==true)&(Time_Set_OK==true)&(nav_pvt_message>10)&(nav_pvt_message!=old_message)){
                  old_message=nav_pvt_message;
                  float sAcc=ubxMessage.navPvt.sAcc/1000;
                  gps_speed=ubxMessage.navPvt.gSpeed; //hier alles naar mm/s !!
                  static int last_flush_time=0;
                  if((millis()-last_flush_time)>60000){
                      Flush_files();
                      last_flush_time=millis();
                      }
                  if((ubxMessage.navPvt.numSV<=MIN_numSV_GPS_SPEED_OK)|((ubxMessage.navPvt.sAcc/1000.0f)>MAX_Sacc_GPS_SPEED_OK)|(ubxMessage.navPvt.gSpeed/1000.0f>MAX_GPS_SPEED_OK)){
                      gps_speed=0;
                      } 
                  Log_to_SD();//hier wordt ook geprint naar serial !!
                  Ublox.push_data(ubxMessage.navPvt.lat/10000000.0f,ubxMessage.navPvt.lon/10000000.0f,gps_speed);   
                  run_count=New_run_detection(ubxMessage.navPvt.heading/100000.0f,S2.avg_s); 
                  alfa_window=Alfa_indicator(M250,M100,ubxMessage.navPvt.heading/100000.0f);
                  if(run_count!=old_run_count)Ublox.run_distance=0;
                  old_run_count=run_count;        
                  M100.Update_distance(run_count);
                  M250.Update_distance(run_count);
                  M500.Update_distance(run_count);
                  M1852.Update_distance(run_count);
                  S2.Update_speed(run_count); 
                  s2.Update_speed(run_count);      
                  S10.Update_speed(run_count);
                  s10.Update_speed(run_count);
                  S1800.Update_speed(run_count);
                  S3600.Update_speed(run_count);
                  A250.Update_Alfa(M250);
                  A500.Update_Alfa(M500);
                  a500.Update_Alfa(M500);
                  }     
      } 
     else if( msgType == MT_NAV_PVT )  { //order messages in ubx should be ascending !!!
          if(Time_Set_OK==true){
                nav_pvt_message++;
                //if(config.logUBX_nav_sat&((nav_pvt_message)%10==0)&(nav_pvt_message>10)){Poll_NAV_SAT();}
                }
          }      
      else if( msgType == MT_NAV_SAT )  { 
          nav_sat_message++;
          Ublox_Sat.push_SAT_info(ubxMessage.navSat);
          #if defined(STATIC_DEBUG_SAT)
              Serial.print ("Mean cno= ");Serial.println (Ublox_Sat.sat_info.Mean_mean_cno);
              Serial.print ("Max cno= ");Serial.println (Ublox_Sat.sat_info.Mean_max_cno);
              Serial.print ("Min cno= ");Serial.println (Ublox_Sat.sat_info.Mean_min_cno);
              Serial.print ("Sats= ");Serial.println (Ublox_Sat.sat_info.Mean_numSV);
              //Serial.print("iTOW ");Serial.print(ubxMessage.navSat.iTOW);
              Serial.print("numSV ");Serial.println(ubxMessage.navSat.numSvs);
              //Serial.print(" len ");Serial.println(ubxMessage.navSat.len);
              
              for(int i=0;i<ubxMessage.navSat.numSvs;i++){
                Serial.print("gnssID "); Serial.print(ubxMessage.navSat.sat[i].gnssId);
                Serial.print("svID "); Serial.print(ubxMessage.navSat.sat[i].svId);
                Serial.print(" cno "); Serial.print(ubxMessage.navSat.sat[i].cno);
                Serial.print(" X4 "); Serial.println(ubxMessage.navSat.sat[i].X4,BIN);
              }
              
          #endif
      }       
  }   
}
 
void taskTwo( void * parameter)
{
  while(true){  
    stat_count++;//ca 1s per screen update
    if (stat_count>config.screen_count)stat_count=0;//screen_count = 2
    Update_bat();
   
    if(RTC_voltage_bat<3.1) low_bat_count++;
    else low_bat_count=0;
    if(long_push==true){
        Off_screen(RTC_OFF_screen);
        Shut_down();
    }
    else if(low_bat_count>10){
        RTC_OFF_screen=1;//Simon screen with info text !!!
        Off_screen(RTC_OFF_screen);
        Shut_down();
    }
    else if(millis()<2000)Update_screen(BOOT_SCREEN);
    else if(GPS_Signal_OK==false) Update_screen(WIFI_ON);
    else if(Time_Set_OK==false) Update_screen(WIFI_ON);
    else if((gps_speed/1000.0f<config.stat_speed)&(Field_choice==false)){
          Update_screen(config.stat_screen[stat_count]);
          }
    else {
          Update_screen(SPEED);
          stat_count=0;
          }
  }
}
