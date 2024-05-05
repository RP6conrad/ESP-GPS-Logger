/*
For OTA without internet access, see : 
https://github.com/italocjs/ESP32_OTA_APMODE/blob/main/Main.cpp
*/
#include <SD.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <EEPROM.h>
#include <LITTLEFS.h>
//#include <LittleFS.h>
#include "Definitions.h"
#include "OTA_html.h"
//#include "ESP_functions.h"
bool downloading_file = false;
const char* host = "esp32";
extern const char E_paper_version[16];
extern RTC_DATA_ATTR float RTC_calibration_bat;
extern RTC_DATA_ATTR int RTC_highest_read;
WebServer server(80);

//SD Card webinterface download section
String webpage = "";  //String to save the html code
String loginIndex = "";
void append_page_header() {
  webpage = html_header;
}

//Saves repeating many lines of code for HTML page footers
void append_page_footer() {
  webpage += F("</body></html>");
}

//SendHTML_Header
void SendHTML_Header() {
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");  //Empty content inhibits Content-length header so we have to close the socket ourselves.
  append_page_header();
  server.sendContent(webpage);
  webpage = "";
}

//SendHTML_Content
void SendHTML_Content() {
  server.sendContent(webpage);
  webpage = "";
}

//SendHTML_Stop
void SendHTML_Stop() {
  server.sendContent("");
  server.client().stop();  //Stop is needed because no content length was sent
}

//ReportSDNotPresent
void ReportSDNotPresent() {
  SendHTML_Header();
  webpage += F("<h3>No SD Card present</h3>");
  webpage += F("<a href='/'>[Back]</a><br><br>");
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop();
}

//ReportFileNotPresent
void ReportFileNotPresent(String target) {
  SendHTML_Header();
  webpage += F("<h3>File does not exist</h3>");
  webpage += F("<a href='/");
  webpage += target + "'>[Back]</a><br><br>";
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop();
}

//File size conversion kb-> MB / GB
String file_size(int kbytes) {
  String fsize = "";
  //if (kbytes < 1024) fsize = String(kbytes) + " B";
  if (kbytes < 1024) fsize = String(kbytes) + " KB";
  else if (kbytes < (1024 * 1024)) fsize = String(kbytes /1024.0, 2) + " MB";
  else fsize = String(kbytes / 1024.0 / 1024.0, 2) + " GB";
  return fsize;
}
//changes JH 19/11/2022, added for formatting timestamp file
String Print_time(time_t timestamp) {
  char buff[30];
  tm *tm_local=localtime(&timestamp);
  if(tm_local->tm_isdst==1)timestamp=timestamp-3600;//correction for littlefs bug if dst is active
  strftime(buff, 30, "%Y-%m-%d  %H:%M:%S", localtime(&timestamp));//was localtime
  return buff;
}


//Download a file from the SD, it is called in void SD_dir()
void SD_file_download(String filename) {
  if (sdOK | LITTLEFS_OK) {
    File download;
    if (sdOK) download = SD.open("/" + filename);
    if (LITTLEFS_OK) download = LITTLEFS.open("/" + filename);
    if (download) {
      downloading_file = true;
      if (sdOK|LITTLEFS_OK) filename.remove(0, 1);  //remove 1 character starting at index 0, this is /, after downloading -> _filename....
      server.sendHeader("Content-Disposition", "attachment; filename=" + filename);
      server.sendHeader("Connection", "close");
      server.streamFile(download, "application/octet-stream");
      download.close();
    } else {
      ReportFileNotPresent("download");
      downloading_file = false;
    }
  } else {
    ReportSDNotPresent();
  }
}

