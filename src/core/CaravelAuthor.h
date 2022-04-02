#pragma once
#include "CaravelWriter.h"

namespace CaravelPM {
  class CaravelAuthor {
  public:
    CaravelAuthor() {};
    static void CreatePackage(std::string pkgName, std::string type, std::map<std::string, std::string> extraProps, CaravelTypeLoader* newLoader);
  };
}
