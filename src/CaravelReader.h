#pragma once

#include <archive.h>
#include <vector>
#include <string>
#include <archive_entry.h>
#include <map>
#include "CaravelUtils.h"

namespace CaravelPM {
  
  class CaravelReader {
  public:
    CaravelReader(std::string pkgFile, std::string name);
    bool Extract();
    bool  Install();
    int CopyFile(struct archive* a, struct archive *w);
    std::string GetMetadata(std::string key);
    ~CaravelReader();
  public:
    std::string RunCommand(const char* command);
  private:
    struct archive *m_Archive;
    struct archive *m_ArchiveWrite;
    std::string m_File;
    std::string m_Name;
    std::map<std::string, std::string> m_Metadata;
    CaravelPkgType m_Type;
  };
}