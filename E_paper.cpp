#include "E_paper.h"
#include "Definitions.h"
#include <LITTLEFS.h>

// row height 14pt spacing 2pt
#define ROW_SPACING 2

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
#ifndef T5_E_PAPER
void Boot_screen(void){};
void Sleep_screen(int choice){};
void Update_screen(int screen){};
#else
char time_now[8];
char time_now_sec[12];
int16_t displayHeight;
int16_t displayWidth;
int bar_length = 1852;
int bar_position = 32;
int total_bar_length = 240;
int run_rectangle_length = 0;
void InfoBar(int offset);
void InfoBarRtc(int offset);
void sdCardInfo(void);
const char* gpsChip(int longname);
char bar_info[8] = "info";
void Speed_font0(String message1, String message2, float speed1, float speed2, float speed, int screen) {
  int decimal = 1;
  if (screen == 2) decimal = 0;          //screen==1 : Run xx.x AVG xx.x
  display.setFont(&FreeSansBold12pt7b);  //screen==2 : Gate xx Exit xx
  display.setCursor(offset, 24);         //screen==3 : Alfa MISS alfa xx.x
  display.print(message1);
  display.setFont(&FreeSansBold18pt7b);
  if (screen <= 2) {                 //Run xx.x Avg xx.x
    display.print(speed1, decimal);  //last 10s max from run
  }
  display.setFont(&FreeSansBold12pt7b);
  display.setCursor(offset + 122, 24);
  display.print(message2);
  display.setFont(&FreeSansBold18pt7b);
  display.print(speed2, decimal);
  display.setFont(&SansSerif_bold_96_nr);
  display.setCursor(offset, 120);
  display.println(speed, 1);
}
void Speed_font1(String message1, String message2, float speed1, float speed2, float speed, int screen) {
  display.setCursor(offset, 36);
  if (screen == 0) {                         //Run "A" AVG
    display.setFont(&SansSerif_bold_46_nr);  //Test for bigger alfa fonts
    display.print(speed1, 1);                //last 10s max from run
    display.setFont(&FreeSansBold12pt7b);
    display.setCursor(offset + 113, 36);
    display.print(message2);
    display.setFont(&SansSerif_bold_46_nr);
    display.print(speed2, 1);
  } else if (screen == 1) {  //Alfa screen, Gate xx Ex xx
    display.setFont(&FreeSansBold12pt7b);
    display.print(message1);
    display.setFont(&SansSerif_bold_46_nr);
    display.print(speed1, 0);
    //display.setCursor(offset + 110, 36);
    display.setFont(&FreeSansBold12pt7b);
    display.print(message2);
    display.setFont(&SansSerif_bold_46_nr);
    display.print(speed2, 0);
  } else if (screen == 2) {  //Alfa= xx.xx
    display.setFont(&FreeSansBold18pt7b);
    display.print(message1);
    display.setFont(&SansSerif_bold_46_nr);
    display.print(speed1, 2);
  } else if (screen == 3) {  //Alfa = MISS
    display.setFont(&FreeSansBold18pt7b);
    display.print(message1);
  }
  display.setFont(&SansSerif_bold_96_nr);
  display.setCursor(offset, 120);
  display.println(speed, 1);
}
void Speed_font3(String message1, float speed) {
  display.setFont(&FreeSansBold24pt7b);
  display.setCursor(offset, 36);
  bar_position = 40;
  display.print(message1);
  display.setCursor(offset, 120);
  display.setFont(&SansSerif_bold_96_nr);
  display.print(speed, 1);
}
int device_boot_log(int rows, int ws) {
  int r = 2, row = ROW_9PT + ROW_SPACING;
  display.setCursor(offset, ROW_2_9PT);
  if (ws) delay(ws);
  display.print(E_paper_version);
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

void Boot_screen(void) {
  display.init();
  display.setRotation(1);
  displayHeight = display.height();
  displayWidth = display.width();
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.drawExampleBitmap(ESP_GPS_logo_40, offset + 198, 6, 40, 40, GxEPD_BLACK);
  InfoBarRtc(offset);
  display.setFont(&FreeSansBold9pt7b);
  display.setCursor(offset, 14);
  if (RTC_voltage_bat < RTC_minimum_voltage_bat) {
    //int cursor = ROW_2_9PT;
    display.println("EPS-GPS sleeping");
    display.print("Go back to sleep...");
    display.setFont(&FreeSansBold12pt7b);
    display.setCursor(offset, 60);
    display.printf("Voltage to low: %.2f", RTC_voltage_bat);
    display.setCursor(offset, 80);
    display.println("Please charge lipo!");
    display.setCursor(offset, 100);
    display.print(RTC_Sleep_txt);
    display.update();
  } else {
    display.println("ESP-GPS booting");
    display.print(E_paper_version);
    display.println(SW_version);
    sdCardInfo();
    display.setCursor(offset, 102);
    display.printf("Logspace left : %d hour", Logtime_left(Free_space()) / 60);
    display.updateWindow(0, 0, displayWidth, displayWidth, true);
    delay(100);
    display.update();
  }
}
void Off_screen(int choice) {  //choice 0 = old screen, otherwise Simon screens
  //int offset=0;
  float session_time = (millis() - start_logging_millis) / 1000;
  display.setRotation(1);
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  int cursor = ROW_3_9PT + ROW_12PT_W_SPACING;
  ESP_GPS_LOGO_40
  TOP_LEFT_TITLE_MSG("ESP-GPS saving");  //row1 14
  DEVICE_BOOT_LOG(4);
  display.setFont(&FreeSansBold12pt7b);
  display.setCursor(offset, cursor);
  if (choice == 0) {
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
  if (choice == 1) {
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
  if (choice == 2) {  //shutdown due low bat
    display.println("Shutdown LOW Bat");
    display.setFont(&FreeSansBold9pt7b);
    display.print("Bat = ");
    display.print(RTC_voltage_bat);
    display.println(" V");
  }
  InfoBar(offset);
  //display.update();
  display.updateWindow(0, 0, displayWidth, displayHeight, true);
  //delay(3000);  //om te voorkomen dat update opnieuw start !!!
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
    if (RTC_Board_Logo == 16)
      display.drawExampleBitmap(OneHundred_logoS_zwart, 195, 0, 48, 48, GxEPD_BLACK);
    if (RTC_Board_Logo == 17)
      display.drawExampleBitmap(FMX_logoS_zwart, 195, 0, 48, 48, GxEPD_BLACK);
    if (RTC_Board_Logo == 18)
      display.drawExampleBitmap(Phantom_logoS_zwart, 195, 0, 48, 48, GxEPD_BLACK);
    if (RTC_Board_Logo == 19)
      display.drawExampleBitmap(f4_foils_logoS_zwart, 195, 0, 48, 48, GxEPD_BLACK);
    if (RTC_Board_Logo == 20)
      display.drawExampleBitmap(Logo_LISA, 195, 0, 48, 48, GxEPD_BLACK);
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
    if (RTC_Sail_Logo == 10) {
      display.drawExampleBitmap(Severne_logoS_zwart, 195, 50, 48, 48, GxEPD_BLACK);
    }
    if (RTC_Sail_Logo == 11) {
      display.drawExampleBitmap(S2maui_logoS_zwart, 195, 50, 48, 48, GxEPD_BLACK);
    }
    if (RTC_Sail_Logo == 12) {
      display.drawExampleBitmap(North_Sails_logoS_zwart, 195, 50, 48, 48, GxEPD_BLACK);
    }
    if (RTC_Sail_Logo == 13) {
      display.drawExampleBitmap(Challenger_Sails_logoS_zwart, 195, 50, 48, 48, GxEPD_BLACK);
    }
    if (RTC_Sail_Logo == 14) {
      display.drawExampleBitmap(Phantom_logoS_zwart, 195, 50, 48, 30, GxEPD_BLACK);  //Patrik_logoS_zwart
    }
    if (RTC_Sail_Logo == 15) {
      display.drawExampleBitmap(Patrik_logoS_zwart, 195, 50, 48, 48, GxEPD_BLACK);  //Patrik_logoS_zwart
    }
    if (RTC_Sail_Logo == 16) {
      display.drawExampleBitmap(Logo_LISA_vertical, 195, 50, 48, 48, GxEPD_BLACK);  //Patrik_logoS_zwart
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
  int posX = displayWidth - batW - 6;  //was -10
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
  if (bat_perc < 100) display.setCursor(offset + 156, (INFO_BAR_ROW));  //was 193
  else display.setCursor(offset + 146, (INFO_BAR_ROW));                 //was 184
  display.print(RTC_voltage_bat + 0.04, 1);
  display.print("V ");
  display.print(int(bat_perc));
  display.print("%");
}
void Sats_level(int offset) {
  if (!ubxMessage.monVER.swVersion[0]) return;
  // int circelL = 5;
  //int circelS = 2;
  int posX = 120 + offset;  //was 176
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
    case M8_115200BD:
      return longname ? "M8 115.2Kbd" : "M8";
      break;
    case M9_9600BD:
      return longname ? "M9 9.6Kbd" : "M9";
      break;
    case M9_38400BD:
      return longname ? "M9 38.4Kbd" : "M9";
      break;
    case M9_115200BD:
      return longname ? "M9 115.2Kbd" : "M9";
      break;
    case M10_9600BD:
      return longname ? "M10 9.6Kbd" : "M10";
      break;
    case M10_38400BD:
      return longname ? "M10 38.4Kbd" : "M10";
      break;
    case M10_115200BD:
      return longname ? "M10 115.2Kbd" : "M10";
      break;
    default:
      return "unknown";
      break;
  }
}
void M8_M10(int offset) {
  display.setFont(&FreeSansBold9pt7b);
  display.setCursor(offset + 60, INFO_BAR_ROW);
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
  if (ubxMessage.navPvt.numSV > 4) M8_M10(offset);
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
  // display.setCursor(offset,102);
  // display.printf("Logspace left : %d hour",Logtime_left(Free_space())/60);
}
void Stats_4lines(String Message1, String Message2, String Message3, String Message4, float Value1, float Value2, float Value3, float Value4) {
#define STAT4_ROW2 150
#define DIS_WIDTH 240
  display.setCursor(offset, ROW_1_18PT);
  display.setFont(&FreeSansBold18pt7b);
  display.print(Message1);
  display.setCursor(STAT4_ROW2, ROW_1_18PT);
  // display.setFont(&FreeSansBold18pt7b);
  if (Value1 < 100.0) display.println(Value1, 2);
  else if (Value1 < 1000.0) display.println(Value1, 1);
  else display.println(Value1, 0);
  display.setCursor(offset, ROW_2_18PT);
  // display.setFont(&FreeSansBold12pt7b);
  display.print(Message2);
  display.setCursor(STAT4_ROW2, ROW_2_18PT);
  // display.setFont(&FreeSansBold18pt7b);
  display.println(Value2, 2);
  display.setCursor(offset, ROW_3_18PT);
  //display.setFont(&FreeSansBold12pt7b);
  display.print(Message3);
  display.setCursor(STAT4_ROW2, ROW_3_18PT);
  // display.setFont(&FreeSansBold18pt7b);
  display.println(Value3, 2);
  display.setCursor(offset, ROW_4_18PT);
  //display.setFont(&FreeSansBold12pt7b);
  display.print(Message4);
  display.setCursor(STAT4_ROW2, ROW_4_18PT);
  // if(Value4<0) display.print("-");
  //display.setFont(&FreeSansBold18pt7b);
  //if(Value4>=0) display.println(Value4,2);
  display.println(Value4, 2);
}
void Stats_2s_3_lines(String Message1, String Message2, String Message3, float Value1, float Value2, float Value3) {
  display.setFont(&FreeSansBold12pt7b);
  display.setCursor(offset, ROW_1_18PT);
  display.print("2l: ");
  display.setFont(&FreeSansBold18pt7b);
  display.print(S2.display_last_run * calibration_speed, 1);  //last 2s max from rundisplay.print(S2.avg_speed[9]*calibration_speed);
  display.setFont(&FreeSansBold12pt7b);
  display.setCursor(120 + offset % 2, ROW_1_18PT);  //zodat SXX niet groter wordt dan 244 pix
  display.print("2s: ");
  display.setFont(&FreeSansBold18pt7b);
  display.print(S2.display_speed[9] * calibration_speed);  //best 2s, was avg_speed[9]
  display.setCursor(offset, ROW_2_18PT);
  display.print(Message1);
  display.println(Value1);  //best 10s(Fast), was avg_speed[9]
  display.setCursor(offset, ROW_3_18PT);
  display.print(Message2);
  display.println(Value2);  //langzaamste 10s(Slow) run van de sessie
  display.setCursor(offset, ROW_4_18PT);
  display.print(Message3);
  display.println(Value3);  //average 5*10s
}
void Update_screen(int screen) {
  static int count, old_screen, update_delay;
  update_time();
  update_epaper = 1;                    //was zonder else
  if ((count / 4) % 20 < 10) offset++;  //elke 4 screen updates shift offset
  else offset--;
  if (offset > 10) offset = 0;
  if (offset < 0) offset = 0;
  int cursor = 0;
  display.fillScreen(GxEPD_WHITE);
  if ((screen != SPEED) & (screen != STATS9) & (screen != STATS8) & (screen != STATSA) & (screen != STATSD)) InfoBar(offset);
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

    if (config.ublox_type == 0xFF) {
      display.setFont(&FreeSansBold12pt7b);
      display.setCursor(offset, (cursor = ROW_4_9PT + ROW_12PT_W_SPACING));
      display.print("Auto detect gps-type");
    } else if (!ubxMessage.monVER.hwVersion[0]) {
      display.setFont(&FreeSansBold12pt7b);
      display.setCursor(offset, (cursor = ROW_3_9PT + ROW_12PT_W_SPACING));
      display.println("Gps initializing");
      if (config.M10_high_nav == M10_HIGH_NAV_RATE) display.println("M10 high nav mode !");
      if (config.M10_high_nav == M10_DEFAULT_NAV) display.println("M10 default nav mode");
    }
    if (screen != old_screen) count = 0;  //eerste keer full update
    Speed_in_Unit(offset);
  }
  if (screen == WIFI_ON) {
    update_delay = 100;  //was 1000
    if (count % 20 < 10) offset++;
    else offset--;
    ESP_GPS_LOGO_40
    TOP_LEFT_TITLE_MSG("ESP-GPS connect");
    DEVICE_BOOT_LOG(2);
    if (SoftAP_connection != true) {
      display.setCursor(offset, 102);
      display.printf("Logspace left : %d hour", Logtime_left(Free_space()) / 60);
    }
    if (Wifi_on == 1) {
      display.setFont(&FreeSansBold12pt7b);
      display.setCursor(offset, (cursor = ROW_3_9PT + ROW_12PT_W_SPACING));
      //display.fillRect(0, ROW_3_9PT+1, 180, INFO_BAR_TOP-ROW_3_9PT-1, GxEPD_WHITE);//clear lower part
      display.print("Ssid: ");
      if (SoftAP_connection == true) display.print("ESP32AP");  //ap mode
      else display.print(actual_ssid);                          //display.print(config.ssid);
      display.setFont(&FreeSansBold9pt7b);
      display.setCursor(offset, (cursor += ROW_9PT_W_SPACING));
      if (SoftAP_connection == true) {
        display.print("Password: password");
        display.setCursor(offset, (cursor += ROW_9PT_W_SPACING));
      }
      display.printf("http://%s", IP_adress.c_str());

    } else {
      display.fillRect(0, 0, 250, 122, GxEPD_WHITE);
      ESP_GPS_LOGO_40
      TOP_LEFT_TITLE_MSG("ESP-GPS ready");
      DEVICE_BOOT_LOG(24);
      display.setFont(&FreeSansBold12pt7b);
      display.setCursor(offset, (cursor = ROW_4_9PT + ROW_12PT_W_SPACING));
      if (ubxMessage.navPvt.numSV < 5) {
        display.println("Waiting for Sat >=5");
        display.setFont(&FreeSansBold9pt7b);
        display.setCursor(offset, 102);
        display.println("Please go outside...       ");
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
      InfoBar(offset);
    }
  }
  if (screen == WIFI_STATION) {
    update_delay = 100;
    ESP_GPS_LOGO_40
    TOP_LEFT_TITLE_MSG("ESP-GPS try to connect");
    DEVICE_BOOT_LOG(2);
    display.setCursor(offset, 102);
    display.printf("Logspace left : %d hour", Logtime_left(Free_space()) / 60);
    display.setFont(&FreeSansBold12pt7b);
    display.setCursor(offset, (cursor = ROW_3_9PT + ROW_12PT_W_SPACING));
    //display.println("Trying to connect...");
    display.print(actual_ssid);
    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(offset, (cursor += ROW_9PT_W_SPACING));
    display.printf("For AP: use magnet in %ds", wifi_search);
    if (screen != old_screen) count = 0;  //eerste keer full update
  }
  if (screen == WIFI_SOFT_AP) {
    update_delay = 100;  //was 500
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
    update_delay = 50;
    int field = config.field_actual;  //default is in config.txt
    bool alfa_screen = false;
    bool nautical_mile_screen = false;
    bool x_10km_screen = false;
    if ((Ublox.alfa_distance / 1000 < 350) & (abs(alfa_window) < 100)) alfa_screen = true;                              //true until 350 m after the jibe
    if (Ublox.alfa_distance / 1000 > 1852) nautical_mile_screen = true;                                                 // true if run exceeds 1852 m
    if (((int)(Ublox.total_distance / 1000000) % 10 == 0) & (Ublox.alfa_distance / 1000 > 1000)) x_10km_screen = true;  //true if distance is x *10 km
    display.setFont(&FreeSansBold6pt7b);
    display.setCursor(displayWidth - 20, INFO_BAR_TOP);
    char c = config.field_actual;
    display.print(c);  //show config field in small font
    if (config.field_actual == SPEED1) {
      field = SPEED2;  //Default actual RUN + AVG screen
      if (nautical_mile_screen) field = SPEED4;
      if (x_10km_screen) field = SPEED5;
      if (alfa_screen) field = SPEED3;
    }
    if (config.field_actual == SPEED2) {
      field = SPEED2;
      if (nautical_mile_screen) field = SPEED4;
    }
    if (config.field_actual == SPEED7) {
      field = SPEED7;
      if (alfa_screen) field = SPEED3;
    }
    if (config.field_actual == SPEED8) {
      field = SPEED8;
      if (alfa_screen) field = SPEED3;
    }
    if (config.field_actual == SPEED9) {                           //1 hour default, but prio alfa, and if good run, last run
      field = SPEED2;                                              //default Run / Avg
      if (nautical_mile_screen) field = SPEED4;                    //after 1852 m
      if (Ublox.alfa_distance / 1000 < 1000) field = SPEED8;       // 350m - 1000m : 1h !!
      if (S10.s_max_speed > S10.display_speed[5]) field = SPEED2;  //if run faster then slowest run, show AVG & run after 1000 m
      if (alfa_screen) field = SPEED3;                             //prio alfa
    }

    if (GPS_Signal_OK == true) {
      //test for bigger font alfa
      if (config.speed_large_font == 2) {  //test for bigger font speed (Simon)
        //int gps_speed_int=(int)(gps_speed*calibration_speed);
        int gps_speed_komma = (int)((gps_speed * calibration_speed) * 10) % 10;
        display.setFont(&FreeSansBold75pt7b);
        display.setCursor(offset - 6, 115);
        display.print((int)(gps_speed * calibration_speed));  //print main in large font, float with rounding ???
        display.setFont(&FreeSansBold30pt7b);
        display.print(".");
        display.setFont(&SansSerif_bold_84_nr);
        display.println(gps_speed_komma);
        //display.println(int((gps_speed * calibration_speed - int(gps_speed * calibration_speed)) * 10), 0);  //int((x-int(x))*10) round to correct digit
      }
    } else {
      display.setFont(&FreeSansBold18pt7b);
      display.setCursor(offset, 60);
      display.print("Low GPS signal !");
    }
    if (field <= SPEED2) {  //Run and AVG first line speed screen
      if (config.speed_large_font == 0) {
        Speed_font0("Run", "Avg ", S10.display_last_run * calibration_speed, S10.avg_5runs * calibration_speed, gps_speed * calibration_speed, 0);
      }
      if (config.speed_large_font == 1) {
        Speed_font1("", "A", S10.display_last_run * calibration_speed, S10.avg_5runs * calibration_speed, gps_speed * calibration_speed, 0);
      }
      if (config.speed_large_font == 3) {
        if (S10.s_max_speed < S10.display_speed[5]) {
          Speed_font3("   SPEED", gps_speed * calibration_speed);
        } else {
          Speed_font3("LAST RUN", S10.s_max_speed * calibration_speed);
        }
      }
    }
    /*
      First 250m after jibe, if Window>99 m : Window and Exit
      Between 250m and 400m after jibe : Result Alfa (speed or MISS)
      Between 400m and 1852m after jibe : Actual Run + AVG
      More then 1852m : NM actual speed and NM Best speed
      */
    if (field == SPEED3) {
      if (config.speed_large_font == 0) {
        if ((abs(alfa_window) < 99) & (Ublox.alfa_distance / 1000 < 255)) {  //Window alleen indien Window<100 en Run>350 meter !!!!&(A500.alfa_speed_max*calibration_speed<1)
          if (alfa_exit > 99) alfa_exit = 99;                                //begrenzen alfa_exit...
          Speed_font0("Gate", " Ex ", alfa_window, alfa_exit, gps_speed * calibration_speed, 2);
        } else {
          if (A500.alfa_speed_max * calibration_speed > 1) {  //laatste alfa was geldig !!!!
            Speed_font0("Alfa ", "Ab ", A500.display_max_speed * calibration_speed, A500.alfa_speed_max * calibration_speed, gps_speed * calibration_speed, 1);
          } else {
            Speed_font0("Alfa MISS", "Ab ", 0, A500.alfa_speed_max * calibration_speed, gps_speed * calibration_speed, 3);
          }
        }
      }
      if (config.speed_large_font == 1) {
        if ((abs(alfa_window) < 99) & (Ublox.alfa_distance / 1000 < 255)) {  //Window alleen indien Window<100 en Run>350 meter !!!!&(A500.alfa_speed_max*calibration_speed<1)
          if (alfa_exit > 99) alfa_exit = 99;                                //begrenzen alfa_exit...
          Speed_font1("Gate", "Ex", alfa_window, alfa_exit, gps_speed * calibration_speed, 1);
        } else {
          if (A500.alfa_speed_max * calibration_speed > 1) {  //laatste alfa was geldig !!!!
            Speed_font1("Alfa= ", "", A500.alfa_speed_max * calibration_speed, 0, gps_speed * calibration_speed, 2);
          } else {
            Speed_font1("Alfa = MISS", "", 0, 0, gps_speed * calibration_speed, 3);
          }
        }
      }
      if (config.speed_large_font == 3) {
        if ((abs(alfa_window) < 99) & (Ublox.alfa_distance / 1000 < 255)) {  //Window alleen indien Window<100 en Run>350 meter !!!!&(A500.alfa_speed_max*calibration_speed<1)
          Speed_font3("   GATE", alfa_window);
        } else {
          if (A500.alfa_speed_max * calibration_speed > 1) {  //laatste alfa was geldig !!!!
            Speed_font3("   Alfa", A500.alfa_speed_max * calibration_speed);
          } else {
            Speed_font3("Alfa MISS", -1);
          }
        }
      }
    }
    if (field == SPEED4) {  //First line speed screen = nautical mile info
      if (config.speed_large_font == 0) {
        Speed_font0("NMa ", " NM ", M1852.display_speed[9] * calibration_speed, M1852.m_max_speed * calibration_speed, gps_speed * calibration_speed, 0);
      }
      if (config.speed_large_font == 1) {
        Speed_font1("NM= ", " ", M1852.m_max_speed * calibration_speed, 0, gps_speed * calibration_speed, 2);
      }
      if (config.speed_large_font == 3) {
        Speed_font3("  N. MILE", M1852.m_max_speed * calibration_speed);
      }
    }
    if (field == SPEED5) {
      if (config.speed_large_font == 1) {
        Speed_font1("Dist ", " ", Ublox.total_distance / 1000000, 0, gps_speed * calibration_speed, 2);  //distance in km xx.xx
      } else if (config.speed_large_font == 3) {
        Speed_font3("Dist ", Ublox.total_distance / 1000000);  //distance in km xx.xx
      } else if (config.speed_large_font == 0) {
        Speed_font0("Run", " Dist", Ublox.total_distance / 1000, Ublox.alfa_distance / 1000000, gps_speed * calibration_speed, 2);
      }
    }
    if (field == SPEED6) {
      if ((config.speed_large_font != 2) & (config.speed_large_font != 4)) {  //Simon font, alleen speed !)
        Speed_font0("2S ", "10S ", S2.display_max_speed * calibration_speed, S10.display_max_speed * calibration_speed, gps_speed * calibration_speed, 1);
      }
    }
    if (field == SPEED7) {
      if (config.speed_large_font == 1) {
        Speed_font1("500m ", " ", M500.m_max_speed * calibration_speed, M500.m_max_speed * calibration_speed, gps_speed * calibration_speed, 2);
      } else if (config.speed_large_font == 3) {
        Speed_font3("500m :", M500.m_max_speed * calibration_speed);
      } else if (config.speed_large_font == 0) {
        Speed_font0("500A", "Max", M500.m_max_speed * calibration_speed, M500.display_speed[9] * calibration_speed, gps_speed * calibration_speed, 1);
      }
    }
    if (config.speed_large_font == 4) {
      double speed = gps_speed * calibration_speed;
      display.setFont(&FreeSansBold75pt7b);
      display.setCursor(offset - 6, 118);
      display.print(speed, 0);
      display.setFont(&SansSerif_bold_84_nr);
      display.print(".");
      int komma = (int)(speed * 10) % 10;
      display.print(komma, 0);
    }
    int log_seconds;
    static int low_speed_seconds;
    static int start_hour_millis = millis();                        //only set @declaration
    log_seconds = (millis() - start_hour_millis) / 1000;            //aantal seconden sinds loggen is gestart
    if (S10.avg_s > 2000) { low_speed_seconds = 0; }                //if the speed is higher then 2000 mm/s, reset the counter
    low_speed_seconds++;                                            //update rate e-paper is about 1s....
    if (low_speed_seconds > 120) { start_hour_millis = millis(); }  //bar will be reset if the 10s speed drops under 2m/s for more then 120 s !!!!
    if (field == SPEED8) {
      if (log_seconds > 3600) { start_hour_millis = millis(); }  //reset bar
      if (config.speed_large_font == 1) {
        Speed_font1("1h: ", " ", S3600.display_max_speed * calibration_speed, S3600.avg_s * calibration_speed, gps_speed * calibration_speed, 2);
      } else if (config.speed_large_font == 3) {
        Speed_font3("1 Hour", S3600.display_max_speed * calibration_speed);
      } else if (config.speed_large_font == 0) {
        Speed_font0("1hA ", "1hB ", S3600.avg_s * calibration_speed, S3600.display_max_speed * calibration_speed, gps_speed * calibration_speed, 0);
      }
    }
    if (field == SPEEDA) {  //paco proposal : 2s actual run and 2s best of session
      if ((config.speed_large_font != 2) & (config.speed_large_font != 4)) {  //Simon font, alleen speed !)
        Speed_font0("CM ", "TM ", S2.display_last_run * calibration_speed, S2.display_max_speed * calibration_speed, gps_speed * calibration_speed, 1);
      }
    }
    /*progress bar**************************************************************************************************************************/
    if (config.speed_large_font == 0) {
      total_bar_length = 180;
      bar_position = 32;
    }
    if (config.speed_large_font == 1) {
      total_bar_length = 240;
      bar_position = 38;
    }
    if ((config.speed_large_font == 2) | (config.speed_large_font == 4)) {
      total_bar_length = 240;
      bar_position = 0;
    }
    bar_length = config.bar_length * 1000 / total_bar_length;
    sprintf(bar_info, "%dm", config.bar_length);  //default in config 1852m
    //strcpy(bar_info,"1852m");//default
    if (field == SPEED3) {
      bar_length = 250 * 1000 / total_bar_length;
      strcpy(bar_info, "250m");
    }
    if (field == SPEED7) {
      bar_length = 500 * 1000 / total_bar_length;
      strcpy(bar_info, "500m");
    }
    if (field == SPEED8) {
      run_rectangle_length = log_seconds * total_bar_length / 3600;
      strcpy(bar_info, "3600s");
    } else {
      run_rectangle_length = (Ublox.alfa_distance / bar_length);
    }
    if (run_rectangle_length > total_bar_length) { run_rectangle_length = total_bar_length; }  //limit bar length for time...
    if ((run_rectangle_length < 180) & (config.speed_large_font == 1)) {
      display.setTextWrap(false);
      display.setFont(&FreeMonoBold9pt7b);  //print bar infoFreeMonoBold9pt7b
      display.setCursor(offset + 186, 48);
      display.print(bar_info);
    }
    if (config.speed_large_font == 0) {
      display.setFont(&FreeSansBold12pt7b);  //print time
      display.setCursor(offset + 180, 43);
      display.print(time_now);
    }
    if ((run_rectangle_length < 160) & (config.speed_large_font == 2)) {
      display.setTextWrap(false);
      display.setFont(&FreeMonoBold9pt7b);  //print bar infoFreeMonoBold9pt7b
      display.setCursor(offset + 186, 10);
      display.print(bar_info);
    }
    display.fillRect(offset, bar_position, run_rectangle_length, 8, GxEPD_BLACK);  //balk voor run_distance weer te geven...
  }

  if (screen == TROUBLE) {
    display.setFont(&FreeSansBold12pt7b);
    display.setCursor(offset, ROW_1_12PT);
    display.println("No GPS frames for");
    display.println("more then 10 s.... ");
    //display.setCursor(offset,120);
    //display.print(time_now);
  }
  if(screen>=STATS1)  { 
    display.setFont(&FreeSansBold6pt7b);
    display.setCursor(displayWidth - 20, INFO_BAR_TOP);
    char c = screen;
    display.print(c);  //show config field in small font
  }
  if (screen == STATS1) {  //2s,10sF,10sS, AVG
    Stats_2s_3_lines("10sF: ", "10sS: ", "AVG:  ",S10.display_speed[9] * calibration_speed,S10.display_speed[5] * calibration_speed, S10.avg_5runs * calibration_speed);
  }
  if (screen == STATS2) {  //alfa 500m,1852m, 1800s,total_dist
    static int toggle = 0;
    if (toggle == 0) {
      Stats_4lines("Dist: ", "1852m: ", "1800s: ", "Alfa: ", Ublox.total_distance / 1000, M1852.display_speed[9] * calibration_speed, S1800.display_max_speed * calibration_speed, A500.avg_speed[9] * calibration_speed);
      toggle = 1;
    } else {
      Stats_4lines("Dist: ", "1852m: ", "3600s: ", "Alfa: ", Ublox.total_distance / 1000, M1852.display_speed[9] * calibration_speed, S3600.display_max_speed * calibration_speed, A500.avg_speed[9] * calibration_speed);
      toggle = 0;
    }
  }
  if (screen == STATS3) {  //100m,250m, 500m,Alfa
    Stats_4lines("100m:", "250m:", "500m:", "Alfa:", M100.display_speed[9] * calibration_speed, M250.display_speed[9] * calibration_speed, M500.display_speed[9] * calibration_speed, A500.avg_speed[9] * calibration_speed);
  }
  if (screen == STATS4) {  //10s,AVG,5 runs, update on the fly !!!S10.display_speed[5] * calibration_speed
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
    }
  }
  if (screen == STATS6) {  //Simon stat screen
    Serial.println("STATS6_Simon_screen");
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

    display.println(S10.avg_5runs * calibration_speed, 2);  //Laat het gemiddelde incl de previous 10s zien mits in top5
    display.setCursor(col2, row2);
    display.println(S10.display_speed[9] * calibration_speed, 2);
    display.setCursor(col2, row3);
    display.println(S10.display_speed[8] * calibration_speed, 2);
    display.setCursor(col2, row4);
    display.println(S10.display_speed[7] * calibration_speed, 2);
    display.setCursor(col2, row5);
    display.println(S10.display_speed[6] * calibration_speed, 2);
    display.setCursor(col2, row6);
    display.println(S10.display_speed[5] * calibration_speed, 2);
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
    display.println(S2.display_speed[9] * calibration_speed, 2);
    display.setCursor(col4, row2);
    display.println(S10.s_max_speed * calibration_speed, 2);  //previous run
    display.setCursor(col4, row3);   
    display.println(Ublox.total_distance / 1000000);
    display.setCursor(col4, row4);
    display.println(A500.avg_speed[9] * calibration_speed, 2);
    display.setCursor(col4, row5);
    display.println(M500.display_speed[9] * calibration_speed, 2);
    display.setCursor(col4, row6);
    display.println(M1852.display_speed[9] * calibration_speed, 2);

    // position line of actual 10s run
    if (S10.s_max_speed * calibration_speed > S10.display_speed[9] * calibration_speed) line = row1;
    else if (S10.s_max_speed * calibration_speed > S10.display_speed[8] * calibration_speed) line = row2;
    else if (S10.s_max_speed * calibration_speed > S10.display_speed[7] * calibration_speed) line = row3;
    else if (S10.s_max_speed * calibration_speed > S10.display_speed[6] * calibration_speed) line = row4;
    else if (S10.s_max_speed * calibration_speed > S10.display_speed[5] * calibration_speed) line = row5;
    else line = row6;
    display.fillRect(0, line + 2, col3 - 10, 2, GxEPD_BLACK);  //lijn voor actuele run weer te geven...
  }

  if (screen == STATS7) {  //Simon bar graph screen
    Serial.println("STATS7_Simon_bar graph");
    int posX = 5;
    int posY = INFO_BAR_TOP;
    int MaxNumberBar = NR_OF_BAR;  //is 42, zie GPS_data.h
    int GraphWidth = 215;          //was 220
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

    display.setCursor(0, 26);
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
  }
    if (screen == STATS8) {
      display.setFont(&FreeSansBold12pt7b);
      for (int i = 9; i > 4; i--) {
        display.setCursor(offset, 24 * (10 - i));
        display.print("500 ");
        display.print(10 - i);
        display.print(": ");
        display.print(M500.avg_speed[i] * calibration_speed, 2);
        display.print(" @");
        display.print(M500.time_hour[i]);
        if (M500.time_min[i] < 10) display.print(":0");
        else display.print(":");
        display.print(M500.time_min[i]);
      }
    }
    if (screen == STATS9) {
      display.setFont(&FreeSansBold12pt7b);
      for (int i = 9; i > 4; i--) {
        display.setCursor(offset, 24 * (10 - i));
        display.print("Run ");
        display.print(10 - i);
        display.print(": ");
        display.print(S10.avg_speed[i] * calibration_speed, 2);
        display.print(" @");
        display.print(S10.time_hour[i]);
        if (S10.time_min[i] < 10) display.print(":0");
        else display.print(":");
        display.print(S10.time_min[i]);
      }
    }
    if (screen == STATSA) {
      display.setFont(&FreeSansBold12pt7b);
      for (int i = 9; i > 4; i--) {
        display.setCursor(offset, 24 * (10 - i));
        display.print("2s: ");
        display.print(10 - i);
        display.print(": ");
        display.print(S2.avg_speed[i] * calibration_speed, 2);
        display.print(" @");
        display.print(S2.time_hour[i]);
        if (S2.time_min[i] < 10) display.print(":0");
        else display.print(":");
        display.print(S2.time_min[i]);
      }
    }
    if (screen == STATSB) {
    Stats_2s_3_lines("10sLast: ", "10sBest: ", "AVG :  ", S10.display_last_run * calibration_speed, S10.display_speed[9] * calibration_speed, S10.avg_5runs * calibration_speed);
    }
    #ifdef TRACKSPEED
    if (screen == STATSC) {
      Stats_4lines("Dis_S:", "Dis:", "Speed:", "Dis_E:", M_500.distance_startline, M_500.track_distance, M_500.Track_speed, M_500.distance_endline);
    }
    if (screen == STATSD) {
      display.setFont(&FreeSansBold12pt7b);
      for (int i = 9; i > 4; i--) {
        display.setCursor(offset, 24 * (10 - i));
        display.print("Track");
        display.print(10 - i);
        display.print(": ");
        display.print(M_500.avg_speed[i] * config.cal_speed, 2);
        display.print(" @");
        display.print(M_500.time_hour[i]);
        if (M_500.time_min[i] < 10) display.print(":0");
        else display.print(":");
        display.print(M_500.time_min[i]);
      }
    }
    #endif
  if (count % 200 == 0) {  //was 200
    if (update_epaper > 0) display.update();
    offset = 0;
  } else {
    if (update_epaper == 2) { display.fillScreen(GxEPD_WHITE); }                           //test
    if (update_epaper > 0) display.updateWindow(0, 0, displayWidth, displayHeight, true);  //was 244,122, true !!!
    if (screen >= STATS1) update_delay = (config.Stat_screens_time - 2) * 1000;            //set delay for all stats screens !
    if(update_delay<100)update_delay=100;//never delay with a negative number.....
    delay(update_delay);                                                                   //update delay function of screen to show
  }
  old_screen = screen;
  count++;
}

#undef ROW_1_9PT
#undef TOP_LEFT_INFO
#undef ROW_3_9PT
#undef ROW_9pt_2
#endif