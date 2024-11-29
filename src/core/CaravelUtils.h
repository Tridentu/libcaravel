#pragma once
#include <CLI11.hpp>
#include <gpgme.h>
#include <iostream>

#include <string.h>
#include "packages/CaravelTypeLoader.hpp"

namespace CaravelPM {

  struct CaravelManifest {
    const char* name;
    const char* type;
  };
  
  size_t writeData(void* ptr, size_t size, size_t nmemb, FILE *stream);
    
  
  
  
  void CheckGPGMEError(gpgme_error_t err, std::string message);

  
  
  
  CaravelTypeLoader* InitLoader();
}
