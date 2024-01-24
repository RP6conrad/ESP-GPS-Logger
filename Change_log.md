  ### Changes SW5.81
* added drop down menu for Sail and Board logos  
* added board logos Carbon Art, Thommen, Bic
* added saillogo S2_Maui=11
* gpx.h changes for better fit to the standard (http -> https, indentations, stand alone, contribution from Michael G)
* removed limits for sample-rate 
* added warning after upload if the sample-rates are too high
* ShortPush39 50ms->10ms
  ### Changes SW5.80
* Extended choice for sample rate : add 4 Hz and 8 Hz  
* extended choices for GNSS 
** M10 4GNSS : GPS+GLONAS+GALILEO+BEIDOU, max 4Hz or lost points....
** M10 3GNSS : GPS+GALILEO+BEIDOU_B1C :  8Hz possible with no lost points (default = GPS+GALILEO+BEIDOU_B1L )
** M10 3GNSS : GPS+GALILEO+GLONAS, max 5Hz or lost points....
** M10 2GNSS : GPS+GLONAS or GPS+GALILEO : 10Hz possible with no lost points 
* Sample_rate limited in function of GNSS settings (M10)
* Wifi AP bug fixes
  ### Changes SW5.79
* Speed screen NM big font, 2 digits after dec. point
* Add file name option name_date_time_MAC
* Time out nav-pvt msg -> 10s, then trouble screen
* Check for nav-pvt msg after init gps, NOK -> then trouble screeen
  ### Changes SW5.78
* Sleeptime 21600s -> 1800s, only screen update if delta_bat_voltage> 0.01 V !!
* Wake-up without refresh screen < 50 ms, with refresh 2665 ms !!
* Start_logging_speed configurable over webserver
* Watchdog when downloading file now 600s
* Added actual 0.5h and actual 1h in speed screen with "big Font"
* Speed_screens can be choosen, analog to the stat_screens : setting 841 will toggle between three screens : 1,4 and 8. Only one digit : toggle from 0...9
* Especially usefull if you have the sealed pushbutton instead of the reed-switch

  ### Changes SW5.77 (mainly by avoprykk)
#### Fixed:
* OTA_Server firmware upload page javascript.
* GPS_Data changed BUFFER_SIZE from 10000 to 9500 as for fitting into 1.5M Image partition.
* Some compiler errors about types.
#### Changed:
* E_paper>
* Display unified row heights macros, changes in screens accordingly
* ESP-GPS logo size to 24x24
* Moved time, gps and battery for all screens to bottom row infobar.
* New screen for GPS initialization.
main.cpp> Ublox initialization moved after Wifi connections closed.
#### New:
* Partial code for OTA autoupload, dev status yet.
M9 15Hz support
* Added support to PlatformIO and partially for ESP-IDF (CMakefile.txt), thats why Rtos.ino moved to main.cpp. Wrapper module: https://github.com/aivoprykk/ESP-GPS-Wrapper
* Filename Rtos.h -> Definitions.h
* Added ESP_functions.h with general functions
* Bugfix Short_push for speed_field choice
* Added field 9 : Run-AVG, Alfa, NM, 1 hour
* Bugfix bat%@boot

   ### Changes  SW 5.0
*   Automatic ### switching to portable with overspeed (>20 m/s)
*   Extra dynamic model Automotive : 2-> automotive
*   for higher accuracy, change of datatypes to double
*   All speeds are now in mm/s (format double)
*   Added STATS3 screen with M100/M500, config.Stat_screens 6
*   Added Definitions.h for general #define
*   Bugfix in new_run_detection, speed m/s -> mm/s
*   Start logging if speed>MIN_SPEED_START_LOGGING, now 2m/s
*   Added log files in oao format
*   Removed logging to csv files
   ### Changes  SW 5.1
*   Add distance of run
*   Add configurable bar in "speed" screen, reflects run_distance, default 1852m = full bar
   ### Changes  SW 5.2
*   Change to proportional font 12pt and 18pt, speed screen now has 2 variables on top
*   Replace sat nr in speed screen with other variable
*   Add a alfa indicator, the actual distance from the start point is shown, has to be < 50m
*   At the same time shows the bar the alfa distance, 250 m = full bar
*   If config.field=1: 
*          automatic switching to alfa screen after jibe until 300 m 
*          then Run /. AVG screen.
*   If config.field=2:        
*          if run distance>1852 m, switch to NM view
*          else  Run screen !!       
   ### Changes  SW 5.3       
*   No "window" anymore for alfa, needs further development
*   Some display ### Changes
*   This version goes to GP3S
   ### Changes  SW 5.4
*   Wifi stays on for 100s, first 10s searching for SSID, then still 90s AP actif if reed switch seen.
*   Station or AP, but not both at the same time !
*   Wifi screen stays as long as connection SSID or AP, no SAT nr in Wifi screen as GPS is not read
*   Changed esp32 lib from 1.0.4 to 1.0.6
   ### Changes to ESP32FTP lib, so Android FTP is now possible
*   FTP status visible in Wifi screen, FTP = 2 : waiting for connection, FTP 5 : Connection OK
   ### Changes  SW 5.42
*   Removed CR in FTP lib for directory listing, as FTP has a problem with MacOS
   ### Changes  SW 5.44
*   Several ### Changes in FTP lib for problem with MacOS
*   Bugfix for e-paper lib BN74
*   Optimization Alfa screen, Window...
*   Test for 1.54" e-paper
   ### Changes  SW 5.45
*   exit distance for alfa = estimation of alfa-circle with current heading
   ### Changes  SW 5.46
*   GPIO 25/26 are RTC, drive capability changed!! Were not working !!
*   extra Button GPIO_Pin12, pullup, give STAT4 screen !!
*   long push GPIO_Pin12 reset STAT4 screen
*   short push toggles next screen
   ### Changes  SW 5.47
*   NM bar start@minimal speed (10 m/s ?)
*   More stats screen over GPIO pin 12
*   Alfa speed screen with Window, avg speed 250m and Exit
*   Sea model does not give a gps-fix if actual height is not on sea-level, default = 0 !!!
*   wifi off screen nr of sats added
*   bugfix bat symbol if bat_voltage>4.2
*   Choice of stat_screens extended : preferred screens are chosen with their nr : Stat_screens = 523 is then stat_screen 5, 2 and 3 !!!
   ### Changes  SW 5.48
*   Added Simon screens
*   RTC offset for sleep screen 
*   RTC txt for sleep screen
*   sleep_screen "speed in km/h" or "knots" depends on calibration
*   nr of stat screens are changed (e_paper.h)
*   low_bat voltage 3.1 -> go to sleep !!!
*   update STAT4 with actual run, added extra display_speed[]
*   choice GPIO12 screens with config
*   choice Logos with config
*   choice sleep-screen, off-screen with config
*   json doc 512 -> 1024 byte
*   boot screen added config fail / ok
   ### Changes SW 5.49
*   Alfa calculation , alfa-circle² to avoid sqrt in calculations, factor 111120 -> 111195
*   Bugfix in alfa calculation (pointer + 1)
*   Shutdown : RTC data only written if new GPS-data is available
   ### Changes SW 5.50
*   Added bar graph screen which shows the speed from al the runs (max=42)
*   Scale  bar value depends on fastest run, min value = 24
*   Speed km/h or knots
   ### Changes SW 5.51
*   new run detection, min speed of 5 m/s
*   max nr of stat screens change to 9
*   bugfix Log_to_SD() before set speed to 0 with bad reception, checksum ubx was not correct with speed 0 !!!
*   Added config.gnss : set for 3 gnss possible, works only with ublox ROM 3.0 !
*   Check for GNSS setting : ubx msg MON_GNSS
*   Add setting GNSS in txt file @ end of session
   ### Changes SW 5.52
*   bugfix for bar graph 
   ### Changes SW 5.53
*   bugfix for filenaming if logUBX=0 !!!
   ### Changes SW 5.54
*   Added extra Board / Sail logo's, design by Hans Scholten
*   Added variable to config.txt : "stat_speed":2,  // speed less then 2 m/s -> stat screens
*   Filenr depends now from existing .txt files !!
*   Shut down screen : add type of E-paper (in case you forgot...)
*   Webserver can now download and delete files from the SD-card !!! 
*   Contribution from Triton_dm on github !!
   ### Changes SW5.55
*   Bugfix for choice screens with GPIO39 after boot
   ### Changes SW5.56 (Contribution from Triton_dm on github !!)
*   Added an upload section to the webserver
*   Enabled the webserver on AP Usage, prevent from stopping during downloading files
*   Add IP Address of the AP to display
*   Add a special LOGO section > 99 and add a special logo of a beer supplier hier @Achensee/Tirol
*   Change the Webinterface to a responsive design
*   prevent delete on one click
   ### Changes SW5.57 (JH)
*   Added file timestamp to webserver !!
   ### Changes SW5.58 ### Changes by tritondm !!
*   fix the download issue with phps extionsion on android chrome
*   add a config section for changing config - thats the base for the next step - config credentials for gps-speedsurfing.com and upload
*   after before upload a config_backup will created
   ### Changes SW5.60 Bugfixes + added sbp file format
*   Changed next file for compiling with Arduino IDE 2.02 (SD(esp32) to SD)
*   C:\Users\andre\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.6\libraries\SD\library.properties
*   Bugfix decoding config logo, GPIO12 and stat_screen
*   Range bar_length now from 100 - 10000
*   Removed fileformat .oao
*   Added new fileformat .sbp
   ### Changes SW5.61 First draft new open source fileformat
*   Some minor ### Changes for the webserver (triton_dm)
*   Separate Board_logo and Sail_logo config
*   Test with other fonts in speed screen, added "speed_large_font" to the config : Run, Alfa and NM can have a bigger font.
*   Changed order config.field, 1=Auto between Run, Alfa & NM, 2=Run & NM, 3=Alfa, 4=NM, 5= Total distance, 6= 2s/10s, 7= 0.5h, 8= 1h
*   Font 46pt_nr and 84pt_nr added, condensed format only digits and decimal point to save memory 
*   Added Github link to main menu
   ### Changes SW5.62
*   Added GPX file format
*   Added GPY file format
*   If no config file : Wifi AP waiting time = 120 s
*   Bugfix config webserver : no switching to km/h if knots was set
   ### Changes SW5.63
*   Added support for BN266 e-paper
*   Changed GPS-parser union to struct, every ubx message has its own struct now
*   Added ubx nav dop Msg for extracting HDOP
*   Added ubx mon ver Msf for extracting sw / hw version ublox
*   Changed baudrate ublox to 38400, necessary for 10Hz navDOP + navPVTD
*   Correction timestamp list files with FTP 
*   Added sail logo "Severne" = 10
*   Added board logo "F2" = 10
*   Bugfix for SSID with white space
*   Added watchtdog for WAKE UP screen (hangs sometimes after OTA)
*   Added loadbalance for flushfiles()
   ### Changes SW5.64
*   Bugfix garbage frame in gpy / ubx
*   Reboot ESP : ublox_off for 1s !!
*   Added support for ublox M10 (Mateksys maxM10s), define in Rtos.h
*   Time stat screen toggle configurable
*   bdrate back to 19200BD, should be ok for 10100 (nav_pvt) + 1026 bytes(nav_dop)
*   Added support for GNSS GPS + GALILEO + BEIDOU for M8 + M10
*   Correction time on sleep-screen with timezone
   ### Changes SW5.65
*   Bugfix stat screen time
   ### Changes SW 5.66
*   Added sw-version + e-paper type to Wifi-on screen + sleep screen Simon
*   baud rate back to 38400 for nav sat msg
*   Added (configurable) ubx nav sat message to ubx file, Rate = 1/10 of NAV PVT, timeshift 18.05 s for ascending order ubx msg
*   This is if you  want to evaluate the signal quality of your gps (ucenter)
*   Filename with Timestamp in it, configurable
*   .sbp file corrections, problem with uploading to gp3s, header change (0xa0, 0xa2)
*   .sbp file correction for sAcc, hDOP
*   added alfa screen to 0.5h / 1h, bar_length gives time passed 30min /60 min
*   sleep_screen Simon : 500m -> 1h best
   ### Changes SW 5.67
*   NAV SAT not with polling, but over cfg msg @ 10% rate of NAV PVT
*   NAV SAT for M10 added @ 10% rate of NAV PVT
*   removed timeshift iTow NAV SAT
*   Added extra option filenaming
*   Some ### Changes to header SBP
*   correction time bug in sleep screen
*   reset 30 min / 60 min bar if speed drops<2m/s for 120s (screen best 0.5h / 1h)
   ### Changes SW 5.68
