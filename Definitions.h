#ifndef RTOS5_H
#define RTOS5_H

//#define STATIC_DEBUG        //indien gps test zonder snelheid en met wifi actief
//#define DLS                  //set date on march 26 1:55, to test daylightsaving
#define T5_E_PAPER           //for compiling without display funtions....
#define GPIO12_ACTIF        //if GPIO12 is used as wake up, standard GPIO12 function is not activated !!
//#define USE_AUTO_OTA_UPDATE
#define VERSION 0
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
#define M8_115200BD 7
#define M9_115200BD 8
#define M10_115200BD 9

#define AUTO_DETECT 0xFF
/*Pin definitions**************************************************************************/
#define SPI_MOSI 23
#define SPI_MISO -1//standaard is MISO GPIO 19 !!!!
#define SPI_CLK 18
#define SDCARD_SS 13
#define SDCARD_CLK 14
#define SDCARD_MOSI 15
#define SDCARD_MISO 2
#define GO_TO_SLEEP_GPIO 39          //default 39
#define WAKE_UP_GPIO_NUM GPIO_NUM_39   //default GPIO_NUM_39 type is no int
#define GO_TO_SLEEP_PULLDOWN 19  //same function as 39, but here default is pulldown
#define HOLD_PIN 21   //pin for high side switch, is high until shutdown ready
#define RXD2 32 //yellow is Tx Ublox, Beitian white is Tx
#define TXD2 33 //green is Rx Ublox, Beitian green is Rx
#define PIN_BAT 35 //adc for bat voltage
#define UBLOX_POWER1 25
#define UBLOX_RTC_GPIO1 GPIO_NUM_25
#define UBLOX_POWER2 26
#define UBLOX_RTC_GPIO2 GPIO_NUM_26
#define UBLOX_POWER3 27
#define UBLOX_GPIO3 GPIO_NUM_27
/*Other definitions...**********************************************************************/
#define CALIBRATION_BAT_V 1.7 //voor proto 1
#define VOLTAGE_100 4.15     //reading 100% bat, after end charging voltage drops to 4.15 V
#define VOLTAGE_0 3.4       //reading 0% bat
#define VOLTAGE_LOW 25      //reading 25% bat
#define MINIMUM_VOLTAGE 3.2     // if lower then minimum_voltage, back to sleep.....was 3.2
#define MINIMUM_VOLTAGE_CHANGE  0.1  //refresh screen if change > 0.01
#define STARTVALUE_HIGHEST_READ 2300  //12 bit ADC, 4.2 V over voltagedivider
#define MAXVALUE_HIGHEST_READ 2700
#define TOLERANCE 100   //mean adc highest read + TOLERANCE before auto calibrate !!!
#define FULLY_CHARGED_LIPO_VOLTAGE 4200.0 // to calculate calibration factor :  4200.0/ADC value bij highest reading while charging
#define uS_TO_S_FACTOR 1000000UL /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  3600UL   //1800UL        /* Time ESP32 will go to sleep (no for 60min, only refresh screen if delta bat > 0.1 V) */
#define WDT_TIMEOUT 120             //120 seconds WDT, opgelet zoeken naar ssid time-out<dan 10s !!!
#define MAX_COUNT_WDT_TASK0 10   // 600 seconds max downloadtime for files !!!
#define MIN_numSV_FIRST_FIX 5     //alvorens start loggen, changed from 4 to 5 7.1/2023
#define MAX_Sacc_FIRST_FIX 2     //alvorens start loggen
#define MIN_numSV_GPS_SPEED_OK 4  //min aantal satellieten voor berekenen snelheid, anders 
#define MAX_Sacc_GPS_SPEED_OK 1   //max waarde Sacc voor berekenen snelheid, anders 0
#define MAX_GPS_SPEED_OK 40       //max snelheid in m/s voor berekenen snelheid, anders 0
#define EEPROM_SIZE 32            //use 8 bytes in eeprom for saving type of ublox
#define TIME_OUT_NAV_PVT 10000    //10s time out ubx nav-pvt msg before screen "TROUBLE" 
#define FORMAT_LITTLEFS_IF_FAILED true
#endif
