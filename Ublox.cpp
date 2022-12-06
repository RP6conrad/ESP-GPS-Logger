#include "Ublox.h"

byte checksumA;
byte checksumB;
int Time_Set_OK;
UBXMessage ubxMessage = {000000000000};//definition here, declaration in ublox.h !!
struct tm tmstruct ;

tmElements_t my_time;  // time elements structure

time_t unix_timestamp; // a timestamp
// The last two bytes of the message is a checksum value, used to confirm that the received payload is valid.
// The procedure used to calculate this is given as pseudo-code in the uBlox manual.
void calcChecksum(unsigned char* CK, int msgSize) {
  memset(CK, 0, 2);
  for (int i = 0; i < msgSize; i++) {
    CK[0] += ((unsigned char*)(&ubxMessage))[i];
    CK[1] += CK[0];
  }
}
// Compares the first two bytes of the ubxMessage struct with a specific message header.
// Returns true if the two bytes match.
boolean compareMsgHeader(const unsigned char* msgHeader) {
  unsigned char* ptr = (unsigned char*)(&ubxMessage);
  return ptr[0] == msgHeader[0] && ptr[1] == msgHeader[1];
}
void Ublox_on(){
   //F(19, HIGH);//Groene LED
   digitalWrite(25, HIGH); 
   digitalWrite(26, HIGH);
   digitalWrite(27, HIGH);
   //digitalWrite(0, HIGH); 
   //digitalWrite(12, HIGH);
   //digitalWrite(34, HIGH);
   delay(20);
}
void Ublox_off(){
  //digitalWrite(19, LOW);//Groene LED uit
  digitalWrite(25, LOW);
  digitalWrite(26, LOW);
  digitalWrite(27, LOW);
  //digitalWrite(0, LOW);
  //digitalWrite(12, LOW);
  //digitalWrite(34, LOW);
}
void Ublox_serial2(int delay_ms){
 for(int i=0;i<delay_ms;i++){//string van ublox to serial, ca 424 char !!
  //while (Serial2.available()) {
           //   Serial.print(char(Serial2.read()));}
    int msgType = processGPS();
     if ( msgType == MT_NAV_ACK){
          Serial.print(" ACK ");
          Serial.print (ubxMessage.navAck.msg_cls);
          Serial.println (ubxMessage.navAck.msg_id);
          }
     if ( msgType == MT_NAV_ID){
          Serial.print("ID= :");
          Serial.print (ubxMessage.ubxId.ubx_id_1);
          Serial.print (ubxMessage.ubxId.ubx_id_2);
          Serial.print (ubxMessage.ubxId.ubx_id_3);
          Serial.print (ubxMessage.ubxId.ubx_id_4);
          Serial.println(ubxMessage.ubxId.ubx_id_5);
          }
     if ( msgType == MT_MON_GNSS){
          config.gnss=ubxMessage.monGNSS.enabled_Gnss;//to save the reported setting, 3 = gps+ glonas, 11= gps + glonas + galileo
          Serial.print("GNSS= :");
          Serial.print (ubxMessage.monGNSS.supported_Gnss);
          Serial.print (ubxMessage.monGNSS.default_Gnss);
          Serial.println (ubxMessage.monGNSS.enabled_Gnss);
          }
     delay(2);   
     } 
}
void Ask_ID(void){
  Serial.print("Ask ublox Unique ID ");     
  for(int i = 0; i < sizeof(UBX_ID); i++) {                        
        Serial2.write( pgm_read_byte(UBX_ID+i) );
        }
  Ublox_serial2(500);
}
//Initialization of the ublox M8N with binary commands
void Init_ublox(void){
  //send configuration data in UBX protocol 
  Serial.print("Set ublox UBX_OUT ");     
  for(int i = 0; i < sizeof(UBLOX_UBX_OUT); i++) {                        
        Serial2.write( pgm_read_byte(UBLOX_UBX_OUT+i) );
        }
  Ublox_serial2(500);
  if(config.dynamic_model==1){
      Serial.print("Set ublox UBX_SEA ");
      for(int i = 0; i < sizeof(UBX_SEA); i++) {                        
        Serial2.write( pgm_read_byte(UBX_SEA+i) );
        }
      Ublox_serial2(500); 
  }
  if(config.dynamic_model==2){
      Serial.print("Set ublox UBX_AUTOMOTIVE ");
      for(int i = 0; i < sizeof(UBX_AUTOMOTIVE); i++) {                        
        Serial2.write( pgm_read_byte(UBX_AUTOMOTIVE+i) );
        }
      Ublox_serial2(500); 
  }
  if(config.gnss=3){
      Serial.print("Set ublox UBX_GNSS3 : GPS, GLONAS & GALILEO ");
      for(int i = 0; i < sizeof(UBX_GNSS3); i++) {                        
        Serial2.write( pgm_read_byte(UBX_GNSS3+i) );
        }
      Ublox_serial2(500); 
      Serial.print("Check MON_GNSS settings ");
      for(int i = 0; i < sizeof(UBX_MON_GNSS); i++) {                        
        Serial2.write( pgm_read_byte(UBX_MON_GNSS+i) );
        }
      Ublox_serial2(500); 
  }
  //Ask_ID();  //Works only if M8N has sw-version 3, beitian BN180 has 2.01 !!!
  Serial.print("Set ublox NAV_PVT_ON ");   
  for(int i = 0; i < sizeof(UBLOX_UBX_NAVPVT_ON); i++) {                        
        Serial2.write( pgm_read_byte(UBLOX_UBX_NAVPVT_ON+i) );
        //delay(5); // simulating a 38400baud pace (or less), otherwise commands are not accepted by the device.
        }
  Ublox_serial2(500); 
  Serial.print("Set ublox to 19200BD "); 
  for(int i = 0; i < sizeof(UBLOX_UBX_BD19200); i++) {                        
        Serial2.write( pgm_read_byte(UBLOX_UBX_BD19200+i) );
        //delay(5); // simulating a 38400baud pace (or less), otherwise commands are not accepted by the device.
        } 
      
  //Ublox_serial2(500);   
  /*Print info about length struct
  static int payloadSize = sizeof(NAV_POSLLH);
  Serial.print("NAV_POSLLH ");
  Serial.println(payloadSize);
  payloadSize = sizeof(NAV_STATUS);
  Serial.print("NAV_NAV_STATUS ");
  Serial.println(payloadSize);
  payloadSize = sizeof(NAV_PVT);
  Serial.print("NAV_PVT ");
  Serial.println(payloadSize);
  */
  Serial2.flush();
  //delay(15);
  Serial2.begin(19200,SERIAL_8N1, RXD2, TXD2);//in Init_ublox last command is change baudrate to 19200, necessary for 10 Hz !!!
  Ublox_serial2(500);   
  
}
//Initialization of the ublox M8N  rate with binary commands, choice between 1..4
void Set_rate_ublox(int rate){
  int sample_rate=2;
  switch(rate){
    case 1:sample_rate=2;break;
    case 5:sample_rate=3;break;
    case 10:sample_rate=4;break;
    case 2:sample_rate=5;break;
    case 18:sample_rate=6;break;
    default:sample_rate=2;
    config.sample_rate=1;
  }
  Serial.print("Set rate Ublox ");
  for(int i = (sample_rate*14-14); i < sample_rate*14; i++) {                        
        Serial2.write( pgm_read_byte(UBLOX_RATE+i) );
        
        //delay(5); // simulating a 38400baud pace (or less), otherwise commands are not accepted by the device.
        }
  Ublox_serial2(500);      
}
void Set_GPS_Time(void){       
        // convert a date and time into unix time, offset 1970
        my_time.Second = ubxMessage.navPvt.second;
        my_time.Hour = ubxMessage.navPvt.hour;
        my_time.Minute = ubxMessage.navPvt.minute;
        my_time.Day = ubxMessage.navPvt.day;
        my_time.Month = ubxMessage.navPvt.month;      // months start from 0, so deduct 1
        my_time.Year = ubxMessage.navPvt.year - 1970; // years since 1900, so deduct 1900
        unix_timestamp =  makeTime(my_time);
        struct timeval tv = { .tv_sec = (unix_timestamp+config.timezone*3600), .tv_usec = 0 };//timezone aanpassen voor Belgie, UTC + 3600 s, zomertijd = UTC + 7200 !!
        settimeofday(&tv, NULL);
        //setenv("TZ", "CET-1", 1);//timezone aanpassen voor Belgie, GMT + 1 uur
        getLocalTime(&tmstruct);//was tmstruct,5000 ???
        Serial.printf("\nNow is : %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct.tm_year)+1900,( tmstruct.tm_mon)+1, tmstruct.tm_mday,tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
        Serial.println("GPS Time is set");
}
// Reads in bytes from the GPS module and checks to see if a valid message has been constructed.
// Returns the type of the message found if successful, or MT_NONE if no message was found.
// After a successful return the contents of the ubxMessage union will be valid, for the 
// message type that was found. Note that further calls to this function can invalidate the
// message content, so you must use the obtained values before calling this function again.
void AddString(void){
        strcat(dataStr, Buffer);//add it onto the end
        strcat(dataStr, ":"); //append the delimeter
        }
int processGPS() {
  static int fpos = 0;
  static unsigned char checksum[2];
  static byte currentMsgType = MT_NONE;
  static int payloadSize = sizeof(UBXMessage);
  
  while ( Serial2.available() ) {
    
    byte c = Serial2.read();    
    //Serial.write(c);
    
    if ( fpos < 2 ) {
      // For the first two bytes we are simply looking for a match with the UBX header bytes (0xB5,0x62)
      if ( c == UBX_HEADER[fpos] )
        fpos++;
      else
      //Serial.print(fpos);
        fpos = 0; // Reset to beginning state.
    }
    else {
      // If we come here then fpos >= 2, which means we have found a match with the UBX_HEADER
      // and we are now reading in the bytes that make up the payload.
      
      // Place the incoming byte into the ubxMessage struct. The position is fpos-2 because
      // the struct does not include the initial two-byte header (UBX_HEADER).
      // the struct does not include the 2 last bytes which are the checksums
      // checksums are not placed in the ubxMessage !!!
      if ( (fpos-2) < payloadSize )
        ((unsigned char*)(&ubxMessage))[fpos-2] = c;

      fpos++;
      
      if ( fpos == 4 ) {
        // We have just received the second byte of the message type header, 
        // so now we can check to see what kind of message it is.
        if ( compareMsgHeader(NAV_PVT_HEADER) ) {
          currentMsgType = MT_NAV_PVT;
          payloadSize = sizeof(NAV_PVT);
          //Serial.print("NAV_PVT\n");
        }
        else if ( compareMsgHeader(MON_GNSS_HEADER) ) {
          currentMsgType = MT_MON_GNSS;
          payloadSize = sizeof(MON_GNSS);
          Serial.println("MT_MON_GNSS\n");
        }
        /*
        else if ( compareMsgHeader(NAV_STATUS_HEADER) ) {
          currentMsgType = MT_NAV_STATUS;
          payloadSize = sizeof(NAV_STATUS);
          Serial.println("NAV_STATUS\n");
        }
        else if ( compareMsgHeader(NAV_ODO_HEADER) ) {
          currentMsgType = MT_NAV_ODO;
          payloadSize = sizeof(NAV_ODO);
          Serial.println("NAV_ODO");
        }
         else if ( compareMsgHeader(NAV_ACK_HEADER) ) {
          currentMsgType = MT_NAV_ACK;
          payloadSize = sizeof(NAV_ACK);
          //Serial.println("NAV_ACK");
        }
        */
        else {
          // unknown message type, bail
          fpos = 0;
          continue;
        }
      }

      if ( fpos == (payloadSize+2) ) {
        // All payload bytes have now been received, so we can calculate the 
        // expected checksum value to compare with the next two incoming bytes.
        calcChecksum(checksum, payloadSize);
      }
      else if ( fpos == (payloadSize+3) ) {
        // First byte after the payload, ie. first byte of the checksum.
        // Does it match the first byte of the checksum we calculated?
        checksumA=c;
        if ( c != checksum[0] ) {
          static int count=0;//ignore first checksum fail !!
          // Checksum doesn't match, reset to beginning state and try again.
          //if(currentMsgType == MT_NAV_ACK) Serial.print("ACK");
          Serial.println("ChecksumNIO");
          if ((sdOK==true)&(Time_Set_OK==true)&(count>1)){
              logERR("ChecksumNIO\n");
              //appendFile(SD,filenameERR, "ChecksumNIO\n");
              }
          count++;
          fpos = 0; 
        }
      }
      else if ( fpos == (payloadSize+4) ) {
        // Second byte after the payload, ie. second byte of the checksum.
        // Does it match the second byte of the checksum we calculated?
        fpos = 0; // We will reset the state regardless of whether the checksum matches.
        checksumB=c;
        if ( c == checksum[1] ) {
          // Checksum matches, we have a valid message.
          return currentMsgType; 
        }
      }
      else if ( fpos > (payloadSize+4) ) {
        // We have now read more bytes than both the expected payload and checksum 
        // together, so something went wrong. Reset to beginning state and try again.
        fpos = 0;
      }
    }
  }
  return MT_NONE;
}


