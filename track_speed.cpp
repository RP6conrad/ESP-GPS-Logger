#include "track_speed.h"
#include <cmath>

// Zorg voor compatibiliteit met platformen die M_PI niet standaard definiëren
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
constexpr double Ubx_to_Rad = 1e-5 * (M_PI / 180.0);
// Functie om de start- en finishlijn te genereren op basis van 2 GPS-punten
TrajectLijnen genereerLoodrechteLijnen(GPS_point p1,GPS_point p2) {
    TrajectLijnen resultaat;
    // WGS84 Ellipsoïde constanten
    const double WGS84_A = 6378137.0;
    const double WGS84_F = 1.0 / 298.257223563;
    double refLatRad = p1.lat * M_PI / 180.0;
    double gemLatRad = ((p2.lat * M_PI / 180.0) + refLatRad) / 2.0;
    double sinLat = std::sin(gemLatRad);
    double eKwadraat = WGS84_F * (2.0 - WGS84_F);
    double W = std::sqrt(1.0 - eKwadraat * sinLat * sinLat);    
    double M = WGS84_A * (1.0 - eKwadraat) / (W * W * W);
    double N = WGS84_A / W;
    double deltaLatRad = (p2.lat - p1.lat) * M_PI / 180.0;
    double deltaLonRad = (p2.lon - p1.lon) * M_PI / 180.0;
    // Exacte WGS84 meters
    double dx = deltaLonRad * N * std::cos(refLatRad);
    double dy = deltaLatRad * M;
    double asHoekRad = std::atan2(dx, dy);
    // 3. Bereken de hoek voor de LOODRECHTE lijnen
    // We tellen er 90 graden (PI / 2) bij op om de lijn haaks op de vaaras te zetten
    double loodrechteHoekRad = asHoekRad + (M_PI / 2.0);
    // 4. Normaliseer de hoek zodat deze netjes tussen -PI en +PI blijft
    if (loodrechteHoekRad > M_PI) {
        loodrechteHoekRad -= (2.0 * M_PI);
    }
    // 5. Definieer de Startlijn (referentiepunt = p1)
    resultaat.startLijn.refLat  = p1.lat;
    resultaat.startLijn.refLon  = p1.lon;
    resultaat.startLijn.hoekRad = loodrechteHoekRad;
    resultaat.startLijn.start_finishRad = asHoekRad;
    // 6. Definieer de Finishlijn (referentiepunt = p2)
    resultaat.finishLijn.refLat  = p2.lat;
    resultaat.finishLijn.refLon  = p2.lon;
    resultaat.finishLijn.hoekRad = loodrechteHoekRad;
    resultaat.finishLijn.start_finishRad = asHoekRad;
    return resultaat;
}
// Functie om de live passage en afstand per lijn te controleren
LijnPassageResultaat controleerLijnPassage(double lat, double lon, uint32_t iTOW,Line_2D lijn, LijnMetingState& state) {
    LijnPassageResultaat resultaat;
    resultaat.isLijnGepasseerd = false;
    resultaat.gewogen_iTOW = 0;
    // 1. Bereken de richtingsvector van de lijn op basis van de hoek in radialen
    double dirX = std::sin(lijn.hoekRad); // Oost-component
    double dirY = std::cos(lijn.hoekRad); // Noord-component
    // 2. Bereken de Normaalvector (90 graden naar links gedraaid t.o.v. de lijnrichting)
    double nX = -dirY;
    double nY = dirX;
    // 3. WGS84 Ellipsoïde correctie voor de exacte locatie (Kontich / België)
    const double WGS84_A = 6378137.0;           // Equatoriale straal
    const double WGS84_F = 1.0 / 298.257223563; // Afplatting
    
    double refLatRad = lijn.refLat * M_PI / 180.0;
    double gemLatRad = ((lat * M_PI / 180.0) + refLatRad) / 2.0;

    double sinLat = std::sin(gemLatRad);
    double eKwadraat = WGS84_F * (2.0 - WGS84_F);
    double W = std::sqrt(1.0 - eKwadraat * sinLat * sinLat);
    
    // Dynamische kromtestralen specifiek voor deze breedtegraad
    double M = WGS84_A * (1.0 - eKwadraat) / (W * W * W); // Noord-Zuid
    double N = WGS84_A / W;                              // Oost-West
    double deltaLatRad = (lat - lijn.refLat) * M_PI / 180.0;
    double deltaLonRad = (lon - lijn.refLon) * M_PI / 180.0;
    // Vertaal naar exacte meters op de WGS84 ellipsoïde
    double xNu = deltaLonRad * N * std::cos(refLatRad);
    double yNu = deltaLatRad * M;
    // 4. Bereken de actuele loodrechte afstand tot de lijn via het inproduct
    resultaat.afstandTotLijn = (xNu * nX) + (yNu * nY);
    // 5. Controleer op lijnpassage (teken slaat om van negatief naar positief of exact nul)
    if (state.eersteFixIngevuld) {
        if (state.vorigeAfstand < 0.0 && resultaat.afstandTotLijn >= 0.0) {
            resultaat.isLijnGepasseerd = true;           
            // Lineaire interpolatie voor sub-sample nauwkeurigheid
            double weegfactor = (0.0 - state.vorigeAfstand) / (resultaat.afstandTotLijn - state.vorigeAfstand);
            double deltaT = (double)(iTOW - state.vorige_iTOW);           
            resultaat.gewogen_iTOW = state.vorige_iTOW + (uint32_t)(weegfactor * deltaT);
        }
    }
    // Update de state voor de volgende fix van deze specifieke lijn
    state.vorigeAfstand = resultaat.afstandTotLijn;
    state.vorige_iTOW = iTOW;
    state.eersteFixIngevuld = true;
    return resultaat;
}

double berekenAfstand(GPS_point p1,GPS_point p2) {
    // WGS84 Ellipsoïde constanten
    const double WGS84_A = 6378137.0;           // Equatoriale straal (halve lange as)
    const double WGS84_F = 1.0 / 298.257223563; // Afplatting van de aarde

    // Zet breedtegraden om naar radialen
    double lat1Rad = p1.lat * M_PI / 180.0;
    double lat2Rad = p2.lat * M_PI / 180.0;
    
    // Bereken de gemiddelde breedtegraad van het traject
    double gemLatRad = (lat1Rad + lat2Rad) / 2.0;

    // Bereken de exacte kromtestralen van de WGS84 ellipsoïde op déze specifieke breedtegraad
    double sinLat = sin(gemLatRad);
    double cosLat = cos(gemLatRad);
    double eKwadraat = WGS84_F * (2.0 - WGS84_F); // Eerste numerieke excentriciteit gekwadrateerd    
    double W = sqrt(1.0 - eKwadraat * sinLat * sinLat);
    // M = Meridiaankromtestraal (voor Noord-Zuid afstanden)
    double M = WGS84_A * (1.0 - eKwadraat) / (W * W * W);    
    // N = Dwarskromtestraal (voor Oost-West afstanden)
    double N = WGS84_A / W;
    // Bereken de verschillen in radialen
    double deltaLatRad = (p2.lat - p1.lat) * M_PI / 180.0;
    double deltaLonRad = (p2.lon - p1.lon) * M_PI / 180.0;
    // Converteer de radiale verschillen naar exacte metrische meters op de WGS84 ellipsoïde
    double dy = deltaLatRad * M;
    double dx = deltaLonRad * N * cosLat;
    // Pythagoras geeft de exacte afstand over de ellipsoïde
    return sqrt(dx * dx + dy * dy);
}
void sort_track(double a[],double b[],double cd[],int size,uint8_t hour[],uint8_t minute[]){
  for(int i=0; i<(size-1); i++) {
        for(int o=0; o<(size-(i+1)); o++) {
                if(a[o] > a[o+1]) {
                    double t = a[o];double r = b[o];double s = cd[o];uint8_t c=hour[o];uint8_t d=minute[o];
                    a[o] = a[o+1]; b[o] = b[o+1]; cd[o]= cd[o+1];hour[o] = hour[o+1];minute[o] = minute[o+1];
                    a[o+1] = t; b[o+1] = r; cd[o+1] = s; hour[o+1] = c; minute[o+1] = d;
                    }
        }
  }     
}

Doppler_track doppler_speed_calculation(long ground_speed,long ground_heading,double track_direction,unsigned long iTOW,bool run_status){
    static Doppler_track t_speed;
    static uint32_t old_iTOW;
    static uint32_t track_iTOW;
    static double sum_doppler_distance;
    static double sum_projected_doppler_distance;
    static bool old_run_status;
    if(run_status){
        uint32_t delta_iTOW=iTOW-old_iTOW;// vb 1000 mm/s en 200 ms is een afstand van 200 mm -> 1000*200/1000 = 200 mm = 0.2 m
        double delta_heading = (ground_heading/100000.0) * M_PI / 180.0-track_direction; // ground_heading in graden*100000, track_direction in radialen
        sum_doppler_distance = sum_doppler_distance + ground_speed*delta_iTOW; 
        sum_projected_doppler_distance=sum_projected_doppler_distance+ground_speed * cos(delta_heading) * delta_iTOW;
        track_iTOW=track_iTOW+delta_iTOW;
        t_speed.doppler_track_speed= sum_doppler_distance/track_iTOW;//*calibration_speed; //m/s
        t_speed.doppler_projected_track_speed = sum_projected_doppler_distance/track_iTOW;
        }
    if(!run_status&old_run_status) {
        t_speed.doppler_track_speed= sum_doppler_distance/track_iTOW; //m/s
        t_speed.doppler_projected_track_speed = sum_projected_doppler_distance/track_iTOW;
        t_speed.doppler_track_distance= sum_doppler_distance; //m/s
        t_speed.doppler_projected_track_distance = sum_projected_doppler_distance;
        sum_projected_doppler_distance=0;
        sum_doppler_distance=0;
        track_iTOW=0;
        }   
    old_run_status=run_status;
    old_iTOW=iTOW;
    
    return t_speed;
}