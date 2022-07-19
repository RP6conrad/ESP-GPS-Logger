# ESP-GPS-Logger
GPS logger for Speedsurfing, based on ESP32 &amp; Ublox M8N hardware.
GPS_logger for speedsurfing This is a project for a gps-speedlogger with a ESP32 µ-controller. The hardware is a e-paper display with build in ESP32 and micro SD-card(lilygo ttgo T5). A gps (ublox compatible)is connected over serial. GPS is configured for sending ubx navpvt messages. Freq can be 1,2,4 or 10 Hz. Every message is logged to the sd-card. For speedsurfing, next values are measured and visible on the e-paper : Max speed 2s, 10s, 5*10s average, speed over 100m, 200m, 500m, 1852 m, max 30 min / 60 min, total distance. SW is written in Arduino IDE. For easy access to the files on the sd, a ftp-server over wifi is integrated. Also OTA (over the air update) of the sw is possible.
Manuals can be found here : 
https://docs.google.com/document/d/1XJD6BV1Xay4nXo0cDuo_IOtxC_f1LyG7DE7X6KGlAB0/edit?usp=sharing
https://docs.google.com/document/d/1j86kj3bNMID3sHCRT7QTYyrL7IHeQnGPec1LajsDfB4/edit?usp=sharing
A help for installing the first SW on the T5 board (written by Tobias) :
https://docs.google.com/document/d/1jIxAyi6wQ-Z860W548Xg--x6Ld8MBHJh7SCCWhSgnAs/edit?usp=sharing
