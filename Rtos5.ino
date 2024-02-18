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
#include "FS.h"
#include "SPI.h"
#include "sys/time.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include "Arduino.h"
#include "Ublox.h"
#include "E_paper.h"
#include "SD_card.h"
#include "GPS_data.h"
#include "ESP32FtpServerJH.h"
#include "OTA_server.h" 
#include <esp_task_wdt.h>
#include <driver/rtc_io.h>
#include <driver/gpio.h>
#include <lwip/apps/sntp.h>
#include <esp32-hal.h>
#include <time.h>
#include <EEPROM.h>
#include "Definitions.h"
#include "ESP_functions.h"

void setup() {
  
  EEPROM.begin(EEPROM_SIZE);
  config.ublox_type = EEPROM.read(0);
  Serial.begin(115200);
  Serial.println("setup Serial");
  Serial.println("Serial Txd is on pin: "+String(TX));
  Serial.println("Serial Rxd is on pin: "+String(RX));
  Serial.println("Configuring WDT...");
  esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL); //add current thread to WDT watch
  SPI.begin(SPI_CLK, SPI_MISO, SPI_MOSI, ELINK_SS); //SPI is used for SD-card and for E_paper display !
  print_wakeup_reason(); //Print the wakeup reason for ESP32, go back to sleep is timer is wake-up source !
  analog_mean = analogRead(PIN_BAT);//fill FIR filter
 //sometimes after OTA hangs here ???
  pinMode(25, OUTPUT);//Power beitian //default drive strength 2, only 2.7V @ ublox gps
  pinMode(26, OUTPUT);//Power beitian
  pinMode(27, OUTPUT);//Power beitian
  rtc_gpio_set_drive_capability(GPIO_NUM_25,GPIO_DRIVE_CAP_3);//see https://www.esp32.com/viewtopic.php?t=5840
  rtc_gpio_set_drive_capability(GPIO_NUM_26,GPIO_DRIVE_CAP_3);//3.0V @ ublox gps current 50 mA
  gpio_set_drive_capability(GPIO_NUM_27,GPIO_DRIVE_CAP_3);//rtc_gpio_ necessary, if not no output on RTC_pins 25 en 26, 13/3/2022
  
  sdSPI.begin(SDCARD_CLK, SDCARD_MISO, SDCARD_MOSI, SDCARD_SS);//default 20 MHz gezet worden !

  struct timeval tv = { .tv_sec =  0, .tv_usec = 0 };
  settimeofday(&tv, NULL);
  if (!SD.begin(SDCARD_SS, sdSPI)) {
        sdOK = false;Serial.println("No SDCard found!");
  }
  else {
    sdOK = true;Serial.println("SDCard found!");
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    uint64_t totalBytes=SD.totalBytes() / (1024 * 1024);
    uint64_t usedBytes=SD.usedBytes() / (1024 * 1024);
    freeSpace=totalBytes-usedBytes;
    Boot_screen();
    Serial.printf("SD Card Size: %lluMB\n", cardSize); 
    Serial.printf("SD Total bytes: %lluMB\n", totalBytes); 
    Serial.printf("SD Used bytes: %lluMB\n", usedBytes); 
    Serial.printf("SD free space: %lluMB\n", totalBytes-usedBytes); 
    Serial.println(F("Loading configuration..."));// Should load default config 
    loadConfiguration(filename, filename_backup, config); // load config file
    //Short_push39.button_count=config.field;//set speed_field choice, so counting from correct speed_field !!
    Serial.print(F("Print config file...")); 
    printFile(filename); 
  } 
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
        //IP_adress =  WiFi.softAPIP().toString();
        Serial.println(IP_adress);
        Serial.println("start OTA Server");
        OTA_setup(); //start webserver and ota - for use on AP Mode
        ota_notrunning=false;
      }
      else{
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
      Update_screen(GPS_INIT_SCREEN); 
      GPS_OK = setupGPS();
      Update_screen(GPS_INIT_SCREEN);
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
                    &t1);            /* Task handle. */
    xTaskCreate(
                    taskTwo,          /* Task function. */
                    "TaskTwo",        /* String with name of task. */
                    10000,            /* Stack size in bytes was 10000, but stack overflow on task 2 ?????? now 20000. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    &t2);            /* Task handle. */
}
 
void loop() { 
  int wdt_task0_duration=millis()-wdt_task0;
  int wdt_task1_duration=millis()-wdt_task1;
  int task_timeout=(WDT_TIMEOUT -1)*1000;//1 second less then reboot timeout 
  if((wdt_task0_duration<task_timeout)&(wdt_task1_duration<task_timeout)){
    feedTheDog_Task0();
    feedTheDog_Task1();
    }
  if((wdt_task0_duration>task_timeout)&(downloading_file)&(max_count_wdt_task0<MAX_COUNT_WDT_TASK0)) {
    max_count_wdt_task0++;
    feedTheDog_Task0();
    wdt_task0=millis();
    Serial.println("Extend watchdog_timeout due long download"); 
    }     
  if((wdt_task0_duration>task_timeout)&(!downloading_file)) Serial.println("Watchdog task0 triggered");
  if(wdt_task1_duration>task_timeout) Serial.println("Watchdog task1 triggered");
  Update_bat();
  delay(100); 
}

void taskOne( void * parameter )
{
 while(true){ 
   wdt_task0=millis();
   #if defined (GPIO12_ACTIF)
     if (Short_push12.Button_pushed())GPIO12_screen++;//toggle screen
     if (GPIO12_screen>config.gpio12_count)GPIO12_screen=0;
     if (Long_push12.Button_pushed()){ s10.Reset_stats(); s2.Reset_stats();a500.Reset_stats();} //resetten stats   
   #endif  
   if(Long_push39.Button_pushed()) Shut_down();

   if (Short_push39.Button_pushed()){
     if(config.speed_count==0){config.field_actual=Short_push39.button_count;}
     else {
       if(Short_push39.button_count>config.speed_count){Short_push39.button_count=0;}
       config.field_actual=config.speed_screen[Short_push39.button_count];
//       Serial.print("config.field_actual ");Serial.println(config.field_actual);
      }
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
     if(!GPS_OK) {
       GPS_OK = setupGPS();
     }
   }
   if((WiFi.status() == WL_CONNECTED)|SoftAP_connection){
     ftpSrv.handleFTP();        //make sure in loop you call handleFTP()!! 
     server.handleClient();
     ftpStatus=ftpSrv.cmdStatus;//for e-paper
//     Init and get the time if WLAN, create "Archive" dir if not present
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
     #if defined USE_AUTO_OTA_UPDATE
       if ((millis() - OTA_CHECK_INTERVAL) > _lastOTACheck) {
         _lastOTACheck = millis();
         checkFirmwareUpdates();
       }
     #endif
     #if defined (STATIC_DEBUG)
       msgType = processGPS(); //debug purposes
       static int testtime;
       if((millis()-testtime)>1000){
         testtime=millis();
         Set_GPS_Time(config.timezone);
         config.timezone++;
       }
     #endif
   }
   else {
     msgType = processGPS(); //only decoding if no Wifi connection
     //while (Serial2.available()) { Serial.print(char(Serial2.read()));}
     if((millis()-last_gps_msg)>TIME_OUT_NAV_PVT){trouble_screen=true;}//check for time out nav-pvt msg
     else {trouble_screen=false;}
   }
   if ( msgType == MT_NAV_DOP ) {      //Logging after NAV_DOP, ublox sends first NAV_PVT, and then NAV_DOP. 
     if((ubxMessage.navPvt.numSV>=MIN_numSV_FIRST_FIX)&((ubxMessage.navPvt.sAcc/1000.0f)<MAX_Sacc_FIRST_FIX)&(ubxMessage.navPvt.valid>=7)&(GPS_Signal_OK==false)){
       GPS_Signal_OK=true;
       first_fix_GPS=millis()/1000;
     }
     if(GPS_Signal_OK==true){
       GPS_delay++;
     }
     if ((Time_Set_OK==false)&(GPS_Signal_OK==true)&(GPS_delay>(TIME_DELAY_FIRST_FIX*config.sample_rate))){//vertraging Time_Set_OK is nu 10 s!!
       static int avg_speed;
       avg_speed=(avg_speed+ubxMessage.navPvt.gSpeed*19)/20; //FIR filter gem. snelheid laatste 20 metingen in mm/s
       if(avg_speed>(config.start_logging_speed*1000)){
         if(Set_GPS_Time(config.timezone)){            
           Time_Set_OK=true;
           Shut_down_Save_session=true;
           start_logging_millis=millis();
           Open_files();//only start logging if GPS signal is OK
         }
       }      //    Alleen speed>0 indien snelheid groter is dan 1m/s + sACC<1 + sat<5 + speed>35 m/s !!!
     }
     if ((sdOK==true)&(Time_Set_OK==true)&(nav_pvt_message>10)&(nav_pvt_message!=old_message)){
       old_message=nav_pvt_message;
       //last_gps_msg=millis();
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
     last_gps_msg=millis(); 
     if(Time_Set_OK==true){
       nav_pvt_message++;
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
     Serial.print("numSV ");Serial.println(ubxMessage.navSat.numSvs);
              
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
    wdt_task1=millis();
    stat_count++;//ca 1s per screen update
    if (stat_count>config.screen_count)stat_count=0;//screen_count = 2
    Update_bat();
    if(RTC_voltage_bat<MINIMUM_VOLTAGE) low_bat_count++;
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
//////////////////// testing screens ///////////////////////////////////////////////////////////
    else if(test_screen==true){   // set in ESP_functions.h bool test_screen = true;          //
      Update_screen(SPEED);       // scrolling through font and fields only works with speed  //
//      Update_screen(config.stat_screen[stat_count]);
      delay(3000);                                                                            //
/////////////////// loop for Stats Screens /////////////////////////////////////////////////////
//      Serial.print("stat_count=");Serial.println(stat_count);
//      Update_screen(config.stat_screen[stat_count]);
//      delay(1000); 
    }
/////////////////////////////////////////////////////////////////////////////////////
    else if(millis()<2000)Update_screen(BOOT_SCREEN);
    else if(trouble_screen) Update_screen(TROUBLE);
    else if(GPS_Signal_OK==false) Update_screen(WIFI_ON);
    else if(Time_Set_OK==false) Update_screen(WIFI_ON);
    #if defined (GPIO12_ACTIF)
      else if(Short_push12.long_pulse){Update_screen(config.gpio12_screen[GPIO12_screen]);}//heeft voorrang, na drukken GPIO_pin 12, 10 STAT4 scherm !!!
    #endif
    else if((gps_speed/1000.0f<config.stat_speed)&(Field_choice==false)){
      Update_screen(config.stat_screen[stat_count]);
    }
    else {
      if(config.speed_large_font==2){
        Update_screen(SPEED2);
      }
      else {
        Update_screen(SPEED);
      }
      stat_count=0;
    }
  }
}
