#ifndef RTOS5_H
#define RTOS5_H

//#define STATIC_DEBUG        //indien gps test zonder snelheid en met wifi actief
//#define DLS                  //set date on march 26 1:55, to test daylightsaving
#define GPIO12_ACTIF        //if GPIO12 is used as wake up, standard GPIO12 function is not activated !!
#define WAKE_UP_GPIO 39          //default 39
#define GPIO_NUM_xx GPIO_NUM_39   //default GPIO_NUM_39 type is no int
#define MIN_SPEED_START_LOGGING 2000        //was 2000 min speed in mm/s over 2 s alvorens start loggen naar SD 
#define TIME_DELAY_FIRST_FIX 10 //10 navpvt messages alvorens start loggen
#define TIME_DELAY_NEW_RUN 10 //int time_delay_new_run
#define EPOCH_2022 1640995200 //start of the year 2022 1640995200
#define UBLOX_TYPE_UNKNOWN 0
#define M8_9600BD 1
#define M10_9600BD 2
#define M8_38400BD 3
#define M10_38400BD 4
#define M9_9600BD 5
#define M9_38400BD 6
#define AUTO_DETECT 0xFF

#if defined(STATIC_DEBUG)
#define MIN_SPEED_START_LOGGING 0  
#endif

#endif
