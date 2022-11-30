#ifndef SBP_H
#define SBP_H
/* Locosys SBP structures */
struct SBP_Header{//length = 64 bytes
   uint16_t Header_length = 0x22;//byte 0-1 : nr of meaningfull bytes in header (MID_FILE_ID)
   char str[62] ="unknown,0,unknown,unknown";//byte 2~63:  MID_FILE_ID(0xfd) will stuff    0xff for remaining bytes
   };
struct SBP_frame{//length = 32 bytes
   uint8_t HDOP;        /* HDOP [0..51] with resolution 0.2 */
   uint8_t SVIDCnt;        /* Number of SVs in solution [0 to 12] */
   uint16_t UtcSec;        /* UTC Second [0 to 59] in seconds with resolution 0.001 */
   uint32_t date_time_UTC_packed; /* refer to protocol doc*/
   uint32_t SVIDList;    /* SVs in solution:  Bit 0=1: SV1, Bit 1=1: SV2, ... , Bit 31=1: SV32 */
   int32_t Lat;            /* Latitude [-90 to 90] in degrees with resolution 0.0000001 */
   int32_t Lon;            /* Longitude [-180 to 180] in degrees with resolution 0.0000001 */
   int32_t AltCM;            /* Altitude from Mean Sea Level in centi meters */
   uint16_t Sog;            /* Speed Over Ground in m/sec with resolution 0.01 */
   uint16_t Cog;            /* Course Over Ground [0 to 360] in degrees with resolution 0.01 */
   int16_t ClmbRte;        /* Climb rate in m/sec with resolution 0.01 */
   uint8_t bitFlags=1;     /* bitFlags, default 0x00,    bit 0=1 indicate the first point after power on */
   uint8_t reserved;
};

#endif