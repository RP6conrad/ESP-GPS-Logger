#include <SD_MMC.h>
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
//#include "E_paper_266.h"//changing the E_paper : delete the build directory (can be found in the compile output)
#include "E_paper.h"
#include "SD_card.h"
#include "GPS_data.h"
#include "ESP32FtpServerJH.h"
#include "OTA_server.h" 
#include <esp_task_wdt.h>
#include "freertos/task.h"//added V3
#include <driver/rtc_io.h>
#include <driver/gpio.h>
#include <lwip/apps/sntp.h>
#include <esp32-hal.h>
#include <time.h>
#include <EEPROM.h>
#include "Definitions.h"
#include <LittleFS.h>
#include "rom/rtc.h"
#include "track_speed.h"
#include "ESP_functions.h"

//#def INCLUDE_BLE
#ifdef INCLUDE_BLE
    #include <NimBLEDevice.h>
    // --- BLE CONFIGURATIE ---
    #define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
    #define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
    NimBLECharacteristic* pBLECharacteristic = nullptr;
    bool bleDeviceConnected = false;
    // Variabele om de 10 Hz timing (elke 100 ms) bij te houden
    unsigned long lastTransmitTime = 0;
    // Compacte struct voor BLE-verzending naar de app (Exact 16 bytes)
    struct __attribute__((__packed__)) BLEGPSData {
        float latitude;     // 4 bytes
        float longitude;    // 4 bytes
        float dopplerSpeed; // 4 bytes
        float heading;      // 4 bytes
    };
    // BLE Connectie callbacks
    class MyBLEServerCallbacks: public NimBLEServerCallbacks {
        void onConnect(NimBLEServer* pServer) { 
            bleDeviceConnected = true; 
            Serial.println("Apparaat verbonden via BLE!");
        }
        void onDisconnect(NimBLEServer* pServer) {
            bleDeviceConnected = false;
            Serial.println("Apparaat losgekoppeld. Starten met adverteren...");
            pServer->startAdvertising(); // Automatisch opnieuw adverteren
        }
    };
#endif
const char* ssid = config.ssid; //WiFi SSID
const char* password = config.password; //WiFi Password
const char* ssid2 = config.ssid2; //WiFi SSID
const char* password2 = config.password2; //WiFi Password
const char* soft_ap_ssid = "ESP32AP"; //accespoint ssid
const char* soft_ap_password = "password"; //accespoint password
bool ap_mode=false;
bool sleep_mode=false;
extern bool reset_boot; 
// Globale variabelen voor het traject en de metingen
Line_2D startLijn;
Line_2D finishLijn;
GPS_point poort1;
GPS_point poort2;
LijnMetingState startState;
LijnMetingState finishState;
Doppler_track result_track_speed;
// Variabelen om de status van de run bij te houden
uint32_t exacteGpsTijdStart = 0;  // Gewogen iTOW start (ms)
uint32_t exacteGpsTijdFinish = 0; // Gewogen iTOW finish (ms)
bool runIsBezig = false;
// Afstand tussen de poorten (wordt eenmalig berekend in setup)
double trajectAfstandMeters = 0.0;
double gemiddeldeSnelheid_mmps;
double doppler_track_speed;
double projected_doppler_track_speed;
double track_speed[10];
double doppler_speed[10];
double projected_doppler_speed[10];
uint8_t track_hour[10];
uint8_t track_minute[10];
double track_time;
LijnPassageResultaat startRes;
LijnPassageResultaat finishRes;

void setup() {
  Serial.begin(115200);
  Serial.print("Actual CPU freq @ boot"); Serial.println (getCpuFrequencyMhz());
  pinMode(2, INPUT_PULLUP);//for SD_MMC mode....
  EEPROM.begin(EEPROM_SIZE);
  config.ublox_type = EEPROM.readByte(0);Serial.print("EEPROM ublox_type=");Serial.println(config.ublox_type);
  config.M10_high_nav=EEPROM.readByte(1);
 // Serial.println("Force setting to autodetect!");
//  EEPROM.writeByte(0,255);EEPROM.commit();
//  config.ublox_type=255;
  if(config.M10_high_nav>3){config.M10_high_nav=0;EEPROM.writeByte(1,NO_M10_GPS);EEPROM.commit();} 
  Serial.print("EEPROM M10_high_nav=");Serial.println(config.M10_high_nav);
  //print_reset_reason(rtc_get_reset_reason(0));//Find out the reset reason, if no SW-reset-> back to deep sleep !
  if(reset_boot==true) {setCpuFrequencyMhz(80);}
  print_wakeup_reason(); //Print the wakeup reason for ESP32, go back to sleep is timer is wake-up source !
  Serial.println("setup Serial");
  Serial.println("Serial Txd is on pin: "+String(TX));
  Serial.println("Serial Rxd is on pin: "+String(RX));
  RTC_highest_read=EEPROM.readInt(2);
  if((RTC_highest_read<STARTVALUE_HIGHEST_READ)|(RTC_highest_read>MAXVALUE_HIGHEST_READ)){
    EEPROM.writeInt(2,STARTVALUE_HIGHEST_READ) ;
    EEPROM.commit();
    RTC_highest_read=STARTVALUE_HIGHEST_READ;
    Serial.println("Eeprom highest read set to starting value !!");
    }
  RTC_calibration_bat= FULLY_CHARGED_LIPO_VOLTAGE/RTC_highest_read;
  Serial.print("RTC_calibration_bat EEPROM = ");
  Serial.println(RTC_calibration_bat);
  Serial.println("Configuring WDT...");
  esp_task_wdt_init(WDT_TIMEOUT,true);
  esp_task_wdt_add(NULL); //add current thread to WDT watch
  analog_mean = analogRead(PIN_BAT);//fill FIR filter
  SPI.begin(SPI_CLK, SPI_MISO, SPI_MOSI, ELINK_SS); //SPI is used for SD-card and for E_paper display !
  //sdSPI.begin(SDCARD_CLK, SDCARD_MISO, SDCARD_MOSI, SDCARD_SS);//default 20 MHz gezet worden !
  struct timeval tv = { .tv_sec =  0, .tv_usec = 0 };
  settimeofday(&tv, NULL);

  if (!SD_MMC.begin("/sdcard", true)) {  
        sdOK = false;
        Serial.println("No SDCard found!");
        if (!LITTLEFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {
          Serial.println("LITTLEFS Mount Failed");
          return;
        } 
    else {
      Serial.print("LITTLEFS Mounted with success. Total space= ");
      int total_bytes = LITTLEFS.totalBytes();
      int used_bytes = LITTLEFS.usedBytes();
      Serial.print(total_bytes);
      Serial.println(" bytes");
      Serial.print("Free space left= ");
      Serial.print(total_bytes-used_bytes);
      Serial.println(" bytes");
      LITTLEFS_OK = true;
      //Boot_screen();
      loadConfiguration(filename, filename_backup, config);  // load config file
      Serial.print(F("Print config file..."));
      if (sdOK|LITTLEFS_OK) printFile(filename); 
    }
  } 
  else {
        sdOK = true;Serial.println("SDCard found!");
        //int *real_freq=0;
        //sdmmc_host_get_real_freq(SDMMC_HOST_SLOT_1,*real_freq);
        uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
        uint64_t totalBytes=SD_MMC.totalBytes() / (1024 * 1024);
        uint64_t usedBytes=SD_MMC.usedBytes() / (1024 * 1024);
        freeSpace=totalBytes-usedBytes;
       // Serial.printf("SD Card Speed: %d MHz\n", real_freq/1000); 
        Serial.printf("SD Card Size: %lluMB\n", cardSize); 
        Serial.printf("SD Total bytes: %lluMB\n", totalBytes); 
        Serial.printf("SD Used bytes: %lluMB\n", usedBytes); 
        Serial.printf("SD free space: %lluMB\n", totalBytes-usedBytes); 
        testFileIO(SD_MMC, "/test.txt");
        Serial.println(F("Loading configuration..."));// Should load default config 
        loadConfiguration(filename, filename_backup, config); // load config file
        //Short_push39.button_count=config.field;//set speed_field choice, so counting from correct speed_field !!
        Serial.print(F("Print config file...")); 
        printFile(filename); 
  } 
  Short_push12.begin(12,1);
  Short_push19.begin(19,0);
  Short_push39.begin(39,1);
  // 1. Voer hier je Google Maps coördinaten in van jouw windsurf speed-strip
  poort1 = { config.p1_lat, config.p1_lon }; 
  poort2 = { config.p2_lat, config.p2_lon };
  // 2. Bereken de vaste afstand
  trajectAfstandMeters = berekenAfstandVincenty(poort1, poort2);
  // 3. Genereer automatisch de haakse start- en finishlijn op basis van de vaaras
  TrajectLijnen mijnTraject = genereerLoodrechteLijnen(poort1, poort2);
  startLijn  = mijnTraject.startLijn;
  finishLijn = mijnTraject.finishLijn;
  RTC_screen_orientation = config.screen_orientation;
  Boot_screen();

   if(RTC_voltage_bat<RTC_minimum_voltage_bat){
      RTC_OFF_screen=1;//Simon screen with info text !!!
      char tekst[32] = "";
      sprintf(tekst, "Shutdown low bat  @ %.1f V\n",RTC_minimum_voltage_bat);
      logERR(tekst);
      strcpy(RTC_Sleep_txt,"Shut down Low Bat !");
      Shut_down();
   }
  if(reset_boot==true){
      RTC_OFF_screen=1;//Simon screen with info text !!!
      char tekst[32] = "";
      sprintf(tekst, "Sleep after reset");
      logERR(tekst);
      strcpy(RTC_Sleep_txt,"Shutdown after reset!");
      Shut_down();
  }
  setCpuFrequencyMhz(240);
  Update_screen(BOOT_SCREEN);
  Serial.print("Actual CPU freq before Wifi.begin(): "); Serial.println (getCpuFrequencyMhz());
  WiFi.onEvent(OnWiFiEvent);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("T5 MAC adress: ");
  WiFi.macAddress(mac);
  actual_ssid=ssid;
 
  Serial.println("search SSID1");
  Search_for_wifi(); 
   if ((WiFi.status() != WL_CONNECTED)&(ap_mode==false)) {
    WiFi.disconnect();
    WiFi.begin(ssid2, password2);
    actual_ssid=ssid2;  
    wifi_search=10;
   }
  if(ap_mode==false) {
    Serial.println("search SSID2");
    Search_for_wifi();
    }

  if(ap_mode==true){
    WiFi.disconnect();
    WiFi.mode(WIFI_AP);
    WiFi.softAP(soft_ap_ssid,soft_ap_password);
    wifi_search=120;
    Serial.println("search AP");
    Search_for_wifi();
  }
  
  if((WiFi.status()== WL_CONNECTED)|SoftAP_connection){
      actual_ssid=WiFi.SSID();
      Serial.println("");
      Serial.print("Connected to ");
      Serial.println(actual_ssid);
      Serial.print("IP address: ");
      Serial.println(IP_adress);
      Wifi_on=true;
      ftpSrv.begin("esp32","esp32");    //username, password for ftp
      OTA_setup();  //start webserver for over the air update
      }
  else{
      detachInterrupt(GO_TO_SLEEP_GPIO);
      Serial.println("No Wifi connection !");
      WiFi.disconnect(true);
      WiFi.mode(WIFI_OFF);
      Wifi_on=false;
      SoftAP_connection=false;
      Update_screen(GPS_INIT_SCREEN); 
      GPS_OK = setupGPS();
      Update_screen(GPS_INIT_SCREEN);
      }
  delay(100);
  #ifdef INCLUDE_BLE
  // --- INITIALISEER NIMBLE v1.3.7 BLE ---
    Serial.println("Initialiseren NimBLE Dummy GPS Service...");
    NimBLEDevice::init("ESP32_Dummy_10Hz");
    NimBLEServer* pBLEServer = NimBLEDevice::createServer();
    pBLEServer->setCallbacks(new MyBLEServerCallbacks());
    // Let op: in NimBLE 1.3.7 maken we de service rechtstreeks via pBLEServer aan
    NimBLEService *pBLEService = pBLEServer->createService(SERVICE_UUID);
    // Aanmaken karakteristiek met NimBLE v1.3.7 eigenschappen
    pBLECharacteristic = pBLEService->createCharacteristic(
                         CHARACTERISTIC_UUID,
                         NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
                       );
    // Handmatige aanmaak van de 0x2902 descriptor voor notificatie-ondersteuning
    pBLECharacteristic->createDescriptor("2902", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);
    pBLEService->start();
    NimBLEAdvertising *pBLEAdvertising = NimBLEDevice::getAdvertising();
    pBLEAdvertising->addServiceUUID(SERVICE_UUID);
    pBLEAdvertising->setScanResponse(true);
    NimBLEDevice::startAdvertising();
    Serial.println("Systeem Online. Dummy 10Hz data start zodra verbonden...");
#endif    
  //Create RTOS task, so logging and e-paper update are separated (update e-paper is blocking, 800 ms !!)
  //xTaskCreate(
  xTaskCreatePinnedToCore(  
                    taskOne,          /* Task function. */
                    "TaskOne",        /* String with name of task. */
                    10000,            /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    &t1,  //&t1,
                    1);            /* Task handle. */
 
 // xTaskCreate(
  xTaskCreatePinnedToCore( 
                    taskTwo,          /* Task function. */
                    "TaskTwo",        /* String with name of task. */
                    10000,            /* Stack size in bytes was 10000, but stack overflow on task 2 ?????? now 20000. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    &t2,//&t2,
                    0);            /* Task handle. */
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
    if(Long_push39.Button_pushed()|Long_push19.Button_pushed()) {
      sleep_mode=true;
      Serial.println("task one delete");vTaskDelete(NULL);
      }//to prevent gps boot in task one....
    static int actual_speed_field=0;  
    if (Short_push39.Button_pushed()|Short_push19.Button_pushed()){  
      if(config.Stat_screens_time==0)stat_count++;
      else actual_speed_field++;
      }
    if (actual_speed_field>config.speed_count) actual_speed_field=0;
    if (stat_count>config.screen_count)stat_count=0;
    config.field_actual=config.speed_screen[actual_speed_field];
    Field_choice=((Short_push39.long_pulse|Short_push19.long_pulse)&(config.Stat_screens_time!=0));//10s wachttijd voor menu field keuze....
   if((WiFi.status() != WL_CONNECTED)&(Wifi_on==true)&(SoftAP_connection==false)){
        Serial.println("No Wifi connection !");
        server.close();
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        Wifi_on=false; 
        sntp_stop();//no messing around with time synchronization anymore, EPOCH time is set to 0 !!! 
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
        // Init and get the time if WLAN, create "Archive" dir if not present
        if(WiFi.status() == WL_CONNECTED){
          if(NTP_time_set==0){
            configTime(0,0,"pool.ntp.org");
            setenv("TZ",TimeZone,1);
            tzset();
            printLocalTime();
            NTP_time_set=true;
            }
          if(!SD_MMC.exists("/Archive")&sdOK){
              SD_MMC.mkdir("/Archive");
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
   else {msgType = processGPS(); //only decoding if no Wifi connection
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
        if ((sdOK|LITTLEFS_OK)&(Time_Set_OK==true)&(nav_pvt_message>10)&(nav_pvt_message!=old_message)){
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
                  // Vertaal binaire data naar leesbare double eenheden
                  double latNu = (double)ubxMessage.navPvt.lat / 10000000.0;
                  double lonNu = (double)ubxMessage.navPvt.lon / 10000000.0;
                  uint32_t iTOW_nu = ubxMessage.navPvt.iTOW;

                  // Voer de live data in de twee poortfuncties
                  startRes  = controleerLijnPassage(latNu, lonNu, iTOW_nu, startLijn, startState);
                  finishRes = controleerLijnPassage(latNu, lonNu, iTOW_nu, finishLijn, finishState);
                  //500 m track auto setting coordinates if still 0 in config !!
                  //Minimal run from 650 m, where average speed last 500 m exceed 5 m/s
                  if((config.p1_lat<0.1)&(M500.m_speed>5000.0)&(Ublox.alfa_distance/1000>650.0)){
                    Auto_set_track();
                    }
                  // --- DETECTIE EN BEREKENING --- 
                  // 1. Startlijn passage
                  if (startRes.isLijnGepasseerd && !runIsBezig) {
                        exacteGpsTijdStart = startRes.gewogen_iTOW;
                        runIsBezig = true;
                        Serial.println("\n[START] Startlijn overschreden! Timer loopt...");
                    }
                 if (runIsBezig){
                    track_time = (double)(iTOW_nu-exacteGpsTijdStart)/1000;
                  }
                  result_track_speed=doppler_speed_calculation(ubxMessage.navPvt.gSpeed,ubxMessage.navPvt.heading,startLijn.start_finishRad,ubxMessage.navPvt.iTOW,runIsBezig);
                  doppler_track_speed = result_track_speed.doppler_track_speed;
                  projected_doppler_track_speed = result_track_speed.doppler_projected_track_speed;
                  // 3. Finishlijn passage & Snelheidsberekening, minimaal 5 s run om glitch te vermijden
                  if (finishRes.isLijnGepasseerd && runIsBezig) {
                        exacteGpsTijdFinish = finishRes.gewogen_iTOW;
                        runIsBezig = false; // Run voltooid
                        // VAARTIJD BEREKENEN: het verschil in milliseconden omrekenen naar seconden
                        double vaartijd_Seconden = (double)(exacteGpsTijdFinish - exacteGpsTijdStart)/1000;
                        // GEMIDDELDE SNELHEID BEREKENEN (m/s)
                        gemiddeldeSnelheid_mmps = trajectAfstandMeters*1000 / vaartijd_Seconden; 
                        getLocalTime(&tmstruct, 0);
                        track_hour[0]=tmstruct.tm_hour; 
                        track_minute[0]=tmstruct.tm_min; 
                        track_speed[0]= gemiddeldeSnelheid_mmps;
                        doppler_speed[0]= doppler_track_speed;
                        projected_doppler_speed[0]= projected_doppler_track_speed;
                        
                        sort_track(track_speed,doppler_speed,projected_doppler_speed,10,track_hour,track_minute);
                        Serial.println("\n=========================================");
                        Serial.println("       >>> RUN SUCCESVOL VOLTOOID <<<     ");
                        Serial.print(" Trajectafstand : "); Serial.print(trajectAfstandMeters, 2); Serial.println(" meter");
                        Serial.print(" Exacte Vaartijd: "); Serial.print(vaartijd_Seconden, 3); Serial.println(" seconden");
                        //Serial.print(" GEM. SNELHEID  : "); Serial.print(gemiddeldeSnelheidKmu, 3); Serial.println(" km/u");
                        Serial.println("=========================================\n");
                        // Reset de states handmatig voor een eventuele volgende run op de terugweg
                        startState.eersteFixIngevuld = false;
                        finishState.eersteFixIngevuld = false;
                        // Toggle finish en start lijn om track in 2 richtingen te meten
                       GPS_point dummy =  poort1;
                        poort1 = poort2;
                        poort2 = dummy;
                        // 2. Bereken de vaste afstand
                        trajectAfstandMeters = berekenAfstandVincenty(poort1, poort2);
                        // 3. Genereer automatisch de haakse start- en finishlijn op basis van de vaaras
                        TrajectLijnen mijnTraject = genereerLoodrechteLijnen(poort1, poort2);
                        startLijn  = mijnTraject.startLijn;
                        finishLijn = mijnTraject.finishLijn;
                        }

                  run_count=New_run_detection(ubxMessage.navPvt.heading/100000.0f,S2.avg_s); 
                  alfa_window=Alfa_indicator(M250,M100,ubxMessage.navPvt.heading/100000.0f);
                  if(run_count!=old_run_count)Ublox.run_distance=0;
                  old_run_count=run_count;        
                  M100.Update_distance(run_count);
                  M250.Update_distance(run_count);
                  M500.Update_distance(run_count);
                  M1852.Update_distance(run_count);
                  S1.Update_speed(run_count); 
                  S2.Update_speed(run_count); 
                  s2.Update_speed(run_count);      
                  S10.Update_speed(run_count);
                  s10.Update_speed(run_count);
                  S1800.Update_speed(run_count);
                  S3600.Update_speed(run_count);
                  A250.Update_Alfa(M250);
                  A500.Update_Alfa(M500);
                  a500.Update_Alfa(M500);
                  //M_500.Update_Track();
                  if(S2.avg_s>4000) S10_previous_run=S10.s_max_speed;//only update to new run value if actual speed > 4 m/s
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
    if(config.Stat_screens_time!=0)stat_count++;//alleen auto switch stat screen als time>0 !!
    if (stat_count>config.screen_count)stat_count=0;//screen_count = 2
    Update_bat();
    if(RTC_voltage_bat<RTC_minimum_voltage_bat) low_bat_count++;
    else low_bat_count=0;
    if(sleep_mode==true){
        Ublox_off();
        Off_screen(RTC_OFF_screen);
        Serial.println("RTC_OFF_screen");
        delay(2000);
        Shut_down();
        vTaskDelete(NULL);//to avoid that screen get new updates !!!!
    }
    else if(low_bat_count>10){
        sleep_mode=true;
        char tekst[32] = "";
        sprintf(tekst, "Shutdown low bat  @ %.1f V\n",RTC_minimum_voltage_bat);
        logERR(tekst);
        Off_screen(2);//off screen with "shutdown low bat"
        delay(2000);
        Shut_down();
        vTaskDelete(NULL);//to avoid that screen get new updates !!!!
    }
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
          Update_screen(SPEED);
          if(config.Stat_screens_time!=0)stat_count=0;
          }
  }
}
