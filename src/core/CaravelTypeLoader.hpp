#pragma once 



#include <dlfcn.h>
#include <map>
#include <vector>
#include <string>
#include <filesystem>
#include <functional>
#include "CaravelTypePlugin.hpp"

namespace CaravelPM {
 

    struct CaravelPackageType {
      std::string name;
      caravel_type_validate_t validate;
      caravel_type_installu_t processInstallUninstaller;
      caravel_type_files_t processFiles;
      CaravelPackageType(){};
      CaravelPackageType(const CaravelPackageType& other) = default;
    };
    
    class CaravelTypeLoader {
    public:
        CaravelTypeLoader();
        ~CaravelTypeLoader();
    public:
        void loadAllTypes();
        CaravelPackageType getPackageType(std::string packageType);
        void destroyPackageType(std::string packageType);
    private:
        void* openPluginSO(std::filesystem::path so_path);
        void* getPackageTypeSO(std::string packageType);
    private:
        std::map<std::string, CaravelPackageType> packageTypes;
        std::map<std::string, void*> packageSOs;

    };
    

    
}
