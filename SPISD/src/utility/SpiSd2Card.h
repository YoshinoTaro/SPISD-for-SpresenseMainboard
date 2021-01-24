/** 
 * Arduino SdFat Library for SPRESENSE based on Arduino SdFat Library
 *
 * This file is part of the Arduino Sd2Card Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Arduino Sd2Card Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef SpiSd2Card_h
#define SpiSd2Card_h

#include "SpiSdInfo.h"
#include <Arduino.h>

class SPIClass;

#define SPI_FULL_SPEED    0
#define SPI_HALF_SPEED    1
#define SPI_QUARTER_SPEED 2

#define SD_INIT_TIMEOUT   2000
#define SD_ERASE_TIMEOUT 10000
#define SD_READ_TIMEOUT    300
#define SD_WRITE_TIMEOUT   600

/** timeout error for command CMD0 */
#define SD_CARD_ERROR_CMD0      0x01
/** CMD8 was not accepted - not a valid SD card*/
#define SD_CARD_ERROR_CMD8      0x02
/** card returned an error response for CMD17 (read block) */
#define SD_CARD_ERROR_CMD17     0x03
/** card returned an error response for CMD24 (write block) */
#define SD_CARD_ERROR_CMD24     0x04
/**  WRITE_MULTIPLE_BLOCKS command failed */
#define SD_CARD_ERROR_CMD25     0x05
/** card returned an error response for CMD58 (read OCR) */
#define SD_CARD_ERROR_CMD58     0x06
/** SET_WR_BLK_ERASE_COUNT failed */
#define SD_CARD_ERROR_ACMD23    0x07
/** card's ACMD41 initialization process timeout */
#define SD_CARD_ERROR_ACMD41    0x08
/** card returned a bad CSR version field */
#define SD_CARD_ERROR_BAD_CSD   0x09
/** erase block group command failed */
#define SD_CARD_ERROR_ERASE     0x0A
/** card not capable of single block erase */
#define SD_CARD_ERROR_ERASE_SINGLE_BLOCK  0x0B
/** Erase sequence timed out */
#define SD_CARD_ERROR_ERASE_TIMEOUT       0x0C
/** card returned an error token instead of read data */
#define SD_CARD_ERROR_READ      0x0D
/** read CID or CSD failed */
#define SD_CARD_ERROR_READ_REG  0x0E
/** timeout while waiting for start of read data */
#define SD_CARD_ERROR_READ_TIMEOUT        0x0F
/** card did not accept STOP_TRAN_TOKEN */
#define SD_CARD_ERROR_STOP_TRAN 0x10
/** card returned an error token as a response to a write operation */
#define SD_CARD_ERROR_WRITE     0x11
/** attempt to write protected block zero */
#define SD_CARD_ERROR_WRITE_BLOCK_ZERO    0x12
/** card did not go ready for a multiple block write */
#define SD_CARD_ERROR_WRITE_MULTIPLE      0x13
/** card returned an error to a CMD13 status check after a write */
#define SD_CARD_ERROR_WRITE_PROGRAMMING   0x14
/** timeout occurred during write programming */
#define SD_CARD_ERROR_WRITE_TIMEOUT       0x15
/** incorrect rate selected */
#define SD_CARD_ERROR_SCK_RATE  0X16

// card types
#define SD_CARD_TYPE_SD1  1
#define SD_CARD_TYPE_SD2  2
#define SD_CARD_TYPE_SDHC 3

class SpiSd2Card 
{
public:
  SpiSd2Card(SPIClass& spi)
    : spi_(spi), errorCode_(0), inBlock_(0)
     ,partialBlockRead_(0), type_(0) {}
  uint32_t cardSize(void);
  uint8_t erase(uint32_t firstBlock, uint32_t lastBlock);
  uint8_t eraseSingleBlockEnable(void);

  uint8_t errorCode(void) const { return errorCode_; }
  uint8_t errorData(void) const { return status_; }

  uint8_t init(void) { return init(SPI_FULL_SPEED); }
  uint8_t init(uint8_t sckRateID);

  void partialBlockRead(uint8_t value);
  uint8_t partialBlockRead(void) const {return partialBlockRead_;}
  uint8_t readBlock(uint32_t block, uint8_t* dst);
  uint8_t readData(uint32_t block, uint16_t offset, uint16_t count, uint8_t* dst);

  /* Read a cards CID register. The CID contains card identification
   * information such as Manufacturer ID, Product name, Product serial
   * number and Manufacturing date. */
  uint8_t readCID(cid_t* cid) { return readRegister(CMD10, cid); }

  /* Read a cards CSD register. The CSD contains Card-Specific Data that
   * provides information regarding access to the card's contents. */
  uint8_t readCSD(csd_t* csd) { return readRegister(CMD9, csd); }

  void readEnd(void);
  uint8_t setSckRate(uint8_t sckRateID);
  uint8_t setSpiClock(uint32_t clock);

  /** Return the card type: SD V1, SD V2 or SDHC */
  uint8_t type(void) const {return type_;}
  uint8_t writeBlock(uint32_t blockNumber, const uint8_t* src);
  uint8_t writeData(const uint8_t* src);
  uint8_t writeStart(uint32_t blockNumber, uint32_t eraseCount);
  uint8_t writeStop(void);

private:
  SPIClass& spi_;
  uint32_t block_;
  uint8_t chipSelectPin_;
  uint8_t errorCode_;
  uint8_t inBlock_;
  uint16_t offset_;
  uint8_t partialBlockRead_;
  uint8_t status_;
  uint8_t type_;

  uint8_t cardAcmd(uint8_t cmd, uint32_t arg);
  uint8_t cardCommand(uint8_t cmd, uint32_t arg);
  uint8_t sendWriteCommand(uint32_t blockNumber, uint32_t eraseCount);
  void error(uint8_t code) {errorCode_ = code;}
  uint8_t readRegister(uint8_t cmd, void* buf);
  void type(uint8_t value) {type_ = value;}
  uint8_t waitNotBusy(uint16_t timeoutMillis);
  uint8_t writeData(uint8_t token, const uint8_t* src);
  uint8_t waitStartBlock(void);
  void spiSend(uint8_t b);
  uint8_t spiRec(void);
};


#endif  // Sd2Card_h
