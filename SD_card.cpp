#include "SD_card.h"
#include <SD.h>
#include "Rtos5.h"
#include "gpx.h"
#include "sbp.h"
#include "gpy.h"
File ubxfile;
File errorfile;
File gpyfile;//new open source file format, work in progress !!
File sbpfile;
File gpxfile;
char filename_NO_EXT[64]="/";
char filenameERR[64]="/";
char filenameUBX[64]="/";
char filenameGPY[64]="/";
char filenameSBP[64]="/";
char filenameGPX[64]="/";
char dataStr[255] = "";//string for logging  !!
char Buffer[50]= "";//string for logging
uint64_t GPS_UTC_ms; //Absolute UTC timewith ms resolution @start logging

struct Config config;

void logERR(const char * message){
  if(config.logTXT){
    errorfile.print(message);
    }
}
//test for existing GPSLOGxxxfiles, open txt,gps + ubx file with new name, or with timestamp !
void Open_files(void){
  if(config.file_date_time){
      //struct tm tmstruct;
      getLocalTime(&tmstruct);
      char extension[16]=".txt";//
      char timestamp[16];
       if(config.file_date_time==1){
          sprintf(timestamp, "_%d%02d%02d%02d%02d", tmstruct.tm_year-100,tmstruct.tm_mon+1,tmstruct.tm_mday,tmstruct.tm_hour,tmstruct.tm_min);
          strcat(filenameERR,config.UBXfile);//copy filename from config
          strcat(filenameERR,timestamp);//add timestamp
          strcat(filenameERR,extension);//add extension.txt 
          }
       if(config.file_date_time==2){
          sprintf(timestamp, "%d%02d%02d%02d%02d_", tmstruct.tm_year-100,tmstruct.tm_mon+1,tmstruct.tm_mday,tmstruct.tm_hour,tmstruct.tm_min);
          strcat(filenameERR,timestamp);//add timestamp
          strcat(filenameERR,config.UBXfile);//copy filename from config
          strcat(filenameERR,extension);//add extension.txt 
          }   
      }
  else{   
      char txt[16]="000.txt";    
      char macAddr[16];
      sprintf(macAddr, "_%2X%2X%2X%2X%2X%2X_", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
      strcat(filenameERR,config.UBXfile);//copy filename from config
      strcat(filenameERR,macAddr);
      int filenameSize=strlen(filenameERR);//dit is dan 7 + NULL = 8
      strcat(filenameERR,txt);//dit wordt dan /BN280A000.txt
      for(int i=0;i<1000;i++){
            filenameERR[filenameSize+2] = '0' + i%10;
            filenameERR[filenameSize+1] = '0' + ((i / 10) % 10);
            filenameERR[filenameSize] = '0' + ((i / 100) % 10);          
            // create if does not exist, do not open existing, write, sync after write
            if (!SD.exists(filenameERR)) {
                              break;
                            }
            }
     }     
  strcpy(filename_NO_EXT,filenameERR);
  filename_NO_EXT [strlen(filename_NO_EXT) - 3] = 0;  // move null-terminator three positions back
  strcpy(filenameUBX,filename_NO_EXT);
  strcat(filenameUBX,"ubx");
  strcpy(filenameSBP,filename_NO_EXT);
  strcat(filenameSBP,"sbp");
  strcpy(filenameGPY,filename_NO_EXT);
  strcat(filenameGPY,"gpy");
  strcpy(filenameGPX,filename_NO_EXT);
  strcat(filenameGPX,"gpx");   
            if(config.logUBX==true){
                  ubxfile=SD.open(filenameUBX, FILE_APPEND);
                  //ubxfile.setBufferSize(4096);
                  //if(setvbuf(file, NULL, _IOFBF, 4096) != 0) {}//enlarge buffer SD handle error
                  }
            #if defined(GPY_H)
            if(config.logGPY==true){
                gpyfile=SD.open(filenameGPY,FILE_APPEND);
                log_GPY_Header(gpyfile);
                }
            #endif
            if(config.logSBP==true){
                sbpfile=SD.open(filenameSBP,FILE_APPEND);
                log_header_SBP(sbpfile);      
                }
             if(config.logGPX==true){
                gpxfile=SD.open(filenameGPX,FILE_APPEND); 
                log_GPX(GPX_HEADER,gpxfile);    
                } 
             if(config.logTXT==true){       
                errorfile=SD.open(filenameERR, FILE_APPEND); 
                }    
}
void Close_files(void){
  log_GPX(GPX_END,gpxfile);
  gpxfile.close();
  ubxfile.close();
  errorfile.close();
  gpyfile.close();
  sbpfile.close();
}
void Flush_files(void){
  if(config.sample_rate<=10){//@18Hz still lost points !!!
    static int load_balance=0;
    if(load_balance==0)ubxfile.flush();
    if(load_balance==1)errorfile.flush();
    if(load_balance==2)gpyfile.flush();
    if(load_balance==3)sbpfile.flush();
    if(load_balance==4){gpxfile.flush();load_balance=-1;}
    load_balance++;
    }  
}
void Add_String(void){
      strcat(dataStr, Buffer);//add it onto the end
      strcat( dataStr, ";"); //append the delimeter
      }
void Log_to_SD(void){
            if(Time_Set_OK==true){
                static long old_iTOW;
                static int interval;
                interval=ubxMessage.navPvt.iTOW-old_iTOW;
                old_iTOW=ubxMessage.navPvt.iTOW;            
                if((interval>time_out_nav_pvt)&(sdOK==true)&(nav_pvt_message_nr>10)){//check for timeout navPvt message !!
                     next_gpy_full_frame=1;
                     dataStr[0] = 0;
                     dtostrf(ubxMessage.navPvt.hour, 2, 0, Buffer);AddString(); 
                     dtostrf(ubxMessage.navPvt.minute, 2, 0, Buffer);AddString(); 
                     dtostrf(ubxMessage.navPvt.second, 2, 0, Buffer);AddString(); 
                     ltoa(nav_pvt_message_nr,Buffer,10);AddString();
                     strcat(dataStr, "Lost ubx frame!\n");
                     if(config.logTXT) {errorfile.print(dataStr);}
                     Serial.print("Lost ubx frame");
                     Serial.println(interval);
                    }
                if(config.logUBX==true){    
                    ubxfile.write(0xB5);
                    ubxfile.write(0x62);
                    ubxfile.write((const uint8_t *)&ubxMessage.navPvt, sizeof(ubxMessage.navPvt));
                   
                    static int old_nav_sat_message=0;
                    if(nav_sat_message!=old_nav_sat_message){
                      old_nav_sat_message=nav_sat_message;
                      ubxfile.write(0xB5);
                      ubxfile.write(0x62);
                      ubxfile.write((const uint8_t *)&ubxMessage.navSat,(ubxMessage.navSat.len+6)); //nav_sat has a variable length, add chkA and chkB !!!
                      }
                    }
                    if(config.logUBX_nav_sat){//only add navDOP msg to ubx file if nav_sat active
                        ubxfile.write(0xB5);
                        ubxfile.write(0x62);
                        ubxfile.write((const uint8_t *)&ubxMessage.navDOP, sizeof(ubxMessage.navDOP));
                        }
                #if defined(GPY_H)    
                if(config.logGPY==true){  
                  log_GPY(gpyfile);   
                  }
                #endif  
                if(config.logSBP==true){  
                  log_SBP(sbpfile); 
                  } 
                 if(config.logGPX==true){   
                  log_GPX(GPX_FRAME,gpxfile); 
                 }
                
        }
                
}              
// Loads the configuration from a file
void loadConfiguration(const char *filename, const char *filename_backup, Config &config) {
  // Open file for reading
  File file;
  if(SD.exists(filename)){
    Serial.println(F("open the config.txt"));
    file = SD.open(filename);
  }else if(SD.exists(filename_backup)){
    Serial.println(F("open the config_backup.txt"));
    file = SD.open(filename_backup);
  }else{
    Serial.println(F("no configuration file found"));
    wifi_search=120;//elongation SoftAP mode to 120s !!!
  }
 
  StaticJsonDocument<1024> doc;
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
                Serial.println(F("Failed to deserialize file, using default configuration"));
                Serial.println(error.f_str());
                config.config_fail=1;
              }
  // Copy values from the JsonDocument to the Config
  config.cal_bat = doc["cal_bat"]|1.75;
  config.cal_speed = doc["cal_speed"]|3.6;
  config.sample_rate = doc["sample_rate"]|1;
  config.gnss = doc["gnss"]|2;
  config.field = doc["speed_field"]|1;
  config.speed_large_font=doc["speed_large_font"]|0;
  config.bar_length = doc["bar_length"]|1852;
  config.Stat_screens = doc["Stat_screens"]|12;
  config.Stat_screens_time = doc["Stat_screens_time"]|2;
  config.stat_speed= doc["stat_speed"]|1;
  config.archive_days= doc["archive_days"]|0;
  config.Stat_screens_persist = config.Stat_screens;
  config.GPIO12_screens = doc["GPIO12_screens"]|12;
  config.GPIO12_screens_persist = config.GPIO12_screens; 
  config.Board_Logo = doc["Board_Logo"]|1;
  config.Sail_Logo = doc["Sail_Logo"]|1;
  config.sleep_off_screen = doc["sleep_off_screen"]|11;
  config.logTXT=doc["logTXT"]|1;
  config.logUBX=doc["logUBX"]|1;
  config.logUBX_nav_sat=doc["logUBX_nav_sat"]|0;
  config.logSBP=doc["logSBP"]|1;
  config.logGPY=doc["logGPY"]|1;
  config.logGPX=doc["logGPX"]|0;
  config.file_date_time=doc["file_date_time"]|1;
  config.dynamic_model = doc["dynamic_model"]|0;//sea model does not give a gps-fix if actual height is not on sea-level, better use model "portable"=0 !!!
  config.timezone = doc["timezone"]|2;
  
  strlcpy(config.UBXfile,                  // <- destination
          doc["UBXfile"] | "/ubxGPS",  // <- source
          sizeof(config.UBXfile));         // <- destination's capacity 
  strlcpy(config.Sleep_info,                  // <- destination
          doc["Sleep_info"] | "My ID",  // <- source
          sizeof(config.Sleep_info));         // <- destination's capacity
          strcpy(RTC_Sleep_txt,config.Sleep_info);    //copy into RTC mem     
  strlcpy(config.ssid,                  // <- destination
          doc["ssid"] | "ssidNOK",  // <- source
          sizeof(config.ssid));         // <- destination's capacity
  strlcpy(config.password,                  // <- destination
          doc["password"] | "/BN220C000.ubx",  // <- source
          sizeof(config.password));         // <- destination's capacity          
   // Close the file (Curiously, File's destructor doesn't close the file)
  file.close();
  if (error) {
              Serial.println(config.cal_bat);
              Serial.println(config.cal_speed);
              Serial.println(config.sample_rate);
              Serial.println(config.logSBP);
              Serial.println(config.logUBX);
              Serial.println(config.ssid);
              Serial.println(config.password);
              Serial.println(config.Sail_Logo);
              }
  RTC_Board_Logo=config.Board_Logo;//copy RTC memory !!
  RTC_Sail_Logo=config.Sail_Logo;//copy to RTC memory !!
  calibration_bat=config.cal_bat;
  calibration_speed=config.cal_speed/1000;//3.6=km/h, 1.94384449 = knots, speed is now in mm/s
  time_out_nav_pvt=(1000/config.sample_rate+75);//max time out = 175 ms
  RTC_SLEEP_screen=config.sleep_off_screen%10;
  RTC_OFF_screen=config.sleep_off_screen/10%10;
  //int Logo_choice=config.Logo_choice;//preserve value config.Logo_choice for config.txt update !!
  int stat_screen=config.Stat_screens;//preserve value config
  int GPIO_12_screens=config.GPIO12_screens;//preserve value config 
  if(config.file_date_time==0) config.logTXT=1;//because txt file is needed for generating new file count !!
  for (int i=0;i<9;i++){
        config.stat_screen[i]=stat_screen%10;//STATSx heeft geen offset !!! 641
        stat_screen=stat_screen/10;
        if(stat_screen>0){
            config.screen_count=i+1;
            }
        config.gpio12_screen[i]=GPIO_12_screens%10;//
        GPIO_12_screens=GPIO_12_screens/10;
        if(GPIO_12_screens>0){
            config.gpio12_count=i+1;
            }
       }
}
// Prints the content of a file to the Serial
void printFile(const char *filename) {
  // Open file for reading
  File file = SD.open(filename);
  if (!file.available()) {
      Serial.println(F("Failed to read file"));
      return;
      }
  // Extract each characters by one by one
  while (file.available()) {
      Serial.print((char)file.read());
      }
  Serial.println();
  // Close the file
  file.close();
}
void AddString(void){
        strcat(dataStr, Buffer);//add it onto the end
        strcat(dataStr, ":"); //append the delimeter
        }

void Model_info(int model){
  if(config.logTXT){
  char tekst[20]="";char message[255]="";
  errorfile.print("Dynamic model: "); 
  if(model==1)errorfile.print("Sea"); 
  else errorfile.print("Portable"); 
  strcat(message, " Msg_nr: ");
  dtostrf(nav_pvt_message_nr, 1, 0, tekst);
  strcat(message,tekst); 
  errorfile.println(message); 
  }
}
void Session_info(GPS_data G){
  char tekst[32]="";char message[512]="";
  errorfile.print("T5 MAC adress: "); 
  for(int i=0;i<6;i++) errorfile.print(mac[i],HEX);
  errorfile.println(" ");
  errorfile.println(SW_version);
  sprintf(tekst, "First fix : %d s\n",first_fix_GPS);
  strcat(message,tekst); 
  sprintf(tekst, "Total time : %d s\n",(millis()-start_logging_millis)/1000);
  strcat(message,tekst); 
  sprintf(tekst, "Total distance : %d m\n",(int)G.total_distance/1000);
  strcat(message,tekst); 
  sprintf(tekst, "Sample rate : %d Hz\n",config.sample_rate);
  strcat(message,tekst);
  sprintf(tekst, "Speed calibration: %f \n",config.cal_speed); 
  strcat(message,tekst); 
  sprintf(tekst, "Timezone : %d h\n",config.timezone);
  strcat(message,tekst); 
  strcat(message,"Dynamic model: ");
  if(config.dynamic_model==1) strcat(message,"Sea");
  else if (config.dynamic_model==2) strcat(message,"Automotive");
  else strcat(message,"Portable");
  strcat(message," \n");
  sprintf(tekst, "Ublox GNSS-enabled : %d\n",ubxMessage.monGNSS.enabled_Gnss);
  strcat(message,tekst);
  if(ubxMessage.monGNSS.enabled_Gnss==3) strcat(message,"GNSS = GPS + GLONAS");
  if(ubxMessage.monGNSS.enabled_Gnss==9) strcat(message,"GNSS = GPS + GALILEO");  
  if(ubxMessage.monGNSS.enabled_Gnss==11) strcat(message,"GNSS = GPS + GLONAS + GALILEO");
  if(ubxMessage.monGNSS.enabled_Gnss==13) strcat(message,"GNSS = GPS + GLONAS + BEIDOU");
  strcat(message," \n");
  strcat(message,"Ublox SW-version : ");
  strcat(message,ubxMessage.monVER.swVersion);
  strcat(message," \n");
  strcat(message,"Ublox HW-version : ");
  strcat(message,ubxMessage.monVER.hwVersion);
  strcat(message," \n");
  #if defined(UBLOX_M10)
  sprintf(tekst,"Ublox M10 ID = %02x%02x%02x%02x%02x%02x\n",ubxMessage.ubxId.ubx_id_1,ubxMessage.ubxId.ubx_id_2,ubxMessage.ubxId.ubx_id_3,ubxMessage.ubxId.ubx_id_4,ubxMessage.ubxId.ubx_id_5,ubxMessage.ubxId.ubx_id_6);
  #else
  sprintf(tekst,"Ublox M8 ID = %02x%02x%02x%02x%02x\n",ubxMessage.ubxId.ubx_id_1,ubxMessage.ubxId.ubx_id_2,ubxMessage.ubxId.ubx_id_3,ubxMessage.ubxId.ubx_id_4,ubxMessage.ubxId.ubx_id_5);
  #endif
  strcat(message,tekst);
  strcat(message,Ublox_type);
  strcat(message," \n");
  errorfile.print(message);                  
}

void Session_results_M(GPS_speed M){ 
  for(int i=9;i>4;i--){
        char tekst[20]="";char message[255]="";
        int Calibration=config.cal_speed*1000;
        dtostrf(M.avg_speed[i]*calibration_speed, 1, 3, tekst);
        strcat(message,tekst); 
        if(Calibration==3600)strcat(message, " km/h "); 
        if((Calibration>=1943)&(Calibration<=1945))strcat(message, " knots "); 
        dtostrf(M.time_hour[i], 1, 0, tekst);
        strcat(message,tekst); 
        strcat(message, ":"); 
        dtostrf(M.time_min[i], 1, 0, tekst);
        strcat(message,tekst); 
        strcat(message, ":"); 
        dtostrf(M.time_sec[i], 1, 0, tekst);
        strcat(message,tekst); 
        strcat(message, " Distance: ");
        dtostrf(M.m_Distance[i]/1000.0f/config.sample_rate, 1, 2, tekst);
        strcat(message,tekst); 
        strcat(message, " Msg_nr: ");
        dtostrf(M.message_nr[i], 1, 0, tekst);
        strcat(message,tekst); 
        strcat(message, " Samples: ");
        dtostrf(M.nr_samples[i], 1, 0, tekst);
        strcat(message,tekst);   
        strcat(message, " Run: "); 
        dtostrf(M.this_run[i], 1, 0, tekst);
        strcat(message,tekst); 
        strcat(message, " M");
        dtostrf(M.m_set_distance, 1, 0, tekst);
        strcat(message,tekst); 
        strcat(message, "\n");
        errorfile.print(message); 
        }
}
void Session_results_S(GPS_time S){ 
  char tekst[20]="";char message[255]="";
  int Calibration=config.cal_speed*1000;
  dtostrf(S.avg_5runs*calibration_speed, 1, 3, tekst);
  strcat(message,tekst); 
  if(Calibration==3600)strcat(message, " km/h avg 5_best_runs\n"); 
  else if((Calibration>=1943)&(Calibration<=1945))strcat(message, " knots avg 5_best_runs\n"); 
  else strcat(message, " avg 5_best_runs\n"); 
  //errorfile.open();   
  errorfile.print(message);
  //errorfile.close();
  //appendFile(SD,filenameERR,message); 
  for(int i=9;i>4;i--){
      char tekst[20]="";char message[255]="";
      dtostrf(S.avg_speed[i]*calibration_speed, 1, 3, tekst);
      strcat(message,tekst);
      if(Calibration==3600)strcat(message, " km/h "); 
      if((Calibration>=1943)&(Calibration<=1945))strcat(message, " knots ");  
      dtostrf(S.time_hour[i], 1, 0, tekst);
      strcat(message,tekst); 
      strcat(message, ":"); 
      dtostrf(S.time_min[i], 1, 0, tekst);
      strcat(message,tekst); 
      strcat(message, ":"); 
      dtostrf(S.time_sec[i], 1, 0, tekst);
      strcat(message,tekst); 
      strcat(message, " Run: "); 
      dtostrf(S.this_run[i], 1, 0, tekst);
      strcat(message,tekst); 
      strcat(message, " S");
      dtostrf(S.time_window, 1, 0, tekst);
      strcat(message,tekst); 
      if(config.logUBX_nav_sat){
        sprintf(tekst," CNO Max: %d Avg: %d Min: %d nr Sat: %d\n",S.Max_cno[i],S.Mean_cno[i],S.Min_cno[i],S.Mean_numSat[i]);
        strcat(message,tekst);
        } 
      else strcat(message, "\n"); 
      errorfile.print(message);    
      }
}
void Session_results_Alfa(Alfa_speed A,GPS_speed M){ 
  for(int i=9;i>4;i--){
      char tekst[20]="";char message[255]="";
      int Calibration=config.cal_speed*1000;
      dtostrf(A.avg_speed[i]*calibration_speed, 1, 3, tekst);
      strcat(message,tekst); 
      if(Calibration==3600)strcat(message, " km/h "); 
      if(Calibration==1943)strcat(message, " knots "); 
      dtostrf(sqrt((float)A.real_distance[i]), 1, 2, tekst);
      strcat(message,tekst); 
      strcat(message, " m "); 
      dtostrf(A.alfa_distance[i], 1, 1, tekst);
      strcat(message,tekst); 
      strcat(message, " m "); 
      dtostrf(A.time_hour[i], 1, 0, tekst);
      strcat(message,tekst); 
      strcat(message, ":"); 
      dtostrf(A.time_min[i], 1, 0, tekst);
      strcat(message,tekst); 
      strcat(message, ":"); 
      dtostrf(A.time_sec[i], 1, 0, tekst);
      strcat(message,tekst); 
      strcat(message, " Run: "); 
      dtostrf(A.this_run[i], 1, 0, tekst);
      strcat(message,tekst);
      strcat(message, " Msg_nr: ");
      dtostrf(A.message_nr[i], 1, 0, tekst);
      strcat(message,tekst); 
      strcat(message, " Alfa");
      dtostrf(M.m_set_distance, 1, 0, tekst);
      strcat(message,tekst); 
      strcat(message, "\n"); 
      errorfile.print(message);   
      }
}
