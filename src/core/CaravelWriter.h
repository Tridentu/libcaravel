#pragma once
#include <archive.h>
#include <vector>
#include <archive_entry.h>
#include <string>
#include <map>
#include "CaravelUtils.h"

namespace CaravelPM {
  class CaravelWriter {
  public:
    CaravelWriter(std::string pkgName, std::string type);
    ~CaravelWriter();
    void AddFile(std::string fileName, std::string name);
    void AddFiles(std::vector<std::string> files);
    void AddManifest();
    void SetMetadata(std::string key, std::string value);
  private:
    struct archive *m_Archive;
    std::string m_Type;
    std::string m_Name;
    std::map<std::string, std::string> m_Metadata; 
  };

};
