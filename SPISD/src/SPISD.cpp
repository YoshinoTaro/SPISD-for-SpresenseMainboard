/**
 * SPISD library for SPRESENSE based on Arduino SD library
 * License: GNU General Public License V3
 * (Because Arduino SD library is licensed with this.)
 */


#include "SPISD.h"

#define MAX_COMPONENT_LEN 12 
#define PATH_COMPONENT_BUFFER_LEN MAX_COMPONENT_LEN+1

/**
 * Parse individual path components from a path.
 *
 *   e.g. after repeated calls '/foo/bar/baz' will be split
 *        into 'foo', 'bar', 'baz'.
 *
 * `buffer` needs to be PATH_COMPONENT_BUFFER_LEN in size.
 * `p_offset` needs to point to an integer of the offset at
 * which the previous path component finished.
 *
 * Returns `true` if more components remain.
 * Returns `false` if this is the last component.
 *   (This means path ended with 'foo' or 'foo/'.)
 */
bool getNextPathComponent(const char *path
       ,unsigned int *p_offset, char *buffer) 
{

  int bufferOffset = 0;
  int offset = *p_offset;

  // Skip root or other separator
  if (path[offset] == '/') offset++;
  
  // Copy the next next path segment
  while (bufferOffset < MAX_COMPONENT_LEN
	   && (path[offset] != '/')
	   && (path[offset] != '\0')) 
  {
    buffer[bufferOffset++] = path[offset++];
  }

  buffer[bufferOffset] = '\0';

  // Skip trailing separator so we can determine 
  // if this is the last component in the path or not.
  if (path[offset] == '/') offset++;
  *p_offset = offset;

  return (path[offset] != '\0');
}

/**
 *  When given a file path (and parent directory--normally root),
 *  this function traverses the directories in the path and at each
 *  level calls the supplied callback function while also providing
 *  the supplied object for context if required.
 *
 *    e.g. given the path '/foo/bar/baz'
 *         the callback would be called at the equivalent of
 *	    '/foo', '/foo/bar' and '/foo/bar/baz'.
 *
 *  The implementation swaps between two different directory/file
 *  handles as it traverses the directories and does not use recursion
 *  in an attempt to use memory efficiently.
 * 
 *  If a callback wishes to stop the directory traversal it should
 *  return false--in this case the function will stop the traversal,
 *  tidy up and return false.
 *
 *  If a directory path doesn't exist at some point this function will
 * also return false and not subsequently call the callback.
 *
 *  If a directory path specified is complete, valid and the callback
 *  did not indicate the traversal should be interrupted then this
 *  function will return true.
 */
boolean walkPath(const char *filepath, SpiSdFile& parentDir
                   ,boolean (*callback)(SpiSdFile& parentDir
                               ,const char *filePathComponent
                               ,boolean isLastComponent
                               ,void *object)
                   ,void *object = NULL) 
{

  SpiSdFile subfile1;
  SpiSdFile subfile2;

  char buffer[PATH_COMPONENT_BUFFER_LEN]; 
  unsigned int offset = 0;

  SpiSdFile *p_parent;
  SpiSdFile *p_child;
  SpiSdFile *p_tmp_sdfile;  
  
  p_child = &subfile1;
  p_parent = &parentDir;

  while (true) {

    boolean moreComponents = getNextPathComponent(filepath, &offset, buffer);
    boolean shouldContinue = callback((*p_parent)
                               ,buffer ,!moreComponents ,object);

    if (!shouldContinue) {
      if (p_parent != &parentDir) 
        (*p_parent).close();
      return false;
    }
    
    if (!moreComponents) break;
    
    boolean exists = (*p_child).open(*p_parent, buffer, O_RDONLY);
    if (p_parent != &parentDir) 
      (*p_parent).close();
    
    if (exists) {
      
      if (p_parent == &parentDir) 
        p_parent = &subfile2;

      p_tmp_sdfile = p_parent;
      p_parent = p_child;
      p_child = p_tmp_sdfile;

    } else {
      return false;
    }
  }
  
  if (p_parent != &parentDir) 
    (*p_parent).close(); 

  return true;
}



/**
 *  The callbacks used to implement various functionality follow.
 *
 *  Each callback is supplied with a parent directory handle,
 *  character string with the name of the current file path component,
 *  a flag indicating if this component is the last in the path and
 *  a pointer to an arbitrary object used for context.
 */
boolean callback_pathExists(SpiSdFile& parentDir
          ,const char *filePathComponent
          ,boolean isLastComponent
          ,void *object) 
{
  SpiSdFile child;

  boolean exists = child.open(parentDir, filePathComponent, O_RDONLY);
  if (exists) 
    child.close(); 
  
  return exists;
}

boolean callback_makeDirPath(SpiSdFile& parentDir
          ,const char *filePathComponent
          ,boolean isLastComponent
          ,void *object) 
{
  SpiSdFile child;
  boolean result = callback_pathExists(parentDir
                     ,filePathComponent ,isLastComponent ,object);
  if (!result) 
    result = child.makeDir(parentDir, filePathComponent);

  return result;
}


boolean callback_remove(SpiSdFile& parentDir
          ,const char *filePathComponent
          ,boolean isLastComponent
          ,void *object) 
{
  if (isLastComponent) 
    return SpiSdFile::remove(parentDir, filePathComponent);
  return true;
}

boolean callback_rmdir(SpiSdFile& parentDir
          ,const char *filePathComponent 
          ,boolean isLastComponent
          ,void *object) 
{
  if (isLastComponent) {
    SpiSdFile f;
    if (!f.open(parentDir, filePathComponent, O_READ)) 
      return false;
    return f.rmDir();
  }

  return true;
}

boolean SpiSDClass::begin(void) 
{
  return card.init(SPI_HALF_SPEED) 
         && volume.init(card) 
         && root.openRoot(volume);
}

boolean SpiSDClass::begin(uint32_t clock) 
{
  return card.init(SPI_HALF_SPEED)
         && card.setSpiClock(clock)
         && volume.init(card) 
         && root.openRoot(volume);
}

SpiSdFile SpiSDClass::getParentDir(const char *filepath, int *index) 
{
  SpiSdFile d1 = root; 
  SpiSdFile d2;

  SpiSdFile *parent = &d1;
  SpiSdFile *subdir = &d2;
  
  const char *origpath = filepath;

  while (strchr(filepath, '/')) {

    if (filepath[0] == '/') {
      filepath++;
      continue;
    }
    
    if (!strchr(filepath, '/')) 
      break;

    uint8_t idx = strchr(filepath, '/') - filepath;
    if (idx > 12) idx = 12;  // dont let them specify long names
    char subdirname[13];
    strncpy(subdirname, filepath, idx);
    subdirname[idx] = 0;

    // close the subdir (we reuse them) if open
    subdir->close();
    if (!subdir->open(parent, subdirname, O_READ)) 
      return SpiSdFile();

    // move forward to the next subdirectory
    filepath += idx;

    // we reuse the objects, close it.
    parent->close();

    // swap the pointers
    SpiSdFile *t = parent;
    parent = subdir;
    subdir = t;
  }

  *index = (int)(filepath - origpath);

  // parent is now the parent diretory of the file!
  return *parent;
}


/**
 *  Open the supplied file path for reading or writing.
 * 
 *  The file content can be accessed via the `file` property of
 *  the `SDClass` object--this property is currently
 *  a standard `SdFile` object from `sdfatlib`.
 *
 *  Defaults to read only.
 *
 *  If `write` is true, default action (when `append` is true) is to
 *  append data to the end of the file.
 *
 *  If `append` is false then the file will be truncated first.
 *
 *  If the file does not exist and it is opened for writing the file
 *  will be created.
 *
 *  An attempt to open a file for reading that does not exist is an
 *  error.
 */

SpiFile SpiSDClass::open(const char *filepath, uint8_t mode) 
{

  int pathidx;

  // do the interative search
  SpiSdFile parentdir = getParentDir(filepath, &pathidx);

  filepath += pathidx;
  if (!filepath[0]) 
    return SpiFile(parentdir, "/");

  SpiSdFile file;
  if (!parentdir.isOpen()) 
    return SpiFile();

  // there is a special case for the Root directory since its a static dir
  if (parentdir.isRoot()) {
    if (!file.open(root, filepath, mode)) 
      return SpiFile();
  } else {
    if (!file.open(parentdir, filepath, mode)) 
      return SpiFile();
    parentdir.close();
  }

  if (mode & (O_APPEND | O_WRITE)) 
    file.seekSet(file.fileSize());

  return SpiFile(file, filepath);
}

boolean SpiSDClass::exists(const char *filepath) 
{
  return walkPath(filepath, root, callback_pathExists);
}

boolean SpiSDClass::mkdir(const char *filepath) 
{
  return walkPath(filepath, root, callback_makeDirPath);
}

boolean SpiSDClass::rmdir(const char *filepath) 
{
  return walkPath(filepath, root, callback_rmdir);
}

boolean SpiSDClass::remove(const char *filepath) 
{
  return walkPath(filepath, root, callback_remove);
}

SpiFile SpiFile::openNextFile(uint8_t mode) 
{
  dir_t p;
  while (_file->readDir(&p) > 0) {

    if (p.name[0] == DIR_NAME_FREE) 
      return SpiFile();

    if (p.name[0] == DIR_NAME_DELETED || p.name[0] == '.') 
      continue;

    // only list subdirectories and files
    if (!DIR_IS_FILE_OR_SUBDIR(&p)) 
      continue;

    // print file name with possible blank fill
    SpiSdFile f;
    char name[13];
    _file->dirName(p, name);

    if (f.open(_file, name, mode)) 
      return SpiFile(f, name);    
    else
      return SpiFile();
  }

  return SpiFile();
}

void SpiFile::rewindDirectory(void) 
{  
  if (isDirectory())
    _file->rewind();
}
