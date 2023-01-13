#ifndef RTOS5_H
#define RTOS5_H

//#define STATIC_DEBUG        //indien gps test zonder snelheid en met wifi actief
//#define DLS                  //set date on march 26 1:55, to test daylightsaving
#define UBLOX_M10           //indien ublox M10 wordt gebruikt, andere ublox init nodig !!!
//#define ALI_M10             // native 38400 bd, so boot with 38400 
#define WAKE_UP_GPIO 39          //default 39
#define GPIO_NUM_xx GPIO_NUM_39   //type is no int
#define MIN_SPEED_START_LOGGING 2000        //was 2000 min speed in mm/s over 2 s alvorens start loggen naar SD 
#define TIME_DELAY_FIRST_FIX 10 //10 navpvt messages alvorens start loggen
#define TIME_DELAY_NEW_RUN 10 //int time_delay_new_run

#if defined(STATIC_DEBUG)
#define MIN_SPEED_START_LOGGING 0  
#endif

#endif
