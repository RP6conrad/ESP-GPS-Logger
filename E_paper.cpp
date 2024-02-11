#include "E_paper.h"
#include "Definitions.h"
#include <LITTLEFS.h>

#define ROW_SPACING 2

// row height 14pt spacing 2pt
#define ROW_6PT 10
#define ROW_6PT_W_SPACING (ROW_6PT + ROW_SPACING)
#define ROW_1_6PT ROW_9PT
#define ROW_2_6PT (ROW_1_6PT + ROW_6PT_W_SPACING)
#define ROW_3_6PT (ROW_2_6PT + ROW_6PT_W_SPACING)
#define ROW_4_6PT (ROW_3_6PT + ROW_6PT_W_SPACING)
#define ROW_5_6PT (ROW_4_6PT + ROW_6PT_W_SPACING)
#define ROW_6_6PT (ROW_5_6PT + ROW_6PT_W_SPACING)
#define ROW_7_6PT (ROW_6_6PT + ROW_6PT_W_SPACING)

// 3 rows 9pt font (9pt font height 18px-22pt)
// row height 14pt spacing 2pt
#define ROW_9PT 14
#define ROW_9PT_W_SPACING (ROW_9PT + ROW_SPACING)
#define ROW_1_9PT ROW_9PT
#define ROW_2_9PT (ROW_1_9PT + ROW_9PT_W_SPACING)  //30
#define ROW_3_9PT (ROW_2_9PT + ROW_9PT_W_SPACING)  //46
#define ROW_4_9PT (ROW_3_9PT + ROW_9PT_W_SPACING)  //62
#define ROW_5_9PT (ROW_4_9PT + ROW_9PT_W_SPACING)  //78
#define ROW_6_9PT (ROW_5_9PT + ROW_9PT_W_SPACING)  //94
#define ROW_7_9PT (ROW_6_9PT + ROW_9PT_W_SPACING)  //110

// 4 rows 12pt (12 pt font height 24-29pt)
// row height 19pt, spacing 2pt
#define ROW_12PT 19
#define ROW_12PT_W_SPACING (ROW_12PT + ROW_SPACING)
#define ROW_1_12PT ROW_12PT                           // 0-25
#define ROW_2_12PT (ROW_1_12PT + ROW_12PT_W_SPACING)  //40  // 27-52
#define ROW_3_12PT (ROW_2_12PT + ROW_12PT_W_SPACING)  //61  // 54-79
#define ROW_4_12PT (ROW_3_12PT + ROW_12PT_W_SPACING)  //82  // 81-106
#define ROW_5_12PT (ROW_4_12PT + ROW_12PT_W_SPACING)  //104 // 81-106

// 4 rows 18pt (18 pt font height 35-42pt)
// row height 25pt, spacing 2pt
#define ROW_18PT 25
#define ROW_18PT_W_SPACING (ROW_18PT + ROW_SPACING)
#define ROW_1_18PT ROW_18PT                           // 0-25
#define ROW_2_18PT (ROW_1_18PT + ROW_18PT_W_SPACING)  //52  // 27-52
#define ROW_3_18PT (ROW_2_18PT + ROW_18PT_W_SPACING)  //79  // 54-79
#define ROW_4_18PT (ROW_3_18PT + ROW_18PT_W_SPACING)  //106 // 81-106

// esp logo
#define ESP_GPS_LOGO_40 display.drawExampleBitmap(ESP_GPS_logo_40, offset + 198, 6, 40, 40, GxEPD_BLACK);
#define ESP_GPS_LOGO_48 display.drawExampleBitmap(ESP_GPS_logo, offset + 178, 0, 48, 48, GxEPD_BLACK);

// bottom area 15px reserved to info bar
#define INFO_BAR_HEIGHT 15
#define INFO_BAR_TOP (displayHeight - INFO_BAR_HEIGHT)
#define INFO_BAR_ROW (displayHeight - 2)

//display.setFont(&FreeSansBold9pt7b);
#define TITLE_9PT \
  display.setFont(&FreeSansBold9pt7b); \
  display.setCursor(offset, ROW_1_9PT);

#define TOP_TITLE_MSG(msg) display.print(msg);
#define TOP_TITLE(msg) \
  TOP_LEFT_TITLE_MSG(msg) \
  display.setCursor(offset, ROW_2_9PT);

#define TOP_LEFT_TITLE(msg) TITLE_9PT TOP_TITLE(msg)
#define TOP_LEFT_TITLE_MSG(msg) TITLE_9PT TOP_TITLE_MSG(msg)

char time_now[8];
char time_now_sec[12];
int16_t displayHeight;
int16_t displayWidth;

void InfoBar(int offset);
void InfoBarRtc(int offset);
void sdCardInfo(void);
const char* gpsChip(int longname);

int device_boot_log(int rows, int ws) {
  int r = 2, row = ROW_9PT + ROW_SPACING;
  //display.fillRect(0, ROW_1_9PT+1,175, r*row, GxEPD_WHITE);
  //display.setFont(&FreeMonoBold8pt7b);
  display.setCursor(offset, ROW_2_9PT);
  if (ws) delay(ws);
  display.print(SW_version);
  if (rows == 2 || rows == 23 || rows == 24 || rows == 234) {
    display.setCursor(offset, ROW_3_9PT);
    if (ws) delay(ws);
    sdCardInfo();
  }
  if (rows == 3 || rows == 23 || rows == 34 || rows == 234) {
    r = (rows == 23 || rows == 34) ? 3 : rows == 234 ? 4
                                                     : 2;
    if (ws) delay(ws);
    display.setCursor(offset, (rows == 234 || rows == 23 || rows == 34) ? ROW_4_9PT : ROW_3_9PT);
    display.printf("Display size %dx%d\n", displayWidth, displayHeight);
  }
  if ((rows == 4 || rows == 24 || rows == 34 || rows == 234) && ubxMessage.monVER.hwVersion[0]) {
    r = (rows == 24 || rows == 34) ? 3 : rows == 234 ? 4
                                                     : 2;
    if (ws) delay(ws);
    display.setCursor(offset, rows == 24 || rows == 34 ? ROW_4_9PT : (rows == 234 ? ROW_5_9PT : ROW_3_9PT));
    display.printf("Gps %s at %dHz", gpsChip(1), config.sample_rate);
  }
  //display.updateWindow(0,ROW_1_9PT+1,175,r*row,true);
  return r;
}
#define DEVICE_BOOT_LOG(rows) device_boot_log(rows, 0)

int rowTestUnit(int xPos) {
  int16_t x1, y1, box_padding, rows, rowheight;
  uint16_t w, h;
  display.getTextBounds("000", xPos, INFO_BAR_TOP, &x1, &y1, &w, &h);
  Serial.print("000");
  Serial.print(" getTextBounds got (");
  Serial.print(x1);
  Serial.print(", ");
  Serial.print(y1);
  Serial.print(", ");
  Serial.print(w);
  Serial.print(", ");
  Serial.print(h);
  Serial.println(")");
  rows = INFO_BAR_TOP / (h + ROW_SPACING);
  rowheight = INFO_BAR_TOP / rows;
  box_padding = ((rowheight - h) / 2);
  Serial.printf("fontbounds: rows:%d rowheight:%d,  fontheight: %d, spacing:%d, box_padding:%d\n", rows, rowheight, h, ROW_SPACING, box_padding);
  for (int row = 0, i = 0, j = INFO_BAR_TOP; i < j; i += rowheight, ++row) {

    if (row == 0 && box_padding > 0)
      i -= box_padding;
    if (i > 0) {
      display.setCursor(xPos, i);
      display.println(i);
    }
    display.fillRect(xPos - 5, i, 5, 1, GxEPD_BLACK);
  }
  return xPos + w + 10;
}

void rowTest() {
  int16_t x1, y1, box_padding, xPos = 20, rows, rowheight;
  uint16_t w, h;
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  //display.fillRect(0, 5, displayWidth, 1, GxEPD_BLACK);
  //display.fillRect(0, INFO_BAR_TOP-1, displayWidth, 1, GxEPD_BLACK);
  //display.fillRect(0, 0, 1, INFO_BAR_TOP, GxEPD_BLACK);
  //6pt test
  display.setFont(&FreeSansBold6pt7b);
  xPos = rowTestUnit(xPos);
  //9pt test
  display.setFont(&FreeSansBold9pt7b);
  xPos = rowTestUnit(xPos);
  //12pt test
  display.setFont(&FreeSansBold12pt7b);
  xPos = rowTestUnit(xPos);
  //18pt test
  display.setFont(&FreeSansBold18pt7b);
  xPos = rowTestUnit(xPos);
  //24pt test
  display.setFont(&FreeSansBold24pt7b);
  xPos = rowTestUnit(xPos);
  display.updateWindow(0, 0, displayWidth, displayHeight, true);
  delay(100);
}

void Boot_screen(void) {
  display.init();
  display.setRotation(1);
  displayHeight = display.height();
  displayWidth = display.width();
  //rowTest();
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  ESP_GPS_LOGO_40
  InfoBarRtc(offset);
  TITLE_9PT
  if (RTC_voltage_bat < MINIMUM_VOLTAGE) {
    int cursor = ROW_2_9PT;
    TOP_LEFT_TITLE("EPS-GPS sleeping")
    display.print("Go back to sleep...");
    display.setFont(&FreeSansBold12pt7b);
    display.setCursor(offset, (cursor += ROW_12PT_W_SPACING));
    display.printf("Voltage to low: %f", RTC_voltage_bat);
    display.setCursor(offset, (cursor += ROW_12PT_W_SPACING));
    display.print("Please charge battery!");
    display.update();
  } else {
    TOP_LEFT_TITLE_MSG("ESP-GPS booting");
    DEVICE_BOOT_LOG(23);
    display.updateWindow(0, 0, displayWidth, displayWidth, true);
    delay(100);
    //display.update();
  }
}
void Off_screen(int choice) {  //choice 0 = old screen, otherwise Simon screens
  //int offset=0;
  float session_time = (millis() - start_logging_millis) / 1000;
  display.setRotation(1);
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  int cursor = ROW_3_9PT + ROW_12PT_W_SPACING;
  if (choice == 0) {
    ESP_GPS_LOGO_40
    TOP_LEFT_TITLE_MSG("ESP-GPS saving");  //row1 14
    DEVICE_BOOT_LOG(4);                    //2 rows until 46
    //display.println("Need for speed!");
    display.setFont(&FreeSansBold12pt7b);
    display.setCursor(offset, cursor);
    if (Shut_down_Save_session == true) {
      display.println("Saving session");
      display.setFont(&FreeSansBold9pt7b);
      display.setCursor(offset, (cursor += ROW_9PT_W_SPACING));
      display.print("Time: ");
      display.print(session_time, 0);
      display.print(" s");
      display.setCursor(offset, (cursor += ROW_9PT_W_SPACING));
      display.print("AVG: ");
      display.print(RTC_avg_10s, 2);
      display.setCursor(offset + 120, cursor);
      display.print("Dist: ");
      display.print(Ublox.total_distance / 1000, 0);
    } else {
      display.println("Going back to sleep");
    }
  } else {
    Serial.println("Off_screen Simon");
    ESP_GPS_LOGO_40
    TOP_LEFT_TITLE_MSG("ESP-GPS saving");
    DEVICE_BOOT_LOG(4);  //row 2 and 3
    //display.println("Need for speed!");
    display.setFont(&FreeSansBold12pt7b);
    display.setCursor(offset, cursor);  //62
    if (Shut_down_Save_session == true) {
      display.println("Saving session");
      display.setFont(&FreeSansBold9pt7b);
      display.setCursor(offset, (cursor += ROW_9PT_W_SPACING));
      display.print("Time: ");
      display.print(session_time, 0);
      display.print(" s");
      display.setCursor(offset, (cursor += ROW_9PT_W_SPACING));
      display.print("AVG: ");
      display.print(RTC_avg_10s, 2);
      display.setCursor(offset + 120, cursor);
      display.print("Dist: ");
      display.print(Ublox.total_distance / 1000, 0);
    } else {
      display.println("Going back to sleep");
    }
  }
  InfoBar(offset);
  display.updateWindow(0, 0, displayWidth, displayHeight, true);
  delay(10000);  //om te voorkomen dat update opnieuw start !!!
}
//Screen in deepsleep, update bat voltage, refresh every 4000s !!
void Sleep_screen(int choice) {
  if (offset > 9) offset--;
  if (offset < 1) offset++;
  display.init();
  display.setRotation(1);
  displayHeight = display.height();
  displayWidth = display.width();
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  InfoBarRtc(offset);
  if (choice == 0) {
    display.setFont(&FreeSansBold18pt7b);
    display.setCursor(offset, 24);
    display.print("Dist: ");
    display.println(RTC_distance, 0);
    display.setCursor(offset, 56);
    display.print("AVG: ");
    display.println(RTC_avg_10s, 2);
    display.setCursor(offset, 88);
    display.print("2s: ");
    display.print(RTC_max_2s);
    display.update();
  } else {
    int row1 = 15;
    int row = 15;
    int row2 = row1 + row;
    int row3 = row2 + row;
    int row4 = row3 + row;
    int row5 = row4 + row;
    int row6 = row5 + row;
    int col1 = 0 + offset;
    int col2 = 34 + offset;
    int col3 = 90 + offset;
    int col4 = 146 + offset;
    //int line=0;
    //display.fillScreen(GxEPD_WHITE);
    //display.fillRect(0,0,255,122,GxEPD_WHITE);

    // Board Logo's:
    // add special logos - funlogos
    //Board logos
    if (RTC_Board_Logo == 1)  //Logo's Simon Dijkstra
      display.drawExampleBitmap(Starboard_logoS_zwart, 195, 0, 48, 48, GxEPD_BLACK);
    if (RTC_Board_Logo == 2)
      display.drawExampleBitmap(Fanatic_logoS_zwart, 195, 0, 48, 48, GxEPD_BLACK);
    if (RTC_Board_Logo == 3)
      display.drawExampleBitmap(JP_logoS_zwart, 195, 0, 48, 48, GxEPD_BLACK);
    if (RTC_Board_Logo == 4)
      display.drawExampleBitmap(NoveNove_logoS_zwart, 195, 0, 48, 48, GxEPD_BLACK);
    if (RTC_Board_Logo == 5)  //Logo's Jan Scholten
      display.drawExampleBitmap(Mistral_logoS_zwart, 195, 0, 48, 48, GxEPD_BLACK);
    if (RTC_Board_Logo == 6)
      display.drawExampleBitmap(Goya_logoS_zwart, 195, 0, 48, 48, GxEPD_BLACK);
    if (RTC_Board_Logo == 7)
      display.drawExampleBitmap(Patrik_logoS_zwart, 195, 0, 48, 48, GxEPD_BLACK);
    if (RTC_Board_Logo == 8)
      display.drawExampleBitmap(Severne_logoS_zwart, 195, 0, 48, 48, GxEPD_BLACK);
    if (RTC_Board_Logo == 9)
      display.drawExampleBitmap(Tabou_logoS_zwart, 195, 0, 48, 48, GxEPD_BLACK);
    if (RTC_Board_Logo == 10)
      display.drawExampleBitmap(F2_logo_zwart, 195, 0, 48, 48, GxEPD_BLACK);
    if (RTC_Board_Logo == 11)  // Schwechater - Austrian Beer - by tritondm
      display.drawExampleBitmap(epd_bitmap_Schwechater, 195, -10, 79, 132, GxEPD_BLACK);
    if (RTC_Board_Logo == 12)
      display.drawExampleBitmap(Thommen1_logo_BW, 195, 0, 64, 48, GxEPD_BLACK);
    if (RTC_Board_Logo == 13)
      display.drawExampleBitmap(BIC_logo_BW, 195, 0, 48, 48, GxEPD_BLACK);
    if (RTC_Board_Logo == 14)
      display.drawExampleBitmap(Carbon_art, 195, 0, 48, 39, GxEPD_BLACK);
    if (RTC_Board_Logo == 15)
      display.drawExampleBitmap(FutureFly_logo_zwart, 195, 0, 48, 48, GxEPD_BLACK);

    // Zeil Logo's:
    if (RTC_Sail_Logo == 1)  //Logo's Simon Dijkstra
      display.drawExampleBitmap(GAsails_logoS_zwart, 195, 50, 48, 48, GxEPD_BLACK);
    if (RTC_Sail_Logo == 2)
      display.drawExampleBitmap(DuoTone_logoS_zwart, 195, 50, 48, 48, GxEPD_BLACK);
    if (RTC_Sail_Logo == 3)
      display.drawExampleBitmap(NP_logoS_zwart, 195, 50, 48, 48, GxEPD_BLACK);
    if (RTC_Sail_Logo == 4)
      display.drawExampleBitmap(Pryde_logoS_zwart, 195, 50, 48, 48, GxEPD_BLACK);
    if (RTC_Sail_Logo == 5)  //Logo's Jan Scholten
      display.drawExampleBitmap(Loftsails_logoS_zwart, 195, 50, 48, 48, GxEPD_BLACK);
    if (RTC_Sail_Logo == 6)
      display.drawExampleBitmap(Gunsails_logoS_zwart, 195, 50, 48, 48, GxEPD_BLACK);
    if (RTC_Sail_Logo == 7)
      display.drawExampleBitmap(Point7_logoS_zwart, 195, 50, 48, 48, GxEPD_BLACK);
    if (RTC_Sail_Logo == 8)
      display.drawExampleBitmap(Simmer_logoS_zwart, 195, 50, 48, 48, GxEPD_BLACK);
    if (RTC_Sail_Logo == 9)
      display.drawExampleBitmap(Naish_logoS_zwart, 195, 50, 48, 48, GxEPD_BLACK);
    if (RTC_Sail_Logo == 10) {  //Severne as Sail logo !!!
      display.drawExampleBitmap(Severne_logoS_zwart, 195, 50, 48, 48, GxEPD_BLACK);
    }
    if (RTC_Sail_Logo == 11) {  //Severne as Sail logo !!!
      display.drawExampleBitmap(S2maui_logoS_zwart, 195, 50, 48, 48, GxEPD_BLACK);
    }
    display.setCursor(col1, 105);  // was 121
    display.setFont(&SF_Distant_Galaxy9pt7b);
    display.print(RTC_Sleep_txt);

    display.setRotation(0);
    display.setCursor(30, 249);  //was 30, 249
    display.setFont(&FreeSansBold6pt7b);
    if ((int)(calibration_speed * 100000) == 194) display.print("speed in knots");  //1.94384449 m/s to knots !!!
    if ((int)(calibration_speed * 1000000) == 3600) display.print("speed in km/h");

    display.setRotation(1);
    // left column
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(col1, row1);
    display.print("AV:");
    display.setCursor(col1, row2);
    display.print("R1:");
    display.setCursor(col1, row3);
    display.print("R2:");
    display.setCursor(col1, row4);
    display.print("R3:");
    display.setCursor(col1, row5);
    display.print("R4:");
    display.setCursor(col1, row6);
    display.print("R5:");

    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(col2, row1);
    display.println(RTC_avg_10s, 2);
    display.setCursor(col2, row2);
    display.println(RTC_R1_10s, 2);
    display.setCursor(col2, row3);
    display.println(RTC_R2_10s, 2);
    display.setCursor(col2, row4);
    display.println(RTC_R3_10s, 2);
    display.setCursor(col2, row5);
    display.println(RTC_R4_10s, 2);
    display.setCursor(col2, row6);
    display.println(RTC_R5_10s, 2);

    // right column
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(col3, row1);
    display.print("2sec:");
    display.setCursor(col3, row2);
    display.print("Dist:");
    display.setCursor(col3, row3);
    display.print("Alph:");
    display.setCursor(col3, row4);
    display.print("1h:");  //
    display.setCursor(col3, row5);
    display.print("NM:");
    display.setCursor(col3, row6);
    display.print("500m:");

    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(col4, row1);
    display.println(RTC_max_2s, 2);
    display.setCursor(col4, row2);
    display.println(RTC_distance, 2);
    display.setCursor(col4, row3);
    display.println(RTC_alp, 2);
    display.setCursor(col4, row4);
    display.println(RTC_1h, 2);  //
    display.setCursor(col4, row5);
    display.println(RTC_mile, 2);
    display.setCursor(col4, row6);
    display.println(RTC_500m, 2);
    display.update();
  }
}
int update_time() {
  int ret = 0;
  if (!NTP_time_set) {
    if (!Gps_time_set) {
      if (Set_GPS_Time(config.timezone)) Gps_time_set = 1;
    }
  }
  if ((!Gps_time_set && !NTP_time_set) || !getLocalTime(&tmstruct)) return 1;
  sprintf(time_now, "%02d:%02d", tmstruct.tm_hour, tmstruct.tm_min);
  sprintf(time_now_sec, "%02d:%02d:%02d", tmstruct.tm_hour, tmstruct.tm_min, tmstruct.tm_sec);
  return ret;
}

//for print hour&minutes with 2 digits
void time_print(int time) {
  if (time < 10) display.print("0");
  display.print(time);
}
void Bat_level(int X_offset, int Y_offset) {
  float bat_symbol = 0;
  display.fillRect(X_offset + 3, Y_offset, 6, 3, GxEPD_BLACK);
  display.fillRect(X_offset, Y_offset + 3, 12, 30, GxEPD_BLACK);  //monitor=(4.2-RTC_voltage_bat)*26
  if (RTC_voltage_bat < VOLTAGE_100) {
    bat_symbol = (VOLTAGE_100 - RTC_voltage_bat) * 28;
    display.fillRect(X_offset + 2, Y_offset + 7, 8, (int)bat_symbol, GxEPD_WHITE);
  }
}
void Bat_level_Simon(int offset) {
  float bat_perc = 100 * (1 - (VOLTAGE_100 - RTC_voltage_bat) / (VOLTAGE_100 - VOLTAGE_0));
  if (bat_perc < 0) bat_perc = 0;
  if (bat_perc > 100) bat_perc = 100;

  int batW = 8;
  int batL = 15;
  int posX = displayWidth - batW - 10;
  int posY = displayHeight - batL;
  int line = 2;
  int seg = 3;
  int segW = batW - 2 * line;
  int segL = (batL - 0.25 * batW - 2 * line - (seg - 1)) / seg;
  display.fillRect(offset + posX, posY, 0.5 * batW, 0.25 * batW, GxEPD_BLACK);                 //battery top
  display.fillRect(offset + posX - 0.25 * batW, posY + 0.25 * batW, batW, batL, GxEPD_BLACK);  //battery body
  if (bat_perc < 67) display.fillRect(offset + posX - 0.25 * batW + line, posY + 0.25 * batW + line, segW, segL, GxEPD_WHITE);
  if (bat_perc < 33) display.fillRect(offset + posX - 0.25 * batW + line, posY + 0.25 * batW + line + 1 * (segL + 1), segW, segL, GxEPD_WHITE);
  if (bat_perc < 1) display.fillRect(offset + posX - 0.25 * batW + line, posY + 0.25 * batW + line + 2 * (segL + 1), segW, segL, GxEPD_WHITE);
  //Serial.printf("info bar cursor pos: %d, display height: %d\n", INFO_BAR_ROW,displayHeight);
  display.setFont(&FreeSansBold9pt7b);
  //display.setCursor(displayWidth-8,(INFO_BAR_ROW-ROW_9PT));
  //display.print("-");
  if (bat_perc < 100) display.setCursor(offset + 193, (INFO_BAR_ROW));
  else display.setCursor(offset + 184, (INFO_BAR_ROW));
  display.print(int(bat_perc));
  display.print("%");
}
void Sats_level(int offset) {
  if (!ubxMessage.monVER.swVersion[0]) return;
  int circelL = 5;
  int circelS = 2;
  int posX = 167 + offset;
  int posY = INFO_BAR_TOP;  //-(circelL+2*circelS);
  int satnum = ubxMessage.navPvt.numSV;
  display.drawExampleBitmap(ESP_Sat_15, posX, posY, 15, 15, GxEPD_BLACK);
  display.setFont(&FreeSansBold9pt7b);
  display.setCursor(posX - (satnum < 10 ? 9 : 18), INFO_BAR_ROW);
  display.print(satnum);
}
const char* gpsChip(int longname) {
  switch (config.ublox_type) {
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
void M8_M10(int offset) {
  display.setFont(&FreeSansBold9pt7b);
  display.setCursor(offset + 110, INFO_BAR_ROW);
  display.print(gpsChip(0));
}
int Time(int offset) {
  if (!update_time()) {
    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(offset, INFO_BAR_ROW);
    display.print(time_now);
  }
  return 0;
}
int TimeRtc(int offset) {
  display.setFont(&FreeSansBold9pt7b);
  display.setCursor(offset, INFO_BAR_ROW);
  display.printf("%d:%d", RTC_hour, RTC_min);
  return 0;
}
int DateTimeRtc(int offset) {
  display.setFont(&FreeSansBold9pt7b);
  display.setCursor(offset, INFO_BAR_ROW);
  display.printf("%02d:%02d %02d-%02d-%02d", RTC_hour, RTC_min, RTC_day, RTC_month, RTC_year);
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

void Speed_in_Unit(int offset) {
  display.setRotation(0);
  display.setFont(&FreeSansBold6pt7b);
  display.setCursor(30, offset + 245);                                            //was 30, 249
  if ((int)(calibration_speed * 100000) == 194) display.print("speed in knots");  //1.94384449 m/s to knots !!!
  if ((int)(calibration_speed * 1000000) == 3600) display.print("speed in km/h");
  display.setRotation(1);
}
void sdCardInfo(void) {
  if (sdOK) display.printf("SD : %d Mb\n", freeSpace);
  if (LITTLEFS_OK) display.printf("Local : %d kb\n", (LITTLEFS.totalBytes() - LITTLEFS.usedBytes()) / 1024);
}
void Update_screen(int screen) {
  static int count, old_screen, update_delay;
  update_time();
  update_epaper = 1;  //was zonder else
  if (count % 20 < 10) offset++;
  else offset--;
  if (offset > 10) offset = 0;
  if (offset < 0) offset = 0;
  int cursor = 0;
  display.fillScreen(GxEPD_WHITE);
  if (screen != SPEED) InfoBar(offset);
  if (screen == BOOT_SCREEN) {
    update_delay = 1000;
    ESP_GPS_LOGO_40
    TOP_LEFT_TITLE_MSG("ESP-GPS config");
    DEVICE_BOOT_LOG(234);
    if (screen != old_screen) count = 0;  //eerste keer full update
    Speed_in_Unit(offset);
    delay(1000);
  }
  if (screen == GPS_INIT_SCREEN) {
    update_delay = 100;
    ESP_GPS_LOGO_40
    TOP_LEFT_TITLE_MSG("ESP-GPS GPS init");
    DEVICE_BOOT_LOG(24);
    if (!ubxMessage.monVER.hwVersion[0]) {
      display.setFont(&FreeSansBold12pt7b);
      display.setCursor(offset, (cursor = ROW_4_9PT + ROW_12PT_W_SPACING));
      display.print("Gps initializing");
    }
    if (screen != old_screen) count = 0;  //eerste keer full update
    Speed_in_Unit(offset);
  }
  if (screen == WIFI_ON) {
    update_delay = 1000;
    if (count % 20 < 10) offset++;
    else offset--;
    ESP_GPS_LOGO_40
    TOP_LEFT_TITLE_MSG("ESP-GPS connect");
    DEVICE_BOOT_LOG(2);

    if (Wifi_on == 1) {
      display.setFont(&FreeSansBold12pt7b);
      display.setCursor(offset, (cursor = ROW_3_9PT + ROW_12PT_W_SPACING));
      //display.fillRect(0, ROW_3_9PT+1, 180, INFO_BAR_TOP-ROW_3_9PT-1, GxEPD_WHITE);//clear lower part
      display.print("Ssid: ");
      if (SoftAP_connection == true) display.print("ESP32AP");  //ap mode
      else display.print(config.ssid);
      display.setFont(&FreeSansBold9pt7b);
      display.setCursor(offset, (cursor += ROW_9PT_W_SPACING));
      if (SoftAP_connection == true) {
        display.print("Password: password");
        display.setCursor(offset, (cursor += ROW_9PT_W_SPACING));
      }
      display.printf("http://%s", IP_adress.c_str());
      //display.setCursor(offset,120);
      //display.println("Use your browser");

    } else {
      display.fillRect(0, 0, 180, 104, GxEPD_WHITE);
      TOP_LEFT_TITLE_MSG("ESP-GPS ready");
      DEVICE_BOOT_LOG(24);

      display.setFont(&FreeSansBold12pt7b);
      display.setCursor(offset, (cursor = ROW_4_9PT + ROW_12PT_W_SPACING));
      if (ubxMessage.navPvt.numSV < 5) {
        display.println("Waiting for Sat >=5");
        display.setFont(&FreeSansBold9pt7b);
        display.setCursor(offset, (cursor += ROW_9PT_W_SPACING));
        display.println("Please go outside");

      } else {
        display.println("Ready for action");
        display.setFont(&FreeSansBold9pt7b);
        display.setCursor(offset, (cursor += ROW_9PT_W_SPACING));
        display.print("Move faster than ");
        if ((int)(calibration_speed * 100000) == 194) {
          display.print(config.start_logging_speed * 1.94384449);
          display.print("kn");
        }
        if ((int)(calibration_speed * 1000000) == 3600) {
          display.print(config.start_logging_speed * 3.6);
          display.print("km/h");
        }
        Speed_in_Unit(offset);
      }
    }
  }
  if (screen == WIFI_STATION) {
    update_delay = 100;
    ESP_GPS_LOGO_40
    TOP_LEFT_TITLE_MSG("ESP-GPS connect");
    DEVICE_BOOT_LOG(2);
    //display.fillRect(0, ROW_4_9PT+1, 190, INFO_BAR_TOP-ROW_4_9PT-10, GxEPD_WHITE);//clear lower part
    display.setFont(&FreeSansBold12pt7b);
    display.setCursor(offset, (cursor = ROW_3_9PT + ROW_12PT_W_SPACING));
    display.print("Create wifi AP");
    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(offset, (cursor += ROW_9PT_W_SPACING));
    display.printf("Use magnet in %ds", wifi_search);
    if (screen != old_screen) count = 0;  //eerste keer full update
  }
  if (screen == WIFI_SOFT_AP) {
    update_delay = 500;
    //display.fillScreen(GxEPD_WHITE);
    ESP_GPS_LOGO_40
    TOP_LEFT_TITLE_MSG("Connect to ESP-GPS");
    DEVICE_BOOT_LOG(2);
    display.setFont(&FreeSansBold12pt7b);
    display.setCursor(offset, (cursor = ROW_3_9PT + ROW_12PT_W_SPACING));
    display.print("Ssid: ESP32AP");
    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(offset, (cursor += ROW_9PT_W_SPACING));
    display.print("Password: password");
    display.setCursor(offset, (cursor += ROW_9PT_W_SPACING));
    display.printf("http://%s/ in %ds\n", IP_adress.c_str(), wifi_search);
    if (screen != old_screen) count = 0;  //eerste keer full update
  }
  if (screen == SPEED) {
    update_delay = 100;
    int run_rectangle_length;
    int field = config.field_actual;  //default is in config.txt
    int bar_position = 32;
    int bar_length = config.bar_length * 1000 / 240;  //default 100% length = 1852 m
    display.setFont(&FreeSansBold6pt7b);
    display.setCursor(displayWidth - 10, INFO_BAR_TOP);
    display.print(config.field_actual);                                                                        //show config field in small font
    if (config.field_actual == 1) {                                                                            //switch alfa, run-AVG, NM!!!
      field = 2;                                                                                               //Default Run-AVG // NM
      if (((int)(Ublox.total_distance / 1000000) % 10 == 0) & (Ublox.alfa_distance / 1000 > 1000)) field = 5;  //indien x*10km, totale afstand laten zien
      if ((Ublox.alfa_distance / 1000 < 350) & (alfa_window < 100)) field = 3;                                 //first 350 m after gibe  alfa screen !!
      if (Ublox.alfa_distance / 1000 > config.bar_length) field = 4;                                           //run longer dan 1852 m, NM scherm !!
    }
    if (config.field_actual == 2) {
      field = 2;                                                     //default actual Run - AVG / NM
      if (Ublox.run_distance / 1000 > config.bar_length) field = 4;  //if run longer dan 1852 m, NM scherm !!
    }
    if (config.field_actual == 7) {
      field = 7;                                                                //default 0.5h
      if ((Ublox.alfa_distance / 1000 < 350) & (alfa_window < 100)) field = 3;  //first 350 m after gibe  alfa screen !!
    }
    if (config.field_actual == 8) {
      field = 8;                                                                //default 1h
      if ((Ublox.alfa_distance / 1000 < 350) & (alfa_window < 100)) field = 3;  //first 350 m after gibe  alfa screen !!
    }
    if (config.field_actual == 9) {  //1 hour default, but first alfa, and if good run, last run
      field = 2;
      if (Ublox.alfa_distance / 1000 > config.bar_length) field = 4;            //run longer dan 1852 m, NM scherm !!
      if (S10.s_max_speed > S10.display_speed[5]) field = 2;                    //if run faster then slowest run, show AVG & run after 1000 m
      if (Ublox.alfa_distance / 1000 < 1000) field = 8;                         // 350m - 1000m : 1h !!
      if ((Ublox.alfa_distance / 1000 < 350) & (alfa_window < 100)) field = 3;  //first 350 m after gibe  alfa screen !!
    }

    if (GPS_Signal_OK == true) {
      display.setFont(&SansSerif_bold_96_nr);
      if ((config.speed_large_font == 1) & (config.field <= 4)) {
        display.setFont(&SansSerif_bold_84_nr);
      }                                    //test for bigger font alfa
      if (config.speed_large_font == 2) {  //test for bigger font speed (Simon)
        display.setFont(&FreeSansBold75pt7b);
        display.setCursor(offset + 5, 115);
        display.print(gps_speed * calibration_speed, 0);  //print main in large font
        display.setFont(&FreeSansBold30pt7b);
        display.print(".");
        display.println(int((gps_speed * calibration_speed - int(gps_speed * calibration_speed)) * 10), 0);  //int((x-int(x))*10) round to correct digit
      } else {
        display.setCursor(offset, 120);
        display.println(gps_speed * calibration_speed, 1);
      }
    } else {
      display.setFont(&FreeSansBold18pt7b);
      display.setCursor(offset, 60);
      display.print("Low GPS signal !");
    }
    display.setFont(&FreeSansBold18pt7b);
    display.setCursor(offset, 25);
    display.setTextColor(GxEPD_BLACK);
    if (field <= 2) {
      if (config.speed_large_font == 0) {
        display.setFont(&FreeSansBold12pt7b);
        display.print("Run ");
        display.setFont(&FreeSansBold18pt7b);
        display.print(S10.s_max_speed * calibration_speed, 1);  //last 10s max from run
        display.setFont(&FreeSansBold12pt7b);
        display.setCursor(offset + 122, 24);
        display.print("Avg ");
        display.setFont(&FreeSansBold18pt7b);
        display.print(S10.avg_5runs * calibration_speed, 1);
      }
      if (config.speed_large_font == 1) {
        bar_position = 40;                       //test voor bigger font, was 42
        display.setFont(&SansSerif_bold_46_nr);  //Test for bigger alfa fonts
        display.setCursor(offset, 36);
        display.print(S10.s_max_speed * calibration_speed, 1);  //last 10s max from run
        display.setFont(&FreeSansBold12pt7b);                   //A voor AVG !!!!
        display.setCursor(offset + 113, 36);
        display.print("A");
        display.setFont(&SansSerif_bold_46_nr);  //Test for bigger alfa fonts
        display.print(S10.avg_5runs * calibration_speed, 1);
      }
    }
    /*
      First 250m after jibe, if Window>99 m : Window and Exit
      Between 250m and 400m after jibe : Result Alfa (speed or MISS)
      Between 400m and 1852m after jibe : Actual Run + AVG
      More then 1852m : NM actual speed and NM Best speed
      */
    if ((field == 3) & (config.speed_large_font == 0)) {
      bar_length = 250 * 1000 / 240;                                  //full bar length with Alfa = 250 meter
      if ((alfa_window < 99) & (Ublox.alfa_distance / 1000 < 255)) {  //Window alleen indien Window<100 en Run>350 meter !!!!&(A500.alfa_speed_max*calibration_speed<1)
        display.setFont(&FreeSansBold12pt7b);
        display.print("Gate ");  // en nog geen geldige alfa
        display.setFont(&FreeSansBold18pt7b);
        display.print(alfa_window, 0);
        display.print(" Ex ");
        display.setFont(&FreeSansBold18pt7b);
        if (alfa_exit > 99) alfa_exit = 99;  //begrenzen alfa_exit...
        display.print(alfa_exit, 0);         //test functie
      } else {
        display.setFont(&FreeSansBold12pt7b);
        display.print("Alfa ");
        display.setFont(&FreeSansBold18pt7b);
        display.print(A500.display_max_speed * calibration_speed, 1);  //best Alfa from session on 500 m !!
        if (A500.alfa_speed_max * calibration_speed > 1) {             //laatste alfa was geldig !!!!
          display.print("Al ");                                        //nieuwe alfa laatste gijp or MISSED !!!!
          display.setFont(&FreeSansBold18pt7b);
          display.print(A500.alfa_speed_max * calibration_speed, 1);  //last alfa, back to zero 500 m after jibe !!
        } else {
          display.setFont(&FreeSansBold18pt7b);  //laatste alfa was ongeldig !!!!
          display.print("MISS");
        }
      }
    }

    if ((field == 3) & (config.speed_large_font == 1)) {
      bar_length = 250 * 1000 / 240;                                  //full bar length with Alfa = 250 meter
      bar_position = 40;                                              //test voor bigger font
      if ((alfa_window < 99) & (Ublox.alfa_distance / 1000 < 255)) {  //Window alleen indien Window<100 en Run>350 meter !!!!&(A500.alfa_speed_max*calibration_speed<1)
        display.setFont(&FreeSansBold12pt7b);
        display.setCursor(offset, 36);
        display.print("Gate");  // en nog geen geldige alfa
        display.setFont(&SansSerif_bold_46_nr);
        display.print(alfa_window, 0);
        display.setFont(&FreeSansBold12pt7b);
        display.print(" Ex ");
        if (alfa_exit > 99) alfa_exit = 99;  //begrenzen alfa_exit...
        display.setFont(&SansSerif_bold_46_nr);
        display.print(alfa_exit, 0);  //test functie
      } else {
        display.setFont(&FreeSansBold18pt7b);
        display.setCursor(offset, 38);
        //display.print(A500.display_max_speed*calibration_speed,1);   //best Alfa from session on 500 m !!
        display.print("Alfa= ");
        display.setCursor(offset + 110, 38);
        display.setFont(&SansSerif_bold_46_nr);
        if (A500.alfa_speed_max * calibration_speed > 1) {            //laatste alfa was geldig !!!!
          display.print(A500.alfa_speed_max * calibration_speed, 1);  //last alfa, back to zero 500 m after jibe !!
        } else {
          display.setFont(&FreeSansBold18pt7b);  //laatste alfa was ongeldig !!!!
          display.print("MISS");
        }
      }
    }
    if (field == 4) {
      if (config.speed_large_font == 0) {
        display.setFont(&FreeSansBold12pt7b);
        display.print("NMa ");  //Actuele nautical mile
        display.setFont(&FreeSansBold18pt7b);
        display.print(M1852.m_max_speed * calibration_speed, 1);  //actueel topspeed 10s van deze run
        display.setFont(&FreeSansBold12pt7b);
        display.setCursor(offset + 129, 24);
        display.print("NM ");
        display.setFont(&FreeSansBold18pt7b);
        display.print(M1852.display_max_speed * calibration_speed, 1);  //Snelste nautical mile van de sessie
      }
      if (config.speed_large_font == 1) {
        bar_position = 40;  //test voor bigger font
        display.setFont(&FreeSansBold18pt7b);
        display.setCursor(offset, 36);
        display.print("NM= ");  //Actuele nautical mile
        display.setFont(&SansSerif_bold_46_nr);
        display.print(M1852.m_max_speed * calibration_speed, 2);  //actueel topspeed NM van deze run
      }
    }
    if (field == 5) {
      if (config.speed_large_font == 1) {
        bar_position = 40;  //test voor bigger font
        display.setFont(&FreeSansBold12pt7b);
        display.setCursor(offset, 36);
        display.print("Dist ");
        display.setFont(&SansSerif_bold_46_nr);
        display.print(Ublox.total_distance / 1000000, 1);  //Total distance in km, als test run_distance
        if (Ublox.total_distance / 1000000 > 99.9) {
          display.setFont(&FreeSansBold12pt7b);
          display.print(" km");
        }
      } else {
        display.setFont(&FreeSansBold12pt7b);
        display.setCursor(offset, 36);
        display.print("Dist ");
        display.setFont(&FreeSansBold18pt7b);
        display.print(Ublox.total_distance / 1000000, 1);  //Total distance in km, als test run_distance
        display.setFont(&FreeSansBold12pt7b);
        display.print(" km");
      }
    }
    if (field == 6) {
      display.setFont(&FreeSansBold12pt7b);
      display.print("2S ");
      display.setFont(&FreeSansBold18pt7b);
      display.print(S2.display_max_speed * calibration_speed, 1);  //best 2s
      display.setFont(&FreeSansBold12pt7b);
      display.setCursor(offset + 124, 24);
      display.print("10S ");
      display.setFont(&FreeSansBold18pt7b);
      display.print(S10.display_max_speed * calibration_speed, 1);  //best 10s run
    }
    if (field == 7) {
      if (config.speed_large_font == 1) {
        bar_position = 40;                     //test voor bigger font, was 42
        display.setFont(&FreeSansBold12pt7b);  // !!!!
        display.setCursor(offset, 36);
        display.print("0.5h:");
        display.print(S1800.display_max_speed * calibration_speed, 2);  //best average over 30 min
        display.setFont(&SansSerif_bold_46_nr);                         //Test for bigger alfa fonts
        display.print(S1800.avg_s * calibration_speed, 2);              //actual average last 30 min
      } else {
        display.setFont(&FreeSansBold12pt7b);
        display.print(".5hA");
        display.setFont(&FreeSansBold18pt7b);
        display.print(S1800.avg_s * calibration_speed, 1);  //actual average last 30 min
        display.setFont(&FreeSansBold12pt7b);
        display.setCursor(offset + 124, 24);
        display.print(".5hB");
        display.setFont(&FreeSansBold18pt7b);
        display.print(S1800.display_max_speed * calibration_speed, 1);  //best average over 30 min
      }
    }
    if (field == 8) {
      if (config.speed_large_font == 1) {
        bar_position = 40;                     //test voor bigger font, was 42
        display.setFont(&FreeSansBold12pt7b);  // !!!!
        display.setCursor(offset, 36);
        display.print("1h:");
        display.print(S3600.display_max_speed * calibration_speed, 2);  //best average over 30 min
        display.setFont(&SansSerif_bold_46_nr);                         //Test for bigger alfa fonts
        display.print(S3600.avg_s * calibration_speed, 2);              //actual average last 30 min
      } else {
        display.setFont(&FreeSansBold12pt7b);
        display.print("1hA ");
        display.setFont(&FreeSansBold18pt7b);
        display.print(S3600.avg_s * calibration_speed, 1);  //actual average last hour
        display.setFont(&FreeSansBold12pt7b);
        display.setCursor(offset + 124, 24);
        display.print("1hB ");
        display.setFont(&FreeSansBold18pt7b);
        display.print(S3600.display_max_speed * calibration_speed, 1);  //best 3600s
      }
    }
    int log_seconds = (millis() - start_logging_millis) / 1000;  //aantal seconden sinds loggen is gestart
    static int low_speed_seconds;
    if (S10.avg_s > 2000) { low_speed_seconds = 0; }  //if the speed is higher then 2000 mm/s, reset the counter
    low_speed_seconds++;
    if (low_speed_seconds > 120) { start_logging_millis = millis(); }  //bar will be reset if the 10s speed drops under 2m/s for more then 120 s !!!!
    run_rectangle_length = (Ublox.alfa_distance / bar_length);         //240 pixels is volledige bar, ublox.alfa_distance zijn mm
    if (field == 7) {
      run_rectangle_length = log_seconds * 240 / 1800;
      if (log_seconds > 1800) { start_logging_millis = millis(); }
    }  //30 minutes = full bar
    if (field == 8) {
      run_rectangle_length = log_seconds * 240 / 3600;
      if (log_seconds > 3600) { start_logging_millis = millis(); }
    }  //60 minutes = full bar
    if (bar_length) {
      display.fillRect(offset, bar_position, run_rectangle_length, 8, GxEPD_BLACK);  //balk voor run_distance weer te geven...
    }
  }
  if (screen == AVG_10S) {
    update_delay = 1000;
    display.setFont(&FreeSansBold12pt7b);
    for (int i = 9; i > 4; i--) {
      display.setCursor(offset, 24 * (10 - i));
      display.print("Run ");
      display.print(10 - i);
      display.print(": ");
      display.print(S10.avg_speed[i] * calibration_speed, 2);
      display.print(" @");
      display.print(S10.time_hour[i]);
      display.print(":");
      display.print(S10.time_min[i]);
    }
  }
  if (screen == SPEED2) {
    update_delay = 100;
    if (GPS_Signal_OK == true) {
      display.setFont(&FreeSansBold75pt7b);
      // change color when 2s speed is in top5
      if (S2.s_max_speed * calibration_speed > S2.avg_speed[5] * calibration_speed) {
        display.fillScreen(GxEPD_BLACK);
        display.setTextColor(GxEPD_WHITE);
      } else {
        display.fillScreen(GxEPD_WHITE);
        display.setTextColor(GxEPD_BLACK);
      }
      if (gps_speed * calibration_speed < 10) {
        display.setCursor(40, 115);
      } else {
        display.setCursor(0, 115);
      }
      display.print(gps_speed * calibration_speed, 0);
      if (gps_speed * calibration_speed < 100) {
        display.setFont(&FreeSansBold30pt7b);
        display.print(".");
        display.println(int((gps_speed * calibration_speed - int(gps_speed * calibration_speed)) * 10), 0);  //int((x-int(x))*10)
        display.setFont(&FreeSansBold9pt7b);
        display.setCursor(160, 14);
        display.print("Run ");
        display.print(S10.s_max_speed * calibration_speed, 1);
        display.setCursor(160, 28);
        display.print("2S ");
        display.print(S2.display_max_speed * calibration_speed, 1);
      }
      Speed_in_Unit(offset);
      display.setTextColor(GxEPD_BLACK);
    } else {
      display.setFont(&FreeSansBold18pt7b);
      display.setCursor(offset, 60);
      display.print("Low GPS signal !");
      Sats_level(offset);
    }
  }
  if (screen == TROUBLE) {
    display.setFont(&FreeSansBold12pt7b);
    display.setCursor(offset, ROW_1_12PT);
    display.println("No GPS frames for");
    display.println("more then 10 s.... ");
    //display.setCursor(offset,120);
    //display.print(time_now);
  }
  if (screen == STATS1) {  //2s,10sF,10sS, AVG
    update_delay = (config.Stat_screens_time - 2) * 1000;
    display.setFont(&FreeSansBold12pt7b);
    display.setCursor(offset, ROW_1_18PT);
    display.print("2l: ");
    display.setFont(&FreeSansBold18pt7b);
    display.print(S2.display_last_run * calibration_speed, 1);  //last 2s max from rundisplay.print(S2.avg_speed[9]*calibration_speed);
    display.setFont(&FreeSansBold12pt7b);
    display.setCursor(120 + offset % 2, ROW_1_18PT);  //zodat SXX niet groter wordt dan 244 pix
    display.print("2s: ");
    display.setFont(&FreeSansBold18pt7b);
    display.print(S2.display_max_speed * calibration_speed);  //best 2s, was avg_speed[9]
    //display.setFont(&FreeSansBold12pt7b);
    display.setCursor(offset, ROW_2_18PT);
    display.print("10sF: ");
    display.println(S10.display_max_speed * calibration_speed);  //best 10s(Fast), was avg_speed[9]
    display.setCursor(offset, ROW_3_18PT);
    display.print("10sS: ");
    display.println(S10.display_speed[5] * calibration_speed);  //langzaamste 10s(Slow) run van de sessie
    display.setCursor(offset, ROW_4_18PT);
    display.print("Avg: ");
    display.println(S10.avg_5runs * calibration_speed);  //average 5*10s
    //Bat_level(offset+222,87);
  }
  if (screen == STATS2) {  //alfa 500m,1852m, 1800s,total_dist
    update_delay = (config.Stat_screens_time - 2) * 1000;
    static int toggle = 0;
    display.setFont(&FreeSansBold18pt7b);
    display.setCursor(offset, ROW_1_18PT);
    display.print("Dist: ");
    display.print(Ublox.total_distance / 1000, 0);
    //display.setFont(&FreeSansBold12pt7b);
    //int posX = 196+offset%2;
    //int satnum = ubxMessage.navPvt.numSV;
    //display.setCursor(posX,22);//zodat SXX niet groter wordt dan 244 pix
    //display.drawExampleBitmap((satnum<1?ESP_noSat_22:ESP_Sat_22), posX, 3, 22, 22, GxEPD_BLACK);
    //display.print("S");
    //display.setCursor(posX+23,22);
    //display.println(ubxMessage.navPvt.numSV);
    //display.setFont(&FreeSansBold18pt7b);
    display.setCursor(offset, ROW_2_18PT);
    display.print("1852m: ");
    display.println(M1852.display_max_speed * calibration_speed);  //was avg_speed[9]
    display.setCursor(offset, ROW_3_18PT);
    if (toggle == 0) {
      display.print("1800s: ");
      display.println(S1800.display_max_speed * calibration_speed);
      display.setCursor(offset, ROW_4_18PT);
      display.print("Alfa: ");
      display.print(A500.avg_speed[9] * calibration_speed);  //best Alfa on 500 m
      toggle = 1;
    } else {
      display.print("3600s: ");
      display.println(S3600.display_max_speed * calibration_speed);
      display.setCursor(offset, ROW_4_18PT);
      display.print(time_now_sec);
      toggle = 0;
    }
    //Bat_level(offset+222,87);
  }
  if (screen == STATS3) {  //100m,250m, 500m,Alfa
    update_delay = (config.Stat_screens_time - 2) * 1000;
    display.setFont(&FreeSansBold18pt7b);
    display.setCursor(offset, ROW_1_18PT);
    display.print("100m: ");
    display.print(M100.avg_speed[9] * calibration_speed);  //best 2s, was avg_speed[9]
    display.setCursor(offset, ROW_2_18PT);
    display.print("250m: ");
    display.println(M250.display_max_speed * calibration_speed);  //best 10s(Fast), was avg_speed[9]
    display.setCursor(offset, ROW_3_18PT);
    display.print("500m: ");
    display.println(M500.avg_speed[9] * calibration_speed);  //langzaamste 10s(Slow) run van de sessie
    display.setCursor(offset, ROW_4_18PT);
    display.print("Alfa: ");
    display.println(A500.avg_speed[9] * calibration_speed, 2);  //average 5*10s
    //Bat_level(offset+222,87);
  }
  if (screen == STATS4) {  //10s,AVG,5 runs, update on the fly !!!
    update_delay = (config.Stat_screens_time - 2) * 1000;
    display.setFont(&FreeSansBold12pt7b);
    display.setCursor(offset, ROW_1_18PT);
    display.print("10s Avg: ");
    display.setFont(&FreeSansBold18pt7b);
    display.println(s10.avg_5runs * calibration_speed, 2);  //eerste regel, avg_5runs krijgt update tijdens run !!
    for (int i = 9; i > 6; i--) {
      display.setCursor(offset, ROW_2_18PT + (9 - i) * ROW_18PT);
      display.setFont(&FreeSansBold12pt7b);
      display.print("R");
      display.print(10 - i);
      display.print(" ");
      display.setFont(&FreeSansBold18pt7b);
      display.print(s10.display_speed[i] * calibration_speed, 1);
      display.setCursor(offset + 118, ROW_2_18PT + (9 - i) * ROW_18PT);
      if (i > 7) {
        display.setFont(&FreeSansBold12pt7b);
        display.print(" R");
        display.print(13 - i);
        display.print(" ");
        display.setFont(&FreeSansBold18pt7b);
        display.print(s10.display_speed[i - 3] * calibration_speed, 1);
      } else {
        display.print(time_now);
      }
    }
  }
  if (screen == STATS5) {  //alfa statistics
    update_delay = (config.Stat_screens_time - 2) * 1000;
    display.setFont(&FreeSansBold12pt7b);
    display.setCursor(offset, ROW_1_18PT);
    display.print("Last Alfa stats ! ");
    display.setFont(&FreeSansBold18pt7b);
    for (int i = 9; i > 6; i--) {
      display.setCursor(offset, ROW_2_18PT + (9 - i) * ROW_18PT);
      display.setFont(&FreeSansBold12pt7b);
      display.print("A");
      display.print(10 - i);
      display.print(" ");
      display.setFont(&FreeSansBold18pt7b);
      display.print(a500.avg_speed[i] * calibration_speed, 1);
      display.setCursor(offset + 118, ROW_2_18PT + (9 - i) * ROW_18PT);
      if (i > 7) {
        display.setFont(&FreeSansBold12pt7b);
        display.print(" A");
        display.print(13 - i);
        display.print(" ");
        display.setFont(&FreeSansBold18pt7b);
        display.print(a500.avg_speed[i - 3] * calibration_speed, 1);
      }
      /*else{
            display.print(time_now);
          }*/
    }
  }
  if (screen == STATS6) {  //Simon stat screen
    Serial.println("STATS6_Simon_screen");
    update_delay = (config.Stat_screens_time - 2) * 1500;
    double S10avgNEW;
    if (S10.s_max_speed > S10.avg_speed[5]) S10avgNEW = (S10.avg_speed[9] + S10.avg_speed[8] + S10.avg_speed[7] + S10.avg_speed[6] + S10.s_max_speed) / 5;
    else S10avgNEW = (S10.avg_speed[9] + S10.avg_speed[8] + S10.avg_speed[7] + S10.avg_speed[6] + S10.avg_speed[5]) / 5;
    int row1 = 15;
    int row = 17;
    int row2 = row1 + row;
    int row3 = row2 + row;
    int row4 = row3 + row;
    int row5 = row4 + row;
    int row6 = row5 + row;
    int col1 = 0 + offset;
    int col2 = 46 + offset;
    int col3 = 114 + offset;
    int col4 = 182 + offset;
    int line = 0;

    //display.setCursor(displayWidth-10,INFO_BAR_TOP);
    //display.setFont(&FreeSansBold6pt7b);
    //display.print(time_now);
    display.setFont(&FreeMonoBold12pt7b);
    display.setCursor(col1, row1);
    display.print("AV:");
    display.setCursor(col1, row2);
    display.print("R1:");
    display.setCursor(col1, row3);
    display.print("R2:");
    display.setCursor(col1, row4);
    display.print("R3:");
    display.setCursor(col1, row5);
    display.print("R4:");
    display.setCursor(col1, row6);
    display.print("R5:");

    display.setFont(&FreeSansBold12pt7b);
    display.setCursor(col2, row1);

    display.println(S10avgNEW * calibration_speed, 2);  //Laat het gemiddelde incl de previous 10s zien mits in top5
    display.setCursor(col2, row2);
    display.println(S10.avg_speed[9] * calibration_speed, 2);
    display.setCursor(col2, row3);
    display.println(S10.avg_speed[8] * calibration_speed, 2);
    display.setCursor(col2, row4);
    display.println(S10.avg_speed[7] * calibration_speed, 2);
    display.setCursor(col2, row5);
    display.println(S10.avg_speed[6] * calibration_speed, 2);
    display.setCursor(col2, row6);
    display.println(S10.avg_speed[5] * calibration_speed, 2);
    // right column
    display.setFont(&FreeMonoBold12pt7b);
    display.setCursor(col3, row1);
    display.print("2sec:");
    display.setCursor(col3, row2);
    display.print("Prv :");
    display.setCursor(col3, row3);
    display.print("Dist:");
    display.setCursor(col3, row4);
    display.print("Alp :");
    display.setCursor(col3, row5);
    display.print("500m:");
    display.setCursor(col3, row6);
    display.print("NM:");

    display.setFont(&FreeSansBold12pt7b);
    display.setCursor(col4, row1);
    display.println(S2.avg_speed[9] * calibration_speed, 2);
    display.setCursor(col4, row2);
    display.println(S10.s_max_speed * calibration_speed, 2);
    display.setCursor(col4, row3);
    display.println(Ublox.total_distance / 1000000);
    display.setCursor(col4, row4);
    display.println(A500.avg_speed[9] * calibration_speed, 2);
    display.setCursor(col4, row5);
    display.println(M500.display_max_speed * calibration_speed, 2);
    display.setCursor(col4, row6);
    display.println(M1852.display_max_speed * calibration_speed, 2);

    // position line of actual 10s run
    if (S10.s_max_speed * calibration_speed > S10.avg_speed[9] * calibration_speed) line = row1;
    else if (S10.s_max_speed * calibration_speed > S10.avg_speed[8] * calibration_speed) line = row2;
    else if (S10.s_max_speed * calibration_speed > S10.avg_speed[7] * calibration_speed) line = row3;
    else if (S10.s_max_speed * calibration_speed > S10.avg_speed[6] * calibration_speed) line = row4;
    else if (S10.s_max_speed * calibration_speed > S10.avg_speed[5] * calibration_speed) line = row5;
    else line = row6;
    display.fillRect(0, line + 2, col3 - 10, 2, GxEPD_BLACK);  //lijn voor actuele run weer te geven...
  }
  if (screen == STATS7) {  //Simon bar graph screen
    Serial.println("STATS7_Simon_bar graph");
    update_delay = (config.Stat_screens_time - 2) * 1500;
    //int DisplayWidth=255;
    //int DisplayHeight=122;
    int posX = 5;
    int posY = INFO_BAR_TOP;
    int MaxNumberBar = NR_OF_BAR;  //is 42, zie GPS_data.h
    //int barLengthMax=70;
    int GraphWidth = 215;  //was 220
    int barWidth = 3;
    int barSpace = 2;
    int barPitch = barWidth + barSpace;
    static int r = 0;
    int top_speed = S10.display_speed[9] * calibration_speed;
    int max_bar = int(top_speed / 5 + 1) * 5;
    if (max_bar < 24) max_bar = 24;
    int step = 3;
    if (max_bar > 45) step = 5;
    int min_bar = (max_bar - step * 8);
    float scale = 80 / (max_bar - min_bar);

    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(0, 15);
    display.println("Graph : Speed runs (10sec)");  //printen top tekst
    r = run_count % MaxNumberBar + 1;               //laatste bar = 0 ?
    display.setFont(&FreeSansBold6pt7b);
    for (int i = 0; i < 9; i++) {
      display.fillRect(offset + posX, posY - (i * 10), 215, 1, GxEPD_BLACK);  //printen hor.lijnen grafiek, van 5 tot 215
      display.setCursor(225 + offset, posY - (i * 10));                       //positie y-as legende
      display.print(min_bar + i * step);                                      //Printen y-as legende speed, van min_bar tot max_bar in 8 steps...
    }

    display.setCursor(0, 30);
    display.print("R1-R5:");
    display.print(S10.display_speed[9] * calibration_speed);
    display.print(" ");
    display.print(S10.display_speed[8] * calibration_speed);
    display.print(" ");
    display.print(S10.display_speed[7] * calibration_speed);
    display.print(" ");
    display.print(S10.display_speed[6] * calibration_speed);
    display.print(" ");
    display.println(S10.display_speed[5] * calibration_speed);

    if (run_count < MaxNumberBar) {
      barWidth = max((GraphWidth - (r * barSpace)) / (r), 3);  //(was r+1),3), laatste bar altijd 0;
      barPitch = (barWidth) + barSpace;
      for (int i = 0; i < r; i++) {
        int barHeight = (S10.speed_run[i] * calibration_speed - min_bar) * scale;
        display.fillRect(offset + posX + (i * barPitch), posY - barHeight, barWidth, barHeight, GxEPD_BLACK);
      }
    } else {
      barWidth = max((GraphWidth - (MaxNumberBar * barSpace)) / MaxNumberBar, 3);
      barPitch = (barWidth) + barSpace;
      for (int i = 0; i < MaxNumberBar; i++) {
        int barHeight = (S10.speed_run[(i + r) % NR_OF_BAR] * calibration_speed - min_bar) * scale;  //was S10.speed_run[i+r%NR_OF_BAR-42]
        display.fillRect(offset + posX + (i * barPitch), posY - barHeight, barWidth, barHeight, GxEPD_BLACK);
      }
    }
#if defined(STATIC_DEBUG)
    Serial.print("Run: ");
    Serial.println(r);
    Serial.print("barWidth: ");
    Serial.println(barWidth);
#endif
  }
  if (screen > BOOT_SCREEN && screen < SPEED) {
    display.setFont(&FreeSansBold6pt7b);
    display.setCursor(displayWidth - 10, INFO_BAR_TOP);
    display.print(screen);
  }
  if (count % 200 == 0) {  //was 200
    if (update_epaper > 0) display.update();
    offset = 0;
  } else {
    if (update_epaper == 2) { display.fillScreen(GxEPD_WHITE); }                           //test
    if (update_epaper > 0) display.updateWindow(0, 0, displayWidth, displayHeight, true);  //was 244,122, true !!!
    delay(update_delay);                                                                   //update delay function of screen to show
  }
  old_screen = screen;
  count++;
}

#undef ROW_1_9PT
#undef TOP_LEFT_INFO
#undef ROW_3_9PT
#undef ROW_9pt_2
