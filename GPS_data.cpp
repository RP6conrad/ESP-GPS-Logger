#include <EEPROM.h>
#include "GPS_data.h"
#include "Ublox.h"
#include "Definitions.h"

uint16_t _gSpeed[BUFFER_SIZE]; 
float _lat[BUFFER_ALFA]; 
float _long[BUFFER_ALFA];
float alfa_exit;//test voor functie afstand punt tot lijn !!
uint16_t _secSpeed[BUFFER_SIZE];
int index_GPS=-1;//bij eerste doorgang op 0 beginnen !!
int index_sec=-1;//bij eerste doorgang op 0 beginnen !!
int alfa_counter;

//Deze functie gaat telkens 3 variabelen van de GPS in een globale buffer steken : doppler snelheid, lat en long.
//Er is gekozen voor een globale buffer omdat deze data ook beschikbaar moeten zijn in andere classes (GPS_speed() en GPS_time).
//Ook de laatste bufferpositie wordt opgeslagen in een globale variable, index_GPS

void GPS_data::push_data(float latitude,float longitude,uint32_t gSpeed) {//gspeed in mm/s !!!
    static int dynamic_state=0;
    if((S2.avg_s>24000)&(config.dynamic_model==1)&(dynamic_state==0)){  //omschakelen naar dynamic_model "portable", only works with speed<25 m/s !!!
          dynamic_state=1;                   //test with 5 m/s, this is 18 km/h
          Serial.print("Set ublox UBX_PORTABLE ");
          Model_info(0);
          if((config.ublox_type==M8_9600BD)|(config.ublox_type==M8_38400BD)){
            for(int i = 0; i < sizeof(UBX_PORTABLE); i++) {                        
                Serial2.write( pgm_read_byte(UBX_PORTABLE+i) );
                }
          }     
          else{ 
          for(int i = 0; i < sizeof(UBX_M10_PORTABLE); i++) {                        
                Serial2.write( pgm_read_byte(UBX_M10_PORTABLE+i) );
                }     
          }    
      }
    if((S2.avg_s<20000)&(config.dynamic_model==1)&(dynamic_state==1)){  //omschakelen naar dynamic_model "portable", only works with speed<25 m/s !!!
              dynamic_state=0;               //test with 4.5 m/s, this is 16.2 km/h
              Serial.print("Set ublox UBX_SEA ");
              Model_info(1);
              if((config.ublox_type==M8_9600BD)|(config.ublox_type==M8_38400BD)){
              for(int i = 0; i < sizeof(UBX_SEA); i++) {                        
                  Serial2.write( pgm_read_byte(UBX_SEA+i) );
                  }
               }
              else{
              for(int i = 0; i < sizeof(UBX_M10_SEA); i++) {                        
                  Serial2.write( pgm_read_byte(UBX_M10_SEA+i) );
                  } 
              }
          }      
    index_GPS++;//altijd index ophogen na update alle instanties  
  _gSpeed[index_GPS%BUFFER_SIZE]=gSpeed;//altijd gSpeed opslaan in array bereik !
  _lat[index_GPS%BUFFER_ALFA]=latitude;
  _long[index_GPS%BUFFER_ALFA]=longitude;
   //alleen afstand optellen als ontvangst goed is, opgelet af en toe sAcc<2  !!!****************************************************
  if((ubxMessage.navPvt.numSV>=FILTER_MIN_SATS)&((ubxMessage.navPvt.sAcc/1000.0f)<FILTER_MAX_sACC)){
        delta_dist=gSpeed/config.sample_rate;//snelheid omrekenen naar afstand !!!
        total_distance=total_distance+delta_dist;
        run_distance=run_distance+delta_dist;
        alfa_distance=alfa_distance+delta_dist;
        }  
  //Opslaan groundSpeed in seconden tact !!**********************************************************************************
  static int avg_gSpeed;//in mm/s
  avg_gSpeed=avg_gSpeed+gSpeed;//in seconden tact opslaan voor 30 min / 60 min gemiddelde snelheid
  if(index_GPS%config.sample_rate==0){    //modulus van index%sample rate
    index_sec++;//ook index_sec mag pas geupdated worden na update instantie
    _secSpeed[index_sec%BUFFER_SIZE]=avg_gSpeed/config.sample_rate;//anders overflow want _secSpeed[] is maar tot 65535 !!!!
    avg_gSpeed=0;
  }
 }
//constructor for GPS_data
GPS_data::GPS_data() {
  index_GPS=0; 
}
//constructor for SAT_info
GPS_SAT_info::GPS_SAT_info() {
  index_SAT_info=0; 
}
//function to extract info out of NAV_SAT, and push it to array
//For every NAV_SAT frame, the Mean CNO, the Max cno, the Min cno and the nr of sats in the nav solution are stored
//Then, the means are calculated out of the last NAV_SAT_BUFFER frames (now 16 frames, @5Hz, this 0.5Hz NAV_SAT ca 32 s)
void GPS_SAT_info::push_SAT_info(struct NAV_SAT nav_sat){
  //#define NAV_SAT_BUFFER 10
  mean_cno=0;min_cno=0xFF;max_cno=0;nr_sats=0;
  for(int i=0;i<nav_sat.numSvs;i++){      //only evaluate nr of Sats that is in NAV_SAT
    if(nav_sat.sat[i].X4&0x8){       //only evaluate nr of Sats who are in nav solution, bit3 from X4
      mean_cno=mean_cno+nav_sat.sat[i].cno;
      if(nav_sat.sat[i].cno<min_cno) min_cno=nav_sat.sat[i].cno;
      if(nav_sat.sat[i].cno>max_cno) max_cno=nav_sat.sat[i].cno;
      nr_sats++;
      }
    }
  if(nr_sats){ //protection divide int/0 !!
    mean_cno=mean_cno/nr_sats;
    sat_info.Mean_cno[index_SAT_info%NAV_SAT_BUFFER]=mean_cno;
    sat_info.Max_cno[index_SAT_info%NAV_SAT_BUFFER]=max_cno;
    sat_info.Min_cno[index_SAT_info%NAV_SAT_BUFFER]=min_cno;
    sat_info.numSV[index_SAT_info%NAV_SAT_BUFFER]=nr_sats;
    mean_cno=0;min_cno=0;max_cno=0;nr_sats=0;
    if(index_SAT_info>NAV_SAT_BUFFER){
      for(int i=0;i<NAV_SAT_BUFFER;i++){
        mean_cno=mean_cno+sat_info.Mean_cno[(index_SAT_info-NAV_SAT_BUFFER+i)%NAV_SAT_BUFFER];
        max_cno=max_cno+sat_info.Max_cno[(index_SAT_info-NAV_SAT_BUFFER+i)%NAV_SAT_BUFFER];
        min_cno=min_cno+sat_info.Min_cno[(index_SAT_info-NAV_SAT_BUFFER+i)%NAV_SAT_BUFFER];
        nr_sats=nr_sats+sat_info.numSV[(index_SAT_info-NAV_SAT_BUFFER+i)%NAV_SAT_BUFFER]; 
      }
    mean_cno=mean_cno/NAV_SAT_BUFFER;  
    max_cno=max_cno/NAV_SAT_BUFFER;  
    min_cno=min_cno/NAV_SAT_BUFFER;  
    nr_sats=nr_sats/NAV_SAT_BUFFER;
    sat_info.Mean_mean_cno=mean_cno;
    sat_info.Mean_max_cno=max_cno;
    sat_info.Mean_min_cno=min_cno;
    sat_info.Mean_numSV=nr_sats;
    } 
    index_SAT_info++; 
}


};
void sort_display(double a[],int size){
  for(int i=0; i<(size-1); i++) {
        for(int o=0; o<(size-(i+1)); o++) {
                if(a[o] > a[o+1]) {
                    double t = a[o];
                    a[o] = a[o+1];
                    a[o+1] = t;
                    }
        }
  }     
}
void sort_run(double a[], uint8_t hour[], uint8_t minute[],uint8_t seconde[],uint8_t mean_cno[],uint8_t max_cno[],uint8_t min_cno[],uint8_t nrSats[],int runs[], int size) {
    for(int i=0; i<(size-1); i++) {
        for(int o=0; o<(size-(i+1)); o++) {
                if(a[o] > a[o+1]) {
                    double t = a[o];int b=hour[o];int c=minute[o];int d=seconde[o];int e=runs[o];int f=mean_cno[o];int g=max_cno[o];int h=min_cno[o];int j=nrSats[o];
                    a[o] = a[o+1];hour[o] = hour[o+1];minute[o] = minute[o+1];seconde[o]=seconde[o+1];runs[o]=runs[o+1];mean_cno[o]=mean_cno[o+1];max_cno[o]=max_cno[o+1];min_cno[o]=min_cno[o+1];nrSats[o]=nrSats[o+1];
                    a[o+1] = t; hour[o+1] = b; minute[o+1] = c;seconde[o+1]=d;runs[o+1]=e;mean_cno[o+1]=f;max_cno[o+1]=g;min_cno[o+1]=h;nrSats[o+1]=j;
                }
        }
    }
}
void sort_run_alfa(double a[], int dis[],int message[],uint8_t hour[], uint8_t minute[],uint8_t seconde[],int runs[], int samples[],int size) {
    for(int i=0; i<(size-1); i++) {
        for(int o=0; o<(size-(i+1)); o++) {
                if(a[o] > a[o+1]) {
                    double t = a[o];int v=dis[o];int x=message[o];int b=hour[o];int c=minute[o];int d=seconde[o];int e=runs[o];int f=samples[o];
                    a[o] = a[o+1];dis[o] = dis[o+1];message[o]=message[o+1];hour[o] = hour[o+1];minute[o] = minute[o+1];seconde[o]=seconde[o+1];runs[o]=runs[o+1];samples[o]=samples[o+1];
                    a[o+1] = t; dis[o+1] = v;message[o+1]=x;hour[o+1] = b; minute[o+1] = c;seconde[o+1]=d;runs[o+1]=e;samples[o+1]=f;
                }
        }
    }
}
/*Instantie om gemiddelde snelheid over een bepaalde afstand te bepalen, bij een nieuwe run opslaan hoogste snelheid van de vorige run*****************/
GPS_speed::GPS_speed(int afstand){
  m_set_distance=afstand;  
}
double GPS_speed::Update_distance(int actual_run){ 
  m_Set_Distance=m_set_distance*1000*config.sample_rate;//opgelet, m_set_distance moet nu in mm, dus *1000 + functie van sample_rate !! 
  m_distance=m_distance+_gSpeed[index_GPS%BUFFER_SIZE];//resolutie = 0.1 mm nu, 2,147,483,647 = 214748 m, dus maar 214 km !! 
  if((index_GPS-m_index)>=BUFFER_SIZE){     //controle buffer overflow
      m_distance=0;
      m_index=index_GPS;
      }
  if(m_distance>m_Set_Distance){          //buffer m_index van gewenste afstand bepalen
        while(m_distance>m_Set_Distance&&(index_GPS-m_index)<BUFFER_SIZE){    
              m_distance=m_distance-_gSpeed[m_index%BUFFER_SIZE];
              m_distance_alfa=m_distance;
              m_index++;
              }
        m_index--;    
        m_distance=m_distance+_gSpeed[m_index%BUFFER_SIZE];
        } 
  m_sample=index_GPS-m_index+1; //controle mogelijk van aantal samples
  //Protection divide by zero !!!
  if(index_GPS-m_index+1){
      m_speed=(double)m_distance/m_sample; //10 samples op 1s aan 10mm/s = 100/10 = 10 mm /s
  }
  if(index_GPS-m_index){
    m_speed_alfa=(double)m_distance_alfa/(index_GPS-m_index); 
    }   
  if(m_distance<m_Set_Distance) m_speed=0; //dit om foute snelheid te voorkomen indien afstand nog niet bereikt!!
  if(m_sample>=BUFFER_SIZE) m_speed=0; //dit om foute snelheid te voorkomen bij BUFFER_SIZE overflow !!
  if(m_speed==0) m_speed_alfa=0;
  if(m_max_speed<m_speed){
        m_max_speed=m_speed;
        getLocalTime(&tmstruct, 0);
        time_hour[0]=tmstruct.tm_hour;
        time_min[0]=tmstruct.tm_min;
        time_sec[0]=tmstruct.tm_sec;
        this_run[0]=actual_run;//om berekening te checken
        avg_speed[0]=m_max_speed; 
        m_Distance[0]=m_distance;
        nr_samples[0]=m_sample;
        message_nr[0]=nav_pvt_message;
        }
   if(m_max_speed>avg_speed[9])display_max_speed=m_max_speed;//update on the fly, dat klopt hier niet !!!
   else display_max_speed=avg_speed[9];      
  if((actual_run!=old_run)&(this_run[0]==old_run)){              //opslaan hoogste snelheid van run + sorteren
      sort_run_alfa(avg_speed,m_Distance,message_nr,time_hour,time_min,time_sec,this_run,nr_samples,10);
      avg_speed[0]=0;
      m_max_speed=0;
      }
  old_run=actual_run;
  return m_max_speed;
}

/*Instantie om gemiddelde snelheid over een bepaald tijdvenster te bepalen*******************************************/
GPS_time::GPS_time(int tijdvenster){
  time_window=tijdvenster;
}
void GPS_time::Reset_stats(void){
  for (int i=0;i<10;i++){
    avg_speed[i]=0;
    display_speed[i]=0;
  }
  avg_5runs=0;
}
float GPS_time::Update_speed(int actual_run){
  if(time_window*config.sample_rate<BUFFER_SIZE){      //indien tijdvenster kleiner is dan de sample_rate*BUFFER, normale buffer gebruiken
        avg_s_sum=avg_s_sum+_gSpeed[index_GPS%BUFFER_SIZE]; //altijd gSpeed optellen bij elke update
        if(index_GPS>=time_window*config.sample_rate){
            avg_s_sum=avg_s_sum-_gSpeed[(index_GPS-(time_window*config.sample_rate))%BUFFER_SIZE];//vanaf 10s bereikt, terug -10s aftrekken van som
            }
            avg_s=(double)avg_s_sum/time_window/config.sample_rate;
            if(s_max_speed<avg_s){
              s_max_speed=avg_s;
              speed_run[actual_run%NR_OF_BAR]=avg_s;
              getLocalTime(&tmstruct, 0);
              time_hour[0]=tmstruct.tm_hour;
              time_min[0]=tmstruct.tm_min;
              time_sec[0]=tmstruct.tm_sec;
              this_run[0]=actual_run;
              avg_speed[0]=s_max_speed; 
              Mean_cno[0]=Ublox_Sat.sat_info.Mean_mean_cno;
              Max_cno[0]=Ublox_Sat.sat_info.Mean_max_cno;
              Min_cno[0]=Ublox_Sat.sat_info.Mean_min_cno;
              Mean_numSat[0]=Ublox_Sat.sat_info.Mean_numSV;
              //Om de avg te actualiseren tijdens de run, gemiddelde berekenen van niet gesorteerde array  !
              if(s_max_speed>avg_speed[5]){
                  avg_5runs=0;
                  for(int i=6;i<10;i++){
                    avg_5runs=avg_5runs+avg_speed[i];
                    }
                  avg_5runs=avg_5runs+avg_speed[0]; 
                  avg_5runs=avg_5runs/5;
                  display_speed[5]=s_max_speed;//actuele run is sneller dan run[5] !!
                  for (int i=9;i>5;i--){        //andere runs kopieren
                     display_speed[i]=avg_speed[i];
                    }
                  sort_display(display_speed,10);  
                }
               if(s_max_speed>avg_speed[9])display_max_speed=s_max_speed;//update on the fly, dat klopt hier niet !!!
               else display_max_speed=avg_speed[9];
              }
            if((actual_run!=old_run)&(this_run[0]==old_run)){          //sorting only if new max during this run !!!
              sort_run(avg_speed,time_hour,time_min,time_sec,Mean_cno,Max_cno,Min_cno,Mean_numSat,this_run,10);
              if(s_max_speed>5000)speed_run_counter ++;//changes SW5.51 min speed bar graph = 5 m/s
              for(int i=0;i<10;i++){
                  display_speed[i]=avg_speed[i];//om een directe update op het scherm mogelijk te maken
                  }
              speed_run[actual_run%NR_OF_BAR]=avg_speed[0];    //SW 5.5
              avg_speed[0]=0;
              s_max_speed=0;
              avg_5runs=0;
              for(int i=5;i<10;i++){
                    avg_5runs=avg_5runs+avg_speed[i];
                    }
                avg_5runs=avg_5runs/5;
              }
            if((actual_run!=reset_display_last_run)&(avg_s>3000)){
              reset_display_last_run=actual_run;
              display_last_run=0;
              }
            else if(display_last_run<s_max_speed){
                display_last_run=s_max_speed;
                }    
            old_run=actual_run;
            return s_max_speed;
  }
  else if(index_GPS%config.sample_rate==0){        //overschakelen naar seconden buffer, maar één update/seconde !!
            avg_s_sum=avg_s_sum+(int)_secSpeed[index_sec%BUFFER_SIZE]; //_secSpeed[BUFFER_SIZE] en index_sec 
            if(index_sec>=time_window){
                avg_s_sum=avg_s_sum-(int)_secSpeed[(index_sec-time_window)%BUFFER_SIZE];//vanaf 10s bereikt, terug -10s aftrekken van som
                }
            avg_s=(double)avg_s_sum/time_window;//in de seconden array staat de gemiddelde van gspeed !!
            //Serial.print("avg_s ");Serial.println(avg_s);
            if(s_max_speed<avg_s){
                s_max_speed=avg_s;
                getLocalTime(&tmstruct, 0);
                time_hour[0]=tmstruct.tm_hour;
                time_min[0]=tmstruct.tm_min;
                time_sec[0]=tmstruct.tm_sec;
                this_run[0]=actual_run;
                avg_speed[0]=s_max_speed;   //s_max_speed niet resetten bij elke run !!!
                }
            if(s_max_speed>avg_speed[9])display_max_speed=s_max_speed;//update on the fly voor S1800 / S3600 
            else display_max_speed=avg_speed[9];
            if((actual_run!=old_run)&(this_run[0]==old_run)){   //sorting only if new max during this run !!!
                  //sort_run(avg_speed,time_hour,time_min,time_sec,this_run,10);
                  sort_run(avg_speed,time_hour,time_min,time_sec,Mean_cno,Max_cno,Min_cno,Mean_numSat,this_run,10);
                  avg_speed[0]=0;
                  s_max_speed=0;
                  avg_5runs=0;
                  for(int i=5;i<10;i++){
                    avg_5runs=avg_5runs+avg_speed[i];
                    }
                  avg_5runs=avg_5runs/5;
                  }
            old_run=actual_run;
            return s_max_speed;
            } 
    //}
    return s_max_speed;//anders compiler waarschuwing control reaches end of non-void function [-Werror=return-type]
 }

Alfa_speed::Alfa_speed(int alfa_radius){
    alfa_circle_square=alfa_radius*alfa_radius;//to avoid sqrt calculation !!
}
/*
 * Opgelet, hier moet de afgelegde afstand kleiner zijn dan 500 m! daarom extra variable in GPS_speed voorzien, m_speed_alfa !!!
 */
float Alfa_speed::Update_Alfa(GPS_speed M){
  //nu de absolute afstfloat Alfa_speed::Alfa_update(GPS_speed M)and berekenen tussen het beginpunt en het eindpunt van de 250m afstand, indien < 50m is dit een alfa !!!
  //opgelet, dit wordt berekend in meter, daarom alfa_circle ook in m !!
  //was (M.m_index-1), moet (M.m_index+1)
  straight_dist_square= (pow((_lat[index_GPS%BUFFER_ALFA]-_lat[(M.m_index+1)%BUFFER_ALFA]),2)+pow(cos(DEG2RAD*_lat[index_GPS%BUFFER_ALFA])*(_long[index_GPS%BUFFER_ALFA]-_long[(M.m_index+1)%BUFFER_ALFA]),2))*111195*111195;//was 111120
  if(straight_dist_square<alfa_circle_square){
    alfa_speed=M.m_speed_alfa;
    if(M.m_sample>=BUFFER_ALFA) alfa_speed=0;//overflow vermijden bij lage snelheden
    if(alfa_speed>alfa_speed_max){
          alfa_speed_max=alfa_speed;
          real_distance[0]=(int)straight_dist_square;
          getLocalTime(&tmstruct, 0);
          time_hour[0]=tmstruct.tm_hour;
          time_min[0]=tmstruct.tm_min;
          time_sec[0]=tmstruct.tm_sec;
          this_run[0]=alfa_counter;//was alfa_count
          avg_speed[0]=alfa_speed_max; 
          message_nr[0]=nav_pvt_message;
          alfa_distance[0]=M.m_distance_alfa/config.sample_rate;
          }
    }
  //if((alfa_speed_max>0.0f)&(straight_dist_square>(alfa_circle_square*1.4))){//alfa max gaat pas op 0 indien 500 m na de gijp, rechte afstand na de gijp
  if(run_count!=old_run_count){ 
      sort_run_alfa(avg_speed,real_distance,message_nr,time_hour,time_min,time_sec,alfa_distance,this_run,10);
      char tekst[20]="";char message[255]=""; 
      strcat(message, " alfa_speed "); 
      dtostrf(M.m_set_distance, 3, 0, tekst);
      strcat(message, "m "); 
      dtostrf(alfa_speed_max*calibration_speed, 2, 2, tekst);
      strcat(message,tekst); 
      strcat(message,"\n");    
      //logERR(message);
      alfa_speed=0;alfa_speed_max=0;
      }
  old_run_count=run_count;    
  if(alfa_speed_max>avg_speed[9]) display_max_speed=alfa_speed_max;//update on the fly, dat klopt hier niet !!!
  else display_max_speed=avg_speed[9];           
  return alfa_speed_max; 
}
void Alfa_speed::Reset_stats(void){
  for (int i=0;i<10;i++){
    avg_speed[i]=0;
  }
}
int New_run_detection(float actual_heading, float S2_speed){
   /*Berekening van de gemiddelde heading over de laatste 10s************************************************************************/
   #define SPEED_DETECTION_MIN 4000       //min average speed over 2s for new run detection (mm/s)
   #define STANDSTILL_DETECTION_MAX 1000  //max average speed over 2s voor stand still detection (mm/s)
   #define MEAN_HEADING_TIME 15           //tijd in s voor berekening gemiddelde heading
   #define STRAIGHT_COURSE_MAX_DEV 10     //max hoek afwijking voor rechtdoor herkenning (graden)
   #define JIBE_COURSE_DEVIATION_MIN 50   //min hoek afwijking voor dettectie jibe (graden)

   static float old_heading,delta_heading,heading;
   static uint32_t delay_counter;
   static int run_counter;
   static bool velocity_0 = false;
   static bool velocity_5 = false;
   int speed_detection_min=SPEED_DETECTION_MIN;//minimum snelheid 4m/s (14 km/h)voor snelheid display
   int standstill_detection_max=STANDSTILL_DETECTION_MAX;//maximum snelheid 1 m/s (3.6 km/h) voor stilstand herkenning, was 1.5 m/s change SW5.51
   //float headAcc=ubxMessage.navPvt.headingAcc/100000.0f;  //heading Accuracy wordt niet gebruikt ???  
   //actual_heading=ubxMessage.navPvt.heading/100000.0f;
   if((actual_heading-old_heading)>300.0f) delta_heading=delta_heading-360.0f;
   if((actual_heading-old_heading)<-300.0f) delta_heading=delta_heading+360.0f;
   old_heading=actual_heading;
   heading=actual_heading+delta_heading;
   /*detectie heading change over 15s is more then 40°, nieuwe run wordt gestart !!***************************************************************************/
   int mean_heading_time=MEAN_HEADING_TIME;//tijd in s voor berekening gemiddelde heading
   int straight_course_max=STRAIGHT_COURSE_MAX_DEV;//max hoek afwijking voor rechtdoor herkenning
   int course_deviation_min=JIBE_COURSE_DEVIATION_MIN;//min hoek afwijking om gijp te detecteren, was 40
   int time_delay_new_run=TIME_DELAY_NEW_RUN;//vertraging om nieuwe run te starten, sw 4.59
   heading_SD=heading;
   Mean_heading=Mean_heading*(mean_heading_time*config.sample_rate-1)/(mean_heading_time*config.sample_rate)+heading/(mean_heading_time*config.sample_rate);
   /*detection stand still, more then 2s with velocity<1m/s**************************************************************************************************/
   if(S2_speed>speed_detection_min)velocity_5=1;    //snelheid was hoger dan 4m/s        
   if((S2_speed<standstill_detection_max)&(velocity_5==1))velocity_0=1;//snelheid is kleiner dan 1m/s
   else velocity_0=0;
   /*Nieuwe run gedetecteerd omwille stilstand **********************************************************************************************************************/
   if(velocity_0==1){
     velocity_5=0;
     delay_counter=0;
    }
   /*Nieuwe run gedetecteerd omwille heading change*****************************************************************************************************************/
   static bool straight_course;
   //if(abs(Mean_heading-heading)<straight_course_max){straight_course=true;}//stabiele koers terug bereikt
   if((abs(Mean_heading-heading)<straight_course_max)&(S2_speed>speed_detection_min)){straight_course=true;}//stabiele koers terug bereikt, added min_speed SW5.51
   if(((abs(Mean_heading-heading)>course_deviation_min)&(straight_course==true))){      
      straight_course=false;
      delay_counter=0;
      alfa_counter++;//jibe detection for alfa_indicator ....
      }
   delay_counter++;   
   if(delay_counter==(time_delay_new_run*config.sample_rate)) run_counter++;   
   return run_counter;   
}

/*hier wordt de actuele "alfa afstand" berekend aan de hand van 2 punten voor de gijp : P1 = 250m en P2 = 100m voor de gijp
*Deze punten bepalen een imaginaire lijn, de loodrechte afstand tot de actuele positie moet kleiner zijn dan 50 m/s
*als het punt P1 gepasseerd wordt
*/
double delta_heading;
double ref_heading;
float Alfa_indicator(GPS_speed M250,GPS_speed M100,float actual_heading){
  static float P1_lat,P1_long,P2_lat,P2_long;
  float P_lat,P_long, P_lat_heading,P_long_heading;
  //,lambda_T,lambda_N,lambda,
  float alfa_afstand;
  static int old_alfa_counter;
  if(alfa_counter!=old_alfa_counter){
    Ublox.alfa_distance=0;//afstand afgelegd sinds jibe detectie      10*100.000/10.000=100 samples ?
    P1_lat=_lat[M250.m_index%BUFFER_ALFA];//dit is het punt op -250 m van de actuele positie
    P1_long=_long[M250.m_index%BUFFER_ALFA];
    P2_lat=_lat[M100.m_index%BUFFER_ALFA];//dit is het punt op -100 m van de actuele positie (snelheid extrapolatie van -250m)
    P2_long=_long[M100.m_index%BUFFER_ALFA]; 
    }
  old_alfa_counter=alfa_counter;  
  P_lat=_lat[index_GPS%BUFFER_ALFA];//actuele positie lat
  P_long=_long[index_GPS%BUFFER_ALFA];//actuele positie long
  /*
  float corr_lat=111120;
  float corr_long=111120*cos(DEG2RAD*_lat[index_GPS%BUFFER_ALFA]);
  lambda_T=(P2_lat-P1_lat)*(P_lat-P1_lat)*corr_lat*corr_lat+(P2_long-P1_long)*(P_long-P1_long)*corr_long*corr_long;
  lambda_N= pow((P2_lat-P1_lat)*corr_lat,2)+pow((P2_long-P1_long)*corr_long,2);
  lambda=lambda_T/lambda_N;
  alfa_afstand=sqrt(pow((P_lat-P1_lat-lambda*(P2_lat-P1_lat))*corr_lat,2)+pow((P_long-P1_long-lambda*(P2_long-P1_long))*corr_long,2));
  */
  P_lat_heading= _lat[(index_GPS-2*config.sample_rate)%BUFFER_ALFA];//-2s  positie lat         //cos(ubxMessage.navPvt.heading*PI/180.0f/100000.0f)*111120+P_lat;//was eerst sin,extra punt berekenen heading, berekenen met afstand/lengte graad !!
  P_long_heading=_long[(index_GPS-2*config.sample_rate)%BUFFER_ALFA];//-2s  positie long//sin(ubxMessage.navPvt.heading*PI/180.0f/100000.0f)*111120*cos(DEG2RAD*P_lat)+P_long;//berekenen met afstand/lengte graad!!
  alfa_exit=Dis_point_line(P1_long,P1_lat,P_long,P_lat,P_long_heading,P_lat_heading);//
  alfa_afstand=Dis_point_line(P_long,P_lat,P1_long,P1_lat,P2_long,P2_lat);
  return alfa_afstand;  //actuele loodrechte afstand tov de lijn P2-P1, mag max 50m zijn voor een geldige alfa !!
}
/*Calculates distance from point with coör lat/long to line which passes points lat_1/long_1 and lat_2/long_2**************************************/
float Dis_point_line(float long_act,float lat_act,float long_1,float lat_1,float long_2,float lat_2){
  float corr_lat=111195;            //meter per breedtegraad
  float corr_long=111195*cos(DEG2RAD*lat_act);//meter per lengtegraad, dit is f(breedtegraad) !
  float lambda_T,lambda_N,lambda,alfa_distance;
  lambda_T=(lat_2-lat_1)*(lat_act-lat_1)*corr_lat*corr_lat+(long_2-long_1)*(long_act-long_1)*corr_long*corr_long;
  lambda_N= pow((lat_2-lat_1)*corr_lat,2)+pow((long_2-long_1)*corr_long,2);
  lambda=lambda_T/lambda_N;
  alfa_distance=sqrt(pow((lat_act-lat_1-lambda*(lat_2-lat_1))*corr_lat,2)+pow((long_act-long_1-lambda*(long_2-long_1))*corr_long,2));
  return alfa_distance;
}
int setupGPS(void) {
  int Cpu_freq = getCpuFrequencyMhz();
  Serial.print("CPU freq 240 ?= "); Serial.println(Cpu_freq);
  //if((config.sample_rate>5)&(config.logUBX)) Cpu_freq = 80;
  setCpuFrequencyMhz(config.cpu_freq);
  config.cpu_freq = getCpuFrequencyMhz();
  int Xtal_freq = getXtalFrequencyMhz();
  Serial.print("CPU freq  ?= "); Serial.println(config.cpu_freq);
  Serial.print("XTAL freq  ?= "); Serial.println(Xtal_freq);
  Ublox_on();//beitian bn220 power supply over output 25,26,27
  Serial2.setRxBufferSize(2048); // increasing buffer size ?
  if((config.ublox_type==M8_38400BD)|(config.ublox_type==M9_38400BD)|  (config.ublox_type==M10_38400BD)){
    Serial2.begin(38400, SERIAL_8N1, RXD2, TXD2); //connection to ublox over serial2  
    }
  else{
   Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2); //connection to ublox over serial2
   }   
  Serial.println("Serial2 Txd is on pin: "+String(TXD2));
  Serial.println("Serial2 Rxd is on pin: "+String(RXD2));
  for(int i=0;i<425;i++){//Startup string van ublox to serial, ca 424 char !!
     while (Serial2.available()) {
              Serial.print(char(Serial2.read()));
              }
     delay(2);   //was delay (1)
     }
  config.ublox_type = EEPROM.read(0);
  if(config.ublox_type==0xFF) {
    Auto_detect_ublox();//only test for ublox type and baudrate if unknown in configuration
    if(config.ublox_type!=UBLOX_TYPE_UNKNOWN){
      EEPROM.write(0, config.ublox_type);
      EEPROM.commit();
      }
    else{
       Serial.println("Can't detect type and or baudrate of ublox....");
      }  
    }     
  if((config.ublox_type==M8_9600BD)|(config.ublox_type==M8_38400BD)){
    Init_ublox(); //switch to ubx protocol
    }
  if((config.ublox_type==M9_9600BD)|(config.ublox_type==M9_38400BD)|(config.ublox_type==M10_9600BD)|(config.ublox_type==M10_38400BD)){
    Init_ubloxM10(); //switch to ubx protocol, same for M9/M10
    }
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
  if((config.ublox_type==M8_9600BD)|(config.ublox_type==M8_38400BD)){
      Set_rate_ublox(config.sample_rate);//after reading config file !! 
  } 
  if((config.ublox_type==M9_9600BD)|(config.ublox_type==M9_38400BD)|(config.ublox_type==M10_9600BD)|(config.ublox_type==M10_38400BD)){
      Set_rate_ubloxM10(config.sample_rate);//after reading config file !! 
  } 
  return 1;
}