//Prints the directory, it is called in void SD_dir()
void printDirectory(const char* dirname, uint8_t levels) {
  File root;
  if (sdOK) root = SD.open(dirname);
  if (LITTLEFS_OK) root = LITTLEFS.open(dirname);
  if (!root) {
    return;
  }
  if (!root.isDirectory()) {
    return;
  }
  File file = root.openNextFile();
  int i = 0;
  while (file) {
    time_t now;
    time(&now);
    if (webpage.length() > 1000) {
      SendHTML_Content();
    }
    if (file.isDirectory()) {
      if (levels == 1) {
        if (String(file.name()) == "/Archive") {
          webpage += "<tr>\n<td>" + String(file.isDirectory() ? "Dir" : "File") + "</td><td></td><td></td><td></td><td></td></tr>";
          printDirectory(file.name(), levels);  //was levels-1
        }
      }
    } else if ((config.archive_days > 0) & (levels == 2) & (file.getLastWrite() < (now - config.archive_days * 24 * 3600)) & (file.getLastWrite() > EPOCH_2022)) {  //older then xx days but younger then 2021, copy to archive, but not config.txt !!
      char buffer[40] = "/Archive";                                                                                                                                 //&(file.getLastWrite()>EPOCH_2022)
      strcat(buffer, file.name());                                                                                                                                  //only rename if not Archive listing !!!
      if ((String(file.name()) != "config.txt") & (String(file.name()) != "/config.txt") & (String(file.name()) != "/config_backup.txt") & (String(file.name()) != "config_backup.txt")) {
        if (sdOK|LITTLEFS_OK) SD.rename(file.name(), buffer);
      }
    } else {
      String filename = String(file.name());
      if (sdOK|LITTLEFS_OK) filename.remove(0, 1);  //remove 1 character starting at index 0
      webpage += "<tr><td width='20%'>" + filename + "</td>";
      int bytes = file.size();
      String fsize = "";
      time_t file_date = file.getLastWrite();  //changes JH 19/11/2022
      String fd = Print_time(file_date);       //Winscp MLSD expect UTC time !!!
      //if (bytes < 1024)                     fsize = String(bytes)+" B";
      //else if(bytes < (1024 * 1024))        fsize = String(bytes/1024.0,3)+" KB";
      //else if(bytes < (1024 * 1024 * 1024)) fsize = String(bytes/1024.0/1024.0,3)+" MB";
      //else                                  fsize = String(bytes/1024.0/1024.0/1024.0,3)+" GB";
      fsize = String(bytes / 1024.0 / 1024.0, 3) + " MB";  //fsize always in MB !!!
      webpage += "<td>" + fsize + "</td>";
      webpage += "<td>" + fd + "</td>";  //
      webpage += "<td>";
      webpage += F("<form action='/' method='post'>");
      webpage += F("<button type='submit' class='button' name='download'");
      webpage += F("' value='");
      webpage += "download_" + String(file.name());
      webpage += F("'>Download</button>");
      webpage += F("</form>");
      webpage += "</td>";
      webpage += "<td>";
      if ((String(file.name()) != "config.txt") & (String(file.name()) != "/config.txt") & (String(file.name()) != "/config_backup.txt") & (String(file.name()) != "config_backup.txt")) {
        webpage += F("<form action='/' method='post'>");
        webpage += F("<button type='submit' name='delete' class='button_del' onclick='return confirmdelete();'");
        webpage += F("' value='");
        webpage += "delete_" + String(file.name());
        webpage += F("'>Delete</button>");
        webpage += F("</form>");
      }
      webpage += "</td>\n";
      webpage += "</tr>";
    }
    file = root.openNextFile();
    i++;
  }
  file.close();
}

//Delete a file from the SD, it is called in void SD_dir()
void SD_file_delete(String filename) {
  if (sdOK | LITTLEFS_OK) {
    //SendHTML_Header();
    File dataFile;
    if (sdOK) dataFile = SD.open("/" + filename, FILE_READ);               //Now read data from SD Card
    if (LITTLEFS_OK) dataFile = LITTLEFS.open("/" + filename, FILE_READ);  //Now read data from SD Card
    if (dataFile) {
      if (sdOK) {
        if (SD.remove("/" + filename)) {
          Serial.println(F("SD File deleted successfully"));
        }  //toegevoegd
      }
      if (LITTLEFS_OK) {
        dataFile.close();
        if (LITTLEFS.remove("/" + filename)) {
          Serial.println(F("LITTLEFS File deleted successfully"));
        }
      }

    }  //toegevoegd
  }    //toegevoegd
       /*
        webpage += F("<div style='overflow-x:auto;'><table id='esplogger'>");
        webpage += "<tr><th>File '"+filename+"' has been erased</th></tr></table></div>"; 
        webpage += F("<a href='/'>[Back]</a><br><br>");
      }
      else
      { 
        webpage += F("<div style='overflow-x:auto;'><table id='esplogger'>");
        webpage += F("<tr><th>File was not deleted - error</th></tr></table></div>");
        webpage += F("<a href='/'>[Back]</a><br><br>");
      }
    } else ReportFileNotPresent("delete");
    append_page_footer(); 
    SendHTML_Content();
    SendHTML_Stop();
  } else ReportSDNotPresent();
*/
}

/*********  FUNCTIONS  **********/
void SD_dir(int archive);
void SD_directory(void) {
  SD_dir(0);
}
void SD_archive_list(void) {
  SD_dir(1);
}
void SD_archive_file(void) {
  SD_dir(2);
}
//Initial page of the server web, list directory and give you the chance of deleting and uploading
void SD_dir(int archive) {
  if (sdOK | LITTLEFS_OK) {
    //Action acording to post, dowload or delete, by MC 2022
    if (server.args() > 0)  //Arguments were received, ignored if there are not arguments
    {
      Serial.println(server.arg(0));

      String Order = server.arg(0);
      Serial.println(Order);

      if (Order.indexOf("download_") >= 0) {
        Order.remove(0, 9);
        SD_file_download(Order);
        Serial.println(Order);
      }

      if ((server.arg(0)).indexOf("delete_") >= 0) {
        Order.remove(0, 7);
        SD_file_delete(Order);
        Serial.println(Order);
      }
    }
    File root;
    if (sdOK) root = SD.open("/");
    if (LITTLEFS_OK) root = LITTLEFS.open("/");
    uint64_t free_kbytes;
    if(LITTLEFS_OK) free_kbytes = (LITTLEFS.totalBytes() - LITTLEFS.usedBytes())/1024;
    if(sdOK) {
        uint64_t totalBytes=SD.totalBytes();
        uint64_t usedBytes=SD.usedBytes();
        free_kbytes=(totalBytes-usedBytes)/1024;
        }
    float bat_perc = 100 * (1 - (VOLTAGE_100 - RTC_voltage_bat) / (VOLTAGE_100 - VOLTAGE_0));
    if (bat_perc>100)bat_perc=100;
     if (bat_perc<0)bat_perc=0;
    String font_color_start = "<FONT COLOR=\"RED\">";
    String font_color_end ="</FONT>"; 
    String voltage_percent;
    String free_space = "Free storage space = " + file_size(free_kbytes);
    String voltage_lipo = "&emsp;Bat voltage = " + String(RTC_voltage_bat, 2) + " Volt";
    String firmware = "Firmware "+ String(SW_version);
    String gps_warning = "<h3>"+ font_color_start+"Sample-rate too high for the actual gnss setting, possible lost points in the log file !!"+font_color_end + "</h3>";
    String CPU_freq =  "<h3>"+ font_color_start+"For 5 Hz sample_rate, CPU freq of 80 MHz is sufficient. For 10 Hz, CPU freq of 160 MHz gives the maximal performance."+font_color_end + "</h3>";
    bool GPS_warning=false;
    bool CPU_freq_warning = false;
    if(bat_perc<VOLTAGE_LOW) voltage_percent = "&emsp;"+font_color_start+"Bat = " + String(bat_perc,0)+" % &emsp;"+font_color_end;
    else voltage_percent = "&emsp;Bat % = " + String(bat_perc,0)+" % &emsp;";
    if ((config.ublox_type == M10_9600BD) | (config.ublox_type == M10_38400BD) | (config.ublox_type == AUTO_DETECT)) {  //limit sample rate for 3/4 GNSS M10, prevent lost points
      if (((config.gnss == 3) & (config.sample_rate> 5)) | ((config.gnss== 4) & (config.sample_rate > 8)) | ((config.gnss== 5) & (config.sample_rate > 4))) {GPS_warning=true;}
       }
    if(((config.sample_rate==5)&!(config.cpu_freq==80))|((config.sample_rate==10)&!(config.cpu_freq==160)))CPU_freq_warning=true;
    if (root) {
      root.rewindDirectory();
      SendHTML_Header();
      webpage += F("<table id='esplogger'>");
      webpage += "<h3>" + free_space + voltage_lipo + voltage_percent + firmware + "</h3>";
      if(GPS_warning) webpage += gps_warning;
      if(CPU_freq_warning) webpage += CPU_freq;
      webpage += F("<tr><th>Name</th><th>Size</th><th>Timestamp</th><th>Download</th><th>Delete</th></tr>");
      printDirectory("/", archive);
      webpage += F("\n</table>");
      SendHTML_Content();
      root.close();
    } else {
      SendHTML_Header();
      webpage += F("<h3>No Files Found</h3>");
    }
    append_page_footer();
    SendHTML_Content();
    SendHTML_Stop();  //Stop is needed because no content length was sent
  } else ReportSDNotPresent();
}

