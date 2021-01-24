/**
 * SPISD library for SPRESENSE based on Arduino SD library
 * License: GNU General Public License V3
 * (Because Arduino SD library is licensed with this.)
 */

#ifndef __SPISD_H__
#define __SPISD_H__

#include <Arduino.h>
#include <utility/SpiSdFat.h>
#include <utility/SpiSdFatUtil.h>

#define FILE_READ O_READ
#define FILE_WRITE (O_READ | O_WRITE | O_CREAT)

class SpiFile : public Stream 
{
private:
  char _name[13]; 
  SpiSdFile *_file;  

public:
  SpiFile(SpiSdFile f, const char *name);    
  SpiFile(void);  
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buf, size_t size);
  virtual int read();
  virtual int peek();
  virtual int available();
  virtual void flush();
  int read(void *buf, uint16_t nbyte);
  boolean seek(uint32_t pos);
  uint32_t position();
  uint32_t size();
  void close();
  operator bool();
  char * name();

  boolean isDirectory(void);
  SpiFile openNextFile(uint8_t mode = O_RDONLY);
  void rewindDirectory(void);
  
  using Print::write;
};


class SpiSDClass 
{
private:
  SpiSd2Card card;
  SpiSdVolume volume;
  SpiSdFile root;
  
  SpiSdFile getParentDir(const char *filepath, int *indx);

public:
  SpiSDClass(SPIClass& spi): card(spi) {}
  boolean begin(void);
  boolean begin(uint32_t clock);
  
  SpiFile open(const char *filename, uint8_t mode = FILE_READ);
  SpiFile open(const String &filename, uint8_t mode = FILE_READ) { 
    return open( filename.c_str(), mode ); 
  }

  boolean exists(const char *filepath);
  boolean exists(const String &filepath) { 
    return exists(filepath.c_str()); 
  }

  boolean mkdir(const char *filepath);
  boolean mkdir(const String &filepath) { 
    return mkdir(filepath.c_str()); 
  }
  
  boolean remove(const char *filepath);
  boolean remove(const String &filepath) { 
    return remove(filepath.c_str()); 
  }
  
  boolean rmdir(const char *filepath);
  boolean rmdir(const String &filepath) { 
    return rmdir(filepath.c_str()); 
  }

private:
  int fileOpenMode;
  
  friend class SpiFile;
  friend boolean callback_openPath(SpiSdFile& ,const char * ,boolean ,void *); 
};


typedef SpiFile    SpiSDFile;
typedef SpiSDClass SpiSDFileSystemClass;
#define SpiSDFileSystem   SD

#endif
