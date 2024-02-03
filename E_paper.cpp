#include "E_paper.h"
#include "Definitions.h"

#define ROW_SPACING 3
// calculate y position from row-number, font-pixel and rown spacing
#define Y_POS 0

//#define ESP_GPS_LOGO_40 display.drawExampleBitmap(ESP_GPS_LOGO_40, offset+displayWidth-12-40, 6, 40, 40, GxEPD_BLACK);
// esp logo
#define ESP_GPS_LOGO_40 display.drawExampleBitmap(ESP_GPS_logo_40, offset+displayWidth-12-40, 0, 40, 40, GxEPD_BLACK);
#define ESP_GPS_LOGO_48 display.drawExampleBitmap(ESP_GPS_logo   , offset+displayWidth-12-48, 0, 48, 48, GxEPD_BLACK);

// bottom area 15px reserved to info bar
#define INFO_BAR_HEIGHT 15
#define INFO_BAR_TOP (displayHeight-INFO_BAR_HEIGHT)
#define INFO_BAR_ROW (displayHeight-2)

#define CURSOR(row, font)          display.setCursor(offset               ,font-9+row*(4/3*font+2*ROW_SPACING)-ROW_SPACING);
#define CURSOR2(row, font)         display.setCursor(offset+displayWidth/2,row*(4/3*font+2*ROW_SPACING)-ROW_SPACING);
#define CURSOR_COL(row, col, font) display.setCursor(offset+col           ,row*(4/3*font+2*ROW_SPACING)-ROW_SPACING);

int ROW_1S;
int Font;
int16_t col1;
int16_t col2;
int16_t col3;
int16_t col4;

int16_t row1boot;
int16_t row2boot;
int16_t row3boot;
int16_t row4boot;
int16_t row5boot;


char time_now[8];
char time_now_sec[12];
int16_t displayHeight;
int16_t displayWidth;

void InfoBar(int offset);
void InfoBarRtc(int offset);
void Speed_in_Unit(int offset);
void sdCardInfo(int offset, int skip_if_not_ok);
const char * gpsChip(int longname);

/*
void Row2Pix(int row, int pix, int space){
  Y_POS==row*(4/3*pix+2*space)-space;
}
*/

//void CURSOR(int row, int font){
//  display.setCursor(offset               ,font-9+row*(4/3*font+2*ROW_SPACING)-ROW_SPACING);
//}

//void CURSORpos(int row, int font){
//  display.setCursor(offset               ,row*(4/3*font+2*ROW_SPACING)-ROW_SPACING);
//}

void SetCols(){
  if(displayHeight<130) {
    col1=0+offset;
    col2=34+offset;
    col3=90+offset;
    col4=146+offset;
  } else {
    col1=0+offset;
    col2=40+offset;
    col3=107+offset;
    col4=173+offset;
  }
}

void SetBoardLogo(){
  if(RTC_Board_Logo==1)  display.drawExampleBitmap(Starboard_logoS_zwart , displayWidth-58, 0, 48, 48, GxEPD_BLACK);
  if(RTC_Board_Logo==2)  display.drawExampleBitmap(Fanatic_logoS_zwart   , displayWidth-52, 0, 48, 48, GxEPD_BLACK);
  if(RTC_Board_Logo==3)  display.drawExampleBitmap(JP_logoS_zwart        , displayWidth-52, 0, 48, 48, GxEPD_BLACK);
  if(RTC_Board_Logo==4)  display.drawExampleBitmap(NoveNove_logoS_zwart  , displayWidth-52, 0, 48, 48, GxEPD_BLACK);
  if(RTC_Board_Logo==5)  display.drawExampleBitmap(Mistral_logoS_zwart   , displayWidth-52, 0, 48, 48, GxEPD_BLACK);
  if(RTC_Board_Logo==6)  display.drawExampleBitmap(Goya_logoS_zwart      , displayWidth-52, 0, 48, 48, GxEPD_BLACK);
  if(RTC_Board_Logo==7)  display.drawExampleBitmap(Patrik_logoS_zwart    , displayWidth-52, 0, 48, 48, GxEPD_BLACK);
  if(RTC_Board_Logo==8)  display.drawExampleBitmap(Severne_logoS_zwart   , displayWidth-52, 0, 48, 48, GxEPD_BLACK);
  if(RTC_Board_Logo==9)  display.drawExampleBitmap(Tabou_logoS_zwart     , displayWidth-52, 0, 48, 48, GxEPD_BLACK);
  if(RTC_Board_Logo==10) display.drawExampleBitmap(F2_logo_zwart         , displayWidth-52, 0, 48, 48, GxEPD_BLACK);
  if(RTC_Board_Logo==11) display.drawExampleBitmap(epd_bitmap_Schwechater, displayWidth-52, -10, 79, 132, GxEPD_BLACK);  
  if(RTC_Board_Logo==12) display.drawExampleBitmap(Thommen1_logo_BW      , displayWidth-52, 0, 64, 48, GxEPD_BLACK);
  if(RTC_Board_Logo==13) display.drawExampleBitmap(BIC_logo_BW           , displayWidth-52, 0, 48, 48, GxEPD_BLACK);  
  if(RTC_Board_Logo==14) display.drawExampleBitmap(Carbon_art            , displayWidth-52, 0, 48, 39, GxEPD_BLACK); 
  if(RTC_Board_Logo==15) display.drawExampleBitmap(FutureFly_logo_zwart  , displayWidth-52, 0, 48, 48, GxEPD_BLACK);       
}
void SetSailLogo(){
  if(RTC_Sail_Logo==1)   display.drawExampleBitmap(GAsails_logoS_zwart   , displayWidth-58, 50, 48, 48, GxEPD_BLACK);
  if(RTC_Sail_Logo==2)   display.drawExampleBitmap(DuoTone_logoS_zwart   , displayWidth-52, 50, 48, 48, GxEPD_BLACK);
  if(RTC_Sail_Logo==3)   display.drawExampleBitmap(NP_logoS_zwart        , displayWidth-52, 50, 48, 48, GxEPD_BLACK);
  if(RTC_Sail_Logo==4)   display.drawExampleBitmap(Pryde_logoS_zwart     , displayWidth-52, 50, 48, 48, GxEPD_BLACK);
  if(RTC_Sail_Logo==5)   display.drawExampleBitmap(Loftsails_logoS_zwart , displayWidth-52, 50, 48, 48, GxEPD_BLACK);
  if(RTC_Sail_Logo==6)   display.drawExampleBitmap(Gunsails_logoS_zwart  , displayWidth-52, 50, 48, 48, GxEPD_BLACK);
  if(RTC_Sail_Logo==7)   display.drawExampleBitmap(Point7_logoS_zwart    , displayWidth-52, 50, 48, 48, GxEPD_BLACK);
  if(RTC_Sail_Logo==8)   display.drawExampleBitmap(Simmer_logoS_zwart    , displayWidth-52, 50, 48, 48, GxEPD_BLACK);
  if(RTC_Sail_Logo==9)   display.drawExampleBitmap(Naish_logoS_zwart     , displayWidth-52, 50, 48, 48, GxEPD_BLACK); 
  if(RTC_Sail_Logo==10)  display.drawExampleBitmap(Severne_logoS_zwart   , displayWidth-52, 50, 48, 48, GxEPD_BLACK);
  if(RTC_Sail_Logo==11)  display.drawExampleBitmap(S2maui_logoS_zwart    , displayWidth-52, 50, 48, 48, GxEPD_BLACK);  
}

void SetFont_Sleep(){
  if(displayHeight<130) {
    display.setFont(&SF_Distant_Galaxy9pt7b);
  } else {
    display.setFont(&SF_Distant_Galaxy12pt7b);
  }
}

void SetFont_Mono(){
  if(displayHeight<130) {
    display.setFont(&FreeMonoBold9pt7b);
  } else{
    display.setFont(&FreeMonoBold12pt7b);
  }
}

void SetFont_S(){
  if(displayHeight<130) {
    display.setFont(&FreeSansBold6pt7b);
    Font=6;
  } else {
    display.setFont(&FreeSansBold9pt7b);
    Font=9;
  }
}
void SetFont_M(){
  if(displayHeight<130) {
    display.setFont(&FreeSansBold9pt7b);
    Font=9;
  } else {
    display.setFont(&FreeSansBold12pt7b);
    Font=12;
  }
}
void SetFont_L(){
  if(displayHeight<130) {
    display.setFont(&FreeSansBold12pt7b);
    Font=12;
  } else {
    display.setFont(&FreeSansBold18pt7b);
    Font=18;
  }
}
void SetFont_XL(){
  if(displayHeight<130) {
    display.setFont(&FreeSansBold18pt7b);
    Font=18;
  } else {
    display.setFont(&FreeSansBold24pt7b);
    Font=24;
  }
}

int device_boot_log(int rows, int ws) {
  SetFont_M();
  Serial.print("Font=");Serial.print(Font);
  CURSOR(2,Font); display.print(SW_version);
  CURSOR(3,Font); sdCardInfo(offset,0);
  CURSOR(4,Font); display.printf("Display size %dx%d\n",displayWidth,displayHeight);
  CURSOR(5,Font); display.printf("Gps %s at %dHz", gpsChip(1), config.sample_rate);
  
//  return r;
}
#define DEVICE_BOOT_LOG(rows) device_boot_log(rows,0)

int rowTestUnit(int xPos) {
  int16_t x1, y1, box_padding, rows, rowheight;
  uint16_t w, h;
  display.getTextBounds("000", xPos, INFO_BAR_TOP, &x1, &y1, &w, &h);
  Serial.print("000"); Serial.print(" getTextBounds got ("); Serial.print(x1); Serial.print(", "); Serial.print(y1); Serial.print(", "); Serial.print(w); Serial.print(", "); Serial.print(h); Serial.println(")"); 
  rows = INFO_BAR_TOP/(h+ROW_SPACING);
  rowheight = INFO_BAR_TOP/rows;
  box_padding = ((rowheight - h)/2);
  Serial.printf("fontbounds: rows:%d rowheight:%d,  fontheight: %d, spacing:%d, box_padding:%d\n", rows, rowheight, h, ROW_SPACING, box_padding);
    for(int row=0,i=0,j=INFO_BAR_TOP; i<j; i+=rowheight,++row){
      
      if(row==0 && box_padding>0)
        i-=box_padding;
      if(i>0){
        display.setCursor(xPos, i);
        display.println(i);
      }
      display.fillRect(xPos-5, i, 5, 1, GxEPD_BLACK);
    }
    return  xPos+w+10;
}

void rowTest() {
  int16_t x1, y1, box_padding, xPos=20, rows, rowheight;
  uint16_t w, h;
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  //display.fillRect(0, 5, displayWidth, 1, GxEPD_BLACK);
  //display.fillRect(0, INFO_BAR_TOP-1, displayWidth, 1, GxEPD_BLACK);
  //display.fillRect(0, 0, 1, INFO_BAR_TOP, GxEPD_BLACK);
  //6pt test
  display.setFont(&FreeSansBold6pt7b);
  xPos=rowTestUnit(xPos);
  //9pt test
  display.setFont(&FreeSansBold9pt7b);
  xPos=rowTestUnit(xPos);
  //12pt test
  display.setFont(&FreeSansBold12pt7b);
  xPos=rowTestUnit(xPos);
  //18pt test
  display.setFont(&FreeSansBold18pt7b);
  xPos=rowTestUnit(xPos);
  //24pt test
  display.setFont(&FreeSansBold24pt7b);
  xPos=rowTestUnit(xPos);
  display.updateWindow(0,0,displayWidth,displayHeight,true);
  delay(100);
}

void Boot_screen(void){
  display.init();
  display.setRotation(1);
  displayHeight = display.height();
  displayWidth = display.width();
  //rowTest();
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
//  int font=12;
  SetFont_M();
  ESP_GPS_LOGO_48;
  InfoBarRtc(offset);
//  TITLE;
  if(RTC_voltage_bat<MINIMUM_VOLTAGE){
    CURSOR(1,Font); display.printf("EPS-GPS sleeping");
    CURSOR(2,Font); display.print("Go back to sleep...");
    CURSOR(3,Font); display.printf("Voltage to low: %f", RTC_voltage_bat);
    CURSOR(4,Font); display.print("Please charge battery!");
    display.update();
  }
  else {
    CURSOR(1,Font); display.printf("ESP-GPS booting");
    display.updateWindow(0,0,displayWidth,displayWidth,true); 
    delay(100);
//    display.update();
  }
}
void Off_screen(int choice){//choice 0 = old screen, otherwise Simon screens
  //int offset=0;
  float session_time=(millis()-start_logging_millis)/1000 ;
  display.setRotation(1);
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  SetFont_M();
  if(choice==0){
    ESP_GPS_LOGO_48;
    CURSOR_COL(1, offset, Font) display.printf("ESP-GPS saving");
//    CURSOR(1,Font); display.printf("ESP-GPS saving");
    if(Shut_down_Save_session==true){
      CURSOR(2,Font); display.print("Saving session");
      CURSOR(3,Font); display.print("Time: "); display.print(session_time,0); display.print(" s");
      CURSOR(4,Font); display.print("AVG: ");display.print(RTC_avg_10s,2);
      CURSOR(5,Font); display.print("Dist: ");display.print(Ublox.total_distance/1000,0);
    }
    else{
      display.println("Going back to sleep");
    }
  }
  else{
    Serial.println("Off_screen Simon");
      
    ESP_GPS_LOGO_48;
    CURSOR(1,Font); display.print("ESP-GPS saving");
    //display.println("Need for speed!");
    if(Shut_down_Save_session==true){
      CURSOR(2,Font); display.print("Saving session");
      CURSOR(3,Font); display.print("Time: "); display.print(session_time,0); display.print(" s");
      CURSOR(4,Font); display.print("AVG: ");display.print(RTC_avg_10s,2);
      CURSOR(5,Font); display.print("Dist: ");display.print(Ublox.total_distance/1000,0);
    } else {
      CURSOR(2,Font); display.print("Going back to sleep");
    }
  }
  InfoBar(offset);
//  display.updateWindow(0,0,displayWidth,displayHeight,true);
  display.update();
  delay(10000);//om te voorkomen dat update opnieuw start !!!
}
//Screen in deepsleep, update bat voltage, refresh every 4000s !!
void Sleep_screen(int choice){
  if (offset>9)offset--;
  if (offset<1)offset++;
  display.init(); 
  display.setRotation(1);
  displayHeight = display.height();
  displayWidth = display.width();
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  InfoBarRtc(offset);
  Speed_in_Unit(offset);
  if(choice==9){
    SetFont_L();
    CURSOR(1,Font); display.print("Dist: "); display.print(RTC_distance,0);
    CURSOR(2,Font); display.print("AVG: ");display.print(RTC_avg_10s,2);
    CURSOR(3,Font); display.print("2s: ");display.print(RTC_max_2s);
    display.updateWindow(0,0,displayWidth,displayHeight,true);
  }
  else{
    SetFont_M();
    SetBoardLogo();
    SetSailLogo();
    SetCols();
/*
    if(displayHeight<130) {
      col1=0;
      col2=34;
      col3=90;
      col4=146;
    } else {
      col1=0;
      col2=40;
      col3=107;
      col4=173;
    }
*/
Serial.print("displayHeight="); Serial.println(displayHeight);
Serial.print("col1="); Serial.println(col1);
Serial.print("col2="); Serial.println(col2);
Serial.print("col3="); Serial.println(col3);
Serial.print("col4="); Serial.println(col4);
     
    display.setRotation(1);
    SetFont_Mono();
    CURSOR_COL(1,col1,Font); display.print("AV:");
    CURSOR_COL(2,col1,Font); display.print("R1:");
    CURSOR_COL(3,col1,Font); display.print("R2:");
    CURSOR_COL(4,col1,Font); display.print("R3:"); 
    CURSOR_COL(5,col1,Font); display.print("R4:");
    CURSOR_COL(6,col1,Font); display.print("R5:"); 
    SetFont_Sleep();
    CURSOR_COL(7,col1,Font); display.print(RTC_Sleep_txt);
    SetFont_M();
    CURSOR_COL(1,col2,Font); display.print(RTC_avg_10s,2);
    CURSOR_COL(2,col2,Font); display.print(RTC_R1_10s,2);
    CURSOR_COL(3,col2,Font); display.print(RTC_R2_10s,2);
    CURSOR_COL(4,col2,Font); display.print(RTC_R3_10s,2); 
    CURSOR_COL(5,col2,Font); display.print(RTC_R4_10s,2);
    CURSOR_COL(6,col2,Font); display.print(RTC_R5_10s,2); 
    SetFont_Mono();
    CURSOR_COL(1,col3,Font); display.print("2sec");
    CURSOR_COL(2,col3,Font); display.print("Dist:");
    CURSOR_COL(3,col3,Font); display.print("Alph:");
    CURSOR_COL(4,col3,Font); display.print("1h  :"); 
    CURSOR_COL(5,col3,Font); display.print("NM  :");
    CURSOR_COL(6,col3,Font); display.print("500m:"); 
    SetFont_M();
    CURSOR_COL(1,col4,Font); display.print(RTC_max_2s,2);
    CURSOR_COL(2,col4,Font); display.print(RTC_distance,2);
    CURSOR_COL(3,col4,Font); display.print(RTC_alp,2);
    CURSOR_COL(4,col4,Font); display.print(RTC_1h,2); 
    CURSOR_COL(5,col4,Font); display.print(RTC_mile,2);
    CURSOR_COL(6,col4,Font); display.print(RTC_500m,2); 

//    display.updateWindow(0,0,displayWidth,displayHeight,true);
    display.update();
  }
}
int update_time() {
  int ret = 0;
  if(!NTP_time_set) {
    if(!Gps_time_set) {
      if(Set_GPS_Time(config.timezone)) Gps_time_set=1;
    }
  }
  if((!Gps_time_set && !NTP_time_set)||!getLocalTime(&tmstruct)) return 1;
  sprintf(time_now,"%02d:%02d",tmstruct.tm_hour,tmstruct.tm_min);
  sprintf(time_now_sec,"%02d:%02d:%02d",tmstruct.tm_hour,tmstruct.tm_min,tmstruct.tm_sec);
  return ret;
}

//for print hour&minutes with 2 digits
void time_print(int time){
  if (time<10)display.print("0"); display.print(time);
}
void Bat_level(int X_offset,int Y_offset){
  float bat_symbol=0;
  display.fillRect(X_offset+3,Y_offset,6,3,GxEPD_BLACK);
  display.fillRect(X_offset,Y_offset+3,12,30,GxEPD_BLACK);//monitor=(4.2-RTC_voltage_bat)*26
  if(RTC_voltage_bat<VOLTAGE_100) {
    bat_symbol=(VOLTAGE_100-RTC_voltage_bat)*28;
    display.fillRect(X_offset+2,Y_offset+7,8,(int)bat_symbol,GxEPD_WHITE);
  }
}
void Bat_level_Simon(int offset){
  float bat_perc=100*(1-(VOLTAGE_100-RTC_voltage_bat)/(VOLTAGE_100-VOLTAGE_0));
  if (bat_perc<0) bat_perc=0;
  if (bat_perc>100) bat_perc=100;
  int batW=8;
  int batL=15;
  int posX=displayWidth - batW-10;
  int posY=displayHeight - batL;
  int line=2;
  int seg=3;
  int segW=batW-2*line;
  int segL=(batL-0.25*batW-2*line-(seg-1))/seg;
  display.fillRect(offset+posX,posY,0.5*batW,0.25*batW,GxEPD_BLACK);//battery top
  display.fillRect(offset+posX-0.25*batW,posY+0.25*batW,batW,batL,GxEPD_BLACK);//battery body
  if (bat_perc<67) display.fillRect(offset+posX-0.25*batW+line, posY+0.25*batW+line,            segW, segL,GxEPD_WHITE);
  if (bat_perc<33) display.fillRect(offset+posX-0.25*batW+line, posY+0.25*batW+line+1*(segL+1), segW, segL,GxEPD_WHITE);
  if (bat_perc<1)  display.fillRect(offset+posX-0.25*batW+line, posY+0.25*batW+line+2*(segL+1), segW, segL,GxEPD_WHITE);
//  Serial.printf("info bar cursor pos: %d, display height: %d\n", INFO_BAR_ROW,displayHeight);
  SetFont_M();
//  display.setCursor(displayWidth-8,(INFO_BAR_ROW-ROW_1M));
//  display.print("-");
  if (bat_perc<100) display.setCursor(offset+displayWidth-60,(INFO_BAR_ROW));
  else display.setCursor(offset+displayWidth-60,(INFO_BAR_ROW));
  display.print(int(bat_perc)); display.print("%");
}
void Sats_level(int offset){
  if(!ubxMessage.monVER.swVersion[0]) return;
  int circelL=5;
  int circelS=2;
  int posX=displayWidth-83+offset;
  int posY=INFO_BAR_TOP;//-(circelL+2*circelS); 
  int satnum = ubxMessage.navPvt.numSV;
  display.drawExampleBitmap(ESP_Sat_15, posX, posY, 15, 15, GxEPD_BLACK);
  SetFont_M();
  display.setCursor(posX-(satnum<10?9:18),INFO_BAR_ROW);
  display.print(satnum);
}
const char * gpsChip(int longname) {
  switch(config.ublox_type) {
    case M8_9600BD:
      return longname ? "M8 9.6Kbd" : "M8";
      break;
    case M8_38400BD:
      return longname ? "M8 38.4Kbd" : "M8";
      break;
    case M9_9600BD:
      return longname ? "M9 9.6Kbd" : "M9";
      break;
    case M9_38400BD:
      return longname ? "M9 38.4Kbd" : "M9";
      break;
    case M10_9600BD:
      return longname ? "M10 9.6Kbd" : "M10";
      break;
    case M10_38400BD:
      return longname ? "M10 38.4Kbd" : "M10";
      break;
    default:
      return "unknown";
      break;
  }
}     
void M8_M10(int offset){
  SetFont_M();
  display.setCursor(offset+110,INFO_BAR_ROW);
  display.print(gpsChip(0));
}
int Time(int offset){
  if(!update_time()){
    SetFont_M();
    display.setCursor(offset,INFO_BAR_ROW);
    display.print(time_now);
  }
  return 0;
}    
int TimeRtc(int offset){
  SetFont_M();
  display.setCursor(offset,INFO_BAR_ROW);
  display.printf("%d:%d",RTC_hour,RTC_min);
  return 0;
}     
int DateTimeRtc(int offset){
  SetFont_M();
  display.setCursor(offset,INFO_BAR_ROW);
  display.printf("%02d:%02d %02d-%02d-%02d",RTC_hour,RTC_min,RTC_day,RTC_month,RTC_year);
  return 0;
}
void InfoBar(int offset) {
  Bat_level_Simon(offset);
  Sats_level(offset);
  if(ubxMessage.navPvt.numSV>4) M8_M10(offset);
  Time(offset);
  //display.updateWindow(0,INFO_BAR_TOP+3,displayWidth,INFO_BAR_HEIGHT-3,true); 
}
void InfoBarRtc(int offset) {
  Bat_level_Simon(offset);
  DateTimeRtc(offset);
}
void Speed_in_Unit(int offset){
  display.setRotation(0);
  display.setFont(&FreeSansBold6pt7b);
  display.setCursor(displayHeight/4,offset+displayWidth-5);//was 30, 249
  if((int)(calibration_speed*100000)==194) display.print("speed in knots");//1.94384449 m/s to knots !!!
  if((int)(calibration_speed*1000000)==3600) display.print("speed in km/h");
  display.setRotation(1);
} 
void sdCardInfo(int offset, int skip_if_not_ok) {
  if(sdOK!=true && skip_if_not_ok) return;
  display.print("SD ");
  if(sdOK!=true) display.print("fail!!!");
  else if(freeSpace>0){
    display.printf("free: %dMb\n",freeSpace);
  }
  else display.print("OK");
}
void Update_screen(int screen){
  static int count,old_screen,update_delay;
  update_time();
  update_epaper=1; //was zonder else
  
  if(count%20<10) offset++;
  else offset--; 
  if(offset>10)offset=0;
  if(offset<0)offset=0;
  display.fillScreen(GxEPD_WHITE);
  SetFont_M();
  if(screen!=SPEED) InfoBar(offset);
  if(screen==BOOT_SCREEN){
    update_delay=1000;
    ESP_GPS_LOGO_48;
    SetFont_M();
    CURSOR(1,Font); display.print("ESP-GPS config");
    if(screen!=old_screen)count=0;//eerste keer full update
    Speed_in_Unit(offset);
    delay(1000);
  }
  if(screen==GPS_INIT_SCREEN){
    update_delay=100;
    ESP_GPS_LOGO_48;
    SetFont_M();
    CURSOR(1,Font); display.print("ESP-GPS GPS init");
    if(!ubxMessage.monVER.hwVersion[0]) {
      CURSOR(2,Font); display.print("Gps initializing");
    }
    if(screen!=old_screen)count=0;//eerste keer full update
    Speed_in_Unit(offset);
  }
  if(screen==WIFI_ON){  
    update_delay=1000;
    if(count%20<10) offset++;
    else offset--;    
    ESP_GPS_LOGO_48;
    SetFont_M();
    CURSOR(1,Font); display.print("ESP-GPS connect");
    if(Wifi_on==1){
      CURSOR(2,Font); display.print("Ssid: ");
      CURSOR(3,Font); if(SoftAP_connection==true) display.print("ESP32AP"); else display.print(config.ssid);
      CURSOR(4,Font); if(SoftAP_connection==true) display.print("Password: password");
      CURSOR(5,Font); display.printf("http://%s", IP_adress.c_str());
    } 
    else {
      display.fillRect(0, 0, 180, 104, GxEPD_WHITE);
      CURSOR(1,Font); display.print("ESP-GPS ready");
      if(ubxMessage.navPvt.numSV<5){
        CURSOR(2,Font); display.print("Waiting for Sat >=5");
        CURSOR(3,Font); display.print("Please go outside");
      }
      else{
        CURSOR(3,Font); display.print("Ready for action");
        CURSOR(4,Font); display.print("Move faster than ");
        if((int)(calibration_speed*100000)==194) {
          display.print(config.start_logging_speed*1.94384449);
          display.print("kn");
        }
        if((int)(calibration_speed*1000000)==3600) {
          display.print(config.start_logging_speed*3.6);
          display.print("km/h");
        }
        Speed_in_Unit(offset);
      }
    }
  }
  if(screen==WIFI_STATION){ 
    update_delay=100;   
    ESP_GPS_LOGO_48;
    SetFont_M();
    CURSOR(1,Font); display.print("ESP-GPS connect");
    CURSOR(2,Font); display.print("Create wifi AP");
    CURSOR(3,Font); display.printf("Use magnet in %ds", wifi_search);
    if(screen!=old_screen)count=0;//eerste keer full update 
  }
  if(screen==WIFI_SOFT_AP){  
    update_delay=500;   
    //display.fillScreen(GxEPD_WHITE);
    ESP_GPS_LOGO_48;
    SetFont_M();
    CURSOR(1,Font); display.print("Connect to ESP-GPS");
    CURSOR(2,Font); display.print("Ssid: ESP32AP");
    CURSOR(3,Font); display.print("Password: password");
    CURSOR(4,Font); display.printf("http://%s/ in %ds\n",IP_adress.c_str(), wifi_search);
    if(screen!=old_screen)count=0;//eerste keer full update 
  }
  if(screen==SPEED){
    update_delay=100;
    int run_rectangle_length;
    int field=config.field_actual;//default is in config.txt
    int bar_position=32;
    int bar_length=config.bar_length*1000/240;//default 100% length = 1852 m
    display.setFont(&FreeSansBold6pt7b);
    display.setCursor(displayWidth-10,INFO_BAR_TOP);
    display.print(config.field_actual);//show config field in small font
    if(config.field_actual==1){         //switch alfa, run-AVG, NM!!!
      field=2 ; //Default Run-AVG // NM
      if(((int)(Ublox.total_distance/1000000)%10==0)&(Ublox.alfa_distance/1000>1000))field=5;//indien x*10km, totale afstand laten zien                 
      if((Ublox.alfa_distance/1000<350)&(alfa_window<100))field=3;//first 350 m after gibe  alfa screen !!
      if(Ublox.alfa_distance/1000>config.bar_length)field=4;//run longer dan 1852 m, NM scherm !! 
    }
    if(config.field_actual==2){
      field=2;//default actual Run - AVG / NM
      if(Ublox.run_distance/1000>config.bar_length)field=4;//if run longer dan 1852 m, NM scherm !!
    }
    if(config.field_actual==7) {
      field=7;//default 0.5h
      if((Ublox.alfa_distance/1000<350)&(alfa_window<100))field=3;//first 350 m after gibe  alfa screen !!  
    } 
    if(config.field_actual==8){
      field=8;//default 1h
       if((Ublox.alfa_distance/1000<350)&(alfa_window<100))field=3;//first 350 m after gibe  alfa screen !!  
    }
    if(config.field_actual==9) {//1 hour default, but first alfa, and if good run, last run
      field=2;
      if(Ublox.alfa_distance/1000>config.bar_length)field=4;//run longer dan 1852 m, NM scherm !! 
      if(S10.s_max_speed>S10.display_speed[5])field=2;//if run faster then slowest run, show AVG & run after 1000 m
      if(Ublox.alfa_distance/1000<1000)field=8;// 350m - 1000m : 1h !!  
      if((Ublox.alfa_distance/1000<350)&(alfa_window<100))field=3;//first 350 m after gibe  alfa screen !!  
    } 

    if(GPS_Signal_OK==true){
      display.setFont(&SansSerif_bold_96_nr);
      if((config.speed_large_font==1)&(config.field<=4)){
        display.setFont(&SansSerif_bold_84_nr);
      }                                                             //test for bigger font alfa
      if(config.speed_large_font==2){                               //test for bigger font speed (Simon)
        display.setFont(&FreeSansBold75pt7b);
        display.setCursor(offset+5,115);
        display.print(gps_speed*calibration_speed,0);               //print main in large font
        display.setFont(&FreeSansBold30pt7b);
        display.print(".");
        display.println(int((gps_speed*calibration_speed-int(gps_speed*calibration_speed))*10),0); //int((x-int(x))*10) round to correct digit
      }
      else {
        display.setCursor(offset,120);
        display.println(gps_speed*calibration_speed,1);
      }
    }
    else{
      display.setFont(&FreeSansBold18pt7b);
      display.setCursor(offset,60);
      display.print("Low GPS signal !");
    }
    display.setFont(&FreeSansBold18pt7b);
    display.setCursor(offset,25);
    display.setTextColor(GxEPD_BLACK);  
    if(field<=2){
      if(config.speed_large_font==0){          
        SetFont_M(); 
        display.print("Run ");
        display.setFont(&FreeSansBold18pt7b);
        display.print(S10.s_max_speed*calibration_speed,1);//last 10s max from run
        SetFont_M();
        display.setCursor(offset+122,24);
        display.print("Avg ");
        display.setFont(&FreeSansBold18pt7b);
        display.print(S10.avg_5runs*calibration_speed,1); 
      }
      if(config.speed_large_font==1){ 
        bar_position=40;//test voor bigger font, was 42
        display.setFont(&SansSerif_bold_46_nr);   //Test for bigger alfa fonts
        display.setCursor(offset,36); 
        display.print(S10.s_max_speed*calibration_speed,1);//last 10s max from run
        SetFont_M();//A voor AVG !!!!
        display.setCursor(offset+113,36);
        display.print("A");
        display.setFont(&SansSerif_bold_46_nr);   //Test for bigger alfa fonts
        display.print(S10.avg_5runs*calibration_speed,1);
      }  
    }
    /*
    First 250m after jibe, if Window>99 m : Window and Exit
    Between 250m and 400m after jibe : Result Alfa (speed or MISS)
    Between 400m and 1852m after jibe : Actual Run + AVG
    More then 1852m : NM actual speed and NM Best speed
    */    
    if((field==3)&(config.speed_large_font==0)){
      bar_length=250*1000/240;//full bar length with Alfa = 250 meter
      if((alfa_window<99)&(Ublox.alfa_distance/1000<255)){  //Window alleen indien Window<100 en Run>350 meter !!!!&(A500.alfa_speed_max*calibration_speed<1)
        SetFont_M();
        display.print("Gate ");                                                                     // en nog geen geldige alfa
        display.setFont(&FreeSansBold18pt7b);   
        display.print(alfa_window,0);           
        display.print(" Ex ");
        display.setFont(&FreeSansBold18pt7b);
        if(alfa_exit>99)alfa_exit=99;//begrenzen alfa_exit...
          display.print(alfa_exit,0);//test functie				
        }
        else{
          SetFont_M();
          display.print("Alfa ");
          display.setFont(&FreeSansBold18pt7b);    
          display.print(A500.display_max_speed*calibration_speed,1);   //best Alfa from session on 500 m !! 
          if(A500.alfa_speed_max*calibration_speed>1){         //laatste alfa was geldig !!!!
            display.print("Al ");           //nieuwe alfa laatste gijp or MISSED !!!!
            display.setFont(&FreeSansBold18pt7b);
            display.print(A500.alfa_speed_max*calibration_speed,1);  //last alfa, back to zero 500 m after jibe !!
          }
          else{
            display.setFont(&FreeSansBold18pt7b);            //laatste alfa was ongeldig !!!!
            display.print("MISS");   
          }
        }
      }

      if((field==3)&(config.speed_large_font==1)){
        bar_length=250*1000/240;//full bar length with Alfa = 250 meter
        bar_position=40;//test voor bigger font 
        if((alfa_window<99)&(Ublox.alfa_distance/1000<255)){  //Window alleen indien Window<100 en Run>350 meter !!!!&(A500.alfa_speed_max*calibration_speed<1)
          SetFont_M();
		      display.setCursor(offset,36);
          display.print("Gate");                                                                     // en nog geen geldige alfa
          display.setFont(&SansSerif_bold_46_nr);   
          display.print(alfa_window,0);
          SetFont_M();           
			   	display.print(" Ex ");
          if(alfa_exit>99)alfa_exit=99;//begrenzen alfa_exit...
            display.setFont(&SansSerif_bold_46_nr); 
            display.print(alfa_exit,0);//test functie				
          }
          else{
            display.setFont(&FreeSansBold18pt7b);            
            display.setCursor(offset,38);
            //display.print(A500.display_max_speed*calibration_speed,1);   //best Alfa from session on 500 m !! 
            display.print("Alfa= ");
            display.setCursor(offset+110,38); 
            display.setFont(&SansSerif_bold_46_nr);        
            if(A500.alfa_speed_max*calibration_speed>1){         //laatste alfa was geldig !!!!
              display.print(A500.alfa_speed_max*calibration_speed,1);  //last alfa, back to zero 500 m after jibe !!
            }
            else{
              display.setFont(&FreeSansBold18pt7b);            //laatste alfa was ongeldig !!!!
              display.print("MISS");   
            }
          }
        } 
        if(field==4){
          if(config.speed_large_font==0){
            SetFont_M(); 
            display.print("NMa ");    //Actuele nautical mile
            display.setFont(&FreeSansBold18pt7b);
            display.print(M1852.m_max_speed*calibration_speed,1);//actueel topspeed 10s van deze run
            SetFont_M();
            display.setCursor(offset+129,24);
            display.print("NM ");
            display.setFont(&FreeSansBold18pt7b);
            display.print(M1852.display_max_speed*calibration_speed,1);   //Snelste nautical mile van de sessie
          }
          if(config.speed_large_font==1){
            bar_position=40;//test voor bigger font 
            display.setFont(&FreeSansBold18pt7b); 
            display.setCursor(offset,36);
            display.print("NM= ");    //Actuele nautical mile
            display.setFont(&SansSerif_bold_46_nr); 
            display.print(M1852.m_max_speed*calibration_speed,2);//actueel topspeed NM van deze run
          }
        }  
        if(field==5){
          if(config.speed_large_font==1){
            bar_position=40;//test voor bigger font 
            SetFont_M();
            display.setCursor(offset,36);
            display.print("Dist ");
            display.setFont(&SansSerif_bold_46_nr); 
            display.print(Ublox.total_distance/1000000,1);//Total distance in km, als test run_distance
            if(Ublox.total_distance/1000000>99.9){
              SetFont_M();
              display.print(" km");
            }
          } 
          else{
            SetFont_M();
            display.setCursor(offset,36);
            display.print("Dist ");
            display.setFont(&FreeSansBold18pt7b);
            display.print(Ublox.total_distance/1000000,1);//Total distance in km, als test run_distance
            SetFont_M();
            display.print(" km");
          }
        }
        if(field==6){
          SetFont_M();
          display.print("2S ");
          display.setFont(&FreeSansBold18pt7b);
          display.print(S2.display_max_speed*calibration_speed,1);  //best 2s
          SetFont_M();
          display.setCursor(offset+124,24);
          display.print("10S ");
          display.setFont(&FreeSansBold18pt7b);
          display.print(S10.display_max_speed*calibration_speed,1);  //best 10s run
        }      
        if(field==7){ 
          if(config.speed_large_font==1){ 
            bar_position=40;//test voor bigger font, was 42
            SetFont_M();// !!!!
            display.setCursor(offset,36);
            display.print("0.5h:");
            display.print(S1800.display_max_speed*calibration_speed,2);   //best average over 30 min
            display.setFont(&SansSerif_bold_46_nr);   //Test for bigger alfa fonts
            display.print(S1800.avg_s*calibration_speed,2);   //actual average last 30 min
          }  
          else{
            SetFont_M();
            display.print(".5hA");
            display.setFont(&FreeSansBold18pt7b);
            display.print(S1800.avg_s*calibration_speed,1);   //actual average last 30 min
            SetFont_M();
            display.setCursor(offset+124,24);
            display.print(".5hB");
            display.setFont(&FreeSansBold18pt7b);
            display.print(S1800.display_max_speed*calibration_speed,1);   //best average over 30 min
          }
        }       
        if(field==8){
          if(config.speed_large_font==1){ 
            bar_position=40;//test voor bigger font, was 42
            SetFont_M();// !!!!
            display.setCursor(offset,36);
            display.print("1h:");
            display.print(S3600.display_max_speed*calibration_speed,2);   //best average over 30 min
            display.setFont(&SansSerif_bold_46_nr);   //Test for bigger alfa fonts
            display.print(S3600.avg_s*calibration_speed,2);   //actual average last 30 min
          }
          else{  
          SetFont_M();
          display.print("1hA ");
          display.setFont(&FreeSansBold18pt7b);
          display.print(S3600.avg_s*calibration_speed,1);   //actual average last hour
          SetFont_M();
          display.setCursor(offset+124,24);
          display.print("1hB ");
          display.setFont(&FreeSansBold18pt7b);
          display.print(S3600.display_max_speed*calibration_speed,1);   //best 3600s
          }
        }
        int log_seconds=(millis()-start_logging_millis)/1000; //aantal seconden sinds loggen is gestart
        static int low_speed_seconds;
        if(S10.avg_s>2000){low_speed_seconds=0;}//if the speed is higher then 2000 mm/s, reset the counter
        low_speed_seconds++;
        if(low_speed_seconds>120){start_logging_millis=millis();}//bar will be reset if the 10s speed drops under 2m/s for more then 120 s !!!!
        run_rectangle_length=(Ublox.alfa_distance/bar_length);//240 pixels is volledige bar, ublox.alfa_distance zijn mm
        if(field==7){
          run_rectangle_length=log_seconds*240/1800;
          if (log_seconds>1800){start_logging_millis=millis();}
        }//30 minutes = full bar
        if(field==8){
          run_rectangle_length=log_seconds*240/3600;
          if (log_seconds>3600){start_logging_millis=millis();}
        }//60 minutes = full bar
        if(bar_length){
          display.fillRect(offset,bar_position,run_rectangle_length,8,GxEPD_BLACK);//balk voor run_distance weer te geven...          
        }
      }
      if(screen==AVG_10S){
        update_delay=1000;
        SetFont_M();
        for(int i=9;i>4;i--){
          display.setCursor(offset,24*(10-i));
          display.print("Run ");display.print(10-i);display.print(": ");
          display.print(S10.avg_speed[i]*calibration_speed,2);display.print(" @");
          display.print(S10.time_hour[i]);display.print(":");
          display.print(S10.time_min[i]);
        }   
      }      
      if(screen==SPEED2){
        update_delay=100;
        if(GPS_Signal_OK==true){
          display.setFont(&FreeSansBold75pt7b);
        // change color when 2s speed is in top5
          if (S2.s_max_speed*calibration_speed>S2.avg_speed[5]*calibration_speed){
              display.fillScreen(GxEPD_BLACK);        
              display.setTextColor(GxEPD_WHITE);
              } 
          else {
              display.fillScreen(GxEPD_WHITE);      
              display.setTextColor(GxEPD_BLACK);
              }
          if(gps_speed*calibration_speed<10){
            display.setCursor(40,115);
          }
          else{
            display.setCursor(0,115);
             }
          display.print(gps_speed*calibration_speed,0);
          if(gps_speed*calibration_speed<100){
            display.setFont(&FreeSansBold30pt7b);
            display.print(".");
            display.println(int((gps_speed*calibration_speed-int(gps_speed*calibration_speed))*10),0); //int((x-int(x))*10)
            SetFont_M();
            display.setCursor(160,14);
            display.print("Run ");
            display.print(S10.s_max_speed*calibration_speed,1);
            display.setCursor(160,28);
            display.print("2S ");
            display.print(S2.display_max_speed*calibration_speed,1);
          }
          Speed_in_Unit(offset);
          display.setTextColor(GxEPD_BLACK);
        }
        else{
          display.setFont(&FreeSansBold18pt7b);
          display.setCursor(offset,60);
          display.print("Low GPS signal !");
          Sats_level(offset);
        }
      }
      if(screen==TROUBLE){ 
         SetFont_M();
         CURSOR(1,Font); display.print("No GPS frames for");
         CURSOR(2,Font); display.print("more then 10 s.... ");
//         CURSOR(3,Font); display.print(time_now);
      }
      if(screen==STATS1){                        //2s,10sF,10sS, AVG
        update_delay=(config.Stat_screens_time-2)*1000;
        SetFont_M();
        CURSOR(1,Font); display.print("2s_prev: ");display.print(S2.display_last_run*calibration_speed,1);
        CURSOR(2,Font); display.print("2s: ");     display.print(S2.display_max_speed*calibration_speed);
        CURSOR(3,Font); display.print("#1 10s: "); display.print(S10.display_max_speed*calibration_speed);
        CURSOR(4,Font); display.print("#5 10s: "); display.print(S10.display_speed[5]*calibration_speed);
        CURSOR(5,Font); display.print("Avg: ");    display.print(S10.avg_5runs*calibration_speed);
//        display.setCursor(offset,ROW_1_18PT);
//        display.print("2l: ");
//        display.setFont(&FreeSansBold18pt7b);
//        display.print(S2.display_last_run*calibration_speed,1);//last 2s max from rundisplay.print(S2.avg_speed[9]*calibration_speed);   
//        SetFont_M();
//        display.setCursor(120+offset%2,ROW_1_18PT);//zodat SXX niet groter wordt dan 244 pix
//        display.print("2s: "); 
//        display.setFont(&FreeSansBold18pt7b);
//        display.print(S2.display_max_speed*calibration_speed);  //best 2s, was avg_speed[9]
        //;
//        display.setCursor(offset,ROW_2_18PT);
//        display.print("10sF: ");display.println(S10.display_max_speed*calibration_speed); //best 10s(Fast), was avg_speed[9]
//        display.setCursor(offset,ROW_3_18PT);
//        display.print("10sS: ");display.println(S10.display_speed[5]*calibration_speed);  //langzaamste 10s(Slow) run van de sessie
//        display.setCursor(offset,ROW_4_18PT);
//        display.print("Avg: ");display.println(S10.avg_5runs*calibration_speed); //average 5*10s
        //Bat_level(offset+222,87);
      }
      if(screen==STATS2){                        //alfa 500m,1852m, 1800s,total_dist
        update_delay=(config.Stat_screens_time-2)*1000;
        static int toggle=0;
        SetFont_M();
        CURSOR(1,Font); display.print("Dist: ");display.print(Ublox.total_distance/1000,0);
        CURSOR(2,Font); display.print("1852m: ");display.println(M1852.display_max_speed*calibration_speed);
        if(toggle==0){
          CURSOR(3,Font); display.print("1800s: ");display.println(S1800.display_max_speed*calibration_speed);
          CURSOR(4,Font); display.print("Alfa: ");display.print(A500.avg_speed[9]*calibration_speed);
          toggle=1;
        }
        else{
          CURSOR(3,Font); display.print("3600s: ");display.println(S3600.display_max_speed*calibration_speed);
          CURSOR(4,Font); display.print(time_now_sec);
          toggle=0;
        }
//        display.setFont(&FreeSansBold18pt7b);
//        display.setCursor(offset,ROW_1_18PT);
//        display.print("Dist: ");display.print(Ublox.total_distance/1000,0);
        //SetFont_L();
        //int posX = 196+offset%2;
        //int satnum = ubxMessage.navPvt.numSV;
        //display.setCursor(posX,22);//zodat SXX niet groter wordt dan 244 pix
        //display.drawExampleBitmap((satnum<1?ESP_noSat_22:ESP_Sat_22), posX, 3, 22, 22, GxEPD_BLACK);
        //display.print("S");
        //display.setCursor(posX+23,22);
        //display.println(ubxMessage.navPvt.numSV);
        //display.setFont(&FreeSansBold18pt7b);
//        display.setCursor(offset,ROW_2_18PT);
//        display.print("1852m: ");display.println(M1852.display_max_speed*calibration_speed);//was avg_speed[9]
//        display.setCursor(offset,ROW_3_18PT);
//        if(toggle==0){
//          display.print("1800s: ");display.println(S1800.display_max_speed*calibration_speed);
//          display.setCursor(offset,ROW_4_18PT);
//          display.print("Alfa: ");display.print(A500.avg_speed[9]*calibration_speed);   //best Alfa on 500 m 
//          toggle=1;
//        }
//        else{
//          display.print("3600s: ");display.println(S3600.display_max_speed*calibration_speed);
//          display.setCursor(offset,ROW_4_18PT);
//          display.print(time_now_sec);
//          toggle=0;
//        }
        //Bat_level(offset+222,87);
      }
      if(screen==STATS3){                        //100m,250m, 500m,Alfa
        update_delay=(config.Stat_screens_time-2)*1000;
        SetFont_M();
        CURSOR(1,Font);  display.print("100m: ");display.print(M100.avg_speed[9]*calibration_speed);
        CURSOR(2,Font);  display.print("250m: ");display.print(M250.display_max_speed*calibration_speed);
        CURSOR(3,Font);  display.print("500m: ");display.print(M500.avg_speed[9]*calibration_speed);
        CURSOR(4,Font);  display.print("Alfa: ");display.print(A500.avg_speed[9]*calibration_speed,2);
//        display.setFont(&FreeSansBold18pt7b);
//        display.setCursor(offset,ROW_1_18PT);
//        display.print("100m: ");display.print(M100.avg_speed[9]*calibration_speed);  //best 2s, was avg_speed[9]
//        display.setCursor(offset,ROW_2_18PT);
//        display.print("250m: ");display.println(M250.display_max_speed*calibration_speed); //best 10s(Fast), was avg_speed[9]
//        display.setCursor(offset,ROW_3_18PT);
//        display.print("500m: ");display.println(M500.avg_speed[9]*calibration_speed);  //langzaamste 10s(Slow) run van de sessie
//        display.setCursor(offset,ROW_4_18PT);
//        display.print("Alfa: ");display.println(A500.avg_speed[9]*calibration_speed,2); //average 5*10s
        //Bat_level(offset+222,87);
      }
      if(screen==STATS4){                        //10s,AVG,5 runs, update on the fly !!!
        update_delay=(config.Stat_screens_time-2)*1000;
        SetFont_M();
        CURSOR(1,Font);  display.print("10s Avg: ");display.print(s10.avg_5runs*calibration_speed,2);
        CURSOR(2,Font);  display.print("R1: ");display.print(s10.display_speed[9]*calibration_speed);
        CURSOR(3,Font);  display.print("R2: ");display.print(s10.display_speed[8]*calibration_speed);
        CURSOR(4,Font);  display.print("R3: ");display.print(s10.display_speed[7]*calibration_speed);
        CURSOR2(2,Font); display.print("R4: ");display.print(s10.display_speed[6]*calibration_speed);
        CURSOR2(3,Font); display.print("R5: ");display.print(s10.display_speed[5]*calibration_speed);
        CURSOR2(4,Font); display.print(time_now);
//        display.setCursor(offset,ROW_1_18PT);
//        display.print("10s Avg: "); 
//        display.setFont(&FreeSansBold18pt7b);
//        display.println(s10.avg_5runs*calibration_speed,2); //eerste regel, avg_5runs krijgt update tijdens run !!
//        for(int i=9;i>6;i--){
//          display.setCursor(offset,ROW_2_18PT+(9-i)*ROW_18PT);
//          SetFont_L();display.print("R");display.print(10-i);display.print(" ");
//          display.setFont(&FreeSansBold18pt7b);display.print(s10.display_speed[i]*calibration_speed,1);
//          display.setCursor(offset+118,ROW_2_18PT+(9-i)*ROW_18PT);
//          if(i>7){
//              SetFont_L();display.print(" R");display.print(13-i);display.print(" ");
//              display.setFont(&FreeSansBold18pt7b);display.print(s10.display_speed[i-3]*calibration_speed,1);
//          }
//          else{
//              display.print(time_now);
//          }
//        }
      }
      if(screen==STATS5){                        //alfa statistics
        update_delay=(config.Stat_screens_time-2)*1000;
        SetFont_M();
        CURSOR(1,Font);  display.print("Last Alfa stats ! ");
        CURSOR(2,Font);  display.print("A1: ");display.print(a500.avg_speed[9]*calibration_speed,1);
        CURSOR(3,Font);  display.print("A2: ");display.print(a500.avg_speed[8]*calibration_speed,1);
        CURSOR(4,Font);  display.print("A3: ");display.print(a500.avg_speed[7]*calibration_speed,1);
        CURSOR2(2,Font); display.print("A4: ");display.print(a500.avg_speed[6]*calibration_speed,1);
        CURSOR2(3,Font); display.print("A5: ");display.print(a500.avg_speed[5]*calibration_speed,1);
        CURSOR2(4,Font); display.print(time_now);
//        display.setCursor(offset,ROW_1_18PT);
//        display.print("Last Alfa stats ! "); 
//        display.setFont(&FreeSansBold18pt7b);
//        for(int i=9;i>6;i--){
//          display.setCursor(offset,ROW_2_18PT+(9-i)*ROW_18PT);
//          SetFont_L();display.print("A");display.print(10-i);display.print(" ");
//          display.setFont(&FreeSansBold18pt7b);display.print(a500.avg_speed[i]*calibration_speed,1);
//          display.setCursor(offset+118,ROW_2_18PT+(9-i)*ROW_18PT);
//          if(i>7){
//            SetFont_L();display.print(" A");display.print(13-i);display.print(" ");
//            display.setFont(&FreeSansBold18pt7b);display.print(a500.avg_speed[i-3]*calibration_speed,1);
//          }
//          /*else{
//            display.print(time_now);
//          }*/
//        }
      }
      if(screen==STATS6){ //Simon stat screen
        Serial.println("STATS6_Simon_screen");
        update_delay=(config.Stat_screens_time-2)*1500;
        double S10avgNEW;
        if (S10.s_max_speed>S10.avg_speed[5]) {
          S10avgNEW=(S10.avg_speed[9]+S10.avg_speed[8]+S10.avg_speed[7]+S10.avg_speed[6]+S10.s_max_speed)/5;
        }
        else {
          S10avgNEW=(S10.avg_speed[9]+S10.avg_speed[8]+S10.avg_speed[7]+S10.avg_speed[6]+S10.avg_speed[5])/5;
        }
        int row1=15;
        int row=17;
        int row2=row1+row;
        int row3=row2+row;
        int row4=row3+row;
        int row5=row4+row;
        int row6=row5+row;
        int col1=0+offset;
        int col2=46+offset;
        int col3=114+offset;
        int col4=182+offset;
        int line=0;
          
        //display.setCursor(displayWidth-10,INFO_BAR_TOP);
        //display.setFont(&FreeSansBold6pt7b);
        //display.print(time_now); 
        display.setFont(&FreeMonoBold12pt7b);
        display.setCursor(col1,row1);
        display.print("AV:");
        display.setCursor(col1,row2);
        display.print("R1:");
        display.setCursor(col1,row3);
        display.print("R2:");
        display.setCursor(col1,row4);
        display.print("R3:");
        display.setCursor(col1,row5);
        display.print("R4:");
        display.setCursor(col1,row6);
        display.print("R5:");
        
        SetFont_M();
        display.setCursor(col2,row1);
    
//        display.println(S10avgNEW*calibration_speed,2); //Laat het gemiddelde incl de previous 10s zien mits in top5
        display.println(S10.display_last_run*calibration_speed,2); //test omdat de regel hiervoor te snel weer reset
        display.setCursor(col2,row2);
        display.println(S10.avg_speed[9]*calibration_speed,2);
        display.setCursor(col2,row3);
        display.println(S10.avg_speed[8]*calibration_speed,2);
        display.setCursor(col2,row4);
        display.println(S10.avg_speed[7]*calibration_speed,2);
        display.setCursor(col2,row5);
        display.println(S10.avg_speed[6]*calibration_speed,2);
        display.setCursor(col2,row6);
        display.println(S10.avg_speed[5]*calibration_speed,2);
    // right column
        display.setFont(&FreeMonoBold12pt7b);
        display.setCursor(col3,row1);
        display.print("2sec:");
        display.setCursor(col3,row2);
        display.print("Prv :");
        display.setCursor(col3,row3);
        display.print("Dist:");
        display.setCursor(col3,row4);
        display.print("Alp :");
        display.setCursor(col3,row5);
        display.print("500m:");
        display.setCursor(col3,row6);
        display.print("NM:");
    
        SetFont_M();
        display.setCursor(col4,row1);
        display.println(S2.avg_speed[9]*calibration_speed,2);
        display.setCursor(col4,row2);
        display.println(S10.s_max_speed*calibration_speed,2);
        display.setCursor(col4,row3);
        display.println(Ublox.total_distance/1000000);
        display.setCursor(col4,row4);
        display.println(A500.avg_speed[9]*calibration_speed,2);
        display.setCursor(col4,row5);
        display.println(M500.display_max_speed*calibration_speed,2);
        display.setCursor(col4,row6);
        display.println(M1852.display_max_speed*calibration_speed,2);
    
    // position line of actual 10s run
        if (S10.s_max_speed*calibration_speed>S10.avg_speed[9]*calibration_speed) line=row1;
        else if (S10.s_max_speed*calibration_speed>S10.avg_speed[8]*calibration_speed) line=row2;
        else if (S10.s_max_speed*calibration_speed>S10.avg_speed[7]*calibration_speed) line=row3;
        else if (S10.s_max_speed*calibration_speed>S10.avg_speed[6]*calibration_speed) line=row4;
        else if (S10.s_max_speed*calibration_speed>S10.avg_speed[5]*calibration_speed) line=row5;
        else line=row6;
        display.fillRect(0,line+2,col3-10,2,GxEPD_BLACK);//lijn voor actuele run weer te geven...
      }
      if(screen==STATS7){ //Simon bar graph screen
        Serial.println("STATS7_Simon_bar graph");
        update_delay=(config.Stat_screens_time-2)*1500;
        //int DisplayWidth=255;
        //int DisplayHeight=122;
        int posX=5;
        int posY=INFO_BAR_TOP;
        int MaxNumberBar=NR_OF_BAR;//is 42, zie GPS_data.h
        //int barLengthMax=70;
        int GraphWidth=215;//was 220
        int barWidth=3;
        int barSpace=2;
        int barPitch=barWidth+barSpace;
        static int r=0;
        int top_speed = S10.display_speed[9]*calibration_speed;
        int max_bar= int(top_speed/5+1)*5;
        if(max_bar<24)max_bar=24;
        int step=3;
        if(max_bar>45)step=5;
        int min_bar=(max_bar-step*8);
        float scale=80/(max_bar-min_bar);
        
        SetFont_M();
        display.setCursor(0,15);
        display.println("Graph : Speed runs (10sec)");//printen top tekst
        r=run_count%MaxNumberBar+1;//laatste bar = 0 ?
        display.setFont(&FreeSansBold6pt7b);
        for(int i=0;i<9;i++){          
          display.fillRect(offset+posX,posY-(i*10),215, 1,GxEPD_BLACK);//printen hor.lijnen grafiek, van 5 tot 215
          display.setCursor(225+offset,posY-(i*10));//positie y-as legende
          display.print(min_bar+i*step);            //Printen y-as legende speed, van min_bar tot max_bar in 8 steps...
        }

        display.setCursor(0,30);
        display.print("R1-R5:");display.print(S10.display_speed[9]*calibration_speed);
        display.print(" ");display.print(S10.display_speed[8]*calibration_speed);
        display.print(" ");display.print(S10.display_speed[7]*calibration_speed);
        display.print(" ");display.print(S10.display_speed[6]*calibration_speed);
        display.print(" ");display.println(S10.display_speed[5]*calibration_speed);

        if (run_count<MaxNumberBar) {
          barWidth=max((GraphWidth-(r*barSpace))/(r),3);//(was r+1),3), laatste bar altijd 0; 
          barPitch=(barWidth)+barSpace; 
          for(int i=0;i<r;i++) {
            int barHeight=(S10.speed_run[i]*calibration_speed-min_bar)*scale; 
            display.fillRect(offset+posX+(i*barPitch),posY-barHeight,barWidth,barHeight,GxEPD_BLACK);
          }
        } else {
          barWidth=max((GraphWidth-(MaxNumberBar*barSpace))/MaxNumberBar,3);
          barPitch=(barWidth)+barSpace; 
          for(int i=0;i<MaxNumberBar;i++) {
            int barHeight=(S10.speed_run[(i+r)%NR_OF_BAR]*calibration_speed-min_bar)*scale;  //was S10.speed_run[i+r%NR_OF_BAR-42]  
            display.fillRect(offset+posX+(i*barPitch),posY-barHeight,barWidth,barHeight,GxEPD_BLACK);
          }    
        }
        #if defined(STATIC_DEBUG)
        Serial.print("Run: ");Serial.println(r);
        Serial.print("barWidth: ");Serial.println(barWidth);
        #endif
      }
      if(screen>BOOT_SCREEN&&screen<SPEED) {
        display.setFont(&FreeSansBold6pt7b);
        display.setCursor(displayWidth-10,INFO_BAR_TOP);
        display.print(screen);
      }
      if(count%200==0){//was 200
        if(update_epaper>0) display.update();
        Serial.print("\n");
        offset=0;
      }
      else {
//        if(update_epaper==2){display.fillScreen(GxEPD_WHITE);}//test
//        if(update_epaper>0) display.updateWindow(0,0,displayWidth,displayHeight,true);//was 244,122, true !!!
        display.updateWindow(0,0,displayWidth,displayHeight,true);
        Serial.print("up ");
        delay(update_delay);//update delay function of screen to show
      }
      old_screen=screen;
      count++;    
    }
/*Eenmaal flankdetectie indien GPIO langer dan push_time gedrukt
* Ook variabele die dan long_pulse_time hoog blijft
* Ook variabele die optelt tot maw elke keer push
*/
    Button_push::Button_push(int GPIO_pin,int push_time,int long_pulse_time,int max_count){
    pinMode(GPIO_pin,INPUT_PULLUP);
    Input_pin=GPIO_pin;
    time_out_millis=push_time;
    max_pulse_time=long_pulse_time;
    max_button_count=max_count;
  }
  boolean Button_push::Button_pushed(void){
  return_value=false;
  button_status=digitalRead(Input_pin);
  if(digitalRead(Input_pin)==1) push_millis=millis();
  if(((millis()-push_millis)>time_out_millis)&(old_button_status==0)){ 
    button_count++;
    if(button_count>max_button_count)button_count=0;
    old_button_status=1;
    millis_10s=millis();
    //Serial.print ("Class button_count ");Serial.print(button_count);
    return_value=true;
  }
  else return_value=false; 
  if((millis()-millis_10s)<(1000*max_pulse_time)) long_pulse=true;
  else long_pulse=false; 
  if(digitalRead(Input_pin)==1) old_button_status=0;
  return return_value;
}
#undef ROW_1M
#undef TOP_LEFT_INFO
#undef ROW_3M
#undef ROW_1M
