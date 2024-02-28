#include "E_paper.h"
#include "Definitions.h"

#define ROW_SPACING 3
// calculate y position from row-number, font-pixel and rown spacing
#define Y_POS 0

// bottom area 15px reserved to info bar
//#define InfoBarHeight 15
//#define InfoBarTop (displayHeight-InfoBarHeight)
//#define InfoBarRow (displayHeight-2)

int ROW_1S;
int Font;
int16_t NumPix;
int16_t TotPix;
int stats;
int16_t y_pos;
int16_t col1;
int16_t col2;
int16_t col3;
int16_t col4;

int16_t row1boot;
int16_t row2boot;
int16_t row3boot;
int16_t row4boot;
int16_t row5boot;

int InfoBarHeight=15;
int InfoBarTop;
int InfoBarRow;
int EspLogoPos;
int SpeedUnitXpos;
int SpeedUnitYpos;
int SpeedUnitTop;
int bar_position;
int FromRight;

extern bool test_screen;
int FieldLoop=0;
int FontLoop=0;

char time_now[8];
char time_now_sec[12];
int16_t displayHeight;
int16_t displayWidth;
int16_t TextWidth;
int16_t TextHeight;
int16_t GraphWidth;


void InfoBar(int offset);
void InfoBarRtc(int offset);
void Speed_in_Unit(int offset);
void sdCardInfo(int offset, int skip_if_not_ok);
const char * gpsChip(int longname);


void CURSOR(int row, int font){
  if(displayHeight<130) display.setCursor(offset               ,3+row*(2*font+ROW_SPACING));  // TotPix=2*Font NumPix=CharPix=4/3*Font (some char/sym below num)
  else                  display.setCursor(offset               ,3+row*(2*font+ROW_SPACING));  // TotPix=2*Font NumPix=CharPix=4/3*Font (some char/sym below num)
}
void CURSOR2(int row, int font){
  if(displayHeight<130) display.setCursor(offset+displayWidth/2,3+row*(2*font+ROW_SPACING));  // TotPix=2*Font NumPix=CharPix=4/3*Font (some char/sym below num)
  else                  display.setCursor(offset+displayWidth/2,3+row*(2*font+ROW_SPACING));  // TotPix=2*Font NumPix=CharPix=4/3*Font (some char/sym below num)
}
void CURSOR_COL(int row, int col, int font){
  if(displayHeight<130) display.setCursor(offset+col           ,3+row*(2*font+ROW_SPACING));  // TotPix=2*Font NumPix=CharPix=4/3*Font (some char/sym below num)
  else                  display.setCursor(offset+col           ,3+row*(2*font+ROW_SPACING));  // TotPix=2*Font NumPix=CharPix=4/3*Font (some char/sym below num)
}

void CURSORC(int row, int font){
  if(displayHeight<130) display.setCursor(offset               ,3+row*(4/3*font+2*ROW_SPACING));  // Compact rows
  else                  display.setCursor(offset               ,3+row*(4/3*font+2*ROW_SPACING));  // Compact rows
}
void CURSOR2C(int row, int font){
  if(displayHeight<130) display.setCursor(offset+displayWidth/2,3+row*(4/3*font+2*ROW_SPACING));  // Compact rows
  else                  display.setCursor(offset+displayWidth/2,3+row*(4/3*font+2*ROW_SPACING));  // Compact rows
}
void CURSOR_COLC(int row, int col, int font){
  if(displayHeight<130) display.setCursor(offset+col           ,3+row*(4/3*font+2*ROW_SPACING));  // Compact rows
  else                  display.setCursor(offset+col           ,3+row*(4/3*font+2*ROW_SPACING));  // Compact rows
}

void YPOS(int row, int font){   // line for previous run
  if(displayHeight<130) y_pos=3+row*(4/3*font+2*ROW_SPACING);
//  if(displayHeight<130) y_pos=font-5+row*(4/3*font+2*ROW_SPACING)-ROW_SPACING;
  else                  y_pos=3+row*(4/3*font+2*ROW_SPACING);
}

/*
void getTextWidthHeight(char str) {
  int16_t x1, x2; uint16_t w, h;
  display.getTextBounds(str, 0, 0, &x1, &x2, &w, &h);
  TextWidth=w;
  TextHeight=h;
}
*/

void SetScreen(){
  display.init();
  display.setRotation(1);
  displayHeight = display.height();
  displayWidth = display.width();
//rowTest();
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  if(displayHeight<130) {InfoBarHeight=15;}
  else                  {InfoBarHeight=20;}
  InfoBarTop=displayHeight-InfoBarHeight;
  InfoBarRow=displayHeight-ROW_SPACING;
}
void SetBarPosition(int font){
  if(displayHeight<130){bar_position=3+2*font+ROW_SPACING+8;}  // biggest font(TotPix) on the row plus pacing and barheight(8)
  else                 {bar_position=3+2*font+ROW_SPACING+8;}  // biggest font(TotPix) on the row plus pacing and barheight(8)
}
void SetCols(int stats){
  if(displayHeight<130) {
    if(stats==0){col1=0+offset;col2=24+offset;col3=85+offset;col4=135+offset;}  // sleep screen 213
    else        {col1=0+offset;col2=32+offset;col3=108+offset;col4=167+offset;} // stats screen 213
  } else {
    if(stats==0){col1=0+offset;col2=30+offset;col3=105+offset;col4=170+offset;} // sleep screen 266
    else        {col1=0+offset;col2=40+offset;col3=135+offset;col4=195+offset;} // stats screen 266
  }
  Serial.print("stats=");Serial.println(stats);
}

void SetESPGPSLogo(){
  if(displayHeight<130) {EspLogoPos=SpeedUnitTop-2*ROW_SPACING-48; display.drawExampleBitmap(ESP_GPS_logo, EspLogoPos, 5, 48, 48, GxEPD_WHITE, GxEPD::bm_flip_x);} 
  else                  {EspLogoPos=SpeedUnitTop-2*ROW_SPACING-48; display.drawExampleBitmap(ESP_GPS_logo, EspLogoPos, 0, 48, 48, GxEPD_WHITE, GxEPD::bm_flip_x);}
}
void SetBoardLogo(){
  if(displayHeight<130) FromRight=60;
  else                  FromRight=64;
  //logos are available in the file fonts/BitmapSurfbuddies.h
  if(RTC_Board_Logo==1)  display.drawExampleBitmap(Starboard_logoS_zwart , displayWidth-FromRight, 0, 48, 48, GxEPD_WHITE, GxEPD::bm_flip_x);
  if(RTC_Board_Logo==2)  display.drawExampleBitmap(Fanatic_logoS_zwart   , displayWidth-FromRight, 0, 48, 48, GxEPD_WHITE, GxEPD::bm_flip_x);
  if(RTC_Board_Logo==3)  display.drawExampleBitmap(JP_logoS_zwart        , displayWidth-FromRight, 0, 48, 48, GxEPD_WHITE, GxEPD::bm_flip_x);
  if(RTC_Board_Logo==4)  display.drawExampleBitmap(NoveNove_logoS_zwart  , displayWidth-FromRight, 0, 48, 48, GxEPD_WHITE, GxEPD::bm_flip_x);
  if(RTC_Board_Logo==5)  display.drawExampleBitmap(Mistral_logoS_zwart   , displayWidth-FromRight, 0, 48, 48, GxEPD_WHITE, GxEPD::bm_flip_x);
  if(RTC_Board_Logo==6)  display.drawExampleBitmap(Goya_logoS_zwart      , displayWidth-FromRight, 0, 48, 48, GxEPD_WHITE, GxEPD::bm_flip_x);
  if(RTC_Board_Logo==7)  display.drawExampleBitmap(Patrik_logoS_zwart    , displayWidth-FromRight, 0, 48, 48, GxEPD_WHITE, GxEPD::bm_flip_x);
  if(RTC_Board_Logo==8)  display.drawExampleBitmap(Severne_logoS_zwart   , displayWidth-FromRight, 0, 48, 48, GxEPD_WHITE, GxEPD::bm_flip_x);
  if(RTC_Board_Logo==9)  display.drawExampleBitmap(Tabou_logoS_zwart     , displayWidth-FromRight, 0, 48, 48, GxEPD_WHITE, GxEPD::bm_flip_x);
  if(RTC_Board_Logo==10) display.drawExampleBitmap(F2_logo_zwart         , displayWidth-FromRight, 0, 48, 48, GxEPD_WHITE, GxEPD::bm_flip_x);
  if(RTC_Board_Logo==11) display.drawExampleBitmap(epd_bitmap_Schwechater, displayWidth-FromRight, -10, 79, 132, GxEPD_WHITE, GxEPD::bm_flip_x);
  if(RTC_Board_Logo==12) display.drawExampleBitmap(Thommen1_logo_BW      , displayWidth-FromRight, 0, 64, 48, GxEPD_WHITE, GxEPD::bm_flip_x);
  if(RTC_Board_Logo==13) display.drawExampleBitmap(BIC_logo_BW           , displayWidth-FromRight, 0, 48, 48, GxEPD_WHITE, GxEPD::bm_flip_x);
  if(RTC_Board_Logo==14) display.drawExampleBitmap(Carbon_art            , displayWidth-FromRight, 0, 48, 48, GxEPD_WHITE, GxEPD::bm_flip_x);
  if(RTC_Board_Logo==15) display.drawExampleBitmap(FutureFly_logo_zwart  , displayWidth-FromRight, 0, 48, 48, GxEPD_WHITE, GxEPD::bm_flip_x);
  if(RTC_Board_Logo==16) display.drawExampleBitmap(OneHundred_logoS_zwart, displayWidth-FromRight, 0, 48, 48, GxEPD_WHITE, GxEPD::bm_flip_x);
}
void SetSailLogo(){
  if(displayHeight<130) FromRight=60;
  else                  FromRight=64;
  if(RTC_Sail_Logo==1)   display.drawExampleBitmap(GAsails_logoS_zwart   , displayWidth-FromRight, 50, 48, 48, GxEPD_WHITE, GxEPD::bm_flip_x);
  if(RTC_Sail_Logo==2)   display.drawExampleBitmap(DuoTone_logoS_zwart   , displayWidth-FromRight, 50, 48, 48, GxEPD_WHITE, GxEPD::bm_flip_x);
  if(RTC_Sail_Logo==3)   display.drawExampleBitmap(NP_logoS_zwart        , displayWidth-FromRight, 50, 48, 48, GxEPD_WHITE, GxEPD::bm_flip_x);
  if(RTC_Sail_Logo==4)   display.drawExampleBitmap(Pryde_logoS_zwart     , displayWidth-FromRight, 50, 48, 48, GxEPD_WHITE, GxEPD::bm_flip_x);
  if(RTC_Sail_Logo==5)   display.drawExampleBitmap(Loftsails_logoS_zwart , displayWidth-FromRight, 50, 48, 48, GxEPD_WHITE, GxEPD::bm_flip_x);
  if(RTC_Sail_Logo==6)   display.drawExampleBitmap(Gunsails_logoS_zwart  , displayWidth-FromRight, 50, 48, 48, GxEPD_WHITE, GxEPD::bm_flip_x);
  if(RTC_Sail_Logo==7)   display.drawExampleBitmap(Point7_logoS_zwart    , displayWidth-FromRight, 50, 48, 48, GxEPD_WHITE, GxEPD::bm_flip_x);
  if(RTC_Sail_Logo==8)   display.drawExampleBitmap(Simmer_logoS_zwart    , displayWidth-FromRight, 50, 48, 48, GxEPD_WHITE, GxEPD::bm_flip_x);
  if(RTC_Sail_Logo==9)   display.drawExampleBitmap(Naish_logoS_zwart     , displayWidth-FromRight, 50, 48, 48, GxEPD_WHITE, GxEPD::bm_flip_x);
  if(RTC_Sail_Logo==10)  display.drawExampleBitmap(Severne_logoS_zwart   , displayWidth-FromRight, 50, 48, 48, GxEPD_WHITE, GxEPD::bm_flip_x);
  if(RTC_Sail_Logo==11)  display.drawExampleBitmap(S2maui_logoS_zwart    , displayWidth-FromRight, 50, 48, 48, GxEPD_WHITE, GxEPD::bm_flip_x);
  if(RTC_Sail_Logo==12)  display.drawExampleBitmap(North_Sails_logoS_zwart, displayWidth-FromRight, 50, 48, 48, GxEPD_WHITE, GxEPD::bm_flip_x);
}
void SetFont_Sleep(){
  if(displayHeight<130){display.setFont(&SF_Distant_Galaxy9pt7b); Font=9;  NumPix=Font*2*3/4; TotPix=Font*2;}
  else                 {display.setFont(&SF_Distant_Galaxy12pt7b);Font=12; NumPix=Font*2*3/4; TotPix=Font*2;}
}
void SetFont_Mono_M(){
  if(displayHeight<130){display.setFont(&FreeMonoBold9pt7b); Font=9;       NumPix=Font*2*3/4; TotPix=Font*2;}
  else                 {display.setFont(&FreeMonoBold12pt7b);Font=12;      NumPix=Font*2*3/4; TotPix=Font*2;}
}
void SetFont_Mono_L(){
  if(displayHeight<130){display.setFont(&FreeMonoBold12pt7b);Font=12;      NumPix=Font*2*3/4; TotPix=Font*2;}
  else                 {display.setFont(&FreeMonoBold15pt7b);Font=15;      NumPix=Font*2*3/4; TotPix=Font*2;}
}
void SetFont_S(){
  if(displayHeight<130){display.setFont(&FreeSansBold6pt7b);Font=6;        NumPix=Font*2*3/4; TotPix=Font*2;}
  else                 {display.setFont(&FreeSansBold9pt7b);Font=9;        NumPix=Font*2*3/4; TotPix=Font*2;}
}
void SetFont_M(){
  if(displayHeight<130){display.setFont(&FreeSansBold9pt7b); Font=9;       NumPix=Font*2*3/4; TotPix=Font*2;}
  else                 {display.setFont(&FreeSansBold12pt7b);Font=12;      NumPix=Font*2*3/4; TotPix=Font*2;}
}
void SetFont_L(){
  if(displayHeight<130){display.setFont(&FreeSansBold12pt7b);Font=12;      NumPix=Font*2*3/4; TotPix=Font*2;}
  else                 {display.setFont(&FreeSansBold15pt7b);Font=15;      NumPix=Font*2*3/4; TotPix=Font*2;}
}
void SetFont_LS(){
  if(displayHeight<130){display.setFont(&FreeSansBold11pt7b);Font=12;      NumPix=Font*2*3/4; TotPix=Font*2;}
  else                 {display.setFont(&FreeSansBold14pt7b);Font=15;      NumPix=Font*2*3/4; TotPix=Font*2;}
}
void SetFont_XL(){
  if(displayHeight<130){display.setFont(&FreeSansBold15pt7b);Font=15;      NumPix=Font*2*3/4; TotPix=Font*2;}
  else                 {display.setFont(&FreeSansBold18pt7b);Font=18;      NumPix=Font*2*3/4; TotPix=Font*2;}
}
void SetFont_XXL(){
  if(displayHeight<130){display.setFont(&FreeSansBold18pt7b);Font=18;      NumPix=Font*2*3/4; TotPix=Font*2;}
  else                 {display.setFont(&FreeSansBold24pt7b);Font=24;      NumPix=Font*2*3/4; TotPix=Font*2;}
}
void SetFont_XXXL(){
  if(displayHeight<130){display.setFont(&FreeSansBold30pt7b);Font=30;      NumPix=Font*2*3/4; TotPix=Font*2;}
  else                 {display.setFont(&FreeSansBold40pt7b);Font=40;      NumPix=Font*2*3/4; TotPix=Font*2;}
}
void SetFont_Speed_I(){
  if(displayHeight<130){display.setFont(&impact50pt7b);Font=50;            NumPix=Font*2*3/4; TotPix=Font*2;} //moet 50 worden
  else                 {display.setFont(&impact60pt7b);Font=60;            NumPix=Font*2*3/4; TotPix=Font*2;}
}
void SetFont_Speed_IL(){
  if(displayHeight<130){display.setFont(&impact60pt7b);Font=60;            NumPix=Font*2*3/4; TotPix=Font*2;}
  else                 {display.setFont(&impact80pt7b);Font=80;            NumPix=Font*2*3/4; TotPix=Font*2;}
}

