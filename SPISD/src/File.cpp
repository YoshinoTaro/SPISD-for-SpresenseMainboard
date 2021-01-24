/**
 * SPISD library for SPRESENSE based on Arduino SD library
 * License: GNU General Public License V3
 * (Because Arduino SD library is licensed with this.)
 */

#include <SPISD.h>

SpiFile::SpiFile(SpiSdFile f, const char *n) 
{
  _file = (SpiSdFile*)malloc(sizeof(SpiSdFile)); 
  if (_file) {
    memcpy(_file, &f, sizeof(SpiSdFile));
    strncpy(_name, n, 12);
    _name[12] = 0;
  }
}

SpiFile::SpiFile(void) 
{
  _file = 0;
  _name[0] = 0;
}

char *SpiFile::name(void) 
{
  return _name;
}

boolean SpiFile::isDirectory(void) 
{
  return (_file && _file->isDir());
}

size_t SpiFile::write(uint8_t val) 
{
  return write(&val, 1);
}

size_t SpiFile::write(const uint8_t *buf, size_t size) 
{
  if (!_file) {
    setWriteError();
    return 0;
  }

  _file->clearWriteError();

  size_t t = _file->write(buf, size);
  if (_file->getWriteError()) {
    setWriteError();
    return 0;
  }

  return t;
}

int SpiFile::peek() 
{
  if (! _file) return 0;

  int c = _file->read();
  if (c != -1) _file->seekCur(-1);

  return c;
}

int SpiFile::read() 
{
  if (_file) return _file->read();
  return -1;
}

int SpiFile::read(void *buf, uint16_t nbyte) 
{
  if (_file) 
    return _file->read(buf, nbyte);
  return 0;
}

int SpiFile::available() 
{
  // if (!_file) return 0;
  uint32_t n = size() - position();
  return n > 0X7FFF ? 0X7FFF : n;
}

void SpiFile::flush() 
{
  if (_file) _file->sync();
}

boolean SpiFile::seek(uint32_t pos) 
{
  if (!_file) return false;
  return _file->seekSet(pos);
}

uint32_t SpiFile::position() 
{
  if (! _file) return -1;
  return _file->curPosition();
}

uint32_t SpiFile::size() 
{
  if (! _file) return 0;
  return _file->fileSize();
}

void SpiFile::close() 
{
  if (_file) {
    _file->close();
    free(_file); 
    _file = 0;
  }
}

SpiFile::operator bool() 
{
  if (_file) return  _file->isOpen();
  return false;
}
