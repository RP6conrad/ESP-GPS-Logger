
# ESP-GPS Logger

GPS Logger for Speedsurfing, based on ESP32 & u-blox hardware. This is an open source project for a GPS Speed Logger with a ESP32 µ-controller. This logger is approved for uploading to [http://gps-speedsurfing.com](http://gps-speedsurfing.com)

![ESP GPS banner 1024x500](https://user-images.githubusercontent.com/58887243/228194449-03b4aee7-f6ac-44cf-936f-beec46ba28fb.jpg)

#### Hardware
- e-paper display with build in ESP32 and micro SD-card - [LILYGO T5](https://lilygo.cc/products/t5-v2-4). 
- GPS module u-blox M8 / M9 / M10 is connected over serial. 
- 2000 mAh protected LiPo (103450). 
- Inductive charging coil. 
- Watersealed housing, electrical box or GoPro housing. 

#### Software
- u-blox is configured for NAV-PVT (+ NAV-DOP + NAV-SAT) messages. 
- Log rate can be 1, 2, 4, 5, 8, 10 or 20 Hz. 
- Every message is logged to the SD card. 
- Logfile type configurable : .ubx, .sbp, .gpy, .gpx, .txt. 
- GPY is the new compact open source format, developed for speedsurfing and is accepted @ GP3S.
- For speedsurfing, next values are measured and visible on the e-paper : 
	- Max speed average over 2s.
	- Max speed average over 10s.
	- 5*10s average from 5 runs.
	- Average speed over 100m, 250m, 500m, 1852m (nautical mile)
	- Highest average over 30 min / 60 min. 
  - Alfa gate & exit live : after a jibe you see the actual "lost height" and "predicted lost height" (should be < 50m) !
	- Total distance from the session.

![GPS_hardware1](https://user-images.githubusercontent.com/58887243/213173720-7f4f0d1d-36a7-4643-a32c-57441f66037f.jpg)	

#### Interface
SW is written in Arduino IDE (C++). For easy access to the files on the sd, a ftp-server over wifi is integrated. Also OTA (over the air update) of the software is possible. SW 5.58 : Thanks to the contribution of tritondm, the webserver is now extended and all files on the sd can be downloaded over this webserver. Also the complete configuration can be changed over the webserver !! No need for ftp anymore, but this is still possible. SW 5.75 : Autodetection Ublox M8,M9,M10 is supported. First time @boot, the type of ublox and baudrate is detected and stored in eeprom memory.

#### Auto upload to SpeedSurf.app
From SW 7.0 the logger can upload its .gpy files to [SpeedSurf.app](https://speedsurf.app) by itself, as soon as it is on Wi-Fi.
- **Only in the 7.x files**, from `Bin_files_B74_partition2MB` or `Bin_files_BN_partition2MB`. The 6.x files in the other folders are the same software without the upload: with it, the software does not fit in the old partition.
- **7.x needs the bigger app partition.** The firmware update page shows it: ".bin has to fit in this Partition : 1.88 MB". If it says 1.25 MB, an update over Wi-Fi cannot change that. Flash once over USB, the same way as the first flashing (see the building manual below), with `Flash_Bin_files/partitions_2MB.bin` instead of `Rtos5.ino.partitions.bin`: bootloader `0x1000`, partition table `0x8000`, `boot_app0.bin` `0xe000`, the 7.x .bin `0x10000`. After that, updates over Wi-Fi work as before. The API key and GPS settings (EEPROM) and config.txt on the SD card stay; the small internal LittleFS (only used without an SD card) is erased.
- **Config page:** paste the full API key (SpeedSurf.app, Settings > Devices) in `apiKey`, set `auto_upload` to AUTO UPLOAD ON, save, and restart the logger.
- **Wi-Fi must be there when the logger starts.** It looks for `ssid` and `ssid2` only at boot, a few seconds each; without Wi-Fi it starts logging and the radio stays off. With a phone hotspot: hotspot on first, then the logger.
- New files are written as `name$.gpy`. After a successful upload they become `name.gpy`; a file the server refuses becomes `name!.gpy` and is not sent again. Only files in the root of the SD card are uploaded, not those in /Archive.
- Back from 6.x to 7.x: check that AUTO UPLOAD is still ON.

#### Arduino / library versions : 
- Arduino IDE 2.02
- ESP32 version 1.06, but next file need a change : 
- Change next file for compiling with Arduino IDE 2.02 (SD(esp32) to SD) C:\Users\andre\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.6\libraries\SD\library.properties
- ArduinoJson version 6.17.2

##### Building manual / first flashing :

https://docs.google.com/document/d/1XJD6BV1Xay4nXo0cDuo_IOtxC_f1LyG7DE7X6KGlAB0/edit?usp=sharing

##### Quick user guide :
https://docs.google.com/document/d/1KfDua0H2_-J25e4L3CR8Znd8aIM6bbTPa6EjC0b7B3U/edit?usp=sharing

##### User manual :
https://docs.google.com/document/d/1j86kj3bNMID3sHCRT7QTYyrL7IHeQnGPec1LajsDfB4/edit?usp=sharing

##### List of contributors :
[https://logiqx.github.io/gps-details/devices/esp-gps/contributors.html](https://logiqx.github.io/gps-details/devices/esp-gps/contributors.html)

##### A help for installing the first SW on the T5 board (written by Tobias, Arduino IDE) :
https://docs.google.com/document/d/1jIxAyi6wQ-Z860W548Xg--x6Ld8MBHJh7SCCWhSgnAs/edit?usp=sharing

##### YouTube : Information about the needed parts !
https://www.youtube.com/watch?v=kbG9EsX_yYk

##### YouTube : How to flash the T5 board en set the configuration !
https://youtu.be/902EDgWqitQ

##### YouTube : Importance of shielding the lipo from inductive charging
https://www.youtube.com/watch?v=GsCrqhh-bEE&t=22s

##### YouTube : PMMA casing and 3D print design with Onshape
https://www.youtube.com/watch?v=7HcuSVd9kAE

##### YouTube : ESP-GPS soldering and assembling (PMMA /3D print casing)
https://www.youtube.com/watch?v=2NX2z1mguc8&t=4s

