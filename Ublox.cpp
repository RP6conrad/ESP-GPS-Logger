#include "Ublox.h"
#include "Rtos5.h"
int Time_Set_OK;
UBXMessage ubxMessage = {000000000000};//definition here, declaration in ublox.h !!
struct tm tmstruct ;

tmElements_t my_time;  // time elements structure

time_t unix_timestamp; // a timestamp

void Ublox_on(){
   //F(19, HIGH);//Groene LED
   digitalWrite(25, HIGH); 
   digitalWrite(26, HIGH);
   digitalWrite(27, HIGH);
   delay(20);
}
void Ublox_off(){
  //digitalWrite(19, LOW);//Groene LED uit
  digitalWrite(25, LOW);
  digitalWrite(26, LOW);
  digitalWrite(27, LOW);
}
void Ublox_serial2(int delay_ms){
 //Serial2.flush();//empty buffer 
 for(int i=0;i<delay_ms;i++){
    int msgType = processGPS();
     if ( msgType == MT_NAV_ACK){
          Serial.print(" ACK ");
          Serial.print (ubxMessage.navAck.msg_cls);
          Serial.println (ubxMessage.navAck.msg_id);
          }
     if ( msgType == MT_NAV_NACK){
          Serial.print(" NACK ");
          Serial.print (ubxMessage.navNack.msg_cls);
          Serial.println (ubxMessage.navNack.msg_id);
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
          Serial.print("GNSS= :");
          Serial.print (ubxMessage.monGNSS.supported_Gnss);
          Serial.print (ubxMessage.monGNSS.default_Gnss);
          Serial.println (ubxMessage.monGNSS.enabled_Gnss);
          }
      if ( msgType == MT_MON_VER){
          Serial.print("SW Ublox=");
          Serial.println(ubxMessage.monVER.swVersion);
          Serial.print ("HW Ublox=");
          Serial.println (ubxMessage.monVER.hwVersion);
          }    
     delay(2);   
     } 
}
void Ask_ID(void){
  Serial.print("Ask ublox Unique ID ");     
  for(int i = 0; i < sizeof(UBX_ID); i++) {                        
        Serial2.write( pgm_read_byte(UBX_ID+i) );
        }
  Ublox_serial2(100);
}
#if !defined(UBLOX_M10)
//Initialization of the ublox M8N with binary commands
void Init_ublox(void){
  int wait=250;
  //send configuration data in UBX protocol 
  Serial.println("Set ublox UBX_OUT ");     
  for(int i = 0; i < sizeof(UBLOX_UBX_OUT); i++) {                        
        Serial2.write( pgm_read_byte(UBLOX_UBX_OUT+i) );
        }
  Ublox_serial2(wait); 
  if(config.dynamic_model==1){
      Serial.println("Set ublox UBX_SEA ");
      for(int i = 0; i < sizeof(UBX_SEA); i++) {                        
        Serial2.write( pgm_read_byte(UBX_SEA+i) );
        }
  Ublox_serial2(wait);       
  }
  if(config.dynamic_model==2){
      Serial.println("Set ublox UBX_AUTOMOTIVE ");
      for(int i = 0; i < sizeof(UBX_AUTOMOTIVE); i++) {                        
        Serial2.write( pgm_read_byte(UBX_AUTOMOTIVE+i) );
        }
      Ublox_serial2(wait); 
      }
  if(config.gnss==3){
      Serial.println("Set ublox UBX_GNSS3 : GPS, GLONAS & GALILEO ");
      for(int i = 0; i < sizeof(UBX_GNSS3); i++) {                        
        Serial2.write( pgm_read_byte(UBX_GNSS3+i) );
        }
      Ublox_serial2(wait); 
      }
  
  if(config.gnss==4){
      Serial.println("Set ublox UBX_GNSS3 : GPS, GLONAS & BEIDOU ");
      for(int i = 0; i < sizeof(UBX_GNSS3_BEIDOU); i++) {                        
        Serial2.write( pgm_read_byte(UBX_GNSS3_BEIDOU+i) );
        }
      Ublox_serial2(wait); 
      }
  Serial.println("Check MON_GNSS settings ");
  for(int i = 0; i < sizeof(UBX_MON_GNSS); i++) {                        
      Serial2.write( pgm_read_byte(UBX_MON_GNSS+i) );
      }
  Ublox_serial2(wait); 
  
  Serial.println("Check UBX_MON_VER ");     
   for(int i = 0; i < sizeof(UBX_MON_VER); i++) {                        
        Serial2.write( pgm_read_byte(UBX_MON_VER+i) );        
        }                 
  Ublox_serial2(wait); 
  //Ask_ID();  //Works only if M8N has sw-version 3, beitian BN180 has 2.01 !!!
  Serial.println("Set ublox NAV_PVT_ON ");   
  for(int i = 0; i < sizeof(UBLOX_UBX_NAVPVT_ON); i++) {                        
        Serial2.write( pgm_read_byte(UBLOX_UBX_NAVPVT_ON+i) );
        }
  Ublox_serial2(wait);         
  Serial.println("Set ublox NAV_DOP_ON ");   
  for(int i = 0; i < sizeof(UBLOX_UBX_NAVDOP_ON); i++) {                        
        Serial2.write( pgm_read_byte(UBLOX_UBX_NAVDOP_ON+i) );
        }
  Ublox_serial2(wait); 
  Serial.println("Set ublox to 38400BD "); 
  for(int i = 0; i < sizeof(UBLOX_UBX_BD38400); i++) {                        
        Serial2.write( pgm_read_byte(UBLOX_UBX_BD38400+i) );
        //delay(5); // simulating a 38400baud pace (or less), otherwise commands are not accepted by the device.
        } 
  Serial2.flush();
  Serial2.begin(38400,SERIAL_8N1, RXD2, TXD2);//in Init_ublox last command is change baudrate to 19200, necessary for 10 Hz  NAV_PVT + NAV_DOP!!!
  Ublox_serial2(wait);     
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
        }
  Ublox_serial2(500);      
}
#endif
#if defined(UBLOX_M10)
//Initialization of the ublox M10N with binary commands
void Init_ubloxM10(void){
  int wait=250;
  //send configuration data in UBX protocol 
  Serial.println("Set ublox M10 NMEA OFF ");     
  for(int i = 0; i < sizeof(UBLOX_M10_NMEA_OFF); i++) {                        
        Serial2.write( pgm_read_byte(UBLOX_M10_NMEA_OFF+i) );
        }
  Ublox_serial2(wait); 
   if(config.gnss<4){
      Serial.println("Set ublox M10 BEIDOU OFF ");     
      for(int i = 0; i < sizeof(UBLOX_M10_BEIDOU_OFF); i++) {                        
            Serial2.write( pgm_read_byte(UBLOX_M10_BEIDOU_OFF+i) );
            }
      Ublox_serial2(wait); 
      }
     if(config.gnss==3){          
      Serial.println("Set ublox M10 GALILEO ON ");     
      for(int i = 0; i < sizeof(UBLOX_M10_GAL_ON); i++) {                        
            Serial2.write( pgm_read_byte(UBLOX_M10_GAL_ON+i) );
            }
      Ublox_serial2(wait);  
      Serial.println("Set ublox M10 GLONAS ON ");     
      for(int i = 0; i < sizeof(UBLOX_M10_GLONAS_ON); i++) {                        
            Serial2.write( pgm_read_byte(UBLOX_M10_GLONAS_ON+i) );
            }
      Ublox_serial2(wait);      
      } 
      if(config.gnss==2){          
      Serial.println("Set ublox M10 GALILEO OFF ");     
      for(int i = 0; i < sizeof(UBLOX_M10_GAL_OFF); i++) {                        
            Serial2.write( pgm_read_byte(UBLOX_M10_GAL_OFF+i) );
            }
      Ublox_serial2(wait);  
      Serial.println("Set ublox M10 GLONAS ON ");     
      for(int i = 0; i < sizeof(UBLOX_M10_GLONAS_ON); i++) {                        
            Serial2.write( pgm_read_byte(UBLOX_M10_GLONAS_ON+i) );
            }
      Ublox_serial2(wait);      
      }     
  Serial.println("Set ublox M10 UBX On ");     
  for(int i = 0; i < sizeof(UBLOX_M10_UBX); i++) {                        
        Serial2.write( pgm_read_byte(UBLOX_M10_UBX+i) );
        }
  Ublox_serial2(wait);    
  Serial.println("Set ublox M10 NAV_PVT_ON ");   
  for(int i = 0; i < sizeof(UBLOX_M10_NAV_PVT); i++) {                        
        Serial2.write( pgm_read_byte(UBLOX_M10_NAV_PVT+i) );
        }
  Ublox_serial2(wait);         
  Serial.println("Set ublox M10 NAV_DOP_ON ");   
  for(int i = 0; i < sizeof(UBLOX_M10_NAV_DOP); i++) {                        
        Serial2.write( pgm_read_byte(UBLOX_M10_NAV_DOP+i) );
        }
  Ublox_serial2(wait);     
  Serial.println("Check UBX_MON_VER ");  //does this work for the M10 ??   
   for(int i = 0; i < sizeof(UBX_MON_VER); i++) {                        
        Serial2.write( pgm_read_byte(UBX_MON_VER+i) );        
        }           
  Ublox_serial2(wait); 
  Serial.println("Check MON_GNSS settings ");
  for(int i = 0; i < sizeof(UBX_MON_GNSS); i++) {                        
      Serial2.write( pgm_read_byte(UBX_MON_GNSS+i) );
      }
  Ublox_serial2(wait); 
  Serial.println("Set ublox M10 to 19200BD "); 
  for(int i = 0; i < sizeof(UBLOX_M10_UBX_BD19200); i++) {                        
        Serial2.write( pgm_read_byte(UBLOX_M10_UBX_BD19200+i) );
        //delay(5); // simulating a 38400baud pace (or less), otherwise commands are not accepted by the device.
        } 
  Serial2.flush();
  Serial2.begin(19200,SERIAL_8N1, RXD2, TXD2);//in Init_ublox last command is change baudrate to 38400, necessary for 10 Hz  NAV_PVT + NAV_DOP!!!
  Ublox_serial2(wait);        
}
//Initialization of the ublox M10N  rate with binary commands, choice between 1..4
void Set_rate_ubloxM10(int rate){
  int sample_rate=2;
  switch(rate){
    case 1:sample_rate=1;break;
    case 2:sample_rate=2;break;
    case 5:sample_rate=3;break;
    case 10:sample_rate=4;break;
    default:sample_rate=1;
    config.sample_rate=1;
    }
  Serial.print("Set rate Ublox M10");
  for(int i = (sample_rate*18-18); i < sample_rate*18; i++) {                        
        Serial2.write( pgm_read_byte(UBLOX_M10_RATE+i) );
        }
  Ublox_serial2(500);      
}
#endif
void Set_GPS_Time(int time_offset){  
        setenv("TZ", "GMT0", 0);//timezone UTC = GMT0
        tzset();     //timezone is set, but ESP32 seems not to accept these time settings ????
        // convert a date and time into unix time, offset 1970
        my_time.Second = ubxMessage.navPvt.second;
        my_time.Hour = ubxMessage.navPvt.hour;
        my_time.Minute = ubxMessage.navPvt.minute;
        my_time.Day = ubxMessage.navPvt.day;
        my_time.Month = ubxMessage.navPvt.month;      
        my_time.Year = ubxMessage.navPvt.year - 1970; // years since 1970, so deduct 1970
        unix_timestamp =  makeTime(my_time);
        //struct timeval tv = { .tv_sec = (unix_timestamp+config.timezone*3600), .tv_usec = 0 };//timezone aanpassen voor Belgie, UTC + 3600 s, zomertijd = UTC + 7200 !!
        struct timeval tv = { .tv_sec = (unix_timestamp+time_offset*3600), .tv_usec = 0 };  //clean utc time !!     
        settimeofday(&tv, NULL);
        getLocalTime(&tmstruct);
        Serial.printf("\nNow is : %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct.tm_year)+1900,( tmstruct.tm_mon)+1, tmstruct.tm_mday,tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
        Serial.println("GPS Time is set");
}

// The last two bytes of the message is a checksum value, used to confirm that the received payload is valid.
// The procedure used to calculate this is given as pseudo-code in the uBlox manual.
void calcChecksum(unsigned char* CK,int msgType, int msgSize) {
  memset(CK, 0, 2);
  for (int i = 0; i < msgSize; i++) {
    if(msgType==MT_NAV_PVT) {CK[0] += ((unsigned char*)(&ubxMessage.navPvt))[i];}
    else if(msgType==MT_NAV_DOP) {CK[0] += ((unsigned char*)(&ubxMessage.navDOP))[i];} 
    else if(msgType==MT_MON_GNSS){CK[0] += ((unsigned char*)(&ubxMessage.monGNSS))[i];} 
    else if(msgType==MT_MON_VER){CK[0] += ((unsigned char*)(&ubxMessage.monVER))[i];} 
    else if(msgType==MT_NAV_ACK){CK[0] += ((unsigned char*)(&ubxMessage.navAck))[i];} 
    else if(msgType==MT_NAV_NACK){CK[0] += ((unsigned char*)(&ubxMessage.navNack))[i];} 
    else {CK[0] += ((unsigned char*)(&ubxMessage))[i];}
    CK[1] += CK[0];
  }
}
// Compares the first two bytes of the ubxMessage struct with a specific message header.
// Returns true if the two bytes match (0xB5 0x62).
boolean compareMsgHeader(const unsigned char* msgHeader) {
  unsigned char* ptr = (unsigned char*)(&ubxMessage.navDummy);
  return ptr[0] == msgHeader[0] && ptr[1] == msgHeader[1];
}
// Reads in bytes from the GPS module and checks to see if a valid message has been constructed.
// Returns the type of the message found if successful, or MT_NONE if no message was found.
// After a successful return the contents of the ubxMessage union will be valid, for the 
// message type that was found. As now every message has its own struct, further calls to this function can invalidate the
// message content if the message was the same, so you must use the obtained values before calling this function again.

int processGPS() {
  static int fpos = 0;
  static unsigned char checksum[2];
  static byte currentMsgType = MT_NONE;
  static int payloadSize = sizeof(ubxMessage.navDummy);
  static uint16_t len;
  while ( Serial2.available() ) {
    byte c = Serial2.read();    
    //Serial.write(c);
    if ( fpos < 2 ) {
      // For the first two bytes we are simply looking for a match with the UBX header bytes (0xB5,0x62)
      if ( c == UBX_HEADER[fpos] )
        fpos++;
      else
        fpos = 0; // Reset to beginning state.
    }
    else {
      // If we come here then fpos >= 2, which means we have found a match with the UBX_HEADER
      // and we are now reading in the bytes that make up the payload.
      // Place the incoming byte into the ubxMessage.navDummy struct. The position is fpos-2 because
      // the struct does not include the initial two-byte header (UBX_HEADER).
      // the struct does not include the 2 last bytes which are the checksums
      // checksums are not placed in the ubxMessage !!!
      if (((fpos-2) < payloadSize)&(fpos<4)){((unsigned char*)(&ubxMessage.navDummy))[fpos-2] = c;} 
      if(fpos==3) {
        // We have just received the second byte of the message type header, 
        // so now we can check to see what kind of message it is.
        //We have to restore cls and id to the correct substructure
        if ( compareMsgHeader(NAV_PVT_HEADER) ) {
          currentMsgType = MT_NAV_PVT;
          payloadSize = sizeof(NAV_PVT);
          ubxMessage.navPvt.cls=ubxMessage.navDummy.cls;
          ubxMessage.navPvt.id=ubxMessage.navDummy.id;
          //Serial.print("hPVT ");
        }
        else if ( compareMsgHeader(MON_GNSS_HEADER) ) {
          currentMsgType = MT_MON_GNSS;
          payloadSize = sizeof(MON_GNSS);
          ubxMessage.monGNSS.cls=ubxMessage.navDummy.cls;
          ubxMessage.monGNSS.id=ubxMessage.navDummy.id;
          //Serial.println("MT_MON_GNSS\n");
        }
        else if ( compareMsgHeader(NAV_DOP_HEADER) ) {
          currentMsgType = MT_NAV_DOP;
          payloadSize = sizeof(NAV_DOP);
          ubxMessage.navDOP.cls=ubxMessage.navDummy.cls;
          ubxMessage.navDOP.id=ubxMessage.navDummy.id;
          //Serial.print("hDOP ");
        }
        else if ( compareMsgHeader(MON_VER_HEADER) ) {
          currentMsgType = MT_MON_VER;
          payloadSize = sizeof(MON_VER);
          ubxMessage.monVER.cls=ubxMessage.navDummy.cls;
          ubxMessage.monVER.id=ubxMessage.navDummy.id;
          //Serial.println("MT_MON_VER\n");
        }
        else if ( compareMsgHeader(NAV_ACK_HEADER) ) {
          currentMsgType = MT_NAV_ACK;
          payloadSize = sizeof(NAV_ACK);
          ubxMessage.navAck.cls=ubxMessage.navDummy.cls;
          ubxMessage.navAck.id=ubxMessage.navDummy.id;
          //Serial.println("NAV_ACK\n");
        }
        else if ( compareMsgHeader(NAV_NACK_HEADER) ) {
          currentMsgType = MT_NAV_NACK;
          payloadSize = sizeof(NAV_NACK);
          ubxMessage.navNack.cls=ubxMessage.navDummy.cls;
          ubxMessage.navNack.id=ubxMessage.navDummy.id;
          //Serial.println("NAV_NACK\n");
        }
        else {
          // unknown message type, bail
          currentMsgType = MT_NONE;
          fpos = 0;
          continue;
        }
      }
      if (((fpos-2) < payloadSize)&(fpos>=4)){
        if(currentMsgType==MT_NAV_PVT) {((unsigned char*)(&ubxMessage.navPvt))[fpos-2] = c;} 
        if(currentMsgType==MT_NAV_DOP) {((unsigned char*)(&ubxMessage.navDOP))[fpos-2] = c;} 
        if(currentMsgType==MT_MON_GNSS) {((unsigned char*)(&ubxMessage.monGNSS))[fpos-2] = c;} 
        if(currentMsgType==MT_MON_VER) {((unsigned char*)(&ubxMessage.monVER))[fpos-2] = c;} 
        if(currentMsgType==MT_NAV_ACK) {((unsigned char*)(&ubxMessage.navAck))[fpos-2] = c;} 
        if(currentMsgType==MT_NAV_NACK) {((unsigned char*)(&ubxMessage.navNack))[fpos-2] = c;} 
      }
       if (fpos==6){
        if(currentMsgType==MT_NAV_PVT){ubxMessage.navPvt.len=payloadSize-6;}
        if(currentMsgType==MT_NAV_DOP){ubxMessage.navDOP.len=payloadSize-6;}
      }
      fpos++;
      if ( fpos == (payloadSize) ) {//was (payloadSize+2)
      // All payload bytes have now been received, so we can calculate the 
      // expected checksum value to compare with the next two incoming bytes.
      // checksum has to calculated out of the correct substructure !!!
        calcChecksum(checksum,currentMsgType,payloadSize-2);//was payload !!
      }
      else if ( fpos == (payloadSize+1) ) {//was (payloadSize+3)
        // First byte after the payload, ie. first byte of the checksum.
        // Does it match the first byte of the checksum we calculated?
        if ( c != checksum[0] ) {
          // Checksum doesn't match, reset to beginning state and try again.
          // Serial.println("CkA NIO");
          if ((Time_Set_OK==true)&(nav_pvt_message_nr>10)){
              logERR("ChecksumA_NIO\n");
              }
          fpos = 0; 
        }
      }
      else if ( fpos == (payloadSize+2) ) {//was (payloadSize+4)
        // Second byte after the payload, ie. second byte of the checksum.
        // Does it match the second byte of the checksum we calculated?
        fpos = 0; // We will reset the state regardless of whether the checksum matches.
        if ( c == checksum[1] ) {
          // Checksum matches, we have a valid message.
          return currentMsgType; 
        }
        else{ if ((Time_Set_OK==true)&(nav_pvt_message_nr>10)){
              //Serial.println("CkB NIO");
              logERR("ChecksumB_NIO\n");
              }
            }
      }    
      else if ( fpos > (payloadSize+2) ) {//was (payloadSize+4)
        // We have now read more bytes than both the expected payload and checksum 
        // together, so something went wrong. Reset to beginning state and try again.
        fpos = 0;
      }
    }
  }
  return MT_NONE;
}


