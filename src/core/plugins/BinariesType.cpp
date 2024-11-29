
#include <caravel/packages/CaravelTypePlugin.hpp>
#include <filesystem>
#include <functional>
#include <string>


const char* getBuild() {
    std::string arch;
    // Use uname to get the machine architecture
    if (system("uname -m > /tmp/arch.txt") == 0) {
        FILE *fp = fopen("/tmp/arch.txt", "r");
        if (fp) {
            char buffer[256];
            if (fgets(buffer, sizeof(buffer), fp) != NULL) {
                arch = buffer;
                arch.erase(arch.find_last_not_of("\n") + 1); // Remove trailing newline
            }
            fclose(fp);
            remove("/tmp/arch.txt"); // Clean up temporary file
        }
    }
    return arch.c_str();

}

extern "C" {
    
    const char* type_name()
    {
        return "binaries";
    }


    
    const char* ver_dir(){
        return getBuild();
    }

    std::string download_dir(){
        std::string downloadd = (std::string("/packages/") + std::string(getBuild() + std::string("/")));
        return downloadd;
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
        {
            const auto& entry = (pkgPath / "install.lua");
            addFile(entry.c_str(), std::string(std::filesystem::relative(entry,(pkgPath))).c_str());
        }
        {
            const auto& entry = (pkgPath / "uninstall.lua");
            addFile(entry.c_str(), std::string(std::filesystem::relative(entry,(pkgPath))).c_str());
        }
    }

    
    bool validate(std::string packageType)
    {
        return std::string(type_name()) == packageType;
    }
}
