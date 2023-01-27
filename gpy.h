#ifndef GPY_H
#define GPY_H
/*
MIT License
First draft of C lib for creating the minimal OpenGNSS format out of the ubx nav pvt frame !
Copyright (c) 2022 RP6conrad

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

//Function declarations
void log_GPY(File file);
void log_GPY_Header(File file);
uint16_t Fletcher16( uint8_t *data, int count );
char gpy_Tx[256]; 
//Struct definitions
struct GPY_Header{
	uint8_t Type_identifier=0xF0;	//Frame identifier, header = 0xF0
  uint8_t Flags;
  uint16_t Length=72;    //length = 6 + 4 * STRING_IO_LENGTH + 2 = 72
	uint16_t DeviceType=2;//ublox = 2
	char deviceDescription[16]="ESP-GPS";
	char deviceName[16]= "Boom";
	char serialNumber[16]="macAddr";
	char firmwareVersion[16]="SW_version";
	uint16_t Checksum;
} __attribute__((__packed__));

struct GPY_Frame {	
  uint8_t   Type_identifier=0xE0;	//Frame identifier for full frame = 0xE0
  uint8_t   Flags=0x0;
	uint16_t  HDOP;	//HDOP
	int64_t   Unix_time;//ms 
	uint32_t  Speed; //mm/s
	uint32_t  Speed_error;//sAccCourse_Over_Ground;
	int32_t   Latitude;
	int32_t   Longitude;
	int32_t   COG;//Course over ground
	uint8_t   Sat; //number of sats
	uint8_t	  fix;
	uint16_t  Checksum;
  } __attribute__((__packed__)); //total = 36 bytes

struct GPY_Frame_compressed {	
  uint8_t   Type_identifier=0xD0;	//Frame identifier for compressed frame = 0xD0
  uint8_t   Flags=0x0;
	uint16_t  HDOP;	//HDOP
	int16_t   delta_time;//ms 
	int16_t   delta_Speed; //mm/
	int16_t   delta_Speed_error;//sAccCourse_Over_Ground;
	int16_t   delta_Latitude;
	int16_t   delta_Longitude;
	int16_t   delta_COG;//delta (course / 1000) !
	uint8_t   Sat; //number of sats
	uint8_t	  fix;
	uint16_t  Checksum;
  }; //total = 20 bytes

struct  GPY_Header gpy_header;
struct  GPY_Frame gpy_frame;
struct  GPY_Frame_compressed gpy_frame_compressed;
//Functions definitions 
/*
 * https://en.wikipedia.org/wiki/Fletcher%27s_checksum
 * Length of the frame in bytes (last 2 bytes = checksum),
 * pointer to the frame bytes buffer.
 * 2byte checksum is returned
 */
uint16_t Fletcher16( uint8_t *data, int count )
{
   uint16_t sum1 = 0;
   uint16_t sum2 = 0;
    /* Sum all the bytes, but not the last 2 */
	for ( int i = 0; i<(count-2); ++i )
	   {
		  sum1 = (sum1 + data[i])&0xFF;//divide by 256 in stead of 255 !!
		  sum2 = (sum2 + sum1)&0xFF;
	   }
   data[count-2]=sum1;
   data[count-1]=sum2;   
   return (sum2 << 8) | sum1;
}
//https://community.particle.io/t/make-epoch-timestamp-maketime-solved/10013
/*
time_t tmConvert_t(int YYYY, byte MM, byte DD, byte hh, byte mm, byte ss)
{
  tmElements_t tmSet;
  tmSet.Year = YYYY;
  tmSet.Month = MM;
  tmSet.Day = DD;
  tmSet.Hour = hh;
  tmSet.Minute = mm;
  tmSet.Second = ss;
  return makeTime(tmSet); 
}
*/
void log_GPY_Header(File file){
 
  for (int i=0;i<16;i++){gpy_header.firmwareVersion[i]=SW_version[i];}
  sprintf(gpy_header.serialNumber, "_%2X%2X%2X%2X%2X%2X_", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	Fletcher16((uint8_t*)&gpy_header,72);
	file.write((const uint8_t *)&gpy_header,72);
}

void log_GPY(File file){
 // convert a date and time into unix time, offset 1970, Arduino 8bytes = LL  !!!!!
 time_t utc_Sec;
 struct tm frame_time;  // time elements structure
 frame_time.tm_sec = ubxMessage.navPvt.second;
 frame_time.tm_hour = ubxMessage.navPvt.hour;
 frame_time.tm_min = ubxMessage.navPvt.minute;
 frame_time.tm_mday = ubxMessage.navPvt.day;
 frame_time.tm_mon = ubxMessage.navPvt.month-1;     //month 0 - 11 with mktime 
 frame_time.tm_year = ubxMessage.navPvt.year - 1900; // years since 1900, so deduct 1900
 frame_time.tm_isdst = 0; // No daylight saving
 utc_Sec =  mktime(&frame_time);//mktime returns local time, so TZ is important !!!
 int64_t utc_ms= utc_Sec*1000LL+(ubxMessage.navPvt.nano+500000)/1000000LL;

//calcultation of delta values
int  delta_time= utc_ms-gpy_frame.Unix_time;//ms 
int  delta_Speed=ubxMessage.navPvt.gSpeed-gpy_frame.Speed; //mm/
int  delta_Speed_error=ubxMessage.navPvt.sAcc-gpy_frame.Speed_error;//sAccCourse_Over_Ground;
int  delta_Latitude=ubxMessage.navPvt.lat-gpy_frame.Latitude;
int  delta_Longitude=ubxMessage.navPvt.lon-gpy_frame.Longitude;
int  delta_COG=ubxMessage.navPvt.heading/1000-gpy_frame.COG/1000;//delta (course / 1000) !
#define  SIGNED_INT 30000 //if delta is more, a full frame is written
int full_frame=0;
static int first_frame=0;
if((delta_time>SIGNED_INT)|(delta_time<-SIGNED_INT))full_frame=1;
if((delta_Speed>SIGNED_INT)|(delta_Speed<-SIGNED_INT))full_frame=1;
if((delta_Speed_error>SIGNED_INT)|(delta_Speed_error<-SIGNED_INT))full_frame=1;
if((delta_Latitude>SIGNED_INT)|(delta_Latitude<-SIGNED_INT))full_frame=1;
if((delta_Longitude>SIGNED_INT)|(delta_Longitude<-SIGNED_INT))full_frame=1;
if((delta_COG>SIGNED_INT)|(delta_COG<-SIGNED_INT))full_frame=1;
if(first_frame==0)full_frame=1;//first frame is always a full frame
if(next_gpy_full_frame){full_frame=1;next_gpy_full_frame=0;}//if a navPvt frame is lost, next frame = full frame !!!
if(full_frame==1){
    gpy_frame.HDOP=ubxMessage.navDOP.hDOP;//ubxMessage.navPvt.pDOP;
    gpy_frame.Unix_time=utc_ms;
    gpy_frame.Speed=ubxMessage.navPvt.gSpeed;
    gpy_frame.Speed_error=ubxMessage.navPvt.sAcc;
    gpy_frame.Latitude=ubxMessage.navPvt.lat;
    gpy_frame.Longitude=ubxMessage.navPvt.lon;
    gpy_frame.COG=ubxMessage.navPvt.heading;
    gpy_frame.Sat=ubxMessage.navPvt.numSV;
    gpy_frame.fix=ubxMessage.navPvt.fixType;
    Fletcher16((uint8_t*)&gpy_frame,36);
    file.write((const uint8_t*)&gpy_frame,36);
    first_frame=1;
    //Serial.println(" full ");
    }   
else{
    gpy_frame_compressed.HDOP=ubxMessage.navDOP.hDOP;//ubxMessage.navPvt.pDOP
    gpy_frame_compressed.delta_time=delta_time;//ms 
    gpy_frame_compressed.delta_Speed=delta_Speed; //mm/
    gpy_frame_compressed.delta_Speed_error=delta_Speed_error;//sAccCourse_Over_Ground;
    gpy_frame_compressed.delta_Latitude=delta_Latitude;
    gpy_frame_compressed.delta_Longitude=delta_Longitude;
    gpy_frame_compressed.delta_COG=delta_COG;//delta (course / 1000) !
    gpy_frame_compressed.Sat=ubxMessage.navPvt.numSV; //number of sats
    gpy_frame_compressed.fix=ubxMessage.navPvt.fixType;
    Fletcher16((uint8_t*)&gpy_frame_compressed,20);
    file.write((const uint8_t *)&gpy_frame_compressed,20);
    //Serial.println("compr ");
    }   
}
#endif