#ifndef TRACK_SPEED_H
#define TRACK_SPEED_H

#include <stdint.h>
#include "GPS_data.h" //use of struct Point
#include "SD_card.h"    

extern float _lat[BUFFER_ALFA]; 
extern float _long[BUFFER_ALFA];
extern GPS_speed M500;
extern int index_GPS;
extern float calibration_speed;

// Een simpel GPS punt
struct GPS_point {
    double lat; // Breedtegraad in graden
    double lon; // Lengtegraad in graden
};

// De lijn structuur (Punt + Richting in radialen, 0 = Noord)
struct Line_2D {
    double refLat;  // Latitude referentiepunt (graden)
    double refLon;  // Longitude referentiepunt (graden)
    double hoekRad; // Richting van de lijn in radialen (0 = Noord, met de klok mee)
    double start_finishRad;//Direction of the start - finish vector in rad
};

// Return struct met de twee loodrechte lijnen voor het traject
struct TrajectLijnen {
   Line_2D startLijn;
   Line_2D finishLijn;
};

// Structuur om de geschiedenis per specifieke lijn bij te houden (voorkomt data-menging)
struct LijnMetingState {
    double vorigeAfstand = 0.0;
    uint32_t vorige_iTOW = 0;
    bool eersteFixIngevuld = false;
};

// De return struct voor het resultaat van een lijnpassage
struct LijnPassageResultaat {
    double afstandTotLijn;     // Actuele afstand (negatief = vóór de lijn, positief = voorbij)
    bool isLijnGepasseerd;     // True op de exacte fix van de passage
    uint32_t gewogen_iTOW;     // Geïnterpoleerde iTOW in ms
};
// Struct with doppler-speed results
struct Doppler_track{
    double doppler_track_speed;
    double doppler_projected_track_speed;
    double doppler_track_distance;
    double doppler_projected_track_distance;
};

extern Line_2D startLijn;
extern Line_2D finishLijn;
extern GPS_point poort1;
extern GPS_point poort2;

// Functie om de start- en finishlijn te genereren op basis van 2 GPS-punten
TrajectLijnen genereerLoodrechteLijnen(GPS_point p1,GPS_point p2);

// Functie om de live passage en afstand per lijn te controleren
LijnPassageResultaat controleerLijnPassage(double lat, double lon, uint32_t iTOW,Line_2D lijn, LijnMetingState& state);
double berekenAfstand(GPS_point p1,GPS_point p2);
double berekenAfstandVincenty(GPS_point p1, GPS_point p2);
void sort_track(double a[],double b[],double cd[],int size,uint8_t hour[],uint8_t minute[]);
Doppler_track doppler_speed_calculation(long ground_speed,long ground_heading,double track_direction,unsigned long iTOW,bool run_status);
void Auto_set_track(void);
#endif // TRACK_SPEED_H

