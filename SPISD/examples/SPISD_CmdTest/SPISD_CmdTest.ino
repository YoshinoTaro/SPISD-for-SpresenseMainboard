/*
 * SD Card Command Test
 * Copyright 2021 Yoshino Taro
 * License: GNU General Public License V3
 * (Because Arduino SD library is licensed with this.)
 */
#include <SPI.h>

#define CMD0   0x00
#define CMD8   0x08
#define CMD55  0x37
#define CMD58  0x3A

#define ACMD23 0x17
#define ACMD41 0x29

#define R1_READY_STATE     0x00
#define R1_IDLE_STATE      0x01
#define R1_ILLEGAL_COMMAND 0x04

#define SD_INIT_TIMEOUT 2000

#define SD_CARD_TYPE_SD1 0x01
#define SD_CARD_TYPE_SD2 0x02
#define SD_CARD_TYPE_SDHC 0x03

uint8_t status_ = 0;
uint8_t inBlock_ = 0;
uint16_t offset_ = 0;
uint8_t type_ = 0;

SPISettings spi_setting = SPISettings(250000, MSBFIRST, SPI_MODE0);

SPIClass spi(5);

void readEnd() {
  if (inBlock_) {
    while (offset_++ < 514) spi.transfer(0xFF);
    inBlock_ = 0;
  }
}

uint8_t waitNotBusy(uint16_t timeoutMills) {
  uint16_t t0 = millis();
  do {
    if (SPI.transfer(0xFF) == 0xFF) {
      return true;
    }
  } while (((uint16_t)millis() - t0) < timeoutMills);
  return false;
}

uint8_t cardCommand(uint8_t cmd, uint32_t arg) {
  readEnd();
  waitNotBusy(300);
  spi.transfer(cmd | 0x40);
  for (int8_t s = 24; s >= 0; s -=8)
      spi.transfer(arg >> s);

  uint8_t crc = 0xFF;
  if (cmd == CMD0) crc = 0x95;
  if (cmd == CMD8) crc = 0x87;
  spi.transfer(crc);

  for (uint8_t i = 0; ((status_ = spi.transfer(0xFF)) & 0x80) && (i != 0xFF); ++i) {
    ;
  }
  return status_;
}

uint8_t cardAcmd(uint8_t cmd, uint32_t arg) {
  cardCommand(CMD55, 0);
  return cardCommand(cmd, arg);
}

void setup() {
  Serial.begin(115200);

  inBlock_ = 0;
  uint16_t t0 = (uint16_t)millis();
  uint32_t arg;

  Serial.println("spi5 try to connect");
  spi.begin();
  
  spi.beginTransaction(spi_setting);
  for (uint8_t i = 0; i < 10; ++i)
    spi.transfer(0xFF);
  spi.endTransaction();

  Serial.println("Try CMD0");
  spi.beginTransaction(spi_setting);
  while ((status_ = cardCommand(CMD0, 0)) != R1_IDLE_STATE) {
    if (((uint16_t)(millis() - t0)) > SD_INIT_TIMEOUT) {
      Serial.println("CMD0 failed.");
      spi.endTransaction();  
      return;
    }
  }
  Serial.println("CMD0 success");
  spi.endTransaction();  

  Serial.println("Try CMD8");
  spi.beginTransaction(spi_setting);
  if ((cardCommand(CMD8, 0x1AA) & R1_ILLEGAL_COMMAND)) {
    Serial.println("SD_CARD_TYPE is SD1");
    type_ = SD_CARD_TYPE_SD1;
  } else {
    for (uint8_t i = 0; i < 4; ++i) {
      status_ = spi.transfer(0xFF);
      //Serial.println(status_, HEX);
    }
    if (status_ != 0xAA) {
      Serial.println("CMD8 failed.");
      spi.endTransaction();  
      return;
    }
    Serial.println("SD_CARD_TYPE is SD2");
    type_ = SD_CARD_TYPE_SD2;
  }
  spi.endTransaction();  
  Serial.println("CMD8 success");

  arg = type_ == SD_CARD_TYPE_SD2 ? 0x40000000 : 0;
  
  Serial.println("Try ACMD41");
  spi.beginTransaction(spi_setting);
  while ((status_ = cardAcmd(ACMD41, arg)) != R1_READY_STATE) {
    if (((uint16_t)(millis() - t0)) > SD_INIT_TIMEOUT) {
      Serial.println("ACMD41 failed.");
      spi.endTransaction();  
      return;     
    }
  }
  spi.endTransaction();  
  Serial.println("ACMD41 success");

  Serial.println("Try CMD58");
  spi.beginTransaction(spi_setting);
  if (type_ == SD_CARD_TYPE_SD2) {
    if (cardCommand(CMD58, 0)) {
      Serial.println("CMD58 failed.");
      spi.endTransaction();      
    }
    if ((spi.transfer(0xFF) & 0xC0) == 0xC0) {
      Serial.println("SD_CARD_TYPE is SDHC");
      type_ = SD_CARD_TYPE_SDHC;
    }
    for (uint8_t i = 0; i < 3; ++i) spi.transfer(0xFF);
  }
  spi.endTransaction();  
  Serial.println("CMD58 success");
  
  Serial.println("SDCard initilization success");
  spi_setting = SPISettings(4000000, MSBFIRST, SPI_MODE0);
}

void loop() {
  // put your main code here, to run repeatedly:

}
