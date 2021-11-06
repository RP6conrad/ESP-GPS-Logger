#include "GPS_data.h"
#include "Ublox.h"
#include "Rtos5.h"

uint16_t _gSpeed[BUFFER_SIZE]; 
float _lat[BUFFER_ALFA]; 
float _long[BUFFER_ALFA];
uint16_t _secSpeed[BUFFER_SIZE];
int index_GPS=-1;//bij eerste doorgang op 0 beginnen !!
int index_sec=-1;//bij eerste doorgang op 0 beginnen !!

//Deze functie gaat telkens 3 variabelen van de GPS in een globale buffer steken : doppler snelheid, lat en long.
//Er is gekozen voor een globale buffer omdat deze data ook beschikbaar moeten zijn in andere classes (GPS_speed() en GPS_time).
//Ook de laatste bufferpositie wordt opgeslagen in een globale variable, index_GPS

void GPS_data::push_data(float latitude,float longitude,uint32_t gSpeed) {//gspeed in mm/s !!!
    static int dynamic_state=0;
    if((S2.avg_s>20000)&(config.dynamic_model==1)&(dynamic_state==0)){  //omschakelen naar dynamic_model "portable", only works with speed<25 m/s !!!
          dynamic_state=1;                   //test with 5 m/s, this is 18 km/h
          Serial.print("Set ublox UBX_PORTABLE ");
          Model_info(0);
          for(int i = 0; i < sizeof(UBX_PORTABLE); i++) {                        
              Serial2.write( pgm_read_byte(UBX_PORTABLE+i) );
              }
      }
    if((S2.avg_s<15000)&(config.dynamic_model==1)&dynamic_state==1){  //omschakelen naar dynamic_model "portable", only works with speed<25 m/s !!!
              dynamic_state=0;               //test with 4.5 m/s, this is 16.2 km/h
              Serial.print("Set ublox UBX_SEA ");
              Model_info(1);
              for(int i = 0; i < sizeof(UBX_SEA); i++) {                        
                  Serial2.write( pgm_read_byte(UBX_SEA+i) );
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
//constructor van GPS_data
GPS_data::GPS_data() {
  index_GPS=0; 
}

void sort_run(double a[], uint8_t hour[], uint8_t minute[],uint8_t seconde[],int runs[], int size) {
    for(int i=0; i<(size-1); i++) {
        for(int o=0; o<(size-(i+1)); o++) {
                if(a[o] > a[o+1]) {
                    double t = a[o];int b=hour[o];int c=minute[o];int d=seconde[o];int e=runs[o];
                    a[o] = a[o+1];hour[o] = hour[o+1];minute[o] = minute[o+1];seconde[o]=seconde[o+1];runs[o]=runs[o+1];
                    a[o+1] = t; hour[o+1] = b; minute[o+1] = c;seconde[o+1]=d;runs[o+1]=e;
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
        message_nr[0]=nav_pvt_message_nr;
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
float GPS_time::Update_speed(int actual_run){
  if(time_window*config.sample_rate<BUFFER_SIZE){      //indien tijdvenster kleiner is dan de sample_rate*BUFFER, normale buffer gebruiken
        avg_s_sum=avg_s_sum+_gSpeed[index_GPS%BUFFER_SIZE]; //altijd gSpeed optellen bij elke update
        if(index_GPS>=time_window*config.sample_rate){
            avg_s_sum=avg_s_sum-_gSpeed[(index_GPS-(time_window*config.sample_rate))%BUFFER_SIZE];//vanaf 10s bereikt, terug -10s aftrekken van som
            }
            avg_s=(double)avg_s_sum/time_window/config.sample_rate;
            if(s_max_speed<avg_s){
              s_max_speed=avg_s;
              getLocalTime(&tmstruct, 0);
              time_hour[0]=tmstruct.tm_hour;
              time_min[0]=tmstruct.tm_min;
              time_sec[0]=tmstruct.tm_sec;
              this_run[0]=actual_run;
              avg_speed[0]=s_max_speed; 
              //Om de avg te actualiseren tijdens de run, gemiddelde berekenen van niet gesorteerde array  !
              if(s_max_speed>avg_speed[5]){
                  avg_5runs=0;
                  for(int i=6;i<10;i++){
                    avg_5runs=avg_5runs+avg_speed[i];
                    }
                  avg_5runs=avg_5runs+avg_speed[0]; 
                  avg_5runs=avg_5runs/5;
                }
               if(s_max_speed>avg_speed[9])display_max_speed=s_max_speed;//update on the fly, dat klopt hier niet !!!
               else display_max_speed=avg_speed[9];
              }
            if((actual_run!=old_run)&(this_run[0]==old_run)){          //sorting only if new max during this run !!!
              sort_run(avg_speed,time_hour,time_min,time_sec,this_run,10);
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
           // }
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
                  sort_run(avg_speed,time_hour,time_min,time_sec,this_run,10);
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

Alfa_speed::Alfa_speed(int circle){
    alfa_circle=circle;
}
/*
 * Opgelet, hier moet de afgelegde afstand kleiner zijn dan 500 m! daarom extra variable in GPS_speed voorzien, m_speed_alfa !!!
 */
float Alfa_speed::Update_Alfa(GPS_speed M){
  //nu de absolute afstfloat Alfa_speed::Alfa_update(GPS_speed M)and berekenen tussen het beginpunt en het eindpunt van de 250m afstand, indien < 50m is dit een alfa !!!
  //opgelet, dit wordt berekend in meter, daarom alfa_circle ook in m !!
  straight_dist= sqrt(pow((_lat[index_GPS%BUFFER_ALFA]-_lat[(M.m_index-1)%BUFFER_ALFA]),2)+pow(cos(DEG2RAD*_lat[index_GPS%BUFFER_ALFA])*(_long[index_GPS%BUFFER_ALFA]-_long[(M.m_index-1)%BUFFER_ALFA]),2))*111120;
  if(straight_dist<alfa_circle){
    alfa_speed=M.m_speed_alfa;
    if(M.m_sample>=BUFFER_ALFA) alfa_speed=0;//overflow vermijden bij lage snelheden
    if(alfa_speed>alfa_speed_max){
          alfa_speed_max=alfa_speed;
          real_distance[0]=(int)straight_dist;
          }
    }
  if((alfa_speed_max>0.0f)&(straight_dist>(M.m_set_distance*0.4))){
      getLocalTime(&tmstruct, 0);
      time_hour[0]=tmstruct.tm_hour;
      time_min[0]=tmstruct.tm_min;
      time_sec[0]=tmstruct.tm_sec;
      this_run[0]=alfa_count;
      avg_speed[0]=alfa_speed_max; 
      message_nr[0]=nav_pvt_message_nr;
      sort_run_alfa(avg_speed,real_distance,message_nr,time_hour,time_min,time_sec,this_run,this_run,10);
      char tekst[20]="";char message[255]=""; 
      strcat(message, " alfa_speed "); 
      dtostrf(M.m_set_distance, 3, 0, tekst);
      strcat(message, "m "); 
      dtostrf(alfa_speed_max*calibration_speed, 2, 2, tekst);
      strcat(message,tekst); 
      strcat(message,"\n");    
      //logERR(message);
      alfa_speed_max=0;alfa_speed=0;
      }    
  return alfa_speed_max; 
}
int New_run_detection(float actual_heading, float S2_speed){
   /*Berekening van de gemiddelde heading over de laatste 10s************************************************************************/
   static float old_heading,delta_heading,heading;
   static uint32_t delay_counter;
   static int run_counter;
   static bool velocity_0 = false;
   static bool velocity_5 = false;
   //float headAcc=ubxMessage.navPvt.headingAcc/100000.0f;  //heading Accuracy wordt niet gebruikt ???  
   //actual_heading=ubxMessage.navPvt.heading/100000.0f;
   if((actual_heading-old_heading)>300.0f) delta_heading=delta_heading-360.0f;
   if((actual_heading-old_heading)<-300.0f) delta_heading=delta_heading+360.0f;
   old_heading=actual_heading;
   heading=actual_heading+delta_heading;
   /*detectie heading change over 15s is more then 40°, nieuwe run wordt gestart !!***************************************************************************/
   int mean_heading_time=15;//tijd in s voor berekening gemiddelde heading
   int straight_course_max=10;//max hoek afwijking voor rechtdoor herkenning
   int course_deviation_min=50;//min hoek afwijking om gijp te detecteren, was 40
   int time_delay_new_run=TIME_DELAY_NEW_RUN;//vertraging om nieuwe run te starten, sw 4.59
   heading_SD=heading;
   Mean_heading=Mean_heading*(mean_heading_time*config.sample_rate-1)/(mean_heading_time*config.sample_rate)+heading/(mean_heading_time*config.sample_rate);
   /*detection stand still, more then 2s with velocity<1m/s**************************************************************************************************/
   int speed_detection_min=5000;//minimum snelheid in mm/s voor snelheid display
   int standstill_detection_max=1500;//maximum snelheid in mm/s voor stilstand herkenning
   if(S2_speed>speed_detection_min)velocity_5=1;    //min gemiddelde over 2 s = 1m/s           
   if((S2_speed<standstill_detection_max)&(velocity_5==1))velocity_0=1;
   else velocity_0=0;
   /*Nieuwe run gedetecteerd*********************************************************************************************************************************/
   static bool straight_course;
   if(abs(Mean_heading-heading)<straight_course_max){straight_course=true;}//stabiele koers terug bereikt
   if(((abs(Mean_heading-heading)>course_deviation_min)&(straight_course==true))|(velocity_0==1)){      
      velocity_5=0;
      straight_course=false;
      delay_counter=0;
      //run_counter++;//delay for new run ....
      }
   delay_counter++;   
   if(delay_counter==(time_delay_new_run*config.sample_rate)) run_counter++;   
   return run_counter;   
}



