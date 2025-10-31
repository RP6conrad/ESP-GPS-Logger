
# ESP-GPS logger

GPS logger for Speedsurfing, based on ESP32 & Ublox  hardware.  This is a open source project for a gps-speedlogger with a ESP32 µ-controller. This logger is approved for uploading to [http://gps-speedsurfing.com](http://gps-speedsurfing.com)
![ESP GPS banner 1024x500](https://user-images.githubusercontent.com/58887243/228194449-03b4aee7-f6ac-44cf-936f-beec46ba28fb.jpg)


#### Hardware :
- e-paper display with build in ESP32 and micro SD-card(lilygo ttgo T5).
- A gps module ublox M8n / M9 / M10 is connected over serial. 
- A 2000 mAh protected lipo (103450) 
- Inductive charging coil
- Watersealed housing, electrical box or GoPro housing
#### Software
- GPS is configured for sending ubx navpvt (+navdop + navsat) messages. 
- Log rate can be 1,2,4,10 or 20 Hz (20Hz only with ublox M9). 
- Every message is logged to the sd-card. 
- Logfile type configurable : .ubx, .sbp, .gpy, .gpx, .txt. 
- gpy is the new compact open source format, developed for speedsurfing and is accepted @ gp3s.
- For speedsurfing, next values are measured and visible on the e-paper : 
	- Max speed average over 2s.
	- Max speed average over 10s.
	- 5*10s average from 5 runs.
	- Average speed over 100m, 200m, 500m, 1852 m (nautical mile)
	- Highest average over 30 min / 60 min. t
  - Alfa gate & exit live : after a jibe you see the actual "lost heigth" and "predicted lost heigth" (should be < 50m) !
	- Total distance from the session.
![GPS_hardware1](https://user-images.githubusercontent.com/58887243/213173720-7f4f0d1d-36a7-4643-a32c-57441f66037f.jpg)	
#### Interface
SW is written in Arduino IDE (C++). For easy access to the files on the sd, a ftp-server over wifi is integrated. Also OTA (over the air update) of the sw is possible. SW 5.58 : Thanks to the contribution of tritondm, the webserver is now extended and all files on the sd can be downloaded over this webserver. Also the complete configuration can be changed over the webserver !! No need for ftp anymore, but this is still possible. SW 5.75 : Autodetection Ublox M8,M9,M10 is supported. First time @boot, the type of ublox and baudrate is detected and stored in eeprom memory.
#### Arduino / library versions : 
- Arduino IDE 2.02
- ESP32 version 1.06, but next file need a change : 
- Change next file for compiling with Arduino IDE 2.02 (SD(esp32) to SD) C:\Users\andre\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.6\libraries\SD\library.properties
- ArduinoJson version 6.17.2
##### Building manual / first flashing :
https://docs.google.com/document/d/1XJD6BV1Xay4nXo0cDuo_IOtxC_f1LyG7DE7X6KGlAB0/edit?usp=sharing
##### Quick User guide
https://docs.google.com/document/d/1KfDua0H2_-J25e4L3CR8Znd8aIM6bbTPa6EjC0b7B3U/edit?usp=sharing
##### User manual :
https://docs.google.com/document/d/1j86kj3bNMID3sHCRT7QTYyrL7IHeQnGPec1LajsDfB4/edit?usp=sharing

##### A help for installing the first SW on the T5 board (written by Tobias, Arduino IDE) :
https://docs.google.com/document/d/1jIxAyi6wQ-Z860W548Xg--x6Ld8MBHJh7SCCWhSgnAs/edit?usp=sharing
##### Youtube : Information about the needed parts ! 
https://www.youtube.com/watch?v=kbG9EsX_yYk
