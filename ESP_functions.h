#ifndef ESP_FUNCTIONS
#define ESP_FUNCTIONS
String IP_adress="0.0.0.0";
const char SW_version[16]="Ver 5.83sd8";//Hier staat de software versie !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#if defined(_GxGDEH0213B73_H_) 
const char E_paper_version[16]="E-paper 213B73";
#endif
#if defined(_GxDEPG0213BN_H_) 
const char E_paper_version[16]="E-paper 213BN";
#endif
#if defined(_GxGDEM0213B74_H_) 
const char E_paper_version[16]="E-paper 213B74";
#endif
#if defined(_GxDEPG0266BN_H_) 
const char E_paper_version[16]="E-paper 266BN";
#endif
char Ublox_type[20]="Ublox unknown...";

int sdTrouble=0;
bool sdOK = false;
bool button = false;
bool LITTLEFS_OK;
bool reed = false;
bool deep_sleep = false;
bool Wifi_on=true;
bool SoftAP_connection = false;
bool GPS_Signal_OK = false;
bool long_push = false;
bool Field_choice = false;
int NTP_time_set = 0;
int Gps_time_set = 0;
bool Shut_down_Save_session = false;
bool trouble_screen = false;
extern bool downloading_file;
int GPS_OK = 0;
int analog_bat;
int first_fix_GPS,run_count,old_run_count,stat_count,GPS_delay;
int start_logging_millis;
volatile int wifi_search=10;//was 10
int ftpStatus=0;
//int time_out_nav_pvt=TIME_OUT_NAV_PVT;
int last_gps_msg=0;
int nav_pvt_message=0;
int old_message=0;
int nav_sat_message=0;
int next_gpy_full_frame=0;
//int nav_pvt_message_nr=0;
int msgType;
int GPIO12_screen=0;//keuze welk scherm
int low_bat_count;
int gps_speed;
float alfa_window;
float analog_mean=2000;
float Mean_heading,heading_SD;
int wdt_task0,wdt_task1;
int max_count_wdt_task0;
int freeSpace;
 /* variables to hold instances of tasks*/
TaskHandle_t t1 = NULL;
TaskHandle_t t2 = NULL;
byte mac[6];  //unique mac adress of esp32
IPAddress local_IP(192,168,4,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

RTC_DATA_ATTR float calibration_speed=3.6;
RTC_DATA_ATTR int offset = 0;
RTC_DATA_ATTR float RTC_distance;
RTC_DATA_ATTR float RTC_avg_10s;
RTC_DATA_ATTR float RTC_max_2s;
//Simon
RTC_DATA_ATTR short RTC_year;
RTC_DATA_ATTR short RTC_month;
RTC_DATA_ATTR short RTC_day;
RTC_DATA_ATTR short RTC_hour;
RTC_DATA_ATTR short RTC_min;
RTC_DATA_ATTR float RTC_alp;
RTC_DATA_ATTR float RTC_500m;
RTC_DATA_ATTR float RTC_1h;
RTC_DATA_ATTR float RTC_mile;
RTC_DATA_ATTR float RTC_R1_10s;
RTC_DATA_ATTR float RTC_R2_10s;
RTC_DATA_ATTR float RTC_R3_10s;
RTC_DATA_ATTR float RTC_R4_10s;
RTC_DATA_ATTR float RTC_R5_10s;
RTC_DATA_ATTR char RTC_Sleep_txt[32]="Your ID";
RTC_DATA_ATTR int RTC_Board_Logo;
RTC_DATA_ATTR int RTC_Sail_Logo;
RTC_DATA_ATTR int RTC_SLEEP_screen=0;
RTC_DATA_ATTR int RTC_OFF_screen=0;
RTC_DATA_ATTR int RTC_counter=0;
//Simon
RTC_DATA_ATTR float calibration_bat=1.75;//bij ontwaken uit deepsleep niet noodzakelijk config file lezen
RTC_DATA_ATTR float RTC_voltage_bat=3.6;
RTC_DATA_ATTR float RTC_old_voltage_bat=3.6;
RTC_DATA_ATTR int RTC_bat_choice = 0;
/*Eenmaal flankdetectie indien GPIO langer dan push_time gedrukt
* Ook variabele die dan long_pulse_time hoog blijft
* Ook variabele die optelt tot maw elke keer push
*/
class Button_push{
            public:
            Button_push(int GPIO_pin,int push_time,int long_pulse_time,int max_count);//constructor
            boolean Button_pushed(void);//return true if button is pushed longer then push_time
            boolean long_pulse;
            int button_count;
            private:
            boolean button_status, old_button_status,return_value;
            int Input_pin;
            int push_millis; 
            int time_out_millis; 
            int millis_10s;
            int max_pulse_time; 
            int max_button_count;        
};
FtpServer ftpSrv;  
GPS_data Ublox; // create an object storing GPS_data !
GPS_SAT_info Ublox_Sat;//create an object storing GPS_SAT info !
GPS_speed M100(100);
GPS_speed M250(250);
GPS_speed M500(500);
GPS_speed M1852(1852);
GPS_time S2(2);
GPS_time s2(2);
GPS_time S10(10);
GPS_time s10(10);//for  stats GPIO_12 screens, reset possible !!
GPS_time S1800(1800);
GPS_time S3600(3600);
Alfa_speed A250(50);
Alfa_speed A500(50);
Alfa_speed a500(50);//for  Alfa stats GPIO_12 screens, reset possible !!
Button_push Short_push12 (12,50,15,1); //GPIO12 pull up, 100ms push time, 15s long_pulse, count 1, STAT screen 4&5
Button_push Long_push12 (12,2000,10,4); //GPIO12 pull up, 2000ms push time, 10s long_pulse, count 4, reset STAT screen 4&5
Button_push Short_push39 (WAKE_UP_GPIO,10,10,9);//was 39
Button_push Long_push39 (WAKE_UP_GPIO,1500,10,9);//was 39

#if defined(_GxDEPG0266BN_H_) //only for screen BN266, Rolzz... !!!
GxIO_Class io(SPI, /*CS=5*/ ELINK_SS, /*DC=*/ 19, /*RST=*/4);
GxEPD_Class display(io, /*RST=*/4, /*BUSY=*/34);
#else
GxIO_Class io(SPI, /*CS=5*/ ELINK_SS, /*DC=*/ ELINK_DC, /*RST=*/ ELINK_RESET);
GxEPD_Class display(io, /*RST=*/ ELINK_RESET, /*BUSY=*/ ELINK_BUSY);
#endif

SPIClass sdSPI(VSPI);//was VSPI

const char *filename = "/config.txt";
const char *filename_backup = "/config_backup.txt"; 

void go_to_sleep(uint64_t sleep_time);
void Update_bat(void);
void taskOne( void * parameter );
void taskTwo( void * parameter);  

  /*
Method to print the reason by which ESP32 has been awaken from sleep
*/
void print_wakeup_reason(){
  analog_bat = analogRead(PIN_BAT);
  RTC_voltage_bat=analog_bat*calibration_bat/1000;
  Serial.print("Battery voltage = ");
  Serial.println(RTC_voltage_bat);
  if(RTC_voltage_bat<MINIMUM_VOLTAGE){
    Boot_screen();
    delay(1000);
    Sleep_screen(RTC_SLEEP_screen);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_xx,0);
    go_to_sleep(4000);
  }
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {    
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO");
                                 pinMode(WAKE_UP_GPIO,INPUT_PULLUP);
                                 while(millis()<200){
                                    if (digitalRead(WAKE_UP_GPIO)==1){
                                      esp_sleep_enable_ext0_wakeup(GPIO_NUM_xx,0); //was 39  1 = High, 0 = Low
                                      go_to_sleep(TIME_TO_SLEEP);
                                      break;
                                      }
                                    }
                                 rtc_gpio_deinit(GPIO_NUM_xx);//was 39   
                                 reed=1;   
                                 esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
                                 Boot_screen();
                                 break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); 
                                 break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer");
                                  analog_mean = analogRead(PIN_BAT);
                                  for(int i=0;i<10;i++){
                                        Update_bat();
                                        }
                                  esp_sleep_enable_ext0_wakeup(GPIO_NUM_xx,0); //was 39  1 = High, 0 = Low
                                  if(abs(RTC_voltage_bat-RTC_old_voltage_bat)>MINIMUM_VOLTAGE_CHANGE){
                                    Sleep_screen(RTC_SLEEP_screen);
                                    RTC_old_voltage_bat=RTC_voltage_bat;
                                    }
                                  go_to_sleep(TIME_TO_SLEEP); //was 4000
                                  break;                               
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); 
                                     break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); 
                                break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); 
              Boot_screen();
              break;
    }
}

void go_to_sleep(uint64_t sleep_time){
  deep_sleep=true;
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  Ublox_off();
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);//flash in deepsleep, CS stays HIGH!!
  gpio_deep_sleep_hold_en();
  esp_sleep_enable_timer_wakeup( uS_TO_S_FACTOR*sleep_time);
  Serial.println("Setup ESP32 to sleep for every " + String((int)sleep_time) + " Seconds");
  Serial.println("Going to sleep now");
  Serial.flush(); 
  delay(3000);//time needed for showing go to sleep screen
  esp_deep_sleep(uS_TO_S_FACTOR*sleep_time);
}

void Shut_down(void){
        long_push=true;
        Ublox_off();
        GPS_Signal_OK=false;
        GPS_delay=0;
        if(Time_Set_OK){    //Only safe to RTC memory if new GPS data is available !!
            Time_Set_OK=false;
            RTC_distance=Ublox.total_distance/1000000;
            RTC_alp=A500.display_max_speed*calibration_speed;
            RTC_500m=M500.avg_speed[9]*calibration_speed;
            RTC_1h=S3600.display_max_speed*calibration_speed;            
            RTC_mile=M1852.display_max_speed*calibration_speed;
            RTC_max_2s= S2.avg_speed[9]*calibration_speed;
            RTC_avg_10s=S10.avg_5runs*calibration_speed;
            RTC_R1_10s=S10.avg_speed[9]*calibration_speed;
            RTC_R2_10s=S10.avg_speed[8]*calibration_speed;
            RTC_R3_10s=S10.avg_speed[7]*calibration_speed;
            RTC_R4_10s=S10.avg_speed[6]*calibration_speed;
            RTC_R5_10s=S10.avg_speed[5]*calibration_speed;
            if(config.logTXT){
              Session_info(Ublox);
              Session_results_S(S2);
              Session_results_S(S10);
              Session_results_S(S1800);
              Session_results_S(S3600);
              Session_results_M(M100);
              Session_results_M(M500);
              Session_results_M(M1852);
              Session_results_Alfa(A250,M250);
              Session_results_Alfa(A500,M500);
              }
            delay(100);
            Close_files(); 
            RTC_year=(tmstruct.tm_year+1900);//local time is corrected with timezone in close_files() !!
            RTC_month=(tmstruct.tm_mon+1);
            RTC_day=(tmstruct.tm_mday);
            RTC_hour=(tmstruct.tm_hour);
            RTC_min=(tmstruct.tm_min);
            }
        RTC_old_voltage_bat=0; //to force refresh the sleep screen when shutting down !!!    
        go_to_sleep(5);//got to sleep after 5 s, this to prevent booting when GPIO39 is still low !     
}
void Update_bat(void){
    analog_bat = analogRead(PIN_BAT);
    analog_mean=analog_bat*0.1+analog_mean*0.9;
    RTC_voltage_bat=analog_mean*calibration_bat/1000;
}
void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.print("NTP Time = ");
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}  
//For RTOS, the watchdog has to be triggered
void feedTheDog_Task0(){
  TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE; // write enable TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
  TIMERG0.wdt_feed=1;                       // feed dog
  TIMERG0.wdt_wprotect=0;                   // write protect
}
void feedTheDog_Task1(){ 
  TIMERG1.wdt_wprotect=TIMG_WDT_WKEY_VALUE; // write enable
  TIMERG1.wdt_feed=1;                       // feed dog
  TIMERG1.wdt_wprotect=0;                   // write protect
} 

void OnWiFiEvent(WiFiEvent_t event){
  switch (event) {
    case SYSTEM_EVENT_STA_CONNECTED:
      Serial.println("ESP32 Connected to SSID Station mode");
      WiFi.mode(WIFI_MODE_STA);//switch off softAP
      Serial.println("ESP32 Soft AP switched off");
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:         //test
      Serial.println("ESP32 disconnected to WIFI");
      //SoftAP_connection=false;
      break;
    case SYSTEM_EVENT_STA_GOT_IP://  @this event no IP !!!         SYSTEM_EVENT_STA_CONNECTED:
      Serial.println("ESP32 Connected to WiFi Network");
      IP_adress =  WiFi.localIP().toString();
      break;
    case SYSTEM_EVENT_AP_START:
      WiFi.softAPConfig(local_IP, gateway, subnet);  
      Serial.println("ESP32 soft AP started");
      break;
    case SYSTEM_EVENT_AP_STACONNECTED:
      Serial.println("Station connected to ESP32 soft AP");
      IP_adress =  WiFi.softAPIP().toString();
      SoftAP_connection=true;
      break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
      Serial.println("Station disconnected from ESP32 soft AP");
      SoftAP_connection=false;
      break;
    default: break;
  }
}
static void setTimeZone(long offset, int daylight)
{
    char cst[17] = {0};
    char cdt[17] = "DST";
    char tz[33] = {0};
    if(offset % 3600){
        sprintf(cst, "UTC%ld:%02ld:%02ld", offset / 3600, abs((offset % 3600) / 60), abs(offset % 60));
    } else {
        sprintf(cst, "UTC%ld", offset / 3600);
    }
    if(daylight != 3600){
        long tz_dst = offset - daylight;
        if(tz_dst % 3600){
            sprintf(cdt, "DST%ld:%02ld:%02ld", tz_dst / 3600, abs((tz_dst % 3600) / 60), abs(tz_dst % 60));
        } else {
            sprintf(cdt, "DST%ld", tz_dst / 3600);
        }
    }
    sprintf(tz, "%s%s", cst, cdt);
    Serial.println(tz);
    setenv("TZ", tz, 1);
    tzset();
}
void IRAM_ATTR isr() {
	wifi_search=150;//to prevent action @ boot
}
/*Eenmaal flankdetectie indien GPIO langer dan push_time gedrukt
* Ook variabele die dan long_pulse_time hoog blijft
* Ook variabele die optelt tot maw elke keer push
*/
Button_push::Button_push(int GPIO_pin, int push_time, int long_pulse_time, int max_count) {
  pinMode(GPIO_pin, INPUT_PULLUP);
  Input_pin = GPIO_pin;
  time_out_millis = push_time;
  max_pulse_time = long_pulse_time;
  max_button_count = max_count;
}
boolean Button_push::Button_pushed(void) {
  return_value = false;
  button_status = digitalRead(Input_pin);
  if (digitalRead(Input_pin) == 1) push_millis = millis();
  if (((millis() - push_millis) > time_out_millis) & (old_button_status == 0)) {
    button_count++;
    if (button_count > max_button_count) button_count = 0;
    old_button_status = 1;
    millis_10s = millis();
    //Serial.print ("Class button_count ");Serial.print(button_count);
    return_value = true;
  } else return_value = false;
  if ((millis() - millis_10s) < (1000 * max_pulse_time)) long_pulse = true;
  else long_pulse = false;
  if (digitalRead(Input_pin) == 1) old_button_status = 0;
  return return_value;
}
#endif
