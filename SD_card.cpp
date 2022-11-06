#include "SD_card.h"
#include <SD.h>
#include "Rtos5.h"
#include "Oao.h"

File ubxfile;
File errorfile;
File oaofile;

char filenameUBX[64]="/";
char filenameERR[64]="/";
char filenameOAO[64]="/";
char dataStr[255] = "";//string for logging NMEA in txt, test for write 2000 chars !!
char Buffer[50]= "";//string for logging
union OAO_Frame oao_pvt ;
union OAO_Header oao_header;
void logERR(const char * message){
  errorfile.print(message);
}
//test for existing GPSLOGxxxfiles, open txt,oao + ubx file with new name !
void Open_files(void){
  logUBX=config.logUBX;
  logOAO=config.logOAO;
  strcat(filenameERR,config.UBXfile);
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
 
  strcpy(filenameOAO,filenameERR);
  filenameOAO[filenameSize+3]='.';
  filenameOAO[filenameSize+4]='o';
  filenameOAO[filenameSize+5]='a';
  filenameOAO[filenameSize+6]='o';
           
            if(logUBX==true){
                  ubxfile=SD.open(filenameUBX, FILE_APPEND);
                  /*ubxfile.print("T5 MAC adress: "); 
                  for(int i=0;i<6;i++) ubxfile.print(mac[i],HEX);
                  ubxfile.print(SW_version);
                  ubxfile.println("");
                  */
                  }
            if(logOAO==true){
                oaofile=SD.open(filenameOAO,FILE_APPEND);
                log_header_OAO();      
                }
            
            errorfile=SD.open(filenameERR, FILE_APPEND);
            Serial.println(filenameUBX); 
            Serial.println(filenameERR); 
            Serial.println(filenameOAO); 
}
void Close_files(void){
  ubxfile.close();
  errorfile.close();
  oaofile.close();
}
void Flush_files(void){
  if(config.sample_rate<=10){//@18Hz still lost points !!!
    ubxfile.flush();
    errorfile.flush();
    oaofile.flush();
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
                if(logOAO==true){  
                  log_OAO();   
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
                     //errorfile.open();   
                     errorfile.print(dataStr);
                     //errorfile.close();
                     //appendFile(SD,filenameERR,dataStr);
                     Serial.print("Lost ubx frame");
                     Serial.println(interval);
              }
        }
                
}              
// Loads the configuration from a file
void loadConfiguration(const char *filename, Config &config) {
  // Open file for reading
  File file = SD.open(filename);
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
  config.GPIO12_screens = doc["GPIO12_screens"]|12;
  config.Logo_choice = doc["Logo_choice"]|12;
  config.sleep_off_screen = doc["sleep_off_screen"]|11;
  config.stat_speed= doc["stat_speed"]|1;
  config.bar_length = doc["bar_length"]|1852;
  config.logCSV=doc["logCSV"]|0;
  config.logUBX=doc["logUBX"]|1;
  config.logOAO=doc["logOAO"]|1;
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
              Serial.println(config.logCSV);
              Serial.println(config.logUBX);
              Serial.println(config.ssid);
              Serial.println(config.password);
              }
  calibration_bat=config.cal_bat;
  calibration_speed=config.cal_speed/1000;//3.6=km/h, 1.94384449 = knots, speed is now in mm/s
  time_out_nav_pvt=(1000/config.sample_rate+150);//max time out = 150 ms
  SLEEP_screen=config.sleep_off_screen%10;
  OFF_screen=config.sleep_off_screen/10%10;
  for (int i=0;i<9;i++){
        config.stat_screen[i]=config.Stat_screens%10;//STATSx heeft geen offset !!! 641
        config.Stat_screens=config.Stat_screens/10;
        if(config.Stat_screens>0){
            config.screen_count=i+1;
            }
        config.gpio12_screen[i]=config.GPIO12_screens%10;//
        config.GPIO12_screens=config.GPIO12_screens/10;
        if(config.GPIO12_screens>0){
            config.gpio12_count=i+1;
            }
        logo_choice[i]=config.Logo_choice%10;//
        config.Logo_choice=config.Logo_choice/10;
        if(config.Logo_choice>0){
            config.logo_count=i+1;
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
  char tekst[20]="";char message[255]="";//Serial.print("T5 MAC adress: ");
  errorfile.print("T5 MAC adress: "); 
  for(int i=0;i<6;i++) errorfile.print(mac[i],HEX);
  errorfile.println(" ");
  errorfile.println(SW_version);
 // strcat(message,"T5 MAC adress: ");  
 // strcat(message,(char*)mac);//Serial.println(WiFi.macAddress());
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
void log_header_OAO(void){
  oao_header.mode=0x0AD0;
  oao_header.identifier=987;
  //oao_header.nickname="HeynenJan";//moet exact 10 bytes zijn ??
  checksum_verify(512,oao_header.bytes);
  oaofile.write((const uint8_t *)&oao_header.bytes,512); 
  //oaofile.seek(0);//test om file pointer naar een positie te zetten
}
void log_OAO(void){
time_t utcSec=tmConvert_t(ubxMessage.navPvt.year, ubxMessage.navPvt.month, ubxMessage.navPvt.day, ubxMessage.navPvt.hour, ubxMessage.navPvt.minute, ubxMessage.navPvt.second);
int64_t utc=(int64_t)utcSec*1000+(ubxMessage.navPvt.nano+500000)/1000000;//om af te ronden
oao_pvt.utc_gnss=utc;
//Serial.printf("UTC littleEndian: %llu\n", oao_pvt.utc_gnss); 
if(ubxMessage.navPvt.nano/1000000==0){oao_pvt.mode=0x0AD4;}
else {oao_pvt.mode=0x0AD5;}
//Serial.printf("Nano: %u\n",ubxMessage.navPvt.nano/1000000);}
oao_pvt.latitude=ubxMessage.navPvt.lat;
oao_pvt.longitude=ubxMessage.navPvt.lon;
oao_pvt.altitude=ubxMessage.navPvt.hMSL;
oao_pvt.speed=ubxMessage.navPvt.gSpeed;//hier kan de max versnelling begrensd worden !!
oao_pvt.heading=ubxMessage.navPvt.heading;
oao_pvt.fix=ubxMessage.navPvt.fixType;
oao_pvt.satellites=ubxMessage.navPvt.numSV;
oao_pvt.accuracy_speed=ubxMessage.navPvt.sAcc;
oao_pvt.accuracy_horizontal=ubxMessage.navPvt.hAcc;
oao_pvt.accuracy_vertical=ubxMessage.navPvt.vAcc;
oao_pvt.accuracy_heading=ubxMessage.navPvt.headingAcc;
oao_pvt.accuracy_pDOP=ubxMessage.navPvt.pDOP;
checksum_verify(52,oao_pvt.bytes_gnss);
oaofile.write((const uint8_t *)&oao_pvt.bytes_gnss,52);
}
