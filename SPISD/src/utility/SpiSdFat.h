/** 
 * Arduino SdFat Library for SPRESENSE based on Arduino SdFat Library
 *
 * This file is part of the Arduino SdFat Library
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
 * along with the Arduino SdFat Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef SpiSdFat_h
#define SpiSdFat_h

#include "SpiSd2Card.h"
#include "SpiFatStructs.h"
#include "Print.h"

#if defined(rewind)
#define REDEFINE_REWIND_FOR_STDIO_H
#undef rewind
#endif

class SpiSdVolume;

/** open() oflag for reading */
// uint8_t const O_READ = 0X01;
#define O_READ    0x01
/** open() oflag - same as O_READ */
// uint8_t const O_RDONLY = O_READ;
#define O_RDONLY  0x01
/** open() oflag for write */
//uint8_t const O_WRITE = 0X02;
#define O_WRITE   0x02
/** open() oflag - same as O_WRITE */
// uint8_t const O_WRONLY = O_WRITE;
#define O_WRONLY  0x02
/** open() oflag for reading and writing */
// uint8_t const O_RDWR = (O_READ | O_WRITE);
#define O_RDWR    0x03
/** open() oflag mask for access modes */
// uint8_t const O_ACCMODE = (O_READ | O_WRITE);
#define O_ACCMODE 0x03

/** 
 * The file offset shall be set to the end of the file prior to each write. */
// uint8_t const O_APPEND = 0X04;
#define O_APPEND  0x04
/** synchronous writes - call sync() after each write */
// uint8_t const O_SYNC = 0X08;
#define O_SYNC    0x08
/** create the file if nonexistent */
// uint8_t const O_CREAT = 0X10;
#define O_CREAT   0x10
/** If O_CREAT and O_EXCL are set, open() shall fail if the file exists */
// uint8_t const O_EXCL = 0X20;
#define O_EXCL    0X20
/** truncate the file to zero length */
// uint8_t const O_TRUNC = 0X40;
#define O_TRUNC   0X40

/** set the file's last access date */
// uint8_t const T_ACCESS = 1;
#define T_ACCESS  1
/** set the file's creation date and time */
// uint8_t const T_CREATE = 2;
#define T_CREATE  2
/** Set the file's write date and time */
// uint8_t const T_WRITE = 4;
#define T_WRITE   4

/** This SdFile has not been opened. */
// uint8_t const FAT_FILE_TYPE_CLOSED = 0;
#define FAT_FILE_TYPE_CLOSED  0
/** SdFile for a file */
// uint8_t const FAT_FILE_TYPE_NORMAL = 1;
#define FAT_FILE_TYPE_NORMAL  1
/** SdFile for a FAT16 root directory */
// uint8_t const FAT_FILE_TYPE_ROOT16 = 2;
#define FAT_FILE_TYPE_ROOT16  2
/** SdFile for a FAT32 root directory */
// uint8_t const FAT_FILE_TYPE_ROOT32 = 3;
#define FAT_FILE_TYPE_ROOT32  3
/** SdFile for a subdirectory */
// uint8_t const FAT_FILE_TYPE_SUBDIR = 4;
#define FAT_FILE_TYPE_SUBDIR  4
/** Test value for directory type */
// uint8_t const FAT_FILE_TYPE_MIN_DIR = FAT_FILE_TYPE_ROOT16;
#define FAT_FILE_TYPE_MIN_DIR 2

/** date field for FAT directory entry */
static inline uint16_t FAT_DATE(uint16_t year, uint8_t month, uint8_t day) {
  return (year - 1980) << 9 | month << 5 | day;
}

static inline uint16_t FAT_YEAR(uint16_t fatDate) {
  return 1980 + (fatDate >> 9);
}

static inline uint8_t FAT_MONTH(uint16_t fatDate) {
  return (fatDate >> 5) & 0XF;
}

static inline uint8_t FAT_DAY(uint16_t fatDate) {
  return fatDate & 0X1F;
}

static inline uint16_t FAT_TIME(uint8_t hour, uint8_t minute, uint8_t second) {
  return hour << 11 | minute << 5 | second >> 1;
}

static inline uint8_t FAT_HOUR(uint16_t fatTime) {
  return fatTime >> 11;
}

static inline uint8_t FAT_MINUTE(uint16_t fatTime) {
  return(fatTime >> 5) & 0X3F;
}

static inline uint8_t FAT_SECOND(uint16_t fatTime) {
  return 2*(fatTime & 0X1F);
}

/** Default date for file timestamps is 1 Jan 2000 */
uint16_t const FAT_DEFAULT_DATE = ((2000 - 1980) << 9) | (1 << 5) | 1;

/** Default time for file timestamp is 1 am */
uint16_t const FAT_DEFAULT_TIME = (1 << 11);

/**
 *  \class SdFile
 *  \brief Access FAT16 and FAT32 files on SD and SDHC cards.
 */
class SpiSdFile : public Print {
public:
  SpiSdFile(void) : type_(FAT_FILE_TYPE_CLOSED) {}

  void clearUnbufferedRead(void) { flags_ &= ~F_FILE_UNBUFFERED_READ; }

  uint8_t close(void);
  uint8_t contiguousRange(uint32_t* bgnBlock, uint32_t* endBlock);
  uint8_t createContiguous(SpiSdFile* dirFile
             ,const char* fileName, uint32_t size);

  uint32_t curCluster(void) const  { return curCluster_; }
  uint32_t curPosition(void) const { return curPosition_; }

  /**
   *  Set the date/time callback function
   *  \param[in] dateTime The user's call back function.  The callback
   *  function is of the form:
   *  \code
   *  void dateTime(uint16_t* date, uint16_t* time) {
   *    uint16_t year;
   *    uint8_t month, day, hour, minute, second;
   *    User gets date and time from GPS or real-time clock here
   *    *date = FAT_DATE(year, month, day);
   *    *time = FAT_TIME(hour, minute, second);
   *  }
   *  \endcode
   *
   *  Sets the function that is called when a file is created or when
   *  a file's directory entry is modified by sync(). All timestamps,
   *  access, creation, and modify, are set when a file is created.
   *  sync() maintains the last access date and last modify date/time.
   */
  static void dateTimeCallback(
                void (*dateTime)(uint16_t* date, uint16_t* time)) 
  {
    dateTime_ = dateTime;
  }

  static void dateTimeCallbackCancel(void) { dateTime_ = 0; }

  uint32_t dirBlock(void) const { return dirBlock_; }
  uint8_t dirEntry(dir_t* dir);
  uint8_t dirIndex(void) const { return dirIndex_; }
  static void dirName(const dir_t& dir, char* name);

  uint32_t fileSize(void) const {return fileSize_;}
  uint32_t firstCluster(void) const {return firstCluster_;}

  uint8_t isDir(void) const  { return type_ >= FAT_FILE_TYPE_MIN_DIR; }
  uint8_t isFile(void) const { return type_ == FAT_FILE_TYPE_NORMAL; }
  uint8_t isOpen(void) const { return type_ != FAT_FILE_TYPE_CLOSED; }
  uint8_t isSubDir(void) const {return type_ == FAT_FILE_TYPE_SUBDIR;}
  uint8_t isRoot(void) const {
    return type_ == FAT_FILE_TYPE_ROOT16 || type_ == FAT_FILE_TYPE_ROOT32;
  }

  uint8_t makeDir(SpiSdFile* dir, const char* dirName);
  uint8_t open(SpiSdFile* dirFile, uint16_t index, uint8_t oflag);
  uint8_t open(SpiSdFile* dirFile, const char* fileName, uint8_t oflag);

  uint8_t openRoot(SpiSdVolume* vol);

  /**
   *  Read the next byte from a file.
   *  \return For success read returns the next byte in the file as an int.
   *  If an error occurs or end of file is reached -1 is returned.
   */
  int16_t read(void) {
    uint8_t b;
    return read(&b, 1) == 1 ? b : -1;
  }

  int16_t read(void* buf, uint16_t nbyte);
  int8_t readDir(dir_t* dir);
  static uint8_t remove(SpiSdFile* dirFile, const char* fileName);
  uint8_t remove(void);

  void rewind(void) { curPosition_ = curCluster_ = 0; }

  uint8_t rmDir(void);
  uint8_t rmRfStar(void);

  uint8_t seekCur(uint32_t pos) { return seekSet(curPosition_ + pos); }
  uint8_t seekEnd(void) { return seekSet(fileSize_); }
  uint8_t seekSet(uint32_t pos);

  /**
   *  Use unbuffered reads to access this file.  Used with Wave
   *  Shield ISR.  Used with Sd2Card::partialBlockRead() in WaveRP.
   *  Not recommended for normal applications.
   */
  void setUnbufferedRead(void) {
    if (isFile()) flags_ |= F_FILE_UNBUFFERED_READ;
  }

  uint8_t timestamp(uint8_t flag, uint16_t year, uint8_t month
            ,uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);

  uint8_t sync(void);

  /** 
   *  Type of this SdFile.  
   *  You should use isFile() or isDir() instead of type()
   *  if possible.
   *  \return The file or directory type.
   */
  uint8_t type(void) const { return type_; }
  uint8_t truncate(uint32_t size);

  /** \return Unbuffered read flag. */
  uint8_t unbufferedRead(void) const {
    return flags_ & F_FILE_UNBUFFERED_READ;
  }

  SpiSdVolume* volume(void) const { return vol_; }
   size_t write(uint8_t b);
  // size_t write(const void* buf, uint16_t nbyte);
  size_t write(const uint8_t* buf, uint32_t nbyte);
  // size_t write(const char* str);
  // size_t write(const char* str);

  uint8_t contiguousRange(uint32_t& bgnBlock, uint32_t& endBlock) 
  { 
    return contiguousRange(&bgnBlock, &endBlock);
  }

  uint8_t createContiguous(SpiSdFile& dirFile
            ,const char* fileName, uint32_t size) 
  {
    return createContiguous(&dirFile, fileName, size);
  }

  static void dateTimeCallback(
                void (*dateTime)(uint16_t& date, uint16_t& time)) 
  {
    oldDateTime_ = dateTime;
    dateTime_ = dateTime ? oldToNew : 0;
  }

  uint8_t dirEntry(dir_t& dir) { return dirEntry(&dir); }
  uint8_t makeDir(SpiSdFile& dir, const char* dirName) {
    return makeDir(&dir, dirName);
  }

  uint8_t open(SpiSdFile& dirFile ,const char* fileName, uint8_t oflag) {
    return open(&dirFile, fileName, oflag);
  }

  uint8_t open(SpiSdFile& dirFile, const char* fileName) {
    return open(dirFile, fileName, O_RDWR);
  }

  uint8_t open(SpiSdFile& dirFile, uint16_t index, uint8_t oflag) {  
    return open(&dirFile, index, oflag);
  }

  uint8_t openRoot(SpiSdVolume& vol) { return openRoot(&vol); }  

  int8_t readDir(dir_t& dir) { return readDir(&dir); }  

  static uint8_t remove(SpiSdFile& dirFile, const char* fileName) {
    return remove(&dirFile, fileName);
  }

private:
  static void (*oldDateTime_)(uint16_t& date, uint16_t& time);
  static void oldToNew(uint16_t* date, uint16_t* time) 
  {
    uint16_t d;
    uint16_t t;
    oldDateTime_(d, t);
    *date = d;
    *time = t;
  }

private:
  // should be 0XF
  static uint8_t const F_OFLAG = (O_ACCMODE | O_APPEND | O_SYNC);
  // available bits
  static uint8_t const F_UNUSED = 0X30;
  // use unbuffered SD read
  static uint8_t const F_FILE_UNBUFFERED_READ = 0X40;
  // sync of directory entry required
  static uint8_t const F_FILE_DIR_DIRTY = 0X80;

// make sure F_OFLAG is ok
#if ((F_UNUSED | F_FILE_UNBUFFERED_READ | F_FILE_DIR_DIRTY) & F_OFLAG)
#error flags_ bits conflict
#endif  // flags_ bits

  uint8_t   flags_;          // See above for definition of flags_ bits
  uint8_t   type_;           // type of file see above for values
  uint32_t  curCluster_;     // cluster for current file position
  uint32_t  curPosition_;    // current file position in bytes from beginning
  uint32_t  dirBlock_;       // SD block that contains directory entry for file
  uint8_t   dirIndex_;       // index of entry in dirBlock 0 <= dirIndex_ <= 0XF
  uint32_t  fileSize_;       // file size in bytes
  uint32_t  firstCluster_;   // first cluster of file
  SpiSdVolume* vol_;         // volume where file is located

  uint8_t addCluster(void);
  uint8_t addDirCluster(void);
  dir_t* cacheDirEntry(uint8_t action);
  static void (*dateTime_)(uint16_t* date, uint16_t* time);
  static uint8_t make83Name(const char* str, uint8_t* name);
  uint8_t openCachedEntry(uint8_t cacheIndex, uint8_t oflags);
  dir_t* readDirCache(void);
};

/**
 *  \brief Cache for an SD data block
 */
union cache_t {
  /** Used to access cached file data blocks. */
  uint8_t  data[512];
  /** Used to access cached FAT16 entries. */
  uint16_t fat16[256];
  /** Used to access cached FAT32 entries. */
  uint32_t fat32[128];
  /** Used to access cached directory entries. */
  dir_t    dir[16];
  /** Used to access a cached MasterBoot Record. */
  mbr_t    mbr;
  /** Used to access to a cached FAT boot sector. */
  fbs_t    fbs;
};

/**
 *  \class SdVolume
 *  \brief Access FAT16 and FAT32 volumes on SD and SDHC cards.
 */
class SpiSdVolume {
public:
  /** Create an instance of SdVolume */
  SpiSdVolume(void) :allocSearchStart_(2), fatType_(0) {}

  /** 
   *  Clear the cache and returns a pointer to the cache.  
   *  Used by the WaveRP
   *  recorder to do raw write to the SD card.  Not for normal apps.
   */
  static uint8_t* cacheClear(void) {
    cacheFlush();
    cacheBlockNumber_ = 0XFFFFFFFF;
    return cacheBuffer_.data;
  }

  /**
   *  Initialize a FAT volume.  Try partition one first then try super
   *  floppy format.
   *
   *  \param[in] dev The Sd2Card where the volume is located.
   *  \return The value one, true, is returned for success and
   *
   *  the value zero, false, is returned for failure.  Reasons for
   *  failure include not finding a valid partition, not finding a valid
   *  FAT file system or an I/O error.
   */
  uint8_t init(SpiSd2Card* dev) { 
    return init(dev, 1) ? true : init(dev, 0);
  }

  uint8_t init(SpiSd2Card* dev, uint8_t part);

  /** \return The volume's cluster size in blocks. */
  uint8_t blocksPerCluster(void) const { return blocksPerCluster_; }

  /** \return The number of blocks in one FAT. */
  uint32_t blocksPerFat(void)  const { return blocksPerFat_; }

  /** \return The total number of clusters in the volume. */
  uint32_t clusterCount(void) const { return clusterCount_; }

  /** \return The shift count required to multiply by blocksPerCluster. */
  uint8_t clusterSizeShift(void) const { return clusterSizeShift_; }

  /** \return The logical block number for the start of file data. */
  uint32_t dataStartBlock(void) const { return dataStartBlock_; }

  /** \return The number of FAT structures on the volume. */
  uint8_t fatCount(void) const { return fatCount_; }

  /** \return The logical block number for the start of the first FAT. */
  uint32_t fatStartBlock(void) const { return fatStartBlock_; }

  /** \return The FAT type of the volume. Values are 12, 16 or 32. */
  uint8_t fatType(void) const { return fatType_;  }

  /** 
   *  \return The number of entries in the root directory for 
   *  FAT16 volumes. 
   */
  uint32_t rootDirEntryCount(void) const { return rootDirEntryCount_; }

  /** 
   *  \return The logical block number for the start of the root directory
   *  on FAT16 volumes or the first cluster number on FAT32 volumes. 
   */
  uint32_t rootDirStart(void) const { return rootDirStart_; }

  /** return a pointer to the Sd2Card object for this volume */
  static SpiSd2Card* sdCard(void) { return sdCard_; }

  /** \deprecated Use: uint8_t SdVolume::init(Sd2Card* dev); */
  uint8_t init(SpiSd2Card& dev) { return init(&dev); }  

  /** \deprecated Use: uint8_t SdVolume::init(Sd2Card* dev, uint8_t vol); */
  uint8_t init(SpiSd2Card& dev, uint8_t part) {
    return init(&dev, part);
  }

private:
  friend class SpiSdFile;

  static uint8_t const CACHE_FOR_READ = 0;
  static uint8_t const CACHE_FOR_WRITE = 1;

  static cache_t cacheBuffer_;        // 512 byte cache for device blocks
  static uint32_t cacheBlockNumber_;  // Logical number of block in the cache
  static SpiSd2Card* sdCard_;         // Sd2Card object for cache
  static uint8_t cacheDirty_;         // cacheFlush() will write block if true
  static uint32_t cacheMirrorBlock_;  // block number for mirror FAT

  uint32_t allocSearchStart_;   // start cluster for alloc search
  uint8_t blocksPerCluster_;    // cluster size in blocks
  uint32_t blocksPerFat_;       // FAT size in blocks
  uint32_t clusterCount_;       // clusters in one FAT
  uint8_t clusterSizeShift_;    // shift to convert cluster count to block count
  uint32_t dataStartBlock_;     // first data block number
  uint8_t fatCount_;            // number of FATs on volume
  uint32_t fatStartBlock_;      // start block for first FAT
  uint8_t fatType_;             // volume type (12, 16, OR 32)
  uint16_t rootDirEntryCount_;  // number of entries in FAT16 root dir
  uint32_t rootDirStart_;       // root start block for FAT16, cluster for FAT32

  uint8_t allocContiguous(uint32_t count, uint32_t* curCluster);
  uint8_t blockOfCluster(uint32_t position) const {
    return (position >> 9) & (blocksPerCluster_ - 1);
  }

  uint32_t clusterStartBlock(uint32_t cluster) const {
    return dataStartBlock_ + ((cluster - 2) << clusterSizeShift_);
  }

  uint32_t blockNumber(uint32_t cluster, uint32_t position) const {
    return clusterStartBlock(cluster) + blockOfCluster(position);
  }

  static uint8_t cacheFlush(void);
  static uint8_t cacheRawBlock(uint32_t blockNumber, uint8_t action);
  static void cacheSetDirty(void) { cacheDirty_ |= CACHE_FOR_WRITE; }
  static uint8_t cacheZeroBlock(uint32_t blockNumber);
  uint8_t chainSize(uint32_t beginCluster, uint32_t* size) const;
  uint8_t fatGet(uint32_t cluster, uint32_t* value) const;
  uint8_t fatPut(uint32_t cluster, uint32_t value);
  uint8_t fatPutEOC(uint32_t cluster) {
    return fatPut(cluster, 0x0FFFFFFF);
  }

  uint8_t freeChain(uint32_t cluster);
  uint8_t isEOC(uint32_t cluster) const {
    return  cluster >= (fatType_ == 16 ? FAT16EOC_MIN : FAT32EOC_MIN);
  }

  uint8_t readBlock(uint32_t block, uint8_t* dst) {
    return sdCard_->readBlock(block, dst);
  }

  uint8_t readData(uint32_t block, uint16_t offset
    ,uint16_t count, uint8_t* dst) {
      return sdCard_->readData(block, offset, count, dst);
  }

  uint8_t writeBlock(uint32_t block, const uint8_t* dst) {
    return sdCard_->writeBlock(block, dst);
  }
};

#ifdef REDEFINE_REWIND_FOR_STDIO_H
/**
 *  rewind is defined as a macro in stdio.h, 
 *  it conflict with SpiSdFile::rewind().
 *  So we have to redefine it as an inline function.
 */
static inline void rewind(FILE *stream)
{
  fseek((stream),0,SEEK_SET);
}
#endif


#endif  // SpiSdFat_h
