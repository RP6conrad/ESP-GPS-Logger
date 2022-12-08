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
#define GPX_HEADER 0
#define GPX_FRAME 1
#define GPX_END 2

void log_GPX(int part,File file){
char bufferTx[256]; 
int i,y; 
if(part==GPX_HEADER){
  int year,month,day,hour,minute,sec;
  year=ubxMessage.navPvt.year;
      month=ubxMessage.navPvt.month;
      day=ubxMessage.navPvt.day;
      hour=ubxMessage.navPvt.hour;
      minute=ubxMessage.navPvt.minute;
      sec=ubxMessage.navPvt.second;
  i= sprintf(bufferTx,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");y=y+i;
  i= sprintf(&bufferTx[y],"<gpx version=\"1.0\">\n");y=y+i;
  i= sprintf(&bufferTx[y],"<time>%d-%d-%dT%d:%d:%dZ</time>\n",year,month,day,hour,minute,sec);y=y+i;
  i= sprintf(&bufferTx[y],"<trk>\n<name>ESP-GPS LOG</name>\n<trkseg>\n");y=y+i;
  file.write((const uint8_t *)&bufferTx,y);
  } 

if(part==GPX_FRAME){  
    if(ubxMessage.navPvt.nano/1000000==0){//only log every full second !!!!
      float lat,lon,hdop,speed;
      int year,month,day,hour,minute,sec;
      lat=(float)ubxMessage.navPvt.lat/10000000.0f;
      lon=ubxMessage.navPvt.lon/10000000.0f;
      hdop=ubxMessage.navPvt.pDOP/1000.0f;
      speed=ubxMessage.navPvt.gSpeed/1000.0f;
      year=ubxMessage.navPvt.year;
      month=ubxMessage.navPvt.month;
      day=ubxMessage.navPvt.day;
      hour=ubxMessage.navPvt.hour;
      minute=ubxMessage.navPvt.minute;
      sec=ubxMessage.navPvt.second;
      i= sprintf(bufferTx,"<trkpt lat=%f lon=%f >\n",lat,lon);y=y+i;
      i= sprintf(&bufferTx[y],"<time>%d-%d-%dT%d:%d:%dZ</time>\n",year,month,day,hour,minute,sec);y=y+i;
      i= sprintf(&bufferTx[y],"<hdop>%f</hdop>\n",hdop);y=y+i;
      i= sprintf(&bufferTx[y],"<extensions>\n<speed>%f</speed>\n</extensions>\n</trkpt>\n",speed);y=y+i;
      file.write((const uint8_t *)&bufferTx,y);
    }
  if(part==GPX_END){
     y= sprintf(bufferTx,"</trkseg>\n</trk></gpx>\n");
     file.write((const uint8_t *)&bufferTx,y);
     }  
}    
/*  
 </trkseg>
  </trk>
</gpx>
<trkpt lat="51.5115702" lon="5.3231929">
    <ele>10.175</ele>
    <time>2019-11-10T13:02:37Z</time>
    <hdop>4</hdop>
    <extensions>
      <speed>1.176</speed>
    </extensions>
  </trkpt>
  */
}
#endif