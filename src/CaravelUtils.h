#pragma once
#include <CLI11.hpp>
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

  const static CaravelPkgTypeValidator CaravelPkgClass;
}