//ReportCouldNotCreateFile
void ReportCouldNotCreateFile(String target) {
  SendHTML_Header();
  webpage += F("<div style='overflow-x:auto;'><table id='esplogger'>");
  webpage += F("<tr><th>Could Not Create Uploaded File (write-protected?)</th></tr></table></div>");
  webpage += F("<a href='/");
  webpage += target + "'>[Back]</a><br><br>";
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop();
}
//Upload a file to the SD
void File_Upload() {
  append_page_header();
  webpage += F("<FORM action='/fupload' method='post' enctype='multipart/form-data'>");
  webpage += F("<div style='overflow-x:auto;'><table id='esplogger'>");
  webpage += F("<tr><th>select File to Upload</th><th>Action</th></tr>");
  webpage += F("<tr><td><input type='file' name='fupload' id = 'fupload' value=''></td><td>");
  webpage += F("<button class='button' type='submit'>Upload File</button></td></tr>");
  webpage += F("</table></div></FORM>");
  webpage += F("<a href='/'>[Back]</a><br><br>");
  append_page_footer();
  server.send(200, "text/html", webpage);
}

//Handles the file upload a file to the SD
File UploadFile;
//Upload a new file to the Filing system
void handleFileUpload() {
  HTTPUpload& uploadfile = server.upload();
  if (uploadfile.status == UPLOAD_FILE_START) {
    String filename = uploadfile.filename;
    if (!filename.startsWith("/")) filename = "/" + filename;
    Serial.print("Upload File Name: ");
    Serial.println(filename);
    if (sdOK) {
      SD.remove(filename);                         //Remove a previous version, otherwise data is appended the file again
      UploadFile = SD.open(filename, FILE_WRITE);  //Open the file for writing in SD (create it, if doesn't exist)
    }
    if (LITTLEFS_OK) {
      LITTLEFS.remove(filename);                         //Remove a previous version, otherwise data is appended the file again
      UploadFile = LITTLEFS.open(filename, FILE_WRITE);  //Open the file for writing in SD (create it, if doesn't exist)
    }
    filename = String();
  } else if (uploadfile.status == UPLOAD_FILE_WRITE) {
    if (UploadFile) UploadFile.write(uploadfile.buf, uploadfile.currentSize);  // Write the received bytes to the file
  } else if (uploadfile.status == UPLOAD_FILE_END) {
    if (UploadFile)  //If the file was successfully created
    {
      UploadFile.close();  //Close the file again
      Serial.print("Upload Size: ");
      Serial.println(uploadfile.totalSize);
      webpage = "";
      append_page_header();
      webpage += F("<div style='overflow-x:auto;'><table id='esplogger'>");
      webpage += F("<tr><th>File was successfully uploaded</th></tr>");
      webpage += F("<tr><th>Uploaded File Name: ");
      webpage += uploadfile.filename + "</th></tr>";
      webpage += F("<tr><th>File Size: ");
      webpage += file_size(uploadfile.totalSize/1024) + "</th></tr></table></div>";
      webpage += F("<a href='/'>[Back]</a><br><br>");
      append_page_footer();
      server.send(200, "text/html", webpage);
    } else {
      ReportCouldNotCreateFile("upload");
    }
  }
}

//Config Part - allow to change the config on txt by webinterface
void Config_TXT() {
  append_page_header();
  webpage += html_config_header;
  html_config(webpage);
  webpage += html_config_footer;
  append_page_footer();
  server.send(200, "text/html", webpage);
}

void handleConfigUpload() {
  if (sdOK | LITTLEFS_OK) {
    File file;
    if (sdOK) {
      SD.remove("/config_backup.txt");
      SD.rename("/config.txt", "/config_backup.txt");
      SD.remove("/config.txt");
      // Open file for writing
      file = SD.open("/config.txt", FILE_WRITE);
      if (!file) {
        Serial.println(F("Failed to create file"));
        return;
      }
    }
    if (LITTLEFS_OK) {
      LITTLEFS.remove("/config_backup.txt");
      LITTLEFS.rename("/config.txt", "/config_backup.txt");
      LITTLEFS.remove("/config.txt");
      // Open file for writing
      file = LITTLEFS.open("/config.txt", FILE_WRITE);
      if (!file) {
        Serial.println(F("Failed to create file"));
        return;
      }
    }
    StaticJsonDocument<1024> doc;
    // Set the values in the document
    //Serial.println("calspeed:"+server.arg("cal_speed"));
    //gnss 4 = GPS + GALILEO + BEIDOU_B1C
    //gnss x = GPS + GLONAS + BEIDOU   (impossible for the M10 ???)
    //gnss 3 = GPS + GLONAS + GALILEO
    //gnss 2 = GPS + GLONAS (default M8 ROM 2)
    //gnss 1 = GPS + GALILEO (not working for M8)
    //gnss 0 = GPS + BEIDOU
    EEPROM.get(1,RTC_highest_read);
    RTC_calibration_bat= FULLY_CHARGED_LIPO_VOLTAGE/RTC_highest_read;
    //doc["cal_bat"] = serialized(server.arg("cal_bat"));
    doc["cal_bat"] = RTC_calibration_bat;
    doc["cal_speed"] = serialized(server.arg("cal_speed"));
    doc["sample_rate"] = server.arg("sample_rate").toInt();
    doc["gnss"] = server.arg("gnss").toInt();
    doc["speed_field"] = server.arg("speed_field").toInt();
    doc["speed_large_font"] = server.arg("speed_large_font").toInt();
    doc["bar_length"] = server.arg("bar_length").toInt();
    doc["Stat_screens"] = server.arg("Stat_screens").toInt();
    doc["Stat_screens_time"] = server.arg("Stat_screens_time").toInt();
    doc["stat_speed"] = server.arg("stat_speed").toInt();
    doc["start_logging_speed"] = server.arg("start_logging_speed").toInt();
    doc["archive_days"] = server.arg("archive_days").toInt();
    doc["GPIO12_screens"] = server.arg("GPIO12_screens").toInt();
    doc["Board_Logo"] = server.arg("Board_Logo").toInt();
    doc["Sail_Logo"] = server.arg("Sail_Logo").toInt();
    doc["sleep_off_screen"] = server.arg("sleep_off_screen").toInt();
    #if defined(_GxDEPG0266BN_H_)
    doc["bat_choice"] = server.arg("bat_choice").toInt();
    #endif
    doc["logTXT"] = server.arg("logTXT").toInt();
    doc["logSBP"] = server.arg("logSBP").toInt();
    doc["logUBX"] = server.arg("logUBX").toInt();
    doc["logUBX_nav_sat"] = server.arg("logUBX_nav_sat").toInt();
    doc["logGPY"] = server.arg("logGPY").toInt();
    doc["logGPX"] = server.arg("logGPX").toInt();
    doc["file_date_time"] = server.arg("file_date_time").toInt();
    doc["dynamic_model"] = server.arg("dynamic_model").toInt();
    doc["GPIO12_screens"] = server.arg("GPIO12_screens").toInt();
    doc["ublox_type"] = server.arg("GPS_type").toInt();//is overwritten @ boot by EEPROM value !!!
    doc["cpu_freq"] = server.arg("CPU_freq").toInt();
    doc["timezone"] = serialized(server.arg("timezone"));
    doc["timezone_DST"] = server.arg("timezone_DST").toInt();
    doc["UBXfile"] = server.arg("UBXfile");
    doc["Sleep_info"] = server.arg("Sleep_info");
    doc["ssid"] = server.arg("ssid");
    doc["password"] = server.arg("password");
    doc["ssid2"] = server.arg("ssid2");
    doc["password2"] = server.arg("password2");
    int Ublox_type = server.arg("GPS_Type").toInt();
    if(Ublox_type == 0xFF) {  //not in config.txt but saved in EEPROM !!!)
      EEPROM.write(0, Ublox_type);
      EEPROM.commit();
    }
    // Pretty Serialize JSON to file
    if (serializeJsonPretty(doc, file) == 0) {
      Serial.println(F("Failed to write to file"));
      SendHTML_Header();
      webpage += F("<div style='overflow-x:auto;'><table id='esplogger'>");
      webpage += F("<tr><th>failed to Upload the file</th></tr></table></div>");
      webpage += F("<a href='/config");
      webpage += "'>[Back]</a><br><br>";
      append_page_footer();
      SendHTML_Content();
      SendHTML_Stop();
    } else {
      // Close the file
      file.close();
      SendHTML_Header();
      webpage += F("<div style='overflow-x:auto;'><table id='esplogger'>");
      webpage += F("<tr><th>Upload was successful!</th></tr></table>");  //</table></div>");
      //webpage += F("<tr><th><p style=\"color:red;\"/p>Sample-rate too high for the actual gnss setting, possible lost points in the log file !!</th></tr></div>");
      /*
      if ((config.ublox_type == M10_9600BD) | (config.ublox_type == M10_38400BD) | (config.ublox_type == AUTO_DETECT)) {  //limit sample rate for 3/4 GNSS M10, prevent lost points
        if (((doc["gnss"] == 3) & (doc["sample_rate"] > 5)) | ((doc["gnss"] == 4) & (doc["sample_rate"] > 8)) | ((doc["gnss"] == 5) & (doc["sample_rate"] > 4))) {
          webpage += F("<tr><th><p style=\"color:red;\"/p><b>Sample-rate too high for the actual gnss setting, possible lost points in the log file !!</b></th></tr></div>");
        }
      } else {
      */  
        webpage += F("</div>");
      //}

      webpage += F("<a href='/config");
      webpage += "'>[Back]</a><br><br>";
      append_page_footer();
      SendHTML_Content();
      SendHTML_Stop();
      if (server.arg("reboot") == "yes") {
        Ublox_off();
        delay(1000);
        ESP.restart();  //restart as wanted
      }
    }
  }
}

//end SD Card webinterface download section

//begin OTA
/* Style */
String style =
  "<style>#file-input,input{width:100%;height:44px;border-radius:4px;margin:10px auto;font-size:15px}"
  "input{background:#f1f1f1;border:0;padding:0 15px}body{background:#3498db;font-family:'sans-serif';font-size:14px;color:#777}"
  "#file-input{padding:0;border:1px solid #ddd;line-height:44px;text-align:left;display:block;cursor:pointer}"
  "#bar,#prgbar{background-color:#f1f1f1;border-radius:10px}#bar{background-color:#3498db;width:0%;height:10px}"
  "form{background:#fff;max-width:358px;margin:75px auto;padding:30px;border-radius:5px;text-align:center}"
  ".btn{background:#3498db;color:#fff;cursor:pointer}</style>";

/* Login page */
void makeLoginString(void) {
  String actual_SW = SW_version;
  String e_type = E_paper_version;
  loginIndex = "";
  loginIndex += F("<form name=loginForm>");
  loginIndex += F("<h1>ESP32 Login</h1>");
  loginIndex += F("<h2>Actual firmware : ");
  loginIndex += actual_SW + "</h2>";
  loginIndex += F("<h2>");
  loginIndex += e_type + "</h2>";
  loginIndex += F("<input name=userid placeholder='User ID'> ");
  loginIndex += F("<input name=pwd placeholder=Password type=Password> ");
  loginIndex += F("<input type=submit onclick=check(this.form) class=btn value=Login></form>");
  loginIndex += F("<script>");
  loginIndex += F("function check(form) {");
  loginIndex += F("if(form.userid.value=='admin' && form.pwd.value=='admin')");  //hier wordt het paswoord bepaald !!
  loginIndex += F("{window.open('/serverIndex')}");
  loginIndex += F("else");
  loginIndex += F("{alert('Error Password or Username')}");
  loginIndex += F("}");
  loginIndex += F("</script>");
  loginIndex += style;
}
/* Server Index Page */
//https://github.com/italocjs/ESP32_OTA_APMODE/blob/main/Main.cpp
String serverIndex =
  "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
  "<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
  "<input type='file' name='update' id='file' onchange='sub(this)' style='display:none'>"
  "<label id='file-input' for='file'>   Choose file...</label>"
  "<input type='submit' class='btn' value='Update'>"
  "<br><br>"
  "<a href='/'>download files</a>"
  "<div id='prg'></div>"
  "<br><div id='prgbar'><div id='bar'></div></div><br></form>"
  "<script>"
  "function sub(obj){"
  "var fileName = obj.value.split('\\\\');"
  "document.getElementById('file-input').innerHTML = '   '+ fileName[fileName.length-1];"
  "};"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  "$.ajax({"
  "url: '/update',"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
  "$('#bar').css('width',Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('success!') "
  "},"
  "error: function (a, b, c) {"
  "}"
  "});"
  "});"
  "</script>"
  + style;

const uint16_t OTA_CHECK_INTERVAL = 5000;  // ms
uint32_t _lastOTACheck = 0;

/* setup function */
void OTA_setup(void) {
  //Wifi connection already started !!
  /*use mdns for host name resolution*/

  /* add initialize sd card */
  if (!MDNS.begin(host)) {  //http://esp32.local
    Serial.println("Error config up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  /*return index page which is stored in serverIndex */
  server.on("/firmware", HTTP_GET, []() {
    makeLoginString();
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });
  server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  //add webserver and fileupload part
  server.on("/", SD_directory);
  server.on("/archive_list", SD_archive_list);
  server.on("/archive_file", SD_archive_file);
  server.on("/upload", File_Upload);
  server.on(
    "/fupload", HTTP_POST, []() {
      server.send(200);
    },
    handleFileUpload);
  server.on("/config", Config_TXT);
  server.on("/configupload", handleConfigUpload);
  /*handling uploading firmware file */
  server.on(
    "/update", HTTP_POST, []() {
      server.sendHeader("Connection", "close");
      server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
      Ublox_off();
      delay(1000);  //to force the ublox again to default (9600 bd)
      ESP.restart();
    },
    []() {
      HTTPUpload& upload = server.upload();
      if (upload.status == UPLOAD_FILE_START) {
        Serial.printf("Update: %s\n", upload.filename.c_str());
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {  //start with max available size
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        /* flashing firmware to ESP*/
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) {  //true to set the size to the current progress
          Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        } else {
          Update.printError(Serial);
        }
      }
    });
  server.begin();

  _lastOTACheck = millis();
}

#if defined USE_AUTO_OTA_UPDATE
#include "HTTPClient.h"
/**
 * from https://github.com/platformio/bintray-secure-ota
*/
#define STRINGV(s) #s
#ifndef OTA_URL
#define OTA_URL \
http:  //esplogger.majasa.ee
#endif
#ifndef OTA_PATH
#define OTA_PATH / api / firmware / versions /
#endif
// Connection port (HTTPS)
const int port = 80;

// Connection timeout
const uint32_t RESPONSE_TIMEOUT_MS = 5000;

// Variables to validate firmware content
volatile int contentLength = 0;
volatile bool isValidContentType = false;

inline String getHeaderValue(String header, String headerName) {
  return header.substring(strlen(headerName.c_str()));
}

String getApiHost() {
  return String(STRINGV(OTA_URL));
}

String getLatestVersionRequestPath() {
  String path = STRINGV(OTA_PATH);
  return String(path + "_latest");
}

String getBinaryRequestPath(const String& version) {
  return String(STRINGV(OTA_PATH) + version + "/firmware_v_" + version + ".bin");
}

void processOTAUpdate(const String version) {
  String currentHost = getApiHost();
  String prevHost = currentHost;
  String firmwarePath = getBinaryRequestPath(version);
  if (!firmwarePath.endsWith(".bin")) {
    Serial.println("Unsupported binary format. OTA update cannot be performed!");
    return;
  }
  WiFiClientSecure client;
  if (!client.connect(currentHost.c_str(), port)) {
    Serial.println("Cannot connect to " + currentHost);
    return;
  }
  bool redirect = true;
  while (redirect) {
    if (currentHost != prevHost) {
      client.stop();
      //client.setCACert(bintray.getCertificate(currentHost));
      if (!client.connect(currentHost.c_str(), port)) {
        Serial.println("Redirect detected! Cannot connect to " + currentHost + " for some reason!");
        return;
      }
    }
    client.print(String("GET ") + firmwarePath + " HTTP/1.1\r\n");
    client.print(String("Host: ") + currentHost + "\r\n");
    client.print("Cache-Control: no-cache\r\n");
    client.print("Connection: close\r\n\r\n");

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > RESPONSE_TIMEOUT_MS) {
        Serial.println("Client Timeout !");
        client.stop();
        return;
      }
    }
    while (client.available()) {
      String line = client.readStringUntil('\n');
      // Check if the line is end of headers by removing space symbol
      line.trim();
      // if the the line is empty, this is the end of the headers
      if (!line.length()) {
        break;  // proceed to OTA update
      }

      // Check allowed HTTP responses
      if (line.startsWith("HTTP/1.1")) {
        if (line.indexOf("200") > 0) {
          //Serial.println("Got 200 status code from server. Proceeding to firmware flashing");
          redirect = false;
        } else if (line.indexOf("302") > 0) {
          //Serial.println("Got 302 status code from server. Redirecting to the new address");
          redirect = true;
        } else {
          //Serial.println("Could not get a valid firmware url");
          //Unexptected HTTP response. Retry or skip update?
          redirect = false;
        }
      }

      // Extracting new redirect location
      if (line.startsWith("Location: ")) {
        String newUrl = getHeaderValue(line, "Location: ");
        //Serial.println("Got new url: " + newUrl);
        newUrl.remove(0, newUrl.indexOf("//") + 2);
        currentHost = newUrl.substring(0, newUrl.indexOf('/'));
        newUrl.remove(newUrl.indexOf(currentHost), currentHost.length());
        firmwarePath = newUrl;
        //Serial.println("firmwarePath: " + firmwarePath);
        continue;
      }

      // Checking headers
      if (line.startsWith("Content-Length: ")) {
        contentLength = atoi((getHeaderValue(line, "Content-Length: ")).c_str());
        Serial.println("Got " + String(contentLength) + " bytes from server");
      }

      if (line.startsWith("Content-Type: ")) {
        String contentType = getHeaderValue(line, "Content-Type: ");
        //Serial.println("Got " + contentType + " payload.");
        if (contentType == "application/octet-stream") {
          isValidContentType = true;
        }
      }
    }

    // check whether we have everything for OTA update
    if (contentLength && isValidContentType) {
      Serial.println("Starting Over-The-Air update. This may take some time to complete ...");
    } else {
      Serial.println("There was no valid content in the response from the OTA server!");
      client.flush();
    }
  }
}

String requestHTTPContent() {
  String payload = "";
  String currentHost = getApiHost();
  WiFiClientSecure client;
  String versionPath = getLatestVersionRequestPath();
  if (!client.connect(currentHost.c_str(), port)) {
    Serial.println("Cannot connect to " + currentHost);
    return payload;
  }
  client.print(String("GET ") + versionPath + " HTTP/1.1\r\n");
  client.print(String("Host: ") + currentHost + "\r\n");
  client.print("Cache-Control: no-cache\r\n");
  client.print("Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > RESPONSE_TIMEOUT_MS) {
      Serial.println("Client Timeout !");
      client.stop();
      return payload;
    }
  }
  while (client.available()) {
    String line = client.readStringUntil('\n');
    int content = 0;
    line.trim();
    // if the the line is empty, this is the end of the headers
    if (!content && !line.length()) {
      content = 1;  // proceed to OTA update
      continue;
    }
    if (line.startsWith("Content-Type: ")) {
      String contentType = getHeaderValue(line, "Content-Type: ");
      //Serial.println("Got " + contentType + " payload.");
      if (contentType == "text/plain") {
        isValidContentType = true;
      }
      continue;
    }
    if (isValidContentType && content == 1) {
      if (!line.length()) {
        continue;
      }
      payload += line;
      break;  //1 line for version string
    }
  }
  client.flush();
  return payload;
}

void checkFirmwareUpdates() {
  // Fetch the latest firmware version
  const String latest = requestHTTPContent();
  if (latest.length() == 0) {
    Serial.println("Could not load info about the latest firmware, so nothing to update. Continue ...");
    return;
  } else if (atoi(latest.c_str()) <= VERSION) {
    //Serial.println("The current firmware is up to date. Continue ...");
    return;
  }

  Serial.println("There is a new version of firmware available: v." + latest);
  processOTAUpdate(latest);
}

#endif