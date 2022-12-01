#include "SD_card.h"
#include <SD.h>
#include "Rtos5.h"
//#include "Oao.h"

File ubxfile;
File errorfile;
File gpsfile;//new open source file format, work in progress !!
File sbpfile;
char filenameUBX[64]="/";
char filenameERR[64]="/";
char filenameGPS[64]="/";
char filenameSBP[64]="/";
char dataStr[255] = "";//string for logging NMEA in txt, test for write 2000 chars !!
char Buffer[50]= "";//string for logging

struct SBP_Header sbp_header;
struct SBP_frame sbp_frame;

void logERR(const char * message){
  errorfile.print(message);
}
//test for existing GPSLOGxxxfiles, open txt,gps + ubx file with new name !
void Open_files(void){
  logUBX=config.logUBX;
  logGPS=config.logGPS;
  logSBP=config.logSBP;
  strcat(filenameERR,config.UBXfile);//copy filename from config
  char txt[16]="000.txt";
  char macAddr[32];
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
  strcpy(filenameUBX,filenameERR);
  filenameUBX[filenameSize+3]='.';
  filenameUBX[filenameSize+4]='u';
  filenameUBX[filenameSize+5]='b';
  filenameUBX[filenameSize+6]='x';
 
  strcpy(filenameGPS,filenameERR);
  filenameGPS[filenameSize+3]='.';
  filenameGPS[filenameSize+4]='o';
  filenameGPS[filenameSize+5]='a';
  filenameGPS[filenameSize+6]='o';

  strcpy(filenameSBP,filenameERR);
  filenameSBP[filenameSize+3]='.';
  filenameSBP[filenameSize+4]='s';
  filenameSBP[filenameSize+5]='b';
  filenameSBP[filenameSize+6]='p';
           
            if(logUBX==true){
                  ubxfile=SD.open(filenameUBX, FILE_APPEND);
                  }
            #if defined(GPS_H)
            if(logGPS==true){
                gpsfile=SD.open(filenameGPS,FILE_APPEND);
                log_header_GPS();      
                }
            #endif
            if(logSBP==true){
                sbpfile=SD.open(filenameSBP,FILE_APPEND);
                log_header_SBP();      
                }
            errorfile=SD.open(filenameERR, FILE_APPEND);
            Serial.println(filenameUBX); 
            Serial.println(filenameERR); 
            Serial.println(filenameGPS); 
}
void Close_files(void){
  ubxfile.close();
  errorfile.close();
  gpsfile.close();
  sbpfile.close();
}
void Flush_files(void){
  if(config.sample_rate<=10){//@18Hz still lost points !!!
    ubxfile.flush();
    errorfile.flush();
    gpsfile.flush();
    sbpfile.flush();
    }  
}
void Add_String(void){
      strcat(dataStr, Buffer);//add it onto the end
      strcat( dataStr, ";"); //append the delimeter
      }
void Log_to_SD(void){
            if(Time_Set_OK==true){
                if(logUBX==true){    
                    //write ubx string to sd, last 2 bytes with checksum are missing !!
                    nav_pvt_message_nr++; 
                    ubxfile.write(0xB5);ubxfile.write(0x62);//ook nog header toevoegen !
                    ubxfile.write((const uint8_t *)&ubxMessage.navPvt, sizeof(ubxMessage.navPvt));
                    ubxfile.write(checksumA);ubxfile.write(checksumB);//checksum toevoegen
                    }
                 #if defined(GPS_H)    
                if(logGPS==true){  
                  log_GPS();   
                  }
                #endif  
                if(logSBP==true){  
                  log_SBP();   
                  }  
                static long old_iTOW;
                static int interval;
                interval=ubxMessage.navPvt.iTOW-old_iTOW;
                old_iTOW=ubxMessage.navPvt.iTOW;            
                if((interval>time_out_nav_pvt)&(sdOK==true)&(Time_Set_OK==true)){
                     dataStr[0] = 0;
                     dtostrf(ubxMessage.navPvt.hour, 2, 0, Buffer);AddString(); 
                     dtostrf(ubxMessage.navPvt.minute, 2, 0, Buffer);AddString(); 
                     dtostrf(ubxMessage.navPvt.second, 2, 0, Buffer);AddString(); 
                     ltoa(interval,Buffer,10);AddString();
                     strcat(dataStr, "Lost ubx frame!\n");
                     errorfile.print(dataStr);
                     Serial.print("Lost ubx frame");
                     Serial.println(interval);
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
  config.dynamic_model = doc["dynamic_model"]|0;//sea model does not give a gps-fix if actual height is not on sea-level, better use model "portable"=0 !!!
  config.timezone = doc["timezone"]|2;
  config.Stat_screens = doc["Stat_screens"]|12;
  config.Stat_screens_persist = config.Stat_screens;
  config.GPIO12_screens = doc["GPIO12_screens"]|12;
  config.GPIO12_screens_persist = config.GPIO12_screens;
  config.Logo_choice = doc["Logo_choice"]|12;
  config.sleep_off_screen = doc["sleep_off_screen"]|11;
  config.stat_speed= doc["stat_speed"]|1;
  config.bar_length = doc["bar_length"]|1852;
  config.logCSV=doc["logCSV"]|0;
  config.logUBX=doc["logUBX"]|1;
  config.logSBP=doc["logSBP"]|1;
  strlcpy(config.UBXfile,                  // <- destination
          doc["UBXfile"] | "/ubxGPS",  // <- source
          sizeof(config.UBXfile));         // <- destination's capacity 
  strlcpy(config.Sleep_info,                  // <- destination
          doc["Sleep_info"] | "My ID",  // <- source
          sizeof(config.Sleep_info));         // <- destination's capacity
          strcpy(Sleep_txt,config.Sleep_info);    //copy into RTC mem     
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
              Serial.println(config.Logo_choice);
              }
  calibration_bat=config.cal_bat;
  calibration_speed=config.cal_speed/1000;//3.6=km/h, 1.94384449 = knots, speed is now in mm/s
  time_out_nav_pvt=(1000/config.sample_rate+150);//max time out = 150 ms
  SLEEP_screen=config.sleep_off_screen%10;
  OFF_screen=config.sleep_off_screen/10%10;
  int Logo_choice=config.Logo_choice;//preserve value config.Logo_choice for config.txt update !!
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
        //add special logos
        if(Logo_choice > 99){
          logo_choice[0]=Logo_choice;
        }else{ // else default logos
          logo_choice[i]=Logo_choice%10;//
          Logo_choice=Logo_choice/10;
          if(Logo_choice>0){
              config.logo_count=i+1;
                }      
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
  char tekst[20]="";char message[255]="";
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
  if(config.gnss==3) strcat(message,"GNSS = GPS + GLONAS");
  if(config.gnss==11) strcat(message,"GNSS = GPS + GLONAS + GALILEO");
  strcat(message," \n");
  errorfile.print(message);                  
}

void Session_results_M(GPS_speed M){ 
  for(int i=9;i>0;i--){
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
  for(int i=9;i>0;i--){
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
  for(int i=9;i>0;i--){
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

void log_header_SBP(void){
  for (int i=35;i<62;i++){sbp_header.str[i]=0xFF;}//fill with 0xFF
  sbpfile.write((const uint8_t *)&sbp_header,64);
}
void log_SBP(void){
uint32_t year=ubxMessage.navPvt.year;
uint8_t month=ubxMessage.navPvt.month;
uint8_t day=ubxMessage.navPvt.day;
uint8_t hour=ubxMessage.navPvt.hour;
uint16_t min=ubxMessage.navPvt.minute;
uint16_t sec=ubxMessage.navPvt.second;
uint32_t numSV=0xFFFFFFFF;
uint32_t PDOP=(ubxMessage.navPvt.pDOP+1)/20;//from mm/s to m/s with 0.2 resolution,reformat pDOP to HDOP 8-bit !!
if(PDOP>255)PDOP=255;//has to fit in 8 bit
uint32_t sdop=ubxMessage.navPvt.sAcc/10;//was sAcc
if(sdop>255)sdop=255;
uint32_t vsdop=ubxMessage.navPvt.vAcc/10;//was headingAcc
if (vsdop>255)vsdop=255;
sbp_frame.UtcSec=ubxMessage.navPvt.second*1000+(ubxMessage.navPvt.nano+500000)/1000000;//om af te ronden
sbp_frame.date_time_UTC_packed=(((year-2000)*12+month)<<22) +(day<<17)+(hour<<12)+(min<<6)+sec; 
sbp_frame.Lat=ubxMessage.navPvt.lat;
sbp_frame.Lon=ubxMessage.navPvt.lon;
sbp_frame.AltCM=ubxMessage.navPvt.hMSL/10;//omrekenen naar cm/s
sbp_frame.Sog=ubxMessage.navPvt.gSpeed/10;//omrekenen naar cm/s
sbp_frame.Cog=ubxMessage.navPvt.heading/1000;//omrekenen naar 0.01 degrees
sbp_frame.SVIDCnt=ubxMessage.navPvt.numSV;
sbp_frame.SVIDList=numSV>>(32-ubxMessage.navPvt.numSV);
sbp_frame.HDOP=PDOP;
sbp_frame.ClmbRte=-ubxMessage.navPvt.velD/10;//omrekenen naar cm/s
sbp_frame.sdop=sdop;
sbp_frame.vsdop=vsdop;
sbpfile.write((const uint8_t *)&sbp_frame,32);
}