*   Analog bat measurement to function()
*   ESP32 time is now local time, min 5 sats visible !!
*   Add check for nav pvt valid flags time&date before setting the time (thanks Peter!)
*   Check for plausible year when GPS time is set (>=2023)
*   Removed timelib.h, only sys.time.h needed
*   Fix time_bug in different screens
*   Add Ubx ID M8(ROM3 necessary,5 byte ID), saved in error.txt 
*   Add Ubx ID M10(6 byte ID), saved in error.txt 
*   Add Class GPS_NAV_INFO + Evaluation last 16 NAV_SAT msg in log file error.txt
*   Add navDOP msg to ubx file, if nav_Sat is active (for analysing data) 
   ### Changes SW 5.69
*   Change baudrate for M10 to 38400 bd, #define ALI_M10 in rtos5.h
*   Bugfix for gpy file : MakeTime -> mktime, offset from 1970 -> 1900, month -1 !!!
*   Option for other wake-up GPIO with #define in rtos5.h
*   Add type e-paper to .txt file
*   Some text in e-paper adapted : Mile ->NM, Alf->Alph, Dis->Dist
   ### Changes SW 5.70
*   Again timebug -> e-paper keyword "now" may not be used for getLocalTime() !!
*   Same issue in SD_card.cpp -> open files !!
*   GPS_SAT_INFO buffer 16 -> 10
*   Set GPS_time @ start logging
*   check for year>2023
*   While waiting for Sats / Min speed : Wifi Off with waiting Sats/Speed on screen
*   Total time in .Txt from start logging
   ### Changes SW 5.71
*   Again bugfix for time in gpy file (offset year 1970->1900, month->-1)
*   Removed #include timelib.h
*   Removed listing of directorys in webserver
*   Add sortTable() after DOM is loaded : now the files are ordered by date after loading all filenames !
*   Timestamp files with FTP command MLSD is now UTC
*   Add type of ublox to .txt file + e-paper shut down
*   https://www.mischianti.org/2021/03/28/how-to-use-sd-card-with-esp32-2/
   ### Changes SW5.72
*   .txt files configurable over webserver, if name_MAC_counter, .txt file is necessary for keeping track counter !!!
*   When Wifi, NTP time is set, if needed, directory "Archive" is created
*   When webserver start, Files older then xx days are copied to directory "Archive", are not visble in the listing anymore.
*   Added link "Archive files" in header : to move files to the Archive dir when older then archive_days
*   Added link "Archive list" in header : to list all the files on the SD, dir "Archive" included
*   Link to overview Board / Sail logo updated
*   STAT1 screen Sat -> last 2s, 3m/s for new value needed
*   STAT6 screen Mile -> NM
*   Sleep screen bat voltage -> bat percent, 4.2=100%, 3.4=0% 
   ### Changes SW5.73
*   Bugfix name ublox M10 38400bd in off screen
*   Downloading _filename bugfix, no underscore
*   timezone changed from int -> float for partial timezones, drop down menu choice  !!
*   Speed screen 5 -> distance always km/h, run -> bat symbol 
*   Added auto detection ublox type M8 / M10 and 9600bd/38400 bd
   ### Changes SW5.74
*   Debouncing reed switch with 200ms @boot
*   Shutdown screen 10s -> 5s
*   Bat voltage calibration min value 1.7 -> 1.6
*   Bugfix GPIO12 screens 
*   Changed default settings when no config.txt
*   Add watchtdog to task0 + task1 separat, Time out = 60 s
*   Changed order Wifi screen, so IP is not in the middle anymore
*   Check bat voltage@boot, if too low back to sleep. This to prevent a bootloop 
  ### Changes SW5.75
* Bugfix time format gpx files
* Added future fly logo (basti)
* Bugfix .txt file M8/M10 serial nr
* Added TROUBLE screen if no ubx message for longer then 2000 ms 
* Added SPEED2 screen with giant Font (Simon Design)
* filesizes always in MB
* override watchtdog_task0 when downloading (large) files
* Added SD Free space Mb in Boot screen
* Boot screens changes by Simon, ESP-GPS logo added 
* Added actual SW version & Type of e-paper to Firmware page
 ### Changes SW5.76
* Sleeptime 3000s -> 21600s
* nav_sat timeout 2000 ms -> 4000 ms
* add 1h to speed screen, setting 9
* bugfix wifiAP screen : name SoftAP
* shutdown screen : Saving your session or Go back to sleep
* Autodetect GPS only once, then saved in EEPROM
* Can be changed over the webserver (configuration), only set AUTODETECT again !
* Added support for the ublox M9 (Beitian BK180), logging@20Hz + 4 gnss simultan


