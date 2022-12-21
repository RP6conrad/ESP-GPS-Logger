#ifndef GPX_H
#define GPX_H
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
//See https://www.topografix.com/gpx_manual.asp#creator
//Doppler speed is not part of the gpx 1.1 frame, speed is then calculated as distance/time !!!
//gpx 1.0 is used here ! 
//https://logiqx.github.io/gps-wizard/gpx/
//Always 1Hz points
#define GPX_HEADER 0
#define GPX_FRAME 1
#define GPX_END 2
/*
<?xml version="1.0" encoding="UTF-8"?>
<gpx creator="ESP-GPS"
     version="1.0"
     xmlns="http://www.topografix.com/GPX/1/0"
     xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
     xsi:schemaLocation="http://www.topografix.com/GPX/1/0 http://www.topografix.com/GPX/1/0/gpx.xsd">
*/     
void log_GPX(int part,File file){
char bufferTx[512]; 
int i,y; 
int year,month,day,hour,minute,sec,sat;
if(part==GPX_HEADER){ 
  i= sprintf(bufferTx,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");y=y+i;
  i= sprintf(&bufferTx[y],"<gpx creator= \"ESP-GPS\"\nversion=\"1.0\"\n xmlns=\"http://www.topografix.com/GPX/1/0\"\n");y=y+i;
  i= sprintf(&bufferTx[y],"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n");y=y+i;
  i= sprintf(&bufferTx[y],"xsi:schemaLocation=\"http://www.topografix.com/GPX/1/0 http://www.topografix.com/GPX/1/0/gpx.xsd\">\n");y=y+i;
  i= sprintf(&bufferTx[y],"<trk><trkseg>");y=y+i;
  file.write((const uint8_t *)&bufferTx,y);
  } 
if(part==GPX_FRAME){  
    if(ubxMessage.navPvt.nano/1000000==0){//only log every full second !!!!
      float lat,lon,hdop,speed,msl,course;
      lat=ubxMessage.navPvt.lat/10000000.0f;
      lon=ubxMessage.navPvt.lon/10000000.0f;
      hdop=ubxMessage.navDOP.hDOP/100.0f;//resolution in ubx nav dop is 0.01
      course=ubxMessage.navPvt.heading/100000.0f;
      speed=ubxMessage.navPvt.gSpeed/1000.0f;
      msl=ubxMessage.navPvt.hMSL/1000.0f;
      sat=ubxMessage.navPvt.numSV;
      speed=ubxMessage.navPvt.gSpeed/1000.0f;
      year=ubxMessage.navPvt.year;
      month=ubxMessage.navPvt.month;
      day=ubxMessage.navPvt.day;
      hour=ubxMessage.navPvt.hour;
      minute=ubxMessage.navPvt.minute;
      sec=ubxMessage.navPvt.second;
      i= sprintf(bufferTx,"<trkpt lat=\"%f\" lon=\"%f\" >\n",lat,lon);y=y+i;
      i= sprintf(&bufferTx[y],"<ele>%f</ele>\n",msl);y=y+i;
      i= sprintf(&bufferTx[y],"<time>%d-%d-%dT%'02d:%'02d:%'02dZ</time>",year,month,day,hour,minute,sec);y=y+i;
      i= sprintf(&bufferTx[y],"<course>%f</course>",course);y=y+i;
      i= sprintf(&bufferTx[y],"<speed>%f</speed>",speed);y=y+i;
      i= sprintf(&bufferTx[y],"<sat>%d</sat>",sat);y=y+i;
      i= sprintf(&bufferTx[y],"<hdop>%f</hdop></trkpt>\n",hdop);y=y+i;
     
      file.write((const uint8_t *)&bufferTx,y);
      }
    }
  if(part==GPX_END){
     y= sprintf(bufferTx,"</trkseg>\n</trk></gpx>\n");
     file.write((const uint8_t *)&bufferTx,y);
     }      
}
#endif