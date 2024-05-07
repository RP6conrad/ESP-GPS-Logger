#ifndef GPS_DATA_H
#define GPS_DATA_H
#include <stdint.h>
#include <math.h>

#define DEG2RAD 0.0174532925  //is PI/180 !!!
#define BUFFER_SIZE 9000  //gewenste buffer grootte voor de GPS groundspeed data,opgelet voor 10Hz moet de buffer veel groter zijn !!! Vb 1852m bij 2000 samples is min 10m/s
//buffer verkleinen van 5000 tot 1000, zodat bij 2Hz en 600s toch 1 sec takt
#define BUFFER_ALFA 2000     //gewenste buffer grootte voor alfa speed berekening, hier moet maar 500 m afstand in kunnen !!10
#define FILTER_MIN_SATS 5   //aan 10 Hz is dit 200 s, dus laagste snelheid is dan 500m/200s, dit is 2.5m/s of <10 km/h
#define FILTER_MAX_sACC 2   
#define NR_OF_BAR 42 //aantal bar in de bar_graph
#define NAV_SAT_BUFFER 10 // GPS_SAT_info buffersize for Mean CNO  values, here 10 NAV_SAT messages
extern  int index_GPS,run_count;
extern  int index_sec;//index van laatste sample 
extern uint16_t _secSpeed[BUFFER_SIZE];
extern float alfa_exit;

// Description of the GPS data processing class
class GPS_data {
  public:
    GPS_data (); // description of the constructor
    float total_distance;
    float run_distance;
    float alfa_distance;
    float delta_dist;
    void push_data(float latitude,float longitude,uint32_t gSpeed); // hier wordt de gps data in de buffer geplaatst
   
  private:
};
class GPS_SAT_info{
  public:
    GPS_SAT_info();
    struct SAT_info{
      uint8_t Mean_cno[NAV_SAT_BUFFER];   //Mean cno value from all sats used in nav
      uint8_t Max_cno[NAV_SAT_BUFFER];    //Max cno value from all sats used in nav      
      uint8_t Min_cno[NAV_SAT_BUFFER];    //Min cno value from all sats used in nav
      uint8_t numSV[NAV_SAT_BUFFER];      //Nr of sats used in nav 
      uint8_t Mean_mean_cno ;  //Mean over last x NAV_SAT messages
      uint8_t Mean_max_cno ;  //Mean over last x NAV_SAT messages
      uint8_t Mean_min_cno ;  //Mean over last x NAV_SAT messages
      uint8_t Mean_numSV ;  //Mean over last x NAV_SAT messages
    } sat_info;
int index_SAT_info;
uint32_t mean_cno,max_cno,min_cno,nr_sats;    
    void push_SAT_info(struct NAV_SAT nav_sat);
};

//void sort_run(double a[], uint8_t hour[], uint8_t minute[],uint8_t seconde[],int runs[],int size);
void sort_run(double a[], uint8_t hour[], uint8_t minute[],uint8_t seconde[],uint8_t mean_cno[],uint8_t max_cno[],uint8_t min_cno[],uint8_t nrSats[],int runs[], int size);
void sort_run_alfa(double a[], int dis[],int message[],uint8_t hour[], uint8_t minute[],uint8_t seconde[],int runs[], int samples[],int size);

int New_run_detection(float actual_heading, float S2_speed);
/*berekenen van de gemiddelde snelheid over een bepaalde afstand, samplerate heeft geen invloed !!****************/
 class GPS_speed{
   public:
    GPS_speed(int afstand); // description of the constructor, length in m where avg speed is calculated
    double Update_distance(int actual_run);
    double m_speed;//speed over de gewenste afstand
    double m_speed_alfa;//speed over de gewenste afstand
    double m_max_speed;//max speed van de laatste run
    float display_max_speed;//Om update on the fly op display
    double avg_speed[10];
    double display_speed[10];
    int m_Distance[10];
    uint8_t time_hour[10];
    uint8_t time_min[10];
    uint8_t time_sec[10];
    int this_run[10];
    int nr_samples[10];
    int message_nr[10];
    int m_index;
    int m_distance;//actuele afstand, de eerstvolgende grotere afstand dan "afstand", vb 100m
    int m_distance_alfa;//de eerstvolgende kleinere afstand dan "afstand", dit voor alfa berekening
    int m_set_distance;//hier wordt de instantie afstand gezet, vb 100m, 200m, 500m....   
    int m_Set_Distance;//hier in de juiste resolutie, dus *1000*sample_rate !!
    int m_sample; 
  private:      
    int old_run;
 };
//berekening van gemiddelde snelheid over een tijdvenster (2s, 10s, 1800s...)****************************************************************
class GPS_time{
  public:
    GPS_time(int tijdvenster); // description of the constructor
    float Update_speed(int actual_run);//update function
    void Reset_stats(void); //reset all stats to 0
    double avg_s;
    int avg_s_sum;
    double s_max_speed;
    float display_max_speed;//Om update on the fly op display
    float display_last_run;
    double avg_speed[10];
    double display_speed[10];
    double avg_5runs;
    uint8_t time_hour[10];
    uint8_t time_min[10];
    uint8_t time_sec[10];
    int this_run[10];
    int time_window; 
    int speed_run_counter;
    uint16_t speed_run[50]; //for bar_graph
    uint8_t Mean_cno[10];
    uint8_t Max_cno[10];
    uint8_t Min_cno[10];
    uint8_t Mean_numSat[10];       
  private:
    int old_run;
    int reset_display_last_run;
    //int last_second;
}; 
/*;h Berekenen van de alfa speed, instantie van GPS_speed 250 /500 kan gebruikt worden + diameter cirkel (normaal 50 m)***********/
class Alfa_speed{
  public:
    Alfa_speed(int alfa_radius);//constructor
    //float Alfa_speed::Update_alfa(GPS_speed M)
    float Update_Alfa(GPS_speed M);   //update function every GPS-sample
    void Reset_stats(void); //reset all stats to 0
    double straight_dist_square;
    double alfa_speed;
    double alfa_speed_max;
    float display_max_speed;//Om update on the fly op display
    double alfa_circle_square;
    double avg_speed[10];
    int real_distance[10];
    uint8_t time_hour[10];
    uint8_t time_min[10];
    uint8_t time_sec[10];
    int this_run[10];
    int message_nr[10];
    int alfa_distance[10];
  private: 
    int alfa_count;
    int old_run_count; 
};
float Dis_point_line(float long_act,float lat_act,float long_1,float lat_1,float long_2,float lat_2);
float Alfa_indicator(GPS_speed M250,GPS_speed M100,float actual_heading);
int setupGPS(void);
#endif
