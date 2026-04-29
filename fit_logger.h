#ifndef FIT_LOGGER_H
#define FIT_LOGGER_H

#include <Arduino.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

// extern GPS-data van Ublox:
extern UBXMessage ubxMessage;   // gedeclareerd in Ublox.h

// FIT epoch offset (1989-12-31 -> Unix)
static const uint32_t FIT_EPOCH_OFFSET = 631065600UL;

// ---------- CRC ----------
static const uint16_t fit_crc_table[16] = {
  0x0000, 0xCC01, 0xD801, 0x1400,
  0xF001, 0x3C00, 0x2800, 0xE401,
  0xA001, 0x6C00, 0x7800, 0xB401,
  0x5000, 0x9C01, 0x8801, 0x4400
};

static uint16_t fit_crc16_step(uint16_t crc, uint8_t byte) {
  uint16_t tmp = fit_crc_table[byte & 0x0F];
  crc = (crc >> 4) ^ tmp;
  tmp = fit_crc_table[(byte >> 4) & 0x0F];
  crc = (crc >> 4) ^ tmp;
  return crc;
}

static uint16_t fit_crc_compute(const uint8_t *buf, size_t len) {
  uint16_t crc = 0;
  for (size_t i = 0; i < len; i++) crc = fit_crc16_step(crc, buf[i]);
  return crc;
}

// ---------- Helpers ----------
static void write_le_u8(File &f, uint8_t v)   { f.write(&v, 1); }
static void write_le_u16(File &f, uint16_t v) { f.write((uint8_t*)&v, 2); }
static void write_le_u32(File &f, uint32_t v) { f.write((uint8_t*)&v, 4); }

// ---------- Globale state ----------
static uint32_t fit_body_size = 0;
static bool fit_start(File &file) {
  if (!file) return false;
  fit_body_size = 0;

  // --- Tijdbron kiezen ---
  uint32_t fit_time = FIT_EPOCH_OFFSET;
  bool valid_gps_time = (ubxMessage.navPvt.valid & 0x04); // bit 2 = validDate/Time

  if (valid_gps_time) {
    // Gebruik GPS-UTC tijd
    struct tm t {};
    t.tm_year = ubxMessage.navPvt.year - 1900;
    t.tm_mon  = ubxMessage.navPvt.month - 1;
    t.tm_mday = ubxMessage.navPvt.day;
    t.tm_hour = ubxMessage.navPvt.hour;
    t.tm_min  = ubxMessage.navPvt.minute;
    t.tm_sec  = ubxMessage.navPvt.second;
    time_t unix_time = mktime(&t);
    if (unix_time > 946684800 && unix_time < 4102444800) {
      fit_time = (uint32_t)unix_time - FIT_EPOCH_OFFSET;
    }
  } else {
    // Fallback: gebruik systeemklok indien redelijk
    time_t now = time(nullptr);
    if (now > 946684800 && now < 4102444800) {
      fit_time = (uint32_t)now-FIT_EPOCH_OFFSET;
    }
  }

  // --- Header placeholder ---
  uint8_t header[14] = {14, 16, 52, 8, 0, 0, 0, 0, '.', 'F', 'I', 'T', 0, 0};
  file.write(header, 14);

  // --- File ID definition ---
  const uint8_t defFileId[] = {
    0x40, 0x00, 0x00,
    0x00, 0x00,
    5,
    0,1,0x0A,
    1,2,0x84,
    2,2,0x84,
    3,4,0x86,
    4,4,0x86
  };
  file.write(defFileId, sizeof(defFileId));
  fit_body_size += sizeof(defFileId);

  // --- File ID data ---
  write_le_u8(file, 0x00);        // data header
  write_le_u8(file, 4);           // type = activity
  write_le_u16(file, 1);          // manufacturer
  write_le_u16(file, 1);          // product
  write_le_u32(file, 1234);       // serial
  write_le_u32(file, fit_time);   // timestamp (veilig)
  fit_body_size += 1 + 1 + 2 + 2 + 4 + 4;

  // --- Record definition (7 velden) ---
  const uint8_t defRecord[] = {
    0x41, 0x00, 0x00,
    0x01, 0x00,
    7,
    0,4,0x85,   // lat
    1,4,0x85,   // lon
    2,2,0x84,   // alt
    6,2,0x84,   // speed
    27,1,0x02,  // gps_accuracy
    29,1,0x02,  // satellites
    253,4,0x86  // timestamp
  };
  file.write(defRecord, sizeof(defRecord));
  fit_body_size += sizeof(defRecord);

  Serial.printf("FIT start: timestamp=%lu (valid_gps=%d)\n", fit_time, valid_gps_time);
  return true;
}


// ---------- Log ----------
static bool fit_log(File &file) {
  if (!file) return false;

  write_le_u8(file, 0x01); // data header

  // Positie in semicirkels
  double lat_deg = ubxMessage.navPvt.lat * 1e-7;
  double lon_deg = ubxMessage.navPvt.lon * 1e-7;
  int32_t lat = (int32_t)llround(lat_deg * ((double)(1LL << 31) / 180.0));
  int32_t lon = (int32_t)llround(lon_deg * ((double)(1LL << 31) / 180.0));

  // Hoogte (aanpassen als je echte altitude hebt)
  uint16_t alt_encoded = (uint16_t)round((0 + 500.0f) * 5.0f);

  // Snelheid (mm/s → cm/s)
  uint32_t spd32 = ubxMessage.navPvt.gSpeed;
  if (spd32 > 65535) spd32 = 65535;
  uint16_t spd = (uint16_t)spd32;

  // HDOP / Satellieten
  uint8_t gps_acc = (uint8_t)min(255, (int)(ubxMessage.navDOP.hDOP / 100));
  uint8_t sats = ubxMessage.navPvt.numSV;

  // Tijd naar FIT epoch
  struct tm t {};
  t.tm_year = ubxMessage.navPvt.year - 1900;
  t.tm_mon  = ubxMessage.navPvt.month - 1;
  t.tm_mday = ubxMessage.navPvt.day;
  t.tm_hour = ubxMessage.navPvt.hour;
  t.tm_min  = ubxMessage.navPvt.minute;
  t.tm_sec  = ubxMessage.navPvt.second;
  time_t unix_time = mktime(&t);
  uint32_t fit_time = (uint32_t)unix_time - FIT_EPOCH_OFFSET;

  // Schrijf record
  write_le_u32(file, lat);
  write_le_u32(file, lon);
  write_le_u16(file, alt_encoded);
  write_le_u16(file, spd);
  write_le_u8(file, gps_acc);
  write_le_u8(file, sats);
  write_le_u32(file, fit_time);

  fit_body_size += 1 + 4 + 4 + 2 + 2 + 1 + 1 + 4;
  return true;
}
static bool fit_finish(File &file) {
  if (!file) return false;

  // Zorg dat alles tot nu toe naar disk is geschreven
  file.flush();

  // Huidige lengte van bestand = header (14) + body
  uint32_t total_size = file.size();
  if (total_size < 14) return false;
  uint32_t body_size = total_size - 14;

  // --- Header ---
  uint8_t header[14];
  header[0] = 14;        // header length
  header[1] = 16;        // protocol version
  header[2] = 52;        // profile version LSB (2100)
  header[3] = 8;         // profile version MSB
  header[4] = (uint8_t)(body_size & 0xFF);
  header[5] = (uint8_t)((body_size >> 8) & 0xFF);
  header[6] = (uint8_t)((body_size >> 16) & 0xFF);
  header[7] = (uint8_t)((body_size >> 24) & 0xFF);
  header[8]  = '.'; header[9] = 'F'; header[10] = 'I'; header[11] = 'T';
  uint16_t hdr_crc = fit_crc_compute(header, 12);
  header[12] = hdr_crc & 0xFF;
  header[13] = hdr_crc >> 8;

  // --- File CRC berekenen ---
  uint16_t file_crc = 0;
  const size_t BUF = 512;
  uint8_t buf[BUF];
  uint32_t offset = 14;
  uint32_t remaining = body_size;

  while (remaining > 0) {
    size_t chunk = min((uint32_t)BUF, remaining);
    file.seek(offset);
    file.read(buf, chunk);
    for (size_t i = 0; i < chunk; i++) {
      file_crc = fit_crc16_step(file_crc, buf[i]);
    }
    offset += chunk;
    remaining -= chunk;
  }

  // --- Header en CRC schrijven ---
  file.seek(0);
  file.write(header, 14);
  file.seek(14 + body_size);
  write_le_u16(file, file_crc);
  file.flush();

  Serial.printf("FIT finish: body=%lu, CRC=0x%04X, headerCRC=0x%04X\n",
                body_size, file_crc, hdr_crc);

  return true;
}


#endif // FIT_LOGGER_H
