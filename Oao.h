#ifndef GPS_H
#define GPS_H
union OAO_Frame {
  struct {
    uint16_t  mode;
    uint8_t   checksum_a;
    uint8_t   checksum_b;
    union {
      struct {
        int32_t   latitude;
        int32_t   longitude;              /* Mode Track 0x0AD1 stops here */
        int32_t   altitude;
        uint32_t  speed;
        uint32_t  heading;
        uint64_t  utc_gnss;
        union {
          struct {
            uint16_t  identifier;         /* Mode Emergency 0x0AD2 stops here */
          } __attribute__((__packed__));
          struct {
            uint8_t   fix;
            uint8_t   satellites;         /* Mode POI 0x0AD3 stops here */
            uint32_t  accuracy_speed;
            uint32_t  accuracy_horizontal;
            uint32_t  accuracy_vertical;
            uint32_t  accuracy_heading;
            uint16_t  accuracy_pDOP;      /* Mode GNSS 0x0AD4 or 0x0AD5 stops here */
          } __attribute__((__packed__));
        };
      } __attribute__((__packed__));
      struct {
        uint64_t  utc_imu;
        int16_t   attitude_w;
        int16_t   attitude_x;
        int16_t   attitude_y;
        int16_t   attitude_z;
        int16_t   angular_velocity_x;
        int16_t   angular_velocity_y;
        int16_t   angular_velocity_z;
        int16_t   linear_acceleration_x;
        int16_t   linear_acceleration_y;
        int16_t   linear_acceleration_z;  /* Mode IMU 0x0AD6 stops here */
      } __attribute__((__packed__));
    };
  } __attribute__((__packed__));
  uint8_t bytes_track     [12];
  uint8_t bytes_imu       [32];
  uint8_t bytes_emergency [34];
  uint8_t bytes_poi       [34];
  uint8_t bytes_gnss      [52];
  uint8_t bytes           [52];
} __attribute__((__packed__));
struct best_t {
  uint32_t utc_seconds;
  uint32_t value;
} __attribute__((__packed__));
union OAO_Header {
  struct {
    uint16_t  mode;               /* Mode Header 0x0AD0 */
    uint8_t   checksum_a;
    uint8_t   checksum_b;
    uint16_t  identifier;
    char      nickname[10];
    uint64_t  start_date;
    int32_t   start_latitude;
    int32_t   start_longitude;
    int32_t   start_altitude;
    uint64_t  end_date;
    int32_t   end_latitude;
    int32_t   end_longitude;
    int32_t   end_altitude;
    uint32_t  distance;
    int32_t   minimum_latitude;
    int32_t   minimum_longitude;
    int32_t   minimum_altitude;
    uint32_t  minimum_speed;
    int32_t   maximum_latitude;
    int32_t   maximum_longitude;
    int32_t   maximum_altitude;
    uint32_t  maximum_speed;
    uint32_t  above_12kn;
    uint32_t  above_12kn_seconds;
    best_t    bests_over_1s   [5];
    best_t    bests_over_10s  [5];
    best_t    bests_over_1h   [5];
    best_t    bests_over_500m [5];
    best_t    bests_over_1000m[5];
    best_t    bests_over_1852m[5];
    best_t    bests_gybe_min  [5];
    uint32_t  elevation_gain;
    uint8_t   unused   [64];
    uint8_t   signature[64];
  } __attribute__((__packed__));
  uint8_t bytes[512];
} __attribute__((__packed__));

/*
 * Length of the frame in bytes,
 * pointer to the frame bytes buffer.
 */
bool checksum_verify(const uint16_t length, uint8_t* buffer) {
    uint8_t checksum_a = 0, checksum_b = 0;
    /* Sum the two first bytes */
    for(uint16_t i = 0; i < 2; i++) {
        checksum_b += (checksum_a += buffer[i]);
    }
    /* Skip the two checksum bytes */
    /* Sum the following bytes */
    for(uint16_t i = 4; i < length; i++) {
        checksum_b += (checksum_a += buffer[i]);
    }
    buffer[2]=checksum_a;
    buffer[3]=checksum_b;
    /* Compare the computed checksum to the checksum in the frame */
    return checksum_a == buffer[2] && checksum_b == buffer[3];
}
//https://community.particle.io/t/make-epoch-timestamp-maketime-solved/10013
time_t tmConvert_t(int YYYY, byte MM, byte DD, byte hh, byte mm, byte ss)
{
  tmElements_t tmSet;
  tmSet.Year = YYYY - 1970;
  tmSet.Month = MM;
  tmSet.Day = DD;
  tmSet.Hour = hh;
  tmSet.Minute = mm;
  tmSet.Second = ss;
  return makeTime(tmSet); 
}
extern File gpsfile;
union OAO_Header oao_header;
union OAO_Frame oao_pvt ;

void log_header_GPS(void){
  oao_header.mode=0x0AD0;
  oao_header.identifier=987;
  checksum_verify(512,oao_header.bytes);
  gpsfile.write((const uint8_t *)&oao_header.bytes,512); 
}

void log_GPS(void){
time_t utcSec=tmConvert_t(ubxMessage.navPvt.year, ubxMessage.navPvt.month, ubxMessage.navPvt.day, ubxMessage.navPvt.hour, ubxMessage.navPvt.minute, ubxMessage.navPvt.second);
int64_t utc=(int64_t)utcSec*1000+(ubxMessage.navPvt.nano+500000)/1000000;//om af te ronden
oao_pvt.utc_gnss=utc;
//Serial.printf("UTC littleEndian: %llu\n", oao_pvt.utc_gnss); 
if(ubxMessage.navPvt.nano/1000000==0){oao_pvt.mode=0x0AD4;}
else {oao_pvt.mode=0x0AD5;}
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
gpsfile.write((const uint8_t *)&oao_pvt.bytes_gnss,52);
}
#endif