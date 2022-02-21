#include "E_paper.h"
extern GPS_speed M100;
extern UBXMessage ubxMessage;
static int update_epaper=2;
void Boot_screen(void){
  int offset =0;
  display.init(); 
  display.setRotation(1);
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeSansBold12pt7b);
  display.setCursor(offset,24);
  display.println("WAKE UP.....");
  display.setCursor(offset,56);
  display.println(SW_version);//change to string / array
  display.setCursor(offset,88);
  display.print("GPS logger");//display.print(calibration_speed,2);
  display.setCursor(offset,120);
  display.print("Need for speed !");
  display.update();
}
void Off_screen(void){
  int offset=0;
  float session_time=millis()/1000 ;
  display.setRotation(1);
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeSansBold12pt7b);
  display.setCursor(offset,24);
  display.println("Save session....");
  display.setCursor(offset,56);
  display.print("Time: ");
  display.print(session_time,0);
  display.print(" S");
  display.setCursor(offset,88);
  display.print("AVG: ");display.print(RTC_avg_10s,2);
  display.setCursor(offset,120);
  display.print("Dis: ");display.print(Ublox.total_distance/1000,0);
  display.update();
  delay(10000);//om te voorkomen dat update opnieuw start !!!
}
//Screen in deepsleep, update bat voltage, refresh every 4000s !!
void Sleep_screen(int info){
  int offset =info%20;
  display.init(); 
  display.setRotation(1);
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeSansBold18pt7b);
  display.setCursor(offset,24);
  display.print("Dis: ");
  display.println(RTC_distance,0);
  display.setCursor(offset,56);
  display.print("AVG: ");display.println(RTC_avg_10s,2);
  display.setCursor(offset,88);
  display.print("2s: ");display.print(RTC_max_2s);//display.println("Hz"); 
  display.setCursor(offset,120);
  display.print("Bat: ");display.print(voltage_bat,2);
  Bat_level(offset);
  display.update();
}
void Bat_level(int offset){
    float bat_symbol=(4.2-voltage_bat)*28;
    display.fillRect(offset+225,87,6,3,GxEPD_BLACK);//display.fillRect(offset+206,105,4,6,GxEPD_BLACK);
    display.fillRect(offset+222,90,12,30,GxEPD_BLACK);//monitor=(4.2-voltage_bat)*26
    display.fillRect(offset+224,94,8,(int)bat_symbol,GxEPD_WHITE);
    }
void Update_screen(int screen){
    static int count,offset,old_screen,update_delay;
    if(screen!=old_screen)update_epaper=2;//klopt niet, altijd wit scherm tussendoor 
    update_epaper=1; //was zonder else
    if(count%20<10) offset++;
    else offset--;  
    display.fillScreen(GxEPD_WHITE); 
    if(screen==SPEED){
        update_delay=100;
        int run_rectangle_length;
        int field=config.field;//standaard keuze van config.txt
        int bar_length=config.bar_length*1000/240;//standaard 100% lengte = 1852 m
        if(config.field==1){                      //alleen switchen indien config.field==1 !!!
             if((Ublox.alfa_distance/1000<400)&(alfa_window<100))field=5;//eerste 300 m na de gijp is het alfa screen zichtbaar !!
             if(Ublox.run_distance/1000>config.bar_length)field=7;//indien run langer dan 1852 m, NM scherm !!
            }
        if(config.field==2){     
            if(Ublox.run_distance/1000>config.bar_length)field=7;//indien run langer dan 1852 m, NM scherm !!
            }
        if(GPS_Signal_OK==true){
              display.setFont(&SansSerif_bold_96_nr);
              display.setCursor(offset+2,120);
              display.println(ubxMessage.navPvt.gSpeed*calibration_speed,1);//calibration_speed is 3.6/1000 !!!
              }
        else{
              display.setCursor(offset,60);
              display.setFont(&FreeSansBold18pt7b);
              display.print("Low GPS signal !");
              }
        display.setCursor(offset,24);//was offset +2
        display.setFont(&FreeSansBold18pt7b);
        display.setTextColor(GxEPD_BLACK);  
        if(field<=2){
            display.setFont(&FreeSansBold12pt7b); 
            display.print("Run ");
            display.setFont(&FreeSansBold18pt7b);
            display.print(S10.s_max_speed*calibration_speed,1);//actueel topspeed 10s van deze run
            //display.print(ref_heading,0);//dit is de referentie heading van 200 m eerder...
            display.setCursor(offset+122,24);
            display.setFont(&FreeSansBold12pt7b);
            display.print("Avg ");
            //display.print("Hd ");
            display.setFont(&FreeSansBold18pt7b);
            display.print(S10.avg_5runs*calibration_speed,1); 
            //display.print(delta_heading,0);//dit is het verschil met de referentie heading
            //display.print(ubxMessage.navPvt.heading/100000.0f,0);
            }
        if(field==3){
            display.setFont(&FreeSansBold12pt7b);
            display.print("Dis ");
            display.setFont(&FreeSansBold18pt7b);
            display.print(Ublox.total_distance/1000,0);//Total distance in meter, als test run_distance
            display.setCursor(offset+135,24);
            display.setFont(&FreeSansBold12pt7b);
            display.print("R ");//15+9+7= 31 px
            display.setFont(&FreeSansBold18pt7b);
            display.print(Ublox.run_distance/1000,0);//Actuele run_distance  4*19=76 px, 107 px
            }
        if(field==4){
            //display.setTextColor(GxEPD_BLACK);  
            display.setFont(&FreeSansBold12pt7b);
            display.print("2S ");
            display.setFont(&FreeSansBold18pt7b);
            display.print(S2.display_max_speed*calibration_speed,1);  //best 2s
            display.setCursor(offset+124,24);
            display.setFont(&FreeSansBold12pt7b);
            display.print("10S ");
            display.setFont(&FreeSansBold18pt7b);
            display.print(S10.display_max_speed*calibration_speed,1);  //best 10s run
            }
        if(field==5){
            bar_length=250*1000/240;//volle lengte = 250m
            //display.setTextColor(GxEPD_BLACK);  
            display.setFont(&FreeSansBold12pt7b);
            if(alfa_window<100){        //was <100 !!!!
                display.print("Wind ");
                display.setFont(&FreeSansBold18pt7b);
                display.print(alfa_window,0);
                }
            else{
                display.print("Alfa ");
                display.setFont(&FreeSansBold18pt7b);    
                display.print(A500.display_max_speed*calibration_speed,1);   //best Alfa on 500 m, A500.alfa_speed_max = actuele alfa !! 
                }
            display.setCursor(offset+130,24);
            display.setFont(&FreeSansBold12pt7b);
            display.print("Aa ");
            display.setFont(&FreeSansBold18pt7b);
            display.print(A500.alfa_speed_max*calibration_speed,1);   //best Alfa on 500 m, A500.alfa_speed_max = actuele alfa !! 
            }
        if(field==6){
            //display.setTextColor(GxEPD_BLACK);  
            display.setFont(&FreeSansBold12pt7b);
            display.print("0.5h ");
            display.setFont(&FreeSansBold18pt7b);
            display.print(S1800.display_max_speed*calibration_speed,1);   //best 1800s
            display.setCursor(offset+124,24);
            display.setFont(&FreeSansBold12pt7b);
            display.print("1h ");
            display.setFont(&FreeSansBold18pt7b);
            display.print(S3600.display_max_speed*calibration_speed,1);   //best 3600s
            }
         if(field==7){
            display.setFont(&FreeSansBold12pt7b); 
            display.print("NMa ");    //Actuele nautical mile
            display.setFont(&FreeSansBold18pt7b);
            display.print(M1852.m_max_speed*calibration_speed,1);//actueel topspeed 10s van deze run
            display.setCursor(offset+129,24);
            display.setFont(&FreeSansBold12pt7b);
            display.print("NM ");
            display.setFont(&FreeSansBold18pt7b);
            display.print(M1852.display_max_speed*calibration_speed,1);   //Snelste nautical mile van de sessie
            }  
        run_rectangle_length=(Ublox.alfa_distance/bar_length);
        if(bar_length){
            display.fillRect(offset,32,run_rectangle_length,8,GxEPD_BLACK);//balk voor run_distance weer te geven...
            }
        }
        if(screen==AVG_10S){
        update_delay=4000;
        display.setFont(&FreeSansBold12pt7b);
        display.setCursor(0,14);
        for(int i=9;i>4;i--){
            display.print("Run");display.print(10-i);display.print(":");
            display.print(S10.avg_speed[i]*calibration_speed,2);display.print("@");
            display.print(S10.time_hour[i]);display.print(":");
            display.println(S10.time_min[i]);
            //display.println(ubxMessage.navPvt.gSpeed/1000.0f*calibration_speed,1);
            }   
        }      
    if(screen==BOOT_SCREEN){
        update_delay=500;
        display.setFont(&FreeSansBold18pt7b);
        display.setCursor(offset,24);
        if(sdOK==true)
        display.print("SD OK!");
        else display.print("No SD!");
        if(config.dynamic_model==1)display.print(" SEA");
        else display.print(" Port");
        display.setCursor(offset,56);
        display.print(config.UBXfile);
        display.setCursor(offset,88);
        display.print("Rate: ");display.print(config.sample_rate);display.println("Hz"); 
        display.setCursor(offset,120);
        display.print("Speed: ");
        if((int)(calibration_speed*100000)==194) display.print("knots");//1.94384449 m/s to knots !!!
        if((int)(calibration_speed*1000000)==3600) display.print("km/h");
        if(screen!=old_screen)count=0;//eerste keer full update
      }
     if(screen==STATS1){                        //2s,10s,AVG,100m
        update_delay=500;
        display.setFont(&FreeSansBold18pt7b);
        display.setCursor(offset,24);
        display.print("2s: ");display.print(S2.avg_speed[9]*calibration_speed);  //best 2s, was avg_speed[9]
        display.setFont(&FreeSansBold12pt7b);
        display.setCursor(202+offset%2,22);//zodat SXX niet groter wordt dan 244 pix
        display.print("S");
        display.println(ubxMessage.navPvt.numSV);
        display.setFont(&FreeSansBold18pt7b);
        display.setCursor(offset,56);
        display.print("10sF: ");display.println(S10.display_max_speed*calibration_speed); //best 10s(Fast), was avg_speed[9]
        display.setCursor(offset,88);
        display.print("10sS: ");display.println(S10.avg_speed[5]*calibration_speed);  //langzaamste 10s(Slow) run van de sessie
        display.setCursor(offset,120);
        display.print("AVG: ");display.println(S10.avg_5runs*calibration_speed); //average 5*10s
        //display.print("M100: ");display.println(M100.m_max_speed*calibration_speed,2); //average 5*10s
        Bat_level(offset);
      }
     if(screen==STATS2){                        //alfa 500m,1852m, 1800s,total_dist
        update_delay=500;
        getLocalTime(&tmstruct, 0);
        static int toggle=0;
        display.setFont(&FreeSansBold18pt7b);
        display.setCursor(offset,24);
        display.print("Dis: ");display.print(Ublox.total_distance/1000,0);
        display.setFont(&FreeSansBold12pt7b);
        display.setCursor(202+offset%2,22);//zodat SXX niet groter wordt dan 244 pix
        display.print("S");
        display.println(ubxMessage.navPvt.numSV);
        display.setFont(&FreeSansBold18pt7b);
        display.setCursor(offset,56);
        display.print("1852m: ");display.println(M1852.display_max_speed*calibration_speed);//was avg_speed[9]
        display.setCursor(offset,88);
        if(toggle==0){
          display.print("1800S: ");display.println(S1800.display_max_speed*calibration_speed);
          display.setCursor(offset,120);
          display.print("Alfa: ");display.print(A500.avg_speed[9]*calibration_speed);   //best Alfa on 500 m 
          toggle=1;
        }
        else{
          display.print("3600S: ");display.println(S3600.display_max_speed*calibration_speed);
          display.setCursor(offset,120);
          display.print(tmstruct.tm_hour);
          display.print(":");display.print(tmstruct.tm_min);
          display.print(":");display.print(tmstruct.tm_sec);
          toggle=0;
          }
        Bat_level(offset);
      }
   if(screen==STATS3){                        //2s,10s,AVG,100m
          update_delay=500;
          display.setFont(&FreeSansBold18pt7b);
          display.setCursor(offset,24);
          display.print("100m: ");display.print(M100.avg_speed[9]*calibration_speed);  //best 2s, was avg_speed[9]
          //display.setFont(&FreeSansBold12pt7b);
          //display.setCursor(202+offset%2,22);//zodat SXX niet groter wordt dan 244 pix
          //display.print("S");
          //display.println(ubxMessage.navPvt.numSV);
          //display.setFont(&FreeSansBold18pt7b);
          display.setCursor(offset,56);
          display.print("250m: ");display.println(M250.display_max_speed*calibration_speed); //best 10s(Fast), was avg_speed[9]
          display.setCursor(offset,88);
          display.print("500m: ");display.println(M500.avg_speed[9]*calibration_speed);  //langzaamste 10s(Slow) run van de sessie
          display.setCursor(offset,120);
          display.print("Alfa: ");display.println(A500.avg_speed[9]*calibration_speed,2); //average 5*10s
          Bat_level(offset);
        }      
    if(screen==WIFI_ON){  
        update_delay=250;   
        display.setFont(&FreeSansBold18pt7b);
        display.setCursor(offset,26);
        if(Wifi_on==1){
              display.print("Wifi on  ");
              if(SoftAP_connection==true) display.print("AP !");//ap mode
              else display.print ("ST !");//station mode
              }
        else display.println("Wifi off");
        display.setCursor(offset,56);
        display.setFont(&FreeSansBold12pt7b);
        display.println(IP_adress);
        display.setCursor(offset,88);
        //display.print("SATS: ");display.print(ubxMessage.navPvt.numSV);//geen GPS meer in Wifi ON !!
        display.print("FTP status: ");display.println(ftpStatus);
        display.setCursor(offset,120);
        display.print("Bat:");display.print(voltage_bat,2);   
        if(screen!=old_screen)count=0;//eerste keer full update 
      }
    if(screen==WIFI_STATION){  
        update_delay=0;   
        display.setFont(&FreeSansBold12pt7b);
        display.setCursor(offset,26);
        display.println("Connecting to ");
        display.setCursor(offset,56);
        display.setFont(&FreeSansBold12pt7b);
        display.println(config.ssid);
        display.setCursor(offset,88);
        display.print("AP on use magnet!");
        //display.print("SATS: ");display.println(ubxMessage.navPvt.numSV);
        display.setCursor(offset,120);
        display.print("Bat: ");
        display.print(voltage_bat,2); 
        display.setCursor(offset+190,120); 
        display.print(wifi_search); 
        if(screen!=old_screen)count=0;//eerste keer full update 
      }
       if(screen==WIFI_SOFT_AP){  
        update_delay=0;   
        display.setFont(&FreeSansBold18pt7b);
        display.setCursor(offset,26);
        display.print("Wifi AP:  ");
        display.setCursor(offset,56);
        display.setFont(&FreeSansBold12pt7b);
        display.println("ESP32AP");
        display.setCursor(offset,88);
        display.print("password ");
        display.setCursor(offset,120);
        display.print("Bat: ");display.print(voltage_bat,2); 
        display.setCursor(offset+190,120); 
        display.print(wifi_search);   
        if(screen!=old_screen)count=0;//eerste keer full update 
      }
    if(count%200==0){//was 200
        if(update_epaper>0) display.update();
        offset=0;
        }
    else {
          if(update_epaper==2){display.fillScreen(GxEPD_WHITE);}//test
          if(update_epaper>0) display.updateWindow(0,0,250,122,true);//was 244,122, true !!!
          delay(update_delay);//update delay function of screen to show
          }
    old_screen=screen;
    count++;    
}

