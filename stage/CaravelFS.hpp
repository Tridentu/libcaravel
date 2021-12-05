#pragma once

#define FUSE_USE_VERSION 31

#include <fuse3/fuse.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <archive.h>
#include <archive_entry.h>
#include <sys/types.h>
#include <vector>
#include <map>

namespace CaravelPM {
  typedef std::map<std::string, std::vector<std::string>> FileContentsMap;
  class CaravelFS {
  public:
    static fuse_operations GetFS();
    static void SetArchive(std::string archive);
    
  public:
    static int GetAttributes(const char* path, stat* stBuf);
    
  private:
    static std::vector<std::string> s_Dirs;
    static std::vector<std::string> s_Files;
    static FileContentsMap s_Contents;
    static archive* s_CurrentArchive = nullptr;
    static std::string s_ArchiveName;
  };
}
