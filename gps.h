#ifndef GPS_H
#define GPS_H
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
uint64_t log_header_GPS(void);
void log_GPS(uint64_t Epoch_ms);
uint16_t Fletcher16( uint8_t *data, int count );
//Struct definitions
struct GNSS_Frame {	
    uint16_t  identifier_size=0x041F;	//Frame identifier&size : 6MSB=identifier 1: 0x04, 10LSB=size : 31 bytes=0x1F !!
    uint16_t  checksum;
	uint32_t  timestamp;	//UTC time, ms relative to epoch
	int32_t   latitude;
	int32_t   longitude;  
	uint32_t  Course_Over_Ground;
	uint32_t  Speed_Over_Ground;
	uint32_t  Speed_accuracy;//sAcc
	uint16_t  HDOP;
	uint8_t   Sat; //number of sats
  }; 
struct GNSS_Header   {
    uint16_t  identifier_size=0x0058;  //Frame identifier&size : 6MSB=identifier: 0x00, 10LSB=size : 88 bytes= 0x58!!	
    uint16_t  checksum;
    char      version[4]="V1";//8b
    uint64_t  Epoch;			//16b  timestamp first GNSS, utc with ms resolution
	uint8_t	  Endian=0x00;		//littleEndian = 0, bigEndian=1
    uint8_t   Compression_type=0x00;//no compression = 0x00
    uint16_t  GNSS_chipset_manufacturer=0x0003;//20 b  0x0003 = ublox
    uint16_t  GNSS_chipset_model=0x0008;//ublox M8n = 0x0008
    uint16_t  Log_device_manufacturer=0x0000;//24 b
    char      Logging_device_name[16]="ESP-GPS" ; 
	char      Logging_device_firmware[16]="5.61" ; 	
	char      Logging_device_serial_nr[16]="MAC" ; 
	char      Logging_device_identifier[16]="RP6_Conrad" ; //24b  +64b = 88b
} ;
extern File gpsfile;
struct  GNSS_Header gps_header;
struct  GNSS_Frame gps_frame;
//Functions definitions 
/*
 * https://en.wikipedia.org/wiki/Fletcher%27s_checksum
 * Length of the frame in bytes,
 * pointer to the frame bytes buffer.
 * 2byte checksum is returned
 */
uint16_t Fletcher16( uint8_t *data, int count )
{
   uint16_t sum1 = 0;
   uint16_t sum2 = 0;
   /* Sum the two first bytes */
   for ( int i = 0; i<1; ++i )
   {
      sum1 = (sum1 + data[i]) % 255;
      sum2 = (sum2 + sum1) % 255;
   }
    /* Skip the two checksum bytes */
    /* Sum the following bytes */
	for ( int i = 4; i<count; ++i )
	   {
		  sum1 = (sum1 + data[i]) % 255;
		  sum2 = (sum2 + sum1) % 255;
	   }
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


uint64_t log_header_GPS(void){
  time_t utcSec=tmConvert_t(ubxMessage.navPvt.year, ubxMessage.navPvt.month, ubxMessage.navPvt.day, ubxMessage.navPvt.hour, ubxMessage.navPvt.minute, ubxMessage.navPvt.second);
  int64_t Epoch_UTC_ms=(int64_t)utcSec*1000+(ubxMessage.navPvt.nano+500000)/1000000;//epoch with ms resolution
  gps_header.Epoch=Epoch_UTC_ms;
  gps_header.checksum=Fletcher16((uint8_t*)&gps_header,88);
  gpsfile.write((const uint8_t *)&gps_header,88); 
  return Epoch_UTC_ms;
}

void log_GPS(uint64_t Epoch_ms){
time_t utcSec=tmConvert_t(ubxMessage.navPvt.year, ubxMessage.navPvt.month, ubxMessage.navPvt.day, ubxMessage.navPvt.hour, ubxMessage.navPvt.minute, ubxMessage.navPvt.second);
int64_t utc=(int64_t)utcSec*1000+(ubxMessage.navPvt.nano+500000)/1000000;//epoch with ms resolution
uint32_t Relative_timestamp=utc-Epoch_ms;
//Serial.printf("UTC littleEndian: %llu\n", oao_pvt.utc_gnss); 
gps_frame.timestamp=Relative_timestamp;
gps_frame.latitude=ubxMessage.navPvt.lat;
gps_frame.longitude=ubxMessage.navPvt.lon;
gps_frame.Course_Over_Ground=ubxMessage.navPvt.heading;
gps_frame.Speed_Over_Ground=ubxMessage.navPvt.gSpeed;
gps_frame.Speed_accuracy=ubxMessage.navPvt.sAcc;
gps_frame.HDOP=ubxMessage.navPvt.pDOP;
gps_frame.Sat=ubxMessage.navPvt.numSV;
gps_frame.checksum=Fletcher16((uint8_t*)&gps_frame,31);
gpsfile.write((const uint8_t *)&gps_frame,31);
}
#endif