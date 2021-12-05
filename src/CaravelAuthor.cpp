
#include "CaravelWriter.h"
#include "CaravelUtils.h"
#include "CaravelAuthor.h"
#include <filesystem>
#include <string>

namespace CaravelPM {

  void CaravelAuthor::CreatePackage(std::string pkgName, CaravelPkgType type){
    CaravelWriter* cw = new CaravelWriter(pkgName,type);
    std::filesystem::path pkgPath = std::filesystem::current_path();
    pkgPath += "/";
    pkgPath += pkgName;
    cw->SetMetadata("name",pkgName);
    switch(type){
    case CaravelPkgType::Binaries:
      cw->SetMetadata("type","binaries");
       for (const auto & entry : std::filesystem::recursive_directory_iterator((pkgPath / "install"))){
	 cw->AddFile(entry.path().c_str(), std::string(std::filesystem::relative(entry.path(),(pkgPath)).c_str()));
       }
      break;
    case CaravelPkgType::Source:
      cw->SetMetadata("type","sources");
      
      break;
    case CaravelPkgType::DotFiles:
      cw->SetMetadata("type","config");
       for (const auto & entry : std::filesystem::recursive_directory_iterator((pkgPath / "dotfiles"))){
	 cw->AddFile(entry.path().c_str(), std::string(std::filesystem::relative(entry.path(),(pkgPath)).c_str()));
       }
      break;
    case CaravelPkgType::Assets:
      cw->SetMetadata("type","assets");
      break;
    }
    
   
    cw->AddFile((pkgPath /  "installer.lua").c_str(), std::string("install.lua"));
    cw->AddFile((pkgPath /  "uninstaller.lua").c_str(), std::string("uninstall.lua"));

    cw->AddManifest();
    delete cw;
  }
}
