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
 * Added STATS3 screen with M100/M500, config.stat_field 6
 * Added Rtos5.h for general #define
 * Bugfix in new_run_detection, speed m/s -> mm/s
 * Start logging if speed>MIN_SPEED_START_LOGGING, now 2m/s
 * Added log files in oao format
 * Removed logging to csv files
 * Changes sw 5.1
 * Add distance of run
 * Add configurable bar in "speed" screen, reflects run_distance, default 1852m = full bar
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
#include "Rtos5.h"
#include "Arduino.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include "ESP32FtpServerJH.h"
#include "OTA_server.h"
#include <esp_task_wdt.h>
#include <driver/rtc_io.h>
#include <driver/gpio.h>

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
#define MAX_SPEED_DISPLAY_STATS 1 //max snelheid in m/s om stat schermen te zien

String IP_adress="0.0.0.0";

int sdTrouble=0;
bool sdOK = false;
bool logCSV = false;
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
int first_fix_GPS,run_count,old_run_count,stat_count,GPS_delay,push_time;
//int stat_fields=6;//4=alleen STATS1 en STATS2, 6 ook STATS3
int wifi_search=10;
int time_out_nav_pvt=1200;
int nav_pvt_message_nr=0;
float analog_mean;
float Mean_heading,heading_SD;
float calibration_speed=3.6;
byte mac[6];  //unique mac adress of esp32
char SW_version[32]=" SW-version 5.1 ";
RTC_DATA_ATTR int bootCount = 0;
RTC_DATA_ATTR float RTC_distance;
RTC_DATA_ATTR float RTC_avg_10s;
RTC_DATA_ATTR float RTC_max_2s;
RTC_DATA_ATTR float calibration_bat=1.75;//bij ontwaken uit deepsleep niet noodzakelijk config file lezen
RTC_DATA_ATTR float voltage_bat;
FtpServer ftpSrv;  
GPS_data Ublox; // create an object storing GPS_data !
GPS_speed M100(100);
GPS_speed M250(250);
GPS_speed M500(500);
GPS_speed M1852(1852);
GPS_time S2(2);
GPS_time S10(10);
//GPS_time S600(600);
GPS_time S1800(1800);
GPS_time S3600(3600);
Alfa_speed A250(50);
Alfa_speed A500(50);
void go_to_sleep(int sleep_time);
GxIO_Class io(SPI, /*CS=5*/ ELINK_SS, /*DC=*/ ELINK_DC, /*RST=*/ ELINK_RESET);
GxEPD_Class display(io, /*RST=*/ ELINK_RESET, /*BUSY=*/ ELINK_BUSY);
SPIClass sdSPI(VSPI);

