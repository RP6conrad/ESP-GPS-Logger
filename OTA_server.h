#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include "OTA_html.h"

const char* host = "esp32";
WebServer server(80);

//SD Card webinterface downlaod section

String webpage = ""; //String to save the html code

void append_page_header(){
  webpage = html_header;
}

//Saves repeating many lines of code for HTML page footers
void append_page_footer()
{ 
  webpage += F("</body></html>");
}

//SendHTML_Header
void SendHTML_Header()
{
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate"); 
  server.sendHeader("Pragma", "no-cache"); 
  server.sendHeader("Expires", "-1"); 
  server.setContentLength(CONTENT_LENGTH_UNKNOWN); 
  server.send(200, "text/html", ""); //Empty content inhibits Content-length header so we have to close the socket ourselves. 
  append_page_header();
  server.sendContent(webpage);
  webpage = "";
}

//SendHTML_Content
void SendHTML_Content()
{
  server.sendContent(webpage);
  webpage = "";
}

//SendHTML_Stop
void SendHTML_Stop()
{
  server.sendContent("");
  server.client().stop(); //Stop is needed because no content length was sent
}

//ReportSDNotPresent
void ReportSDNotPresent()
{
  SendHTML_Header();
  webpage += F("<h3>No SD Card present</h3>"); 
  webpage += F("<a href='/'>[Back]</a><br><br>");
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop();
}

//ReportFileNotPresent
void ReportFileNotPresent(String target)
{
  SendHTML_Header();
  webpage += F("<h3>File does not exist</h3>"); 
  webpage += F("<a href='/"); webpage += target + "'>[Back]</a><br><br>";
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop();
}

//File size conversion
String file_size(int bytes)
{
  String fsize = "";
  if (bytes < 1024)                 fsize = String(bytes)+" B";
  else if(bytes < (1024*1024))      fsize = String(bytes/1024.0,3)+" KB";
  else if(bytes < (1024*1024*1024)) fsize = String(bytes/1024.0/1024.0,3)+" MB";
  else                              fsize = String(bytes/1024.0/1024.0/1024.0,3)+" GB";
  return fsize;
}



//Download a file from the SD, it is called in void SD_dir()
void SD_file_download(String filename)
{
  if (sdOK) 
  { 
    File download = SD.open("/"+filename);
    if (download) 
    {
      //server.sendHeader("Content-Type", "text/text");
      server.sendHeader("Content-Disposition", "attachment; filename="+filename);
      server.sendHeader("Connection", "close");
      server.streamFile(download, "application/octet-stream");
      download.close();
    } else ReportFileNotPresent("download"); 
  } else ReportSDNotPresent();
}

//Prints the directory, it is called in void SD_dir() 
void printDirectory(const char * dirname, uint8_t levels)
{
  
  File root = SD.open(dirname);

  if(!root){
    return;
  }
  if(!root.isDirectory()){
    return;
  }
  File file = root.openNextFile();
  int i = 0;
  while(file){
    if (webpage.length() > 1000) {
      SendHTML_Content();
    }
    if(file.isDirectory()){
      webpage += "<tr>\n<td>"+String(file.isDirectory()?"Dir":"File")+"</td><td></td><td></td></tr>";
      printDirectory(file.name(), levels-1);
    }
    else
    {
      webpage += "<tr>\n<td width='20%'>"+String(file.name())+"</td>";
      int bytes = file.size();
      String fsize = "";
      if (bytes < 1024)                     fsize = String(bytes)+" B";
      else if(bytes < (1024 * 1024))        fsize = String(bytes/1024.0,3)+" KB";
      else if(bytes < (1024 * 1024 * 1024)) fsize = String(bytes/1024.0/1024.0,3)+" MB";
      else                                  fsize = String(bytes/1024.0/1024.0/1024.0,3)+" GB";
      webpage += "<td>"+fsize+"</td>";
      //webpage += "<td>"+String(file.lastWriteDate)+"</td>"; //gettimestamp
      webpage += "<td>";
      webpage += F("<FORM action='/' method='post'>"); 
      webpage += F("<button type='submit' class='button' name='download'"); 
      webpage += F("' value='"); webpage +="download_"+String(file.name()); webpage +=F("'>Download</button>");
      webpage += "</td>";
      webpage += "<td>";
      if(String(file.name()) != "config.txt" | String(file.name()) != "/config.txt" | String(file.name()) != "/config_backup.txt" | String(file.name()) != "config_backup.txt"){
        webpage += F("<FORM action='/' method='post'>"); 
        webpage += F("<button type='submit' name='delete' class='button' onclick='return confirmdelete();'"); 
        webpage += F("' value='"); webpage +="delete_"+String(file.name()); webpage +=F("'>Delete</button>");
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
void SD_file_delete(String filename) 
{ 
  if (sdOK) { 
    SendHTML_Header();
    File dataFile = SD.open("/"+filename, FILE_READ); //Now read data from SD Card 
    if (dataFile)
    {
      if (SD.remove("/"+filename)) {
        Serial.println(F("File deleted successfully"));
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
} 

/*********  FUNCTIONS  **********/
//Initial page of the server web, list directory and give you the chance of deleting and uploading
void SD_dir()
{
  if (sdOK) 
  {
    //Action acording to post, dowload or delete, by MC 2022
    if (server.args() > 0 ) //Arguments were received, ignored if there are not arguments
    { 
      Serial.println(server.arg(0));
  
      String Order = server.arg(0);
      Serial.println(Order);
      
      if (Order.indexOf("download_")>=0)
      {
        Order.remove(0,9);
        SD_file_download(Order);
        Serial.println(Order);
      }
  
      if ((server.arg(0)).indexOf("delete_")>=0)
      {
        Order.remove(0,7);
        SD_file_delete(Order);
        Serial.println(Order);
      }
    }

    File root = SD.open("/");
    if (root) {
      root.rewindDirectory();
      SendHTML_Header();    
      webpage += F("<table id='esplogger'>\n");
      webpage += F("<tr>\n<th>Name</th><th>Size</th><th>Download</th><th>Delete</th>\n</tr>");
      printDirectory("/",0);
      webpage += F("\n</table>");
      SendHTML_Content();
      root.close();
    }
    else 
    {
      SendHTML_Header();
      webpage += F("<h3>No Files Found</h3>");
    }
    append_page_footer();
    SendHTML_Content();
    SendHTML_Stop();   //Stop is needed because no content length was sent
  } else ReportSDNotPresent();
}

//ReportCouldNotCreateFile
void ReportCouldNotCreateFile(String target)
{
  SendHTML_Header();
  webpage += F("<div style='overflow-x:auto;'><table id='esplogger'>");
  webpage += F("<tr><th>Could Not Create Uploaded File (write-protected?)</th></tr></table></div>"); 
  webpage += F("<a href='/"); webpage += target + "'>[Back]</a><br><br>";
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop();
}
//Upload a file to the SD
void File_Upload()
{
  append_page_header();
  webpage += F("<FORM action='/fupload' method='post' enctype='multipart/form-data'>");
  webpage += F("<div style='overflow-x:auto;'><table id='esplogger'>");
  webpage += F("<tr><th>select File to Upload</th><th>Action</th></tr>");
  webpage += F("<tr><td><input type='file' name='fupload' id = 'fupload' value=''></td><td>");
  webpage += F("<button class='button' type='submit'>Upload File</button></td></tr>");
  webpage += F("</table></div></FORM>");
  webpage += F("<a href='/'>[Back]</a><br><br>");
  append_page_footer();
  server.send(200, "text/html",webpage);
}

//Handles the file upload a file to the SD
File UploadFile;
//Upload a new file to the Filing system
void handleFileUpload()
{ 
  HTTPUpload& uploadfile = server.upload();
  if(uploadfile.status == UPLOAD_FILE_START)
  {
    String filename = uploadfile.filename;
    if(!filename.startsWith("/")) filename = "/"+filename;
    Serial.print("Upload File Name: "); Serial.println(filename);
    SD.remove(filename);                         //Remove a previous version, otherwise data is appended the file again
    UploadFile = SD.open(filename, FILE_WRITE);  //Open the file for writing in SD (create it, if doesn't exist)
    filename = String();
  }
  else if (uploadfile.status == UPLOAD_FILE_WRITE)
  {
    if(UploadFile) UploadFile.write(uploadfile.buf, uploadfile.currentSize); // Write the received bytes to the file
  } 
  else if (uploadfile.status == UPLOAD_FILE_END)
  {
    if(UploadFile)          //If the file was successfully created
    {                                    
      UploadFile.close();   //Close the file again
      Serial.print("Upload Size: "); Serial.println(uploadfile.totalSize);
      webpage = "";
      append_page_header();
      webpage += F("<div style='overflow-x:auto;'><table id='esplogger'>");
      webpage += F("<tr><th>File was successfully uploaded</th></tr>"); 
      webpage += F("<tr><th>Uploaded File Name: "); webpage += uploadfile.filename+"</th></tr>";
      webpage += F("<tr><th>File Size: "); webpage += file_size(uploadfile.totalSize) + "</th></tr></table></div>"; 
      webpage += F("<a href='/'>[Back]</a><br><br>");
      append_page_footer();
      server.send(200,"text/html",webpage);
    } 
    else
    {
      ReportCouldNotCreateFile("upload");
    }
  }
}

//Config Part - allow to change the config on txt by webinterface
void Config_TXT()
{
  append_page_header();
  webpage += html_config_header;
  html_config(webpage);
  webpage += html_config_footer;
  append_page_footer();
  server.send(200, "text/html",webpage);
}

void handleConfigUpload() {
 if (sdOK) 
  {
   SD.remove("/config_backup.txt");
   SD.rename("/config.txt", "/config_backup.txt");
   SD.remove("/config.txt");
  
    // Open file for writing
    File file = SD.open("/config.txt", FILE_WRITE);
    if (!file) {
      Serial.println(F("Failed to create file"));
      return;
    }
    StaticJsonDocument<1024> doc;
  
    // Set the values in the document
    doc["cal_bat"] = server.arg("cal_bat"); 
    doc["cal_speed"] = server.arg("cal_speed"); 
    doc["sample_rate"] = server.arg("sample_rate");
    
    doc["gnss"] = server.arg("gnss");
    doc["speed_field"] = server.arg("speed_field");
    doc["bar_length"] = server.arg("bar_length");
    doc["Stat_screens"] = server.arg("Stat_screens"); 
    doc["GPIO12_screens"] = server.arg("GPIO12_screens"); 
    doc["Logo_choice"] = server.arg("Logo_choice");
    doc["sleep_off_screen"] = server.arg("sleep_off_screen");
    doc["logOAO"] = server.arg("logOAO"); 
    doc["logUBX"] = server.arg("logUBX");
    doc["dynamic_model"] = server.arg("dynamic_model");
    doc["GPIO12_screens"] = server.arg("GPIO12_screens");
    doc["timezone"] = server.arg("timezone");
    doc["UBXfile"] = server.arg("UBXfile");
    doc["Sleep_info"] = server.arg("Sleep_info");
    doc["ssid"] = server.arg("ssid");
    doc["password"] = server.arg("password");
      
    // Pretty Serialize JSON to file
    if (serializeJsonPretty(doc, file) == 0) {
      Serial.println(F("Failed to write to file"));
      SendHTML_Header();
      webpage += F("<div style='overflow-x:auto;'><table id='esplogger'>");
      webpage += F("<tr><th>failed to Upload the file</th></tr></table></div>"); 
      webpage += F("<a href='/config"); webpage += "'>[Back]</a><br><br>";
      append_page_footer();
      
    }else{
      // Close the file
      file.close();
      SendHTML_Header();
      webpage += F("<div style='overflow-x:auto;'><table id='esplogger'>");
      webpage += F("<tr><th>Upload was successful!</th></tr></table></div>"); 
      webpage += F("<a href='/config"); webpage += "'>[Back]</a><br><br>";
      append_page_footer();
      delay(2000);
      if(server.arg("reboot") == "yes"){
        ESP.restart(); //possible restart
      }
    }
  }
}

//end SD Card webinterface downlaod section

//begin OTA
/* Style */
String style =
"<style>#file-input,input{width:100%;height:44px;border-radius:4px;margin:10px auto;font-size:15px}"
"input{background:#f1f1f1;border:0;padding:0 15px}body{background:#3498db;font-family:sans-serif;font-size:14px;color:#777}"
"#file-input{padding:0;border:1px solid #ddd;line-height:44px;text-align:left;display:block;cursor:pointer}"
"#bar,#prgbar{background-color:#f1f1f1;border-radius:10px}#bar{background-color:#3498db;width:0%;height:10px}"
"form{background:#fff;max-width:258px;margin:75px auto;padding:30px;border-radius:5px;text-align:center}"
".btn{background:#3498db;color:#fff;cursor:pointer}</style>";

/* Login page */
String loginIndex = 
"<form name=loginForm>"
"<h1>ESP32 Login</h1>"
"<input name=userid placeholder='User ID'> "
"<input name=pwd placeholder=Password type=Password> "
"<input type=submit onclick=check(this.form) class=btn value=Login></form>"
"<script>"
"function check(form) {"
"if(form.userid.value=='admin' && form.pwd.value=='admin')"//hier wordt het paswoord bepaald !!
"{window.open('/serverIndex')}"
"else"
"{alert('Error Password or Username')}"
"}"
"</script>" + style;
 
/* Server Index Page */
String serverIndex = 
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
"<input type='file' name='update' id='file' onchange='sub(this)' style=display:none>"
"<label id='file-input' for='file'>   Choose file...</label>"
"<input type='submit' class=btn value='Update'>"
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
"</script>" + style;

/* setup function */
void OTA_setup(void) {
  //Wifi connection already started !!
  /*use mdns for host name resolution*/

  /* add initialize sd card */
  if (!MDNS.begin(host)) { //http://esp32.local
    Serial.println("Error config up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  /*return index page which is stored in serverIndex */
  server.on("/firmware", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });
  server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  //add webserver and fileupload part
  server.on("/", SD_dir);
  server.on("/upload",   File_Upload);
  server.on("/fupload",  HTTP_POST,[](){ server.send(200);}, handleFileUpload);
  server.on("/config",   Config_TXT);
  server.on("/configupload", handleConfigUpload);
  /*handling uploading firmware file */
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  server.begin();
}
