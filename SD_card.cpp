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
  errorfile.print(message);
}
//test for existing GPSLOGxxxfiles, open txt,gps + ubx file with new name !
void Open_files(void){
  strcat(filenameERR,config.UBXfile);//copy filename from config
  char txt[16]="000.txt";
  char macAddr[16];
  sprintf(macAddr, "_%2X%2X%2X%2X%2X%2X_", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
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
            errorfile=SD.open(filenameERR, FILE_APPEND);     
}
void Close_files(void){
  Set_GPS_Time(config.timezone);
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
                     errorfile.print(dataStr);
                     Serial.print("Lost ubx frame");
                     Serial.println(interval);
                    }
                if(config.logUBX==true){    
                    //write ubx string to sd, added check for nr of bytes written
                    int total_bytes_written=0;
                    int bytes_written=0;
                    nav_pvt_message_nr++; 
                    bytes_written=ubxfile.write(0xB5);total_bytes_written=bytes_written;
                    bytes_written=ubxfile.write(0x62);total_bytes_written=total_bytes_written+bytes_written;
                    bytes_written=ubxfile.write((const uint8_t *)&ubxMessage.navPvt, sizeof(ubxMessage.navPvt));
                    total_bytes_written+bytes_written;
                    if(total_bytes_written>100) {errorfile.print("Write_error SD");Serial.print("Write error SD");}
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
  //Serial.print((char)file.read());
  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/v6/assistant to compute the capacity.
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
  config.Stat_screens_persist = config.Stat_screens;
  config.GPIO12_screens = doc["GPIO12_screens"]|12;
  config.GPIO12_screens_persist = config.GPIO12_screens; 
  config.Board_Logo = doc["Board_Logo"]|1;
  config.Sail_Logo = doc["Sail_Logo"]|1;
  config.sleep_off_screen = doc["sleep_off_screen"]|11;
  config.logCSV=doc["logCSV"]|0;
  config.logUBX=doc["logUBX"]|1;
  config.logSBP=doc["logSBP"]|1;
  config.logGPY=doc["logGPY"]|1;
  config.logGPX=doc["logGPX"]|0;
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
  char tekst[20]="";char message[255]="";
  errorfile.print("Dynamic model: "); 
  if(model==1)errorfile.print("Sea"); 
  else errorfile.print("Portable"); 
  strcat(message, " Msg_nr: ");
  dtostrf(nav_pvt_message_nr, 1, 0, tekst);
  strcat(message,tekst); 
  errorfile.println(message); 
}
void Session_info(GPS_data G){
  char tekst[32]="";char message[512]="";
  errorfile.print("T5 MAC adress: "); 
  for(int i=0;i<6;i++) errorfile.print(mac[i],HEX);
  errorfile.println(" ");
  errorfile.println(SW_version);
  strcat(message, "RTOS5, First_fix: "); 
  dtostrf(first_fix_GPS, 1, 0, tekst);
  strcat(message,tekst); 
  strcat(message," s\nTotal time: "); 
  dtostrf(millis()/1000, 1, 0, tekst);
  strcat(message,tekst); 
  strcat(message," s\nTotal Distance: "); 
  dtostrf(G.total_distance/1000, 1, 0, tekst);
  strcat(message,tekst); 
  strcat(message," m\nSample rate: ");
  dtostrf(config.sample_rate, 1, 0, tekst);
  strcat(message,tekst); 
  strcat(message," Hz\n");
  strcat(message,"Speed calibration: ");
  dtostrf(config.cal_speed, 1, 5, tekst);
  strcat(message,tekst); 
  strcat(message," \n");
  strcat(message,"Dynamic model: ");
  if(config.dynamic_model==1) strcat(message,"Sea");
  else if (config.dynamic_model==2) strcat(message,"Automotive");
  else strcat(message,"Portable");
  strcat(message," \n");
  strcat(message,"Ublox MON GNSS : ");
  dtostrf(ubxMessage.monGNSS.enabled_Gnss,1,0,tekst);
  strcat(message,tekst); 
  strcat(message," \n");
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
  strcat(message,"Ublox GNSS-enabled : ");
  dtostrf(ubxMessage.monGNSS.enabled_Gnss, 1, 0, tekst);
  strcat(message,tekst); 
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
      strcat(message, "\n"); 
      //errorfile.open();
      errorfile.print(message);    
      //errorfile.write((const uint8_t *)&message, sizeof(message));
      //errorfile.close(); 
      //appendFile(SD,filenameERR,message);
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
