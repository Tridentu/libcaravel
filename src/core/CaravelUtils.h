#pragma once
#include <CLI11.hpp>
#include <gpgme.h>
#include <iostream>

#include <string.h>

namespace CaravelPM {
  enum class CaravelPkgType {
    Binaries,
    DotFiles,
    Source,
    Assets  
      
  };

  struct CaravelManifest {
    const char* name;
    const char* type;
  };
  
  size_t writeData(void* ptr, size_t size, size_t nmemb, FILE *stream);
    
  struct CaravelPkgTypeValidator : public CLI::Validator {

    CaravelPkgTypeValidator(){
      name_ = "CRAVPKGTYPE";
      func_ = [](const std::string& str){
	if (str != "binaries" && str != "config" && str != "assets" && str != "source"){
	  return std::string("the package type must be either binaries, config, assets or source.");
	} else {
	  return std::string();
	}
      };
    }
  };
  
  void CheckGPGMEError(gpgme_error_t err, std::string message);

  const static CaravelPkgTypeValidator CaravelPkgClass;
}
