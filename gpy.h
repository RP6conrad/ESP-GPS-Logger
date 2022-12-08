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
uint16_t Fletcher16( uint8_t *data, int count );
//Struct definitions
struct GNSS_Frame {	
  uint8_t  Type_identifier=0xE0;	//Frame identifier
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
  }; //total = 36 bytes

struct  GNSS_Frame gps_frame;
//Functions definitions 
/*
 * https://en.wikipedia.org/wiki/Fletcher%27s_checksum
 * Length of the frame in bytes (last 2 bytes = checsum),
 * pointer to the frame bytes buffer.
 * 2byte checksum is returned
 */
uint16_t Fletcher16( uint8_t *data, int count )
{
   uint16_t sum1 = 0;
   uint16_t sum2 = 0;
    /* Sum all the bytes, but not the last 2 */
	for ( int i = 4; i<(count-2); ++i )
	   {
		  sum1 = (sum1 + data[i])&0xFF;//divide by 256 in stead of 255 !!
		  sum2 = (sum2 + sum1)&0xFF;
	   }
   data[count-2]=sum1;
   data[count-1]=sum2;   
   return (sum2 << 8) | sum1;
}
//https://community.particle.io/t/make-epoch-timestamp-maketime-solved/10013
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

void log_GPY(File file){
time_t utcSec=tmConvert_t(ubxMessage.navPvt.year, ubxMessage.navPvt.month, ubxMessage.navPvt.day, ubxMessage.navPvt.hour, ubxMessage.navPvt.minute, ubxMessage.navPvt.second);
int64_t utc=(int64_t)utcSec*1000+(ubxMessage.navPvt.nano+500000)/1000000;//epoch with ms resolution
//Serial.printf("UTC littleEndian: %llu\n",utc); 
gps_frame.HDOP=ubxMessage.navPvt.pDOP;
gps_frame.Unix_time=utc;
gps_frame.Speed=ubxMessage.navPvt.gSpeed;
gps_frame.Speed_error=ubxMessage.navPvt.sAcc;
gps_frame.Latitude=ubxMessage.navPvt.lat;
gps_frame.Longitude=ubxMessage.navPvt.lon;
gps_frame.COG=ubxMessage.navPvt.heading;
gps_frame.Sat=ubxMessage.navPvt.numSV;
gps_frame.fix=ubxMessage.navPvt.fixType;
Fletcher16((uint8_t*)&gps_frame,36);
file.write((const uint8_t *)&gps_frame,36);
}
#endif