
#include <caravel/CaravelTypePlugin.hpp>
#include <filesystem>
#include <functional>
#include <string>

extern "C" {
    
    const char* type_name()
    {
        return "binaries";
    }
    
    bool process_iu(std::string packageName, std::filesystem::path newInstallPath){
      std::filesystem::path uninstallPath("/usr/share/caravel-uninstall/");
      std::string uninstallFile (packageName);
      uninstallFile += ".lua";
      uninstallPath /= uninstallFile;
      if(std::filesystem::exists(uninstallPath)){
        std::filesystem::remove(uninstallPath);
      }
      std::filesystem::copy(newInstallPath, uninstallPath);
      return true;
        
    }
    
    void process_files(std::filesystem::path pkgPath, std::function<void (const char *, const char *)> addFile) {
        for (const auto & entry : std::filesystem::recursive_directory_iterator((pkgPath / "install"))){
                addFile(entry.path().c_str(), std::string(std::filesystem::relative(entry.path(),(pkgPath))).c_str());
        }
    }

    
    bool validate(std::string packageType)
    {
        return std::string(type_name()) == packageType;
    }
}
