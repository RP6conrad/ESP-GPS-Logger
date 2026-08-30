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

double berekenAfstandVincenty(GPS_point p1, GPS_point p2) {
    const double a = 6378137.0;
    const double b = 6356752.314245;
    const double f = 1.0 / 298.257223563;

    double L = (p2.lon - p1.lon) * M_PI / 180.0;
    double U1 = atan((1.0 - f) * tan(p1.lat * M_PI / 180.0));
    double U2 = atan((1.0 - f) * tan(p2.lat * M_PI / 180.0));
    
    double sinU1 = sin(U1), cosU1 = cos(U1);
    double sinU2 = sin(U2), cosU2 = cos(U2);

    double lambda = L, lambdaP;
    double sinSigma, cosSigma, sigma, sinAlpha, cosSqAlpha, cos2SigmaM;
    int iterLimit = 100;

    do {
        double sinLambda = sin(lambda), cosLambda = cos(lambda);
        sinSigma = sqrt((cosU2 * sinLambda) * (cosU2 * sinLambda) + 
                        (cosU1 * sinU2 - sinU1 * cosU2 * cosLambda) * (cosU1 * sinU2 - sinU1 * cosU2 * cosLambda));
        if (sinSigma == 0) return 0; // Coördinaten vallen samen

        cosSigma = sinU1 * sinU2 + cosU1 * cosU2 * cosLambda;
        sigma = atan2(sinSigma, cosSigma);
        sinAlpha = cosU1 * cosU2 * sinLambda / sinSigma;
        cosSqAlpha = 1.0 - sinAlpha * sinAlpha;
        cos2SigmaM = cosSigma - 2.0 * sinU1 * sinU2 / cosSqAlpha;
        
        if (isnan(cos2SigmaM)) cos2SigmaM = 0; // Speciaal geval bij de evenaar
        
        double C = f / 16.0 * cosSqAlpha * (4.0 + f * (4.0 - 3.0 * cosSqAlpha));
        lambdaP = lambda;
        lambda = L + (1.0 - C) * f * sinAlpha * 
                 (sigma + C * sinSigma * (cos2SigmaM + C * cosSigma * (-1.0 + 2.0 * cos2SigmaM * cos2SigmaM)));
    } while (fabs(lambda - lambdaP) > 1e-12 && --iterLimit > 0);

    if (iterLimit == 0) return NAN; // Formule convergeert niet

    double uSq = cosSqAlpha * (a * a - b * b) / (b * b);
    double A = 1.0 + uSq / 16384.0 * (4096.0 + uSq * (-768.0 + uSq * (320.0 - 175.0 * uSq)));
    double B = uSq / 1024.0 * (256.0 + uSq * (-128.0 + uSq * (74.0 - 47.0 * uSq)));
    double deltaSigma = B * sinSigma * (cos2SigmaM + B / 4.0 * (cosSigma * (-1.0 + 2.0 * cos2SigmaM * cos2SigmaM) -
                        B / 6.0 * cos2SigmaM * (-3.0 + 4.0 * sinSigma * sinSigma) * (-3.0 + 4.0 * cos2SigmaM * cos2SigmaM)));

    return b * A * (sigma - deltaSigma);
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
// Deze functie gaat de start en finish vastleggen op het begin en einde van de laatste 500 meter run
// Nuttig voor testdoeleinden !!
void Auto_set_track(void){
    static Point P1,P2; //These 2 points determinate the reference line for the track_speed !!!
    config.p2_lat=_lat[M500.m_index%BUFFER_ALFA];//dit is het punt op -500 m van de actuele positie
    config.p2_lon=_long[M500.m_index%BUFFER_ALFA];
    config.p1_lat=_lat[index_GPS%BUFFER_ALFA];//dit is de actuele positie
    config.p1_lon=_long[index_GPS%BUFFER_ALFA];
    poort1 = { config.p1_lat, config.p1_lon }; 
    poort2 = { config.p2_lat, config.p2_lon };
    // 2. Bereken de vaste afstand
    trajectAfstandMeters = berekenAfstandVincenty(poort1, poort2);
    // 3. Genereer automatisch de haakse start- en finishlijn op basis van de vaaras
    TrajectLijnen mijnTraject = genereerLoodrechteLijnen(poort1, poort2);
    startLijn  = mijnTraject.startLijn;
    finishLijn = mijnTraject.finishLijn;
}