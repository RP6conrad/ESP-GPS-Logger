#ifndef SBP_H
#define SBP_H

/*Function declarations*/
//void log_header_SBP(File file);
//void log_SBP(File file);

//extern File sbpfile;
//extern UBXMessage ubxMessage; 
/* Locosys SBP structures */
struct SBP_Header{//length = 64 bytes
   uint16_t Text_length = 30;//byte 0-1 : nr of meaningfull bytes in header (MID_FILE_ID)
   uint8_t Id1 = 0xa0;//seems to be necessary so that GP3S accept .sbp
   uint8_t Id2 = 0xa2;//seems to be necessary so that GP3S accept .sbp
   uint16_t Again_length = 30;
   uint8_t Start = 0xfd;
   char Identity[57] ="ESP-GPS,0,unknown,unknown";//byte 7~63:  MID_FILE_ID(0xfd) will stuff    0xff for remaining bytes
   }__attribute__((__packed__));
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
   uint8_t sdop;     /* GT31 */
   uint8_t vsdop;
}__attribute__((__packed__));
/* Definition in SD_card.cpp*/
struct SBP_Header sbp_header;
struct SBP_frame sbp_frame;

void log_header_SBP(File file){
  for (int i=32;i<64;i++){((unsigned char*)(&sbp_header))[i]=0xFF;}//fill with 0xFF
  file.write((const uint8_t *)&sbp_header,64);
}
void log_SBP(File file){
uint32_t year=ubxMessage.navPvt.year;
uint8_t month=ubxMessage.navPvt.month;
uint8_t day=ubxMessage.navPvt.day;
uint8_t hour=ubxMessage.navPvt.hour;
uint16_t min=ubxMessage.navPvt.minute;
uint16_t sec=ubxMessage.navPvt.second;
uint32_t numSV=0xFFFFFFFF;
uint32_t HDOP=(ubxMessage.navDOP.hDOP+1)/20;//from 0.01 resolution to 0.2 ,reformat pDOP to HDOP 8-bit !!
if(HDOP>255)HDOP=255;//has to fit in 8 bit
uint32_t sdop=ubxMessage.navPvt.sAcc/10;//was sAcc
if(sdop>255)sdop=255;
uint32_t vsdop=ubxMessage.navPvt.vAcc/10;//was headingAcc ???
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
sbp_frame.HDOP=HDOP;
sbp_frame.ClmbRte=-ubxMessage.navPvt.velD/10;//omrekenen naar cm/s
sbp_frame.sdop=sdop;
sbp_frame.vsdop=vsdop;
file.write((const uint8_t *)&sbp_frame,32);
}

#endif