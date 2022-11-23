#include <ETH.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WiFiGeneric.h>
//#include <WiFiMulti.h>
//#include <WiFiScan.h>
#include <WiFiServer.h>
#include <WiFiSTA.h>
#include <WiFiType.h>
#include <WiFiUdp.h>

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
 * Filenr depends now from existing .txt files !! Bug if only log OAO files was active should be ok now...
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
 */
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "TimeLib.h"
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
#define WDT_TIMEOUT 20             //20 seconds WDT, opgelet zoeken naar ssid time-out<dan 10s !!!

#define MIN_numSV_FIRST_FIX 4     //alvorens start loggen
#define MAX_Sacc_FIRST_FIX 2     //alvorens start loggen


#define MIN_numSV_GPS_SPEED_OK 4  //min aantal satellieten voor berekenen snelheid, anders 
#define MAX_Sacc_GPS_SPEED_OK 1   //max waarde Sacc voor berekenen snelheid, anders 0
#define MAX_GPS_SPEED_OK 40       //max snelheid in m/s voor berekenen snelheid, anders 0
//#define MAX_SPEED_DISPLAY_STATS 1 //max snelheid in m/s om stat schermen te zien

String IP_adress="0.0.0.0";

int sdTrouble=0;
bool sdOK = false;
//bool logCSV = false;
bool logUBX = true;
bool logOAO = true;
bool button = false;
bool reed = false;
bool deep_sleep = false;
bool Wifi_on=true;
bool SoftAP_connection = false;
bool GPS_Signal_OK = false;
bool long_push = false;
bool Field_choice = false;

int analog_bat;
int first_fix_GPS,run_count,old_run_count,stat_count,GPS_delay;
int wifi_search=10;
int ftpStatus=0;
int time_out_nav_pvt=1200;
int nav_pvt_message_nr=0;
int msgType;
int stat_screen=5;//keuze stat scherm indien stilstand
int GPIO12_screen=0;//keuze welk scherm
int low_bat_count;
int gps_speed;
float alfa_window;
float analog_mean;
float Mean_heading,heading_SD;

byte mac[6];  //unique mac adress of esp32
char SW_version[32]="SW-version 5.58";//Hier staat de software versie !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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
RTC_DATA_ATTR float RTC_mile;
RTC_DATA_ATTR float RTC_R1_10s;
RTC_DATA_ATTR float RTC_R2_10s;
RTC_DATA_ATTR float RTC_R3_10s;
RTC_DATA_ATTR float RTC_R4_10s;
RTC_DATA_ATTR float RTC_R5_10s;
RTC_DATA_ATTR char Sleep_txt[32]="Your ID";
RTC_DATA_ATTR int logo_choice[10];
RTC_DATA_ATTR int SLEEP_screen=0;
RTC_DATA_ATTR int OFF_screen=0;
//Simon
RTC_DATA_ATTR float calibration_bat=1.75;//bij ontwaken uit deepsleep niet noodzakelijk config file lezen
RTC_DATA_ATTR float voltage_bat;
FtpServer ftpSrv;  
GPS_data Ublox; // create an object storing GPS_data !
GPS_speed M100(100);
GPS_speed M250(250);
GPS_speed M500(500);
GPS_speed M1852(1852);
GPS_time S2(2);
GPS_time s2(2);
GPS_time S10(10);
GPS_time s10(10);//om tussentijdse stats, kan gereset worden !!
GPS_time S1800(1800);
GPS_time S3600(3600);
Alfa_speed A250(50);
Alfa_speed A500(50);
Alfa_speed a500(50);//tussentijdse Alfa stats
Button_push Short_push12 (12,100,15,1);//GPIO pin 12 is nog vrij, button_count 0 en 1 !!!
Button_push Long_push12 (12,2000,10,4);
Button_push Short_push39 (39,100,10,8);//was 39
Button_push Long_push39 (39,1500,10,8);//was 39
GxIO_Class io(SPI, /*CS=5*/ ELINK_SS, /*DC=*/ ELINK_DC, /*RST=*/ ELINK_RESET);
GxEPD_Class display(io, /*RST=*/ ELINK_RESET, /*BUSY=*/ ELINK_BUSY);
SPIClass sdSPI(VSPI);

const char *filename = "/config.txt";
const char *filename_backup = "/config_backup.txt"; 
Config config;  


  /*
Method to print the reason by which ESP32 has been awaken from sleep
*/
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    voltage_bat=analog_mean*calibration_bat/1000;
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); reed=1;
                                 rtc_gpio_deinit(GPIO_NUM_39);//was 39
                                 esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
                                 Boot_screen();
                                 break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); 
                                 break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer");
                                  analog_mean = analogRead(PIN_BAT);
                                  for(int i=0;i<10;i++){
                                        analog_bat = analogRead(PIN_BAT);
                                        analog_mean=analog_bat*0.1+analog_mean*0.9;
                                        }
                                  voltage_bat=analog_mean*calibration_bat/1000;
                                  esp_sleep_enable_ext0_wakeup(GPIO_NUM_39,0); //was 39  1 = High, 0 = Low
                                  Sleep_screen(SLEEP_screen);
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
        if(Time_Set_OK){    //Anly safe to RTC if new GPS data is available !!
            Time_Set_OK=false;
            RTC_year=(tmstruct.tm_year+1900);
            RTC_month=(tmstruct.tm_mon+1);
            RTC_day=(tmstruct.tm_mday);
            RTC_hour=(tmstruct.tm_hour);
            RTC_min=(tmstruct.tm_min);
            RTC_distance=Ublox.total_distance/1000000;
            RTC_alp=A500.display_max_speed*calibration_speed;
            RTC_500m=M500.avg_speed[9]*calibration_speed;
            RTC_mile=M1852.display_max_speed*calibration_speed;
            RTC_max_2s= S2.avg_speed[9]*calibration_speed;
            RTC_avg_10s=S10.avg_5runs*calibration_speed;
            RTC_R1_10s=S10.avg_speed[9]*calibration_speed;
            RTC_R2_10s=S10.avg_speed[8]*calibration_speed;
            RTC_R3_10s=S10.avg_speed[7]*calibration_speed;
            RTC_R4_10s=S10.avg_speed[6]*calibration_speed;
            RTC_R5_10s=S10.avg_speed[5]*calibration_speed;
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
            delay(100);
            Close_files();  
            }
        go_to_sleep(10);//got to sleep na 10s     
}

//For RTOS, the watchdog has to be triggered
void feedTheDog(){
  TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE; // write enable
  TIMERG0.wdt_feed=1;                       // feed dog
  TIMERG0.wdt_wprotect=0;                   // write protect
 
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
    case SYSTEM_EVENT_STA_GOT_IP://  Op dit moment nog geen IP !!!         SYSTEM_EVENT_STA_CONNECTED:
      Serial.println("ESP32 Connected to WiFi Network");
      IP_adress =  WiFi.localIP().toString();
      break;
    case SYSTEM_EVENT_AP_START:
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
void setup() {
  Serial.begin(115200);
  Serial.println("setup Serial");
  Serial.println("Serial Txd is on pin: "+String(TX));
  Serial.println("Serial Rxd is on pin: "+String(RX));
  SPI.begin(SPI_CLK, SPI_MISO, SPI_MOSI, ELINK_SS); //SPI is used for SD-card and for E_paper display !
  print_wakeup_reason(); //Print the wakeup reason for ESP32, go back to sleep is timer is wake-up source !
 
  pinMode(25, OUTPUT);//Power beitian //standaard maar drive strength 2, haal dan 2.7V aan de ublox gps
  pinMode(26, OUTPUT);//Power beitian
  pinMode(27, OUTPUT);//Power beitian
  rtc_gpio_set_drive_capability(GPIO_NUM_25,GPIO_DRIVE_CAP_3);//zie ook https://www.esp32.com/viewtopic.php?t=5840
  rtc_gpio_set_drive_capability(GPIO_NUM_26,GPIO_DRIVE_CAP_3);//haal nu 3.0V aan de ublox gps met 50 mA
  gpio_set_drive_capability(GPIO_NUM_27,GPIO_DRIVE_CAP_3);//rtc noodzakelijk, anders geen spanning op RTC_pin 25 en 26, 13/3/2022
  
  Ublox_on();//beitian bn220 power supply over output 25,26,27
  Serial2.setRxBufferSize(1024); // increasing buffer size ?
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2); //connection to ublox over serial2
  Serial.println("Serial2 9600bd Txd is on pin: "+String(TXD2));
  Serial.println("Serial2 9600bd Rxd is on pin: "+String(RXD2));
  
  for(int i=0;i<600;i++){//Startup string van ublox to serial, ca 424 char !!
     while (Serial2.available()) {
              Serial.print(char(Serial2.read()));
              }
     delay(2);   //was delay (1)
     }
   
 

  sdSPI.begin(SDCARD_CLK, SDCARD_MISO, SDCARD_MOSI, SDCARD_SS);//zou standaard op 20 MHz gezet worden !
 
  struct timeval tv = { .tv_sec =  0, .tv_usec = 0 };
  settimeofday(&tv, NULL);
  if (!SD.begin(SDCARD_SS, sdSPI)) {
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
  Init_ublox(); //switch to ubx protocol+ baudrate 19200
  Set_rate_ublox(config.sample_rate);//after reading config file !!    
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
    server.handleClient(); // wait for client handle, and update BAT Status, this section should be moved to the loop... //client coutner wait until download is finished to prevent stoping the server during download
    analog_bat = analogRead(PIN_BAT);
    analog_mean=analog_bat*0.1+analog_mean*0.9;
    voltage_bat=analog_mean*calibration_bat/1000;
    if(digitalRead(39)==false){//was 39
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
      //IP_adress =  WiFi.localIP().toString();
      ftpSrv.begin("esp32","esp32");    //username, password for ftp
      OTA_setup();  //start webserver for over the air update
      }
  else{
      Serial.println("No Wifi connection !");
      WiFi.disconnect(true);
      WiFi.mode(WIFI_OFF);
      Wifi_on=false;
  }
  analog_mean=voltage_bat/calibration_bat*1000;//voltage_bat staat in RTC mem !!!
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
  feedTheDog();
  analog_bat = analogRead(PIN_BAT);
  analog_mean=analog_bat*0.1+analog_mean*0.9;
  voltage_bat=analog_mean*calibration_bat/1000;
  delay(100);  
}

void taskOne( void * parameter )
{
 while(true){ 
   feedTheDog();

   if (Short_push12.Button_pushed())GPIO12_screen++;//toggle scherm
   if (GPIO12_screen>config.gpio12_count)GPIO12_screen=0;
   if (Long_push12.Button_pushed()){ s10.Reset_stats(); s2.Reset_stats();a500.Reset_stats();} //resetten stats   
     
   if(Long_push39.Button_pushed()) Shut_down();//was 39
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
       }   
   if((WiFi.status() == WL_CONNECTED)|SoftAP_connection){
        ftpSrv.handleFTP();        //make sure in loop you call handleFTP()!! 
        server.handleClient();
        ftpStatus=ftpSrv.cmdStatus;//for e-paper
        //delay(1);
        }
   else msgType = processGPS(); //alleen decodering indien geen Wifi verbinding
          //while (Serial2.available()) {
          //    Serial.print(char(Serial2.read()));}   
   if ( msgType == MT_NAV_PVT ) { 
    static int nav_pvt_message=0;
    if((ubxMessage.navPvt.numSV>=MIN_numSV_FIRST_FIX)&((ubxMessage.navPvt.sAcc/1000.0f)<MAX_Sacc_FIRST_FIX)&(GPS_Signal_OK==false)){
                GPS_Signal_OK=true;
                Set_GPS_Time();
                first_fix_GPS=millis()/1000;
                }
    if(GPS_Signal_OK==true){
           GPS_delay++;
          }
    if ((Time_Set_OK==false)&(GPS_Signal_OK==true)&(GPS_delay>(TIME_DELAY_FIRST_FIX*config.sample_rate))){//vertraging Time_Set_OK is nu 10 s!!
      int avg_speed=(avg_speed+ubxMessage.navPvt.gSpeed*19)/20; //FIR filter gem. snelheid laatste 20 metingen in mm/s
      if(avg_speed>MIN_SPEED_START_LOGGING){
        Time_Set_OK=true;
        Open_files();//alleen maar openen indien ontvangst OK !!
        
        }
    } 
//    Alleen speed>0 indien snelheid groter is dan 1m/s + sACC<1 + sat<5 + speed>35 m/s !!!
    if(Time_Set_OK==true)nav_pvt_message++;
    if ((sdOK==true)&(Time_Set_OK==true)&(nav_pvt_message>10)){
              float sAcc=ubxMessage.navPvt.sAcc/1000;
              gps_speed=ubxMessage.navPvt.gSpeed; //hier alles naar mm/s !!
              static int last_flush_time=0;
              //Log_to_SD();//hier wordt ook geprint naar serial !!
              if((millis()-last_flush_time)>60000){
                Flush_files();
                last_flush_time=millis();
                }
              if((ubxMessage.navPvt.numSV<=MIN_numSV_GPS_SPEED_OK)|((ubxMessage.navPvt.sAcc/1000.0f)>MAX_Sacc_GPS_SPEED_OK)|(ubxMessage.navPvt.gSpeed/1000.0f>MAX_GPS_SPEED_OK)){
                gps_speed=0;
                //ubxMessage.navPvt.gSpeed=0;//checksum is wrong when setting navPvt, so first Log_to_SD !!!
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
  }  
}
 
void taskTwo( void * parameter)
{
  while(true){
    feedTheDog();
   
    stat_count++;//ca 1s per screen update
    if (stat_count>config.screen_count)stat_count=0;//screen_count = 2
    
    // Serial.println(stat_count);
    analog_bat = analogRead(PIN_BAT);
    analog_mean=analog_bat*0.05+analog_mean*0.95;
    voltage_bat=analog_mean*calibration_bat/1000;
    if(voltage_bat<3.1) low_bat_count++;
    else low_bat_count=0;
    if((long_push==true)|(low_bat_count>10)){
        Off_screen(OFF_screen);
        Shut_down();
    }
    else if(millis()<2000)Update_screen(BOOT_SCREEN);
    else if(GPS_Signal_OK==false) Update_screen(WIFI_ON);//((Wifi_on==true)&(ubxMessage.navPvt.gSpeed/1000.0f<2)) Update_screen(WIFI_ON);//toch wifi info indien GPS fix de eerste 100s, JH 14/2/2022
   // else if(Short_push12.long_pulse){Update_screen(config.gpio12_screen[GPIO12_screen]);}//heeft voorrang, na drukken GPIO_pin 12, 10 STAT4 scherm !!!
    else if((gps_speed/1000.0f<config.stat_speed)&(Field_choice==false)){//nu configureerbaar via config.txt
          Update_screen(config.stat_screen[stat_count]);
          /*
          if(stat_count<2)Update_screen(config.stat_screen[stat_count]);//STATS1=3, STATS5=7
          else if(stat_count<4) Update_screen(config.stat_screen[1]);
          else if(stat_count<6) Update_screen(config.stat_screen[2]);
          else Update_screen(config.stat_screen[3]);//max 4 stat schermen !!
          */
          }
    else {
          Update_screen(SPEED);
          stat_count=0;
          }
  }
}
