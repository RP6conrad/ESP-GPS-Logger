
# ESP-GPS logger

GPS logger for Speedsurfing, based on ESP32 & Ublox  hardware.  This is a open source project for a gps-speedlogger with a ESP32 µ-controller. This logger is approved for uploading to [http://gps-speedsurfing.com](http://gps-speedsurfing.com)

![GPS_hardware1](https://user-images.githubusercontent.com/58887243/213173720-7f4f0d1d-36a7-4643-a32c-57441f66037f.jpg)
#### Hardware :
- e-paper display with build in ESP32 and micro SD-card(lilygo ttgo T5).
- A gps module ublox M8n / M10 is connected over serial. 
- A 2000 mAh protected lipo (103450) 
- Inductive charging coil
- Watersealed housing, electrical box or GoPro housing
#### Software
- GPS is configured for sending ubx navpvt (+navdop + navsat) messages. 
- Log rate can be 1,2,4 or 10 Hz. 
- Every message is logged to the sd-card. 
- Logfile type configurable : .ubx, .sbp, .gpy, .gpx, .txt.
- For speedsurfing, next values are measured and visible on the e-paper : 
	- Max speed average over 2s.
	- Max speed average over 10s.
	- 5*10s average from 5 runs.
	- Average speed over 100m, 200m, 500m, 1852 m (nautical mile)
	- Highest average over 30 min / 60 min. t
	- Total distance from the session.
	
#### Interface
SW is written in Arduino IDE (C++). For easy access to the files on the sd, a ftp-server over wifi is integrated. Also OTA (over the air update) of the sw is possible. SW 5.58 : Thanks to the contribution of tritondm, the webserver is now extended and all files on the sd can be downloaded over this webserver. Also the complete configuration can be changed over the webserver !! No need for ftp anymore, but this is still possible. SW 5.68 : Ublox M10 is supported, see for the correct bin file. 
##### Building manual / first flashing :
https://docs.google.com/document/d/1XJD6BV1Xay4nXo0cDuo_IOtxC_f1LyG7DE7X6KGlAB0/edit?usp=sharing

##### User manual :
https://docs.google.com/document/d/1j86kj3bNMID3sHCRT7QTYyrL7IHeQnGPec1LajsDfB4/edit?usp=sharing

##### A help for installing the first SW on the T5 board (written by Tobias, Arduino IDE) :
https://docs.google.com/document/d/1jIxAyi6wQ-Z860W548Xg--x6Ld8MBHJh7SCCWhSgnAs/edit?usp=sharing
