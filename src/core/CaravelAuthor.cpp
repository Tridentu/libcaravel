
#include "CaravelWriter.h"
#include "CaravelUtils.h"
#include "CaravelAuthor.h"
#include <filesystem>
#include <string>

namespace CaravelPM {

  void CaravelAuthor::CreatePackage(std::string pkgName, std::string type, std::map<std::string, std::string> extraProps, CaravelTypeLoader* newLoader){
    if(!newLoader)
        return;
    CaravelWriter* cw = new CaravelWriter(pkgName,type);
    std::filesystem::path pkgPath = std::filesystem::current_path();
    pkgPath += "/";
    pkgPath += pkgName;
    cw->SetMetadata("name",pkgName);
    auto packageType = newLoader->getPackageType(type);
    if(!packageType.name.empty()){
        cw->SetMetadata("type",type);
        packageType.processFiles(pkgPath, [&](const char* path1, const char* path2){
            cw->AddFile(path1, path2);
        });
    }
  

    for (auto const& [key, val] : extraProps){
        cw->SetMetadata(key, val);
    }
    
   
    cw->AddFile((pkgPath /  "installer.lua").c_str(), std::string("install.lua"));
    cw->AddFile((pkgPath /  "uninstaller.lua").c_str(), std::string("uninstall.lua"));

    cw->AddManifest();
    delete cw;
  }
}