const char *filename = "/config.txt"; 
Config config;  
/*
Method to print the reason by which ESP32 has been awaken from sleep
*/
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  //Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));
  switch(wakeup_reason)
  {
    voltage_bat=analog_mean*calibration_bat/1000;
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); reed=1;
                                 rtc_gpio_deinit(GPIO_NUM_39);
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
                                  esp_sleep_enable_ext0_wakeup(GPIO_NUM_39,0); //1 = High, 0 = Low
                                  Sleep_screen(bootCount);
                                  go_to_sleep(4000);
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
  //esp_task_wdt_delete(NULL);
  //esp_task_wdt_deinit();
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
void Long_push(int push_time){
    static int pushtime;
    button=digitalRead(39);
    if(button==1)pushtime=millis();
    if((millis()-pushtime)>push_time){
      //Serial.println("Button 2s");
      long_push=true;
      //digitalWrite(19, LOW);//Groene LED uit 
      Ublox_off();
      if(Time_Set_OK==true){
        Time_Set_OK=false;
        GPS_Signal_OK=false;
        GPS_delay=0;
        RTC_distance=Ublox.total_distance/1000;
        RTC_max_2s= S2.avg_speed[9]*calibration_speed;
        RTC_avg_10s=S10.avg_5runs*calibration_speed;
        Session_info(Ublox);
        Session_results_S(S2);
        Session_results_S(S10);
        //Session_results_S(S600);
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
    }
    if((long_push==true)&(button==1)){
      go_to_sleep(10);
    }    
}
bool Edge_pin39 (void){
  static bool last_state=true;
  bool result;
  if(digitalRead(39)&!last_state)result=true;
  else result=false;
  last_state=digitalRead(39);
  return result;
}
//For RTOS, the watchdog has to be triggered
void feedTheDog(){
  // feed dog 0
  TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE; // write enable
  TIMERG0.wdt_feed=1;                       // feed dog
  TIMERG0.wdt_wprotect=0;                   // write protect
  // feed dog 1
  TIMERG1.wdt_wprotect=TIMG_WDT_WKEY_VALUE; // write enable
  TIMERG1.wdt_feed=1;                       // feed dog
  TIMERG1.wdt_wprotect=0;                   // write protect
} 
void OnWiFiEvent(WiFiEvent_t event){
  switch (event) {
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
  pinMode(39,INPUT_PULLUP); 
  Serial.begin(115200);
  Serial.println("setup Serial");
  Serial.println("Serial Txd is on pin: "+String(TX));
  Serial.println("Serial Rxd is on pin: "+String(RX));
  SPI.begin(SPI_CLK, SPI_MISO, SPI_MOSI, ELINK_SS); //SPI is used for SD-card and for E_paper display !
  print_wakeup_reason(); //Print the wakeup reason for ESP32, go back to sleep is timer is wake-up source !
  pinMode(25, OUTPUT);//Power beitian //standaard maar drive strength 2, haal dan 2.7V aan de ublox gps
  pinMode(26, OUTPUT);//Power beitian
  pinMode(27, OUTPUT);//Power beitian
  gpio_set_drive_capability(GPIO_NUM_25,GPIO_DRIVE_CAP_3);//zie ook https://www.esp32.com/viewtopic.php?t=5840
  gpio_set_drive_capability(GPIO_NUM_26,GPIO_DRIVE_CAP_3);//haal nu 3.0V aan de ublox gps met 50 mA
  gpio_set_drive_capability(GPIO_NUM_27,GPIO_DRIVE_CAP_3);
  Ublox_on();//beitian bn220 power supply over output 25,26,27
  Serial2.setRxBufferSize(1024); // increasing buffer size ?
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2); //connection to ublox over serial2
  Serial.println("Serial2 9600bd Txd is on pin: "+String(TXD2));
  Serial.println("Serial2 9600bd Rxd is on pin: "+String(RXD2));
  for(int i=0;i<600;i++){//Startup string van ublox to serial, ca 424 char !!
     while (Serial2.available()) {
              Serial.print(char(Serial2.read()));
              }
     delay(1);   
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
        loadConfiguration(filename, config); // load config file
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
  while ((WiFi.status() != WL_CONNECTED)&(SoftAP_connection==false)) {
    if(digitalRead(39)==false){
      WiFi.disconnect();
      WiFi.mode(WIFI_AP);
      WiFi.softAP(soft_ap_ssid, soft_ap_password);
      wifi_search=100;
    }
    if(wifi_search<=10) Update_screen(WIFI_STATION);
    else Update_screen(WIFI_SOFT_AP);
    //delay(250);
    Serial.print(".");
    wifi_search--;if(wifi_search<=0)break;
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
  //pinMode(19, OUTPUT);
  //digitalWrite(19, HIGH);//Groene LED aan
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
   Long_push(1000);//go to sleep
   if((Edge_pin39()==true)&((millis()-push_time)>500)){//bouncing reedswitch
      push_time=millis();
      config.field++;
      if (config.field>8)config.field=1;
      }
   if((millis()-push_time)<10000)Field_choice=true;//10s wachttijd voor menu field keuze....
   else Field_choice=false;
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
        delay(1);
        }
   int msgType = processGPS();
          //while (Serial2.available()) {
          //    Serial.print(char(Serial2.read()));}
   /*       
   int delta_t=millis()-time_out_nav_PVT;
   if((delta_t>time_out_limit)&(Time_Set_OK==true)){
        GPS_Signal_OK==false;
        Ublox_off();
        Serial.println("Time out ubx navPVT message, new start ublox !!"); 
        delay(500);
        Ublox_on();
        Init_ublox(); 
        time_out_limit=30000;    //na 30s terug opstart 
      }
   */   
   if ( msgType == MT_NAV_PVT ) { 
    //ubxMessage.navPvt.gSpeed=10000;//test snelheid = 10 m/s
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
              int gps_speed=ubxMessage.navPvt.gSpeed; //hier alles naar mm/s !!
              static int last_flush_time=0;
              if((millis()-last_flush_time)>60000){
                Flush_files();
                last_flush_time=millis();
                }
              if((ubxMessage.navPvt.numSV<=MIN_numSV_GPS_SPEED_OK)|((ubxMessage.navPvt.sAcc/1000.0f)>MAX_Sacc_GPS_SPEED_OK)|(ubxMessage.navPvt.gSpeed/1000.0f>MAX_GPS_SPEED_OK)){
                gps_speed=0;
                ubxMessage.navPvt.gSpeed=0;
                }
               
              Log_to_SD();//hier wordt ook geprint naar serial !!
              
              Ublox.push_data(ubxMessage.navPvt.lat/10000000.0f,ubxMessage.navPvt.lon/10000000.0f,gps_speed);   
              run_count=New_run_detection(ubxMessage.navPvt.heading/100000.0f,S2.avg_s); 
              if(run_count!=old_run_count)Ublox.run_distance=0;
              old_run_count=run_count;        
              M100.Update_distance(run_count);
              M250.Update_distance(run_count);
              M500.Update_distance(run_count);
              M1852.Update_distance(run_count);
              S2.Update_speed(run_count);    
              S10.Update_speed(run_count);
              //S600.Update_speed(run_count);//buffer = 5000, aan 10 Hz dus 500s
              S1800.Update_speed(run_count);
              S3600.Update_speed(run_count);
              A250.Update_Alfa(M250);
              A500.Update_Alfa(M500);
              }  
      } 
  }  
}
 
void taskTwo( void * parameter)
{
  while(true){
    stat_count++;
    feedTheDog(); 
    //int stack_free=uxTaskGetStackHighWaterMark(NULL);//dit geeft het aantal vrij geheugen van de stack !!!
                        //Waarschijnlijk andere oorzaak van stack overflow 
    //Serial.print("Task2 update e-paper ");
    //Serial.println(stack_free);  
    analog_bat = analogRead(PIN_BAT);
    analog_mean=analog_bat*0.05+analog_mean*0.95;
    voltage_bat=analog_mean*calibration_bat/1000;
    if(long_push==true)Off_screen();
    else if(millis()<2000)Update_screen(BOOT_SCREEN);
    else if(GPS_Signal_OK==false) Update_screen(WIFI_ON);
    else if((ubxMessage.navPvt.gSpeed/1000.0f<MAX_SPEED_DISPLAY_STATS)&(Field_choice==false)){
          if(stat_count%config.stat_field<2)Update_screen(STATS1);//was 10/5
          else if(stat_count%config.stat_field<4) Update_screen(STATS2);
          else Update_screen(STATS3);
          }
    else {
          Update_screen(SPEED);
          stat_count=0;
          }
  }
}