int device_boot_log(int rows, int ws) {
  SetFont_S();
  CURSOR(2,Font); display.print(SW_version);
  CURSOR(3,Font); sdCardInfo(offset,0);
  CURSOR(4,Font); display.printf("Display size %dx%d\n",displayWidth,displayHeight);
  CURSOR(5,Font); display.printf("Gps %s at %dHz", gpsChip(1), config.sample_rate);
}
#define DEVICE_BOOT_LOG(rows) device_boot_log(rows,0)

int rowTestUnit(int xPos) {
  int16_t x1, y1, box_padding, rows, rowheight;
  uint16_t w, h;
  display.getTextBounds("000", xPos, InfoBarTop, &x1, &y1, &w, &h);
  Serial.print("000"); Serial.print(" getTextBounds got ("); Serial.print(x1); Serial.print(", "); Serial.print(y1); Serial.print(", "); Serial.print(w); Serial.print(", "); Serial.print(h); Serial.println(")"); 
  rows = InfoBarTop/(h+ROW_SPACING);
  rowheight = InfoBarTop/rows;
  box_padding = ((rowheight - h)/2);
  Serial.printf("fontbounds: rows:%d rowheight:%d,  fontheight: %d, spacing:%d, box_padding:%d\n", rows, rowheight, h, ROW_SPACING, box_padding);
    for(int row=0,i=0,j=InfoBarTop; i<j; i+=rowheight,++row){
      
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
  //display.fillRect(0, InfoBarTop-1, displayWidth, 1, GxEPD_BLACK);
  //display.fillRect(0, 0, 1, InfoBarTop, GxEPD_BLACK);
  //6pt test
  display.setFont(&FreeSansBold6pt7b);
  xPos=rowTestUnit(xPos);
  //9pt test
  display.setFont(&FreeSansBold9pt7b);
  xPos=rowTestUnit(xPos);
  //12pt test
  display.setFont(&FreeSansBold12pt7b);
  xPos=rowTestUnit(xPos);
  //15pt test
  display.setFont(&FreeSansBold15pt7b);
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
  SetScreen();
  SetFont_M();
  SetESPGPSLogo();
  InfoBarRtc(offset);
  if(RTC_voltage_bat<MINIMUM_VOLTAGE){
    CURSOR(1,Font); display.printf("EPS-GPS sleeping");
    CURSOR(2,Font); display.print("Go back to sleep...");
    CURSOR(4,Font); display.printf("Voltage to low: %f", RTC_voltage_bat);
    CURSOR(5,Font); display.print("Please charge battery!");
    display.update();
  }   else {
    CURSOR(1,Font); display.printf("ESP-GPS booting");
    display.updateWindow(0,0,displayWidth,displayWidth,true); 
    delay(100);
  }
}
void Off_screen(int choice){//choice 0 = old screen, otherwise Simon screens
  SetScreen();
  //int offset=0;
  float session_time=(millis()-start_logging_millis)/1000 ;
  display.setRotation(1);
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  SetFont_M();
  if(choice==0){
    SetESPGPSLogo();
    CURSOR(1,Font); display.print("ESP-GPS saving");
    if(Shut_down_Save_session==true){
      CURSOR(2,Font); display.print("Saving session");
      CURSOR(3,Font); display.print("Time: "); display.print(session_time,0); display.print(" s");
      CURSOR(4,Font); display.print("AVG: ");display.print(RTC_avg_10s,2);
      CURSOR(5,Font); display.print("Dist: ");display.print(Ublox.total_distance/1000,0);
    }
    else{
    CURSOR(2, Font); display.print("Going back to sleep");
    }
  }
  else{
    Serial.println("Off_screen Simon");
      
    SetESPGPSLogo();
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
  display.updateWindow(0,0,displayWidth,displayHeight,true);
  display.update();
  delay(10000);//om te voorkomen dat update opnieuw start !!!
}
//Screen in deepsleep, update bat voltage, refresh every 4000s !!
void Sleep_screen(int choice){
  SetScreen();
  offset=0;
  display.init(); 
  display.setRotation(1);
  displayHeight = display.height();
  displayWidth = display.width();
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  if(choice==0){
    SetFont_XL();
    CURSOR(1,Font); display.print("Dist: "); display.print(RTC_distance,0);
    CURSOR(2,Font); display.print("AVG: ");display.print(RTC_avg_10s,2);
    CURSOR(3,Font); display.print("2s: ");display.print(RTC_max_2s);
  }
  else{
    SetFont_M();
    InfoBarRtc(offset);
    SetBoardLogo();
    SetSailLogo();
    SetCols(0);
    CURSOR_COLC(1,col1,Font); display.print("AV");
    CURSOR_COLC(2,col1,Font); display.print("R1");
    CURSOR_COLC(3,col1,Font); display.print("R2");
    CURSOR_COLC(4,col1,Font); display.print("R3"); 
    CURSOR_COLC(5,col1,Font); display.print("R4");
    CURSOR_COLC(6,col1,Font); display.print("R5"); 

    CURSOR_COLC(1,col2,Font); display.print(":");display.print(RTC_avg_10s,2);
    CURSOR_COLC(2,col2,Font); display.print(":");display.print(RTC_R1_10s,2);
    CURSOR_COLC(3,col2,Font); display.print(":");display.print(RTC_R2_10s,2);
    CURSOR_COLC(4,col2,Font); display.print(":");display.print(RTC_R3_10s,2); 
    CURSOR_COLC(5,col2,Font); display.print(":");display.print(RTC_R4_10s,2);
    CURSOR_COLC(6,col2,Font); display.print(":");display.print(RTC_R5_10s,2); 

    CURSOR_COLC(1,col3,Font); display.print("2sec");
    CURSOR_COLC(2,col3,Font); display.print("Dist");
    CURSOR_COLC(3,col3,Font); display.print("Alph");
    CURSOR_COLC(4,col3,Font); display.print("1h"); 
    CURSOR_COLC(5,col3,Font); display.print("NM");
    CURSOR_COLC(6,col3,Font); display.print("500m"); 

    CURSOR_COLC(1,col4,Font); display.print(":");display.print(RTC_max_2s,2);
    CURSOR_COLC(2,col4,Font); display.print(":");display.print(RTC_distance,2);
    CURSOR_COLC(3,col4,Font); display.print(":");display.print(RTC_alp,2);
    CURSOR_COLC(4,col4,Font); display.print(":");display.print(RTC_1h,2); 
    CURSOR_COLC(5,col4,Font); display.print(":");display.print(RTC_mile,2);
    CURSOR_COLC(6,col4,Font); display.print(":");display.print(RTC_500m,2); 
    Speed_in_Unit(offset);
    SetFont_Sleep();
    CURSOR_COLC(7,col1,Font); display.print(RTC_Sleep_txt);
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
  int batL;
  if(displayHeight<130) batL=15;
  else                  batL=20;
  int posX=displayWidth - batW-3;
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
//  Serial.printf("info bar cursor pos: %d, display height: %d\n", InfoBarRow,displayHeight);
  SetFont_M();
//  display.setCursor(displayWidth-8,(InfoBarRow-ROW_1M));
//  display.print("-");
  if(displayHeight<130) {
    if (bat_perc<100) display.setCursor(offset+displayWidth-35,InfoBarRow);
    else              display.setCursor(offset+displayWidth-60,InfoBarRow);
  } else {
    if (bat_perc<100) display.setCursor(offset+displayWidth-65,InfoBarRow);
    else              display.setCursor(offset+displayWidth-90,InfoBarRow);
  }
  if (RTC_bat_choice==1) { display.print(int(bat_perc)); display.print("%");}
  else             { display.print(RTC_voltage_bat,2); display.print("V");}
}
void Bat_level_print(int volt ){
  float bat_perc=100*(1-(VOLTAGE_100-RTC_voltage_bat)/(VOLTAGE_100-VOLTAGE_0));
  if (bat_perc<0) bat_perc=0;
  if (bat_perc>100) bat_perc=100;
  if (volt==1) {display.print(RTC_voltage_bat,2); display.print("V");}
  else         {display.print(int(bat_perc)); display.print("%");}
}
void Sats_level(int offset){
  if(!ubxMessage.monVER.swVersion[0]) return;
  SetFont_M();
//  int circelL=5;
//  int circelS=2;
  int posX=displayWidth*0.63+offset;
  int posY=InfoBarTop;//-(circelL+2*circelS); 
//  Serial.print("posX=");Serial.print(posX);
//  Serial.print("posY=");Serial.print(posY);
//  Serial.print("InfoBarRow=");Serial.print(InfoBarRow);
  int satnum = ubxMessage.navPvt.numSV;
  if(displayHeight<130) {
    display.drawExampleBitmap(ESP_Sat_15, posX, posY, 15, 15, GxEPD_BLACK);
  } else {
    display.drawExampleBitmap(ESP_Sat_22, posX, posY, 22, 22, GxEPD_BLACK);
  }
  display.setCursor(posX-(satnum<10?Font:2*Font),InfoBarRow);
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
  int posX=displayWidth*0.35+offset;
  display.setCursor(posX,InfoBarRow);
  display.print(gpsChip(0));
}
int Time(int offset){
  if(!update_time()){
    SetFont_M();
    display.setCursor(offset,InfoBarRow);
    display.print(time_now);
  }
  return 0;
}    
int TimeRtc(int offset){
  SetFont_M();
  display.setCursor(offset,InfoBarRow);
  display.printf("%d:%d",RTC_hour,RTC_min);
  return 0;
}     
int DateTimeRtc(int offset){
  SetFont_M();
  Serial.print("InfoBarRow=");Serial.println(InfoBarRow);
  display.setCursor(offset,InfoBarRow);
  display.printf("%02d:%02d %02d-%02d-%02d",RTC_hour,RTC_min,RTC_day,RTC_month,RTC_year);
  return 0;
}
void InfoBar(int offset) {
  Bat_level_Simon(offset);
  Sats_level(offset);
  if(ubxMessage.navPvt.numSV>4) M8_M10(offset);
  Time(offset);
}
void InfoBarRtc(int offset) {
  Bat_level_Simon(offset);
  DateTimeRtc(offset);
}
void Speed_in_Unit(int offset){
  display.setRotation(0);
  SetFont_S();
  SpeedUnitXpos=offset+displayWidth-5;
  SpeedUnitYpos=InfoBarHeight+3*ROW_SPACING;
  SpeedUnitTop=SpeedUnitXpos-Font;
  display.setCursor(SpeedUnitYpos,SpeedUnitXpos);//reversed as rotated
  if((int)(calibration_speed*100000)==194)   display.print("speed in knots");//1.94384449 m/s to knots !!!
  if((int)(calibration_speed*1000000)==3600) display.print("speed in km/h");
  display.setRotation(1);
  SetFont_M();
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
  SetScreen();
  static int count,old_screen,update_delay;
  update_time();
  update_epaper=1; //was zonder else
  if(count%20<5) offset++;
  else offset--; 
  if(offset>5)offset=0;
  if(offset<0)offset=0;
  display.fillScreen(GxEPD_WHITE);
  SetFont_M();
  if(screen!=SPEED) InfoBar(offset);
  if(screen!=SPEED) Speed_in_Unit(offset);
  if(screen==BOOT_SCREEN){
    update_delay=1000;
    SetESPGPSLogo();
    SetFont_M();
    CURSOR(1,Font); display.print("ESP-GPS config");
    if(screen!=old_screen)count=0;//eerste keer full update
    delay(1000);
  }
  if(screen==GPS_INIT_SCREEN){
    update_delay=100;
    SetFont_M();
    SetESPGPSLogo();
    CURSOR(1,Font); display.print("ESP-GPS GPS init");
    if(!ubxMessage.monVER.hwVersion[0]) {
      SetFont_L();
      CURSOR(3,Font); display.print("Gps initializing");
    }
    if(screen!=old_screen)count=0;//eerste keer full update
  }
  if(screen==WIFI_ON){
    update_delay=1000;
    SetESPGPSLogo();
    CURSOR(1,Font);                               display.print("ESP-GPS connect");
    if(Wifi_on==1){
      SetFont_L();
      CURSOR(2,Font);                             display.print("Ssid: ");
      if(SoftAP_connection==true)                 display.print("ESP32AP"); else display.print(config.ssid);
      SetFont_M();
      CURSOR(3,Font); if(SoftAP_connection==true) display.print("Password: password");
      CURSOR(4,Font);                             display.printf("http://%s", IP_adress.c_str());
    } 
    else {
      display.fillRect(0, 0, EspLogoPos, InfoBarTop, GxEPD_WHITE);
      SetFont_M();
      CURSOR(1,Font);                             display.print("ESP-GPS ready");
      SetFont_L();
      if(ubxMessage.navPvt.numSV<5){
        CURSOR(2,Font);                           display.print("Waiting for Sat >=5");
        CURSOR(3,Font);                           display.print("Please go outside");
      }
      else{
        CURSOR(2,Font);                           display.print("Ready for action");
        CURSOR(3,Font); SetFont_M();              display.print("Move faster than ");
        if((int)(calibration_speed*100000)==194) {
                                                  display.print(config.start_logging_speed*1.94384449,0);
                                                  display.print("kn");
        }
        if((int)(calibration_speed*1000000)==3600) {
                                                  display.print(config.start_logging_speed*3.6,0);
                                                  display.print("km/h");
        }
      }
    }
    if(screen!=old_screen)count=0;//eerste keer full update
  }
  if(screen==WIFI_STATION){ 
    update_delay=100;   
    SetESPGPSLogo();
    SetFont_M();
    CURSOR(1,Font); display.print("ESP-GPS connect");
    CURSOR(2,Font); display.print("Create wifi AP");
    CURSOR(4,Font); 
    SetFont_L();    display.printf("Use magnet in %ds", wifi_search);
    if(screen!=old_screen)count=0;//eerste keer full update 
  }
  if(screen==WIFI_SOFT_AP){  
    update_delay=500;   
    //display.fillScreen(GxEPD_WHITE);
    SetESPGPSLogo();
    SetFont_M();
    CURSOR(1,Font); display.print("Connect to ESP-GPS");
    SetFont_L();
    CURSOR(2,Font); display.print("Ssid: ESP32AP");
    SetFont_M();
    CURSOR(3,Font); display.print("Password: password");
    CURSOR(4,Font); display.printf("http://%s/ in %ds\n",IP_adress.c_str(), wifi_search);
    if(screen!=old_screen)count=0;//eerste keer full update 
  }
  if(screen==SPEED){
    update_delay=100;
    int run_rectangle_length;
    int field=config.field_actual;//default is in config.txt
    SetFont_M(); 
    int bar_length=config.bar_length*1000/displayWidth;                              //default 100% length = 1852 m
    CURSOR_COL(1,offset+displayWidth-2*Font,Font); display.print(field);             //show config field in small font

///////////////for screentest purposes ///////////////////////////
    if(test_screen==true){
      config.field_actual=FieldLoop;
      config.speed_large_font=FontLoop;
      if(FieldLoop<=7) FieldLoop++;
      else { FieldLoop=1; FontLoop++;}
      if(FontLoop>=2) FontLoop=0;
      config.field_actual=FieldLoop;
    }
//      Serial.print("FontLoop=");Serial.println(FontLoop);
//      Serial.print("FieldLoop=");Serial.println(FieldLoop);
//////////////////////////////////////////////////////////////////
///////////////for screentest purposes ///////////////////////////
//    field=1;   //actual Run - AVG
//    field=2;   //default actual Run - AVG / NM
//    field=3;   //first 350 m after gibe  alfa screen
//    field=4;   //run longer dan 1852 m, NM scherm
//    field=5;   //indien x*10km, totale afstand laten zien
//    field=7;   //default 0.5h
//    field=8;   //default 1h
//    config.speed_large_font=1;  //smaller speed, but larger info
//////////////////////////////////////////////////////////////////
/*    
    int calibration_speed=1;
    float S2.display_max_speed=56.5;  // S2.avg_speed[9]=56.5;
    float S10.display_max_speed=55.0; //S10.avg_speed[9]=55.0;
    float S10.s_max_speed=56.5;
    float S10.avg_5runs=55.0;
    int alfa_window=45;
    int alfa_exit=65;
    float A500.alfa_speed_max=42.1;
    float A500.display_max_speed=40.5;
    float A500.alfa_speed_max=24.0;
    float M1852.m_max_speed=48.1;
    float M1852.display_max_speed=45.3;
    float Ublox.total_distance=8563;
    float S1800.avg_s=40.5;
    float S1800.display_max_speed=42.1;
    float S3600.avg_s=36.1;
    float S3600.display_max_speed=37.4;
    int run_rectangle_length=displayWidth*0.62;
*/
  if(FieldLoop<1){
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
  }

    if(GPS_Signal_OK==true){
//    if(GPS_Signal_OK==false){    //////////// for test purposes - set bool test_screen = false in ESP_functions ////////////////

      if(config.speed_large_font==0) SetFont_Speed_IL();                    // larger font for speed and smaller font for runs
      else  SetFont_Speed_I();                                              // smaller font for speed and bigger font for runs

      if(gps_speed*calibration_speed<10) TextWidth=1*Font;                  // 1 digit speed
      else                               TextWidth=2*Font;                  // 2 digit speed
      display.setCursor(offset+displayWidth/2-TextWidth/2,displayHeight-2); // center speed
      display.print(gps_speed * calibration_speed, 1);
    } else {
      SetFont_XL();
      CURSOR_COL(3,offset,Font); display.print("Low GPS signal !");
      Sats_level(offset);
    }

    if(field<=2){
      if(config.speed_large_font==0){
        SetFont_L();  CURSOR(1,Font);  SetFont_M();         display.print("Run ");
        SetFont_XL();                                       display.print(S10.s_max_speed*calibration_speed,2);          //last 10s max from run
        SetFont_L();  CURSOR2(1,Font); SetFont_M();         display.print("Avg ");
        SetFont_XL();                                       display.print(S10.avg_5runs*calibration_speed,2);            //average 10s from 5 runs
        SetFont_L();
      } else {
        SetFont_M();    CURSOR(1,Font);                     display.print("Run ");
                        CURSOR(2,Font);                     display.print("Avg ");
        SetFont_XXXL();                                     display.print(S10.s_max_speed*calibration_speed,0);          //last 10s max from run
        SetFont_XL();              display.print("."); display.print(int((S10.s_max_speed*calibration_speed-int(S10.s_max_speed*calibration_speed))*100),0); //int((x-int(x))*100)
        SetFont_M();    CURSOR2(2,Font); SetFont_XXXL();    display.print(S10.avg_5runs*calibration_speed,0);            //average 10s from 5 runs
        SetFont_XL();              display.print("."); display.print(int((S10.avg_5runs*calibration_speed  -int(S10.avg_5runs*calibration_speed))*100),0); //int((x-int(x))*100)
        SetFont_XXL();
      }
      SetBarPosition(Font);
    }

    /*
    First 250m after jibe, if Window>99 m : Window and Exit
    Between 250m and 400m after jibe : Result Alfa (speed or MISS)
    Between 400m and 1852m after jibe : Actual Run + AVG
    More then 1852m : NM actual speed and NM Best speed
    */    

    if(field==3){
      if(config.speed_large_font==0){
        if((alfa_window<99)&(Ublox.alfa_distance/1000<255)){                                //Window alleen indien Window<100 en Run>350 meter !!!!&(A500.alfa_speed_max*calibration_speed<1)
          SetFont_L(); CURSOR(1,Font);  SetFont_M();         display.print("Gate ");
          SetFont_XL();                                      display.print(alfa_window,0); //last 10s max from run
          SetFont_L(); CURSOR2(1,Font); SetFont_M();         display.print("Ex ");
          if(alfa_exit>99)alfa_exit=99;                                                     //begrenzen alfa_exit...
          SetFont_XL();                                    display.print(alfa_exit,0);     //average 10s from 5 runs
          SetFont_L();
        } else {
          SetFont_L(); CURSOR(1,Font);  SetFont_M();         display.print("Alfa ");
          SetFont_XL();                                      display.print(A500.display_max_speed*calibration_speed,1);  //best Alfa from session on 500 m !! 
          if(A500.alfa_speed_max*calibration_speed>1){                                      //laatste alfa was geldig !!!!
            SetFont_L(); CURSOR2(1,Font); SetFont_M();       display.print("A1 ");
            SetFont_XL();                                    display.print(A500.alfa_speed_max*calibration_speed,1); //last alfa, back to zero 500 m after jibe !!
            SetFont_L();
          } else {
            SetFont_L();CURSOR2(1,Font);SetFont_XL();        display.print("MISS");
          }
          SetFont_XL();
        }
      } else {
        if((alfa_window<99)&(Ublox.alfa_distance/1000<255)){                                //Window alleen indien Window<100 en Run>350 meter !!!!&(A500.alfa_speed_max*calibration_speed<1)
          SetFont_M();      CURSOR(1,Font);                   display.print("Gate ");
                            CURSOR(2,Font);                   display.print("Ex     ");
          SetFont_XXXL();                                     display.print(alfa_window,0); //last 10s max from run
            if(alfa_exit>99)alfa_exit=99;                                                   //begrenzen alfa_exit...
          SetFont_M();      CURSOR2(2,Font); SetFont_XXXL();  display.print(alfa_exit,0);   //average 10s from 5 runs
        } else {
          SetFont_M();      CURSOR(1,Font);                   display.print("Alfa ");
          if(A500.alfa_speed_max*calibration_speed>1){                                      //laatste alfa was geldig !!!!
                            CURSOR(2,Font);                   display.print("A1 ");
            SetFont_XXXL();                                   display.print(A500.display_max_speed*calibration_speed,1);  //best Alfa from session on 500 m !! 
          } else {
            SetFont_M();    CURSOR2(2,Font);SetFont_XXXL();   display.print("MISS");
          }
        }
        SetFont_XXL();
      }
      SetBarPosition(Font);
    }
    if(field==4){
      if(config.speed_large_font==0){
        SetFont_L();  CURSOR(1,Font);  SetFont_M();         display.print("NMa ");
        SetFont_XL();                                       display.print(M1852.m_max_speed*calibration_speed,1);        //actueel topspeed 10s van deze run
        SetFont_L();  CURSOR2(1,Font); SetFont_M();         display.print("NM ");
        SetFont_XL();                                       display.print(M1852.display_max_speed*calibration_speed,1);  //Snelste nautical mile van de sessie
        SetFont_L();
      } else {
        SetFont_M();    CURSOR(1,Font);                     display.print("NMa ");
                        CURSOR(2,Font);                     display.print("NM   ");
        SetFont_XXXL();                                     display.print(M1852.m_max_speed*calibration_speed,0);        //actueel topspeed 10s van deze run
        SetFont_XL();              display.print("."); display.print(int((M1852.m_max_speed*calibration_speed      -int(M1852.m_max_speed*calibration_speed))*100),0); //int((x-int(x))*100)
        SetFont_M();    CURSOR2(2,Font); SetFont_XXXL();    display.print(M1852.display_max_speed*calibration_speed,0);  //Snelste nautical mile van de sessie
        SetFont_XL();              display.print("."); display.print(int((M1852.display_max_speed*calibration_speed-int(M1852.display_max_speed*calibration_speed))*100),0); //int((x-int(x))*100)
        SetFont_XXL();
      }
      SetBarPosition(Font);
    }  
    if(field==5){
      if(config.speed_large_font==0){
        SetFont_L();  CURSOR(1,Font);  SetFont_M();         display.print("Dist ");
        SetFont_XL();                                       display.print(Ublox.total_distance/1000000,0);//Total distance in km, als test run_distance
        if(Ublox.total_distance/1000000>99.9)               display.print(" km");
        else                                                display.print(" m");
        SetFont_L();  CURSOR2(1,Font); SetFont_M();         display.print("Bat ");
        SetFont_XL();                                       Bat_level_print(0);
        SetFont_L();
      } else {
        SetFont_M();    CURSOR(1,Font);                     display.print("Dist ");
                        CURSOR(2,Font);                     display.print("Bat ");
        SetFont_XXXL();                                     display.print(Ublox.total_distance/1000000,0);//Total distance in km, als test run_distance
        if(Ublox.total_distance/1000000>99.9){SetFont_M();  display.print(" km");} 
        else                                 {SetFont_M();  display.print(" m");}
        SetFont_M();    CURSOR2(2,Font); SetFont_XXXL();    Bat_level_print(0);
        SetFont_XXL();
      }
      SetBarPosition(Font);
    }
    if(field==6){
      if(config.speed_large_font==0){
        SetFont_L();  CURSOR(1,Font);  SetFont_M();         display.print("2S ");
        SetFont_XL();                                       display.print(S2.display_max_speed*calibration_speed,1);     //best 2s
        SetFont_L();  CURSOR2(1,Font); SetFont_M();         display.print("10S ");
        SetFont_XL();                                       display.print(S10.display_max_speed*calibration_speed,1);    //best 10s run
        SetFont_L();
      } else {
        SetFont_M();    CURSOR(1,Font);                     display.print("2S ");
                        CURSOR(2,Font);                     display.print("10S ");
        SetFont_XXXL();                                     display.print(S2.display_max_speed*calibration_speed,0);     //last 10s max from run
        SetFont_XL();              display.print("."); display.print(int((S2.display_max_speed*calibration_speed-int(S2.display_max_speed*calibration_speed))*100),0); //int((x-int(x))*100)
        SetFont_M();    CURSOR2(2,Font); SetFont_XXXL();    display.print(S10.display_max_speed*calibration_speed,0);    //average 10s from 5 runs
        SetFont_XL();              display.print("."); display.print(int((S10.display_max_speed*calibration_speed-int(S10.display_max_speed*calibration_speed))*100),0); //int((x-int(x))*100)
        SetFont_XXL();
      }
      SetBarPosition(Font);
    }
    if(field==7){
      if(config.speed_large_font==0){
        SetFont_L();  CURSOR(1,Font);  SetFont_M();         display.print("0.5hA");
        SetFont_XL();                                       display.print(S1800.avg_s*calibration_speed,1);              //actual average last 30 min
        SetFont_L();  CURSOR2(1,Font); SetFont_M();         display.print("0.5hB");
        SetFont_XL();                                       display.print(S1800.display_max_speed*calibration_speed,1);  //best average over 30 min
        SetFont_L();
      } else {
        SetFont_M();    CURSOR(1,Font);                     display.print("0.5hA");
                        CURSOR(2,Font);                     display.print("0.5hB");
        SetFont_XXXL();                                     display.print(S1800.avg_s*calibration_speed,0);            //actual average last 30 min
        SetFont_XL();              display.print("."); display.print(int((S1800.avg_s*calibration_speed-int(S1800.avg_s*calibration_speed))*100),0); //int((x-int(x))*100)
        SetFont_M();    CURSOR2(2,Font); SetFont_XXXL();    display.print(S1800.display_max_speed*calibration_speed,0);  //best average over 30 min
        SetFont_XL();              display.print("."); display.print(int((S1800.display_max_speed*calibration_speed-int(S1800.display_max_speed*calibration_speed))*100),0); //int((x-int(x))*100)
        SetFont_XXL();
      }
      SetBarPosition(Font);
    }
    if(field==8){
      if(config.speed_large_font==0){
        SetFont_L();  CURSOR(1,Font);  SetFont_M();         display.print("1hA ");
        SetFont_XL();                                       display.print(S3600.avg_s*calibration_speed,1);              //actual average last 60 min
        SetFont_L();  CURSOR2(1,Font); SetFont_M();         display.print("1hB ");
        SetFont_XL();                                       display.print(S3600.display_max_speed*calibration_speed,1);  //best average over 60 min
        SetFont_L();
      } else {
        SetFont_M();    CURSOR(1,Font);                     display.print("1hA ");
                        CURSOR(2,Font);                     display.print("1hB ");
        SetFont_XXXL();                                     display.print(S3600.avg_s*calibration_speed,0);            //actual average last 60 min
          SetFont_XL();            display.print("."); display.print(int((S3600.avg_s*calibration_speed-int(S3600.avg_s*calibration_speed))*100),0); //int((x-int(x))*100)
        SetFont_M();    CURSOR2(2,Font); SetFont_XXXL();    display.print(S3600.display_max_speed*calibration_speed,0);  //best average over 60 min
        SetFont_XL();              display.print("."); display.print(int((S3600.display_max_speed*calibration_speed-int(S3600.display_max_speed*calibration_speed))*100),0); //int((x-int(x))*100)
        SetFont_XXL();
      }
      SetBarPosition(Font);
    }
    int log_seconds=(millis()-start_logging_millis)/1000;                                                 //aantal seconden sinds loggen is gestart
    static int low_speed_seconds;
    if(S10.avg_s>2000){low_speed_seconds=0;}                                                              //if the speed is higher then 2000 mm/s, reset the counter
    low_speed_seconds++;
    if(low_speed_seconds>120){start_logging_millis=millis();}                                             //bar will be reset if the 10s speed drops under 2m/s for more then 120 s !!!!
    run_rectangle_length=(Ublox.alfa_distance/bar_length);                                                //240 pixels is volledige bar, ublox.alfa_distance zijn mm
    if(field==7){
      run_rectangle_length=log_seconds*240/1800;
      if (log_seconds>1800){start_logging_millis=millis();}                                               //30 minutes = full bar
    }
    if(field==8){
      run_rectangle_length=log_seconds*240/3600;
      if (log_seconds>3600){start_logging_millis=millis();}
    }                                                                                                     //60 minutes = full bar
    if(bar_length){
       display.fillRect(offset,bar_position,run_rectangle_length,8,GxEPD_BLACK);                          //balk voor run_distance weer te geven...          
//       display.fillRect(offset,bar_position,displayWidth*0.62,8,GxEPD_BLACK);                          //balk test op 62% lengte 
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
//    if(GPS_Signal_OK==false){       /////////////////////////// for test purposes ////////////////////////
    if(GPS_Signal_OK==true){
      SetFont_Speed_I();
      display.setTextSize(2); 
      if ((S2.s_max_speed*calibration_speed>S2.avg_speed[5]*calibration_speed)&(S2.s_max_speed*calibration_speed>config.start_logging_speed*1000)){// change color when 2s speed is in top5
        display.fillScreen(GxEPD_BLACK);        
        display.setTextColor(GxEPD_WHITE);
      } else {
        display.fillScreen(GxEPD_WHITE);      
        display.setTextColor(GxEPD_BLACK);
      }
      if(gps_speed*calibration_speed<10){
        display.setCursor(Font,displayHeight-5);
      } else{
        display.setCursor(10,displayHeight-5);
      }
      display.print(gps_speed*calibration_speed,0);               //print main in large font
      if(gps_speed*calibration_speed<100){
        display.setTextSize(1); 
        display.print(".");
        display.println(int((gps_speed*calibration_speed-int(gps_speed*calibration_speed))*10),0); //int((x-int(x))*10)
        display.setTextSize(1); 
        SetFont_L();
        CURSOR_COL(1,displayWidth*0.62,Font); display.print("2S:"); display.print(S2.display_max_speed*calibration_speed,1);
        CURSOR_COL(2,displayWidth*0.62,Font); display.print("R1:"); display.print(S10.s_max_speed*calibration_speed,1);
      }
      display.setTextColor(GxEPD_BLACK);
    } else {
      SetFont_XL();
      CURSOR_COL(2,offset,Font); display.print("Low GPS signal !");
      Sats_level(offset);
    }
  }
  if(screen==TROUBLE){ 
    SetFont_L();
    CURSOR(2,Font); display.print("No GPS frames for");
    CURSOR(3,Font); display.print("more then 10s.... ");
    CURSOR(4,Font); display.print(time_now);
  }
  if(screen==STATS1){                        //2s,10sF,10sS, AVG
    update_delay=(config.Stat_screens_time-2)*1000;
    SetFont_XL();
    CURSORC(1,Font); display.print("2s prev");  CURSOR2C(1,Font); display.print(":");display.print(S2.display_last_run*calibration_speed,2);
    CURSORC(2,Font); display.print("2s max");   CURSOR2C(2,Font); display.print(":");display.print(S2.display_max_speed*calibration_speed,2);
    CURSORC(3,Font); display.print("10s max");  CURSOR2C(3,Font); display.print(":");display.print(S10.display_max_speed*calibration_speed,2);
    CURSORC(4,Font); display.print("10s R5 ");  CURSOR2C(4,Font); display.print(":");display.print(S10.display_speed[5]*calibration_speed,2);
    CURSORC(5,Font); display.print("10s Avg");  CURSOR2C(5,Font); display.print(":");display.print(S10.avg_5runs*calibration_speed,2);
  }
  if(screen==STATS2){                        //total_dist,1852m,1800s,alfa 500m
    update_delay=(config.Stat_screens_time-2)*1000;
    static int toggle=0;
    SetFont_XL();
    CURSORC(2,Font);   display.print("Dist");  CURSOR2C(2,Font); display.print(":");display.print(Ublox.total_distance/1000,0);
    CURSORC(3,Font);   display.print("NM  ");  CURSOR2C(3,Font); display.print(":");display.print(M1852.display_max_speed*calibration_speed,2);
    if(toggle==0){
      CURSORC(4,Font); display.print("0.5hA"); CURSOR2C(4,Font); display.print(":");display.print(S1800.display_max_speed*calibration_speed,2);
      CURSORC(5,Font); display.print("Alfa "); CURSOR2C(5,Font); display.print(":");display.print(A500.avg_speed[9]*calibration_speed,2);
      toggle=1;
    }
    else{
      CURSORC(4,Font); display.print("3600s"); CURSOR2C(4,Font); display.print(":");display.println(S3600.display_max_speed*calibration_speed,2);
      CURSORC(5,Font); display.print(time_now_sec);
      toggle=0;
    }
    //Bat_level(offset+222,87);
  }
  if(screen==STATS3){                        //100m,250m, 500m,Alfa
    update_delay=(config.Stat_screens_time-2)*1000;
    SetFont_XL();
    CURSORC(2,Font);  display.print("100m"); CURSOR2C(2,Font); display.print(":");display.print(M100.avg_speed[9]*calibration_speed,2);
    CURSORC(3,Font);  display.print("250m"); CURSOR2C(3,Font); display.print(":");display.print(M250.display_max_speed*calibration_speed,2);
    CURSORC(4,Font);  display.print("500m"); CURSOR2C(4,Font); display.print(":");display.print(M500.avg_speed[9]*calibration_speed,2);
    CURSORC(5,Font);  display.print("Alfa"); CURSOR2C(5,Font); display.print(":");display.print(A500.avg_speed[9]*calibration_speed,2);
  }
  if(screen==STATS4){                        //10s AVG, 5 runs, update on the fly !!!
    update_delay=(config.Stat_screens_time-2)*1000;
    SetFont_L();   CURSORC(1,Font);                 display.print("avg 10s:");    
                   CURSORC(4,Font); SetFont_XXXL(); display.print(s10.avg_5runs*calibration_speed,1);
    SetFont_L();   
    CURSOR2C(2,Font); display.print("R1: "); display.print(s10.display_speed[9]*calibration_speed,2);
    CURSOR2C(3,Font); display.print("R2: "); display.print(s10.display_speed[8]*calibration_speed,2);
    CURSOR2C(4,Font); display.print("R3: "); display.print(s10.display_speed[7]*calibration_speed,2);
    CURSOR2C(5,Font); display.print("R4: "); display.print(s10.display_speed[6]*calibration_speed,2);
    CURSOR2C(6,Font); display.print("R5: "); display.print(s10.display_speed[5]*calibration_speed,2);
    SetFont_M();CURSORC(7,Font); display.print(time_now);
  }
  if(screen==STATS5){                        //alfa AVG, 5runs, update on the fly !!!
    update_delay=(config.Stat_screens_time-2)*1000;
    SetFont_L();   CURSORC(1,Font);                 display.print("avg Alfa:");
                   CURSORC(4,Font); SetFont_XXXL(); display.print((a500.avg_speed[9]+a500.avg_speed[8]+a500.avg_speed[7]+a500.avg_speed[6]+a500.avg_speed[5])/5*calibration_speed,1);
    SetFont_L();   
    CURSOR2C(2,Font); display.print("A1: "); display.print(a500.avg_speed[9]*calibration_speed,2);
    CURSOR2C(3,Font); display.print("A2: "); display.print(a500.avg_speed[8]*calibration_speed,2);
    CURSOR2C(4,Font); display.print("A3: "); display.print(a500.avg_speed[7]*calibration_speed,2);
    CURSOR2C(5,Font); display.print("A4: "); display.print(a500.avg_speed[6]*calibration_speed,2);
    CURSOR2C(6,Font); display.print("A5: "); display.print(a500.avg_speed[5]*calibration_speed,2);
    SetFont_M();CURSORC(7,Font); display.print(time_now);
  }
  if(screen==STATS6){ //Simon stat screen
    Serial.println("STATS6_Simon_screen");
    update_delay=(config.Stat_screens_time-2)*1500;
    int line;
    double S10avgNEW;
    if (S10.s_max_speed>S10.avg_speed[5]) {
      S10avgNEW=(S10.avg_speed[9]+S10.avg_speed[8]+S10.avg_speed[7]+S10.avg_speed[6]+S10.s_max_speed)/5;
    }
    else {
      S10avgNEW=(S10.avg_speed[9]+S10.avg_speed[8]+S10.avg_speed[7]+S10.avg_speed[6]+S10.avg_speed[5])/5;
    }
    SetCols(1);
    SetFont_LS();
    CURSOR_COLC(1,col1,Font); display.print("AV");
    CURSOR_COLC(2,col1,Font); display.print("R1");
    CURSOR_COLC(3,col1,Font); display.print("R2");
    CURSOR_COLC(4,col1,Font); display.print("R3"); 
    CURSOR_COLC(5,col1,Font); display.print("R4");
    CURSOR_COLC(6,col1,Font); display.print("R5"); 

    CURSOR_COLC(1,col2,Font); display.print(":");display.print(S10avgNEW * calibration_speed, 2);
    CURSOR_COLC(2,col2,Font); display.print(":");display.print(S10.avg_speed[9]*calibration_speed,2);
    CURSOR_COLC(3,col2,Font); display.print(":");display.print(S10.avg_speed[8]*calibration_speed,2);
    CURSOR_COLC(4,col2,Font); display.print(":");display.print(S10.avg_speed[7]*calibration_speed,2); 
    CURSOR_COLC(5,col2,Font); display.print(":");display.print(S10.avg_speed[6]*calibration_speed,2);
    CURSOR_COLC(6,col2,Font); display.print(":");display.print(S10.avg_speed[5]*calibration_speed,2); 

    CURSOR_COLC(1,col3,Font); display.print("2sec");
    CURSOR_COLC(2,col3,Font); display.print("Prv");
    CURSOR_COLC(3,col3,Font); display.print("Dist");
    CURSOR_COLC(4,col3,Font); display.print("Alp"); 
    CURSOR_COLC(5,col3,Font); display.print("500");
    CURSOR_COLC(6,col3,Font); display.print("Nm"); 

    CURSOR_COLC(1,col4,Font); display.print(":");display.print(S2.avg_speed[9]*calibration_speed);
    CURSOR_COLC(2,col4,Font); display.print(":");display.print(S10.display_last_run*calibration_speed,2);
    CURSOR_COLC(3,col4,Font); display.print(":");display.print(Ublox.total_distance/1000000);
    CURSOR_COLC(4,col4,Font); display.print(":");display.print(A500.avg_speed[9]*calibration_speed,2); 
    CURSOR_COLC(5,col4,Font); display.print(":");display.print(M500.display_max_speed*calibration_speed,2);
    CURSOR_COLC(6,col4,Font); display.print(":");display.print(M1852.display_max_speed*calibration_speed,2);

 // position line of actual 10s run
    if      (S10.s_max_speed*calibration_speed>S10.avg_speed[9]*calibration_speed) YPOS(1,Font);
    else if (S10.s_max_speed*calibration_speed>S10.avg_speed[8]*calibration_speed) YPOS(2,Font);
    else if (S10.s_max_speed*calibration_speed>S10.avg_speed[7]*calibration_speed) YPOS(3,Font);
    else if (S10.s_max_speed*calibration_speed>S10.avg_speed[6]*calibration_speed) YPOS(4,Font);
    else if (S10.s_max_speed*calibration_speed>S10.avg_speed[5]*calibration_speed) YPOS(5,Font);
    else YPOS(6,Font);
    display.fillRect(0,y_pos+2,col3-10,2,GxEPD_BLACK);//lijn voor actuele run weer te geven...
  }
  if(screen==STATS7){ //Simon bar graph screen
    Serial.println("STATS7_Simon_bar graph");
    update_delay=(config.Stat_screens_time-2)*1500;
    int posX=5;
    int posY=InfoBarTop;
    int MaxNumberBar=NR_OF_BAR;//is 42, zie GPS_data.h
    if(displayHeight<130) GraphWidth=displayWidth-45;
    else                  GraphWidth=displayWidth-55;
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
    
    SetFont_S();
    CURSOR(1,Font); display.print("Graph: Speed runs(10sec)");
    CURSOR(2,Font); display.print("R1-R5:");display.print(S10.display_speed[9]*calibration_speed,1);
                    display.print(" ");     display.print(S10.display_speed[8]*calibration_speed,1);
                    display.print(" ");     display.print(S10.display_speed[7]*calibration_speed,1);
                    display.print(" ");     display.print(S10.display_speed[6]*calibration_speed,1);
                    display.print(" ");     display.print(S10.display_speed[5]*calibration_speed,1);
    r=run_count%MaxNumberBar+1;//laatste bar = 0 ?
    for(int i=0;i<9;i++){          
      display.fillRect(offset+posX,posY-(i*10),GraphWidth, 1,GxEPD_BLACK);     //printen hor.lijnen grafiek, van 5 tot 215
      display.setCursor(GraphWidth+10+offset,posY-(i*10));                     //positie y-as legende
      display.print(min_bar+i*step);                                           //Printen y-as legende speed, van min_bar tot max_bar in 8 steps...
    }
    if (run_count<MaxNumberBar) {
      barWidth=max((GraphWidth-(r*barSpace))/(r),3);                           //(was r+1),3), laatste bar altijd 0; 
      barPitch=(barWidth)+barSpace; 
      for(int i=0;i<r;i++) {
        int barHeight=(S10.speed_run[i]*calibration_speed-min_bar)*scale; 
        int barHeightM=max(barHeight,0); 
        display.fillRect(offset+posX+(i*barPitch),posY-barHeightM,barWidth,barHeight,GxEPD_BLACK);
      }
    } else {
      barWidth=max((GraphWidth-(MaxNumberBar*barSpace))/MaxNumberBar,3);
      barPitch=(barWidth)+barSpace; 
      for(int i=0;i<MaxNumberBar;i++) {
        int barHeight=(S10.speed_run[(i+r)%NR_OF_BAR]*calibration_speed-min_bar)*scale;  //was S10.speed_run[i+r%NR_OF_BAR-42]  
        int barHeightM=max(barHeight,0); 
        display.fillRect(offset+posX+(i*barPitch),posY-barHeightM,barWidth,barHeight,GxEPD_BLACK);
      }    
    }
    #if defined(STATIC_DEBUG)
    Serial.print("Run: ");Serial.println(r);
    Serial.print("barWidth: ");Serial.println(barWidth);
    #endif
  }
  if(screen>BOOT_SCREEN&&screen<SPEED) {
    display.setFont(&FreeSansBold6pt7b);
    display.setCursor(displayWidth-10,InfoBarTop);
    display.print(screen);
  }
  if(count%200==0){//was 200
    if(update_epaper>0) display.update();
    offset=0;
  }
  else {
    display.setRotation(1);//needed for 266BN screen
    display.updateWindow(0,0,displayWidth,displayHeight,true);
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
  } else return_value=false; 
  if((millis()-millis_10s)<(1000*max_pulse_time)) long_pulse=true;
  else long_pulse=false; 
  if(digitalRead(Input_pin)==1) old_button_status=0;
  return return_value;
}
#undef ROW_1M
#undef TOP_LEFT_INFO
#undef ROW_3M
#undef ROW_1M
