
#pragma once 

#include <caravel/CaravelTypePlugin.hpp>
#include <filesystem>
#include <functional>
#include <string>



extern "C" {
    
    const char* type_name()
    {
        return "config";
    }
    
    bool process_iu(std::string packageName){
      return true;
    }
    
    void process_files(std::filesystem::path pkgPath, std::function<void (const char *, const char *)> addFile) {
        for (const auto & entry : std::filesystem::recursive_directory_iterator((pkgPath / "dotfiles"))){
                addFile(entry.path().c_str(), std::string(std::filesystem::relative(entry.path(),(pkgPath))).c_str());
        }
    }

    
    bool validate(std::string packageType)
    {
        return std::string(type_name()) == packageType;
    }
}
