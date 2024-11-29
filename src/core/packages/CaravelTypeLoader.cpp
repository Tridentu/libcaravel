



#include <dlfcn.h>
#include <cstring>
#include <cstdio>
#include <map>
#include <string>
#include <iostream>
#include <algorithm>
#include "CaravelTypeLoader.hpp"

namespace CaravelPM {
    
    
    CaravelTypeLoader::CaravelTypeLoader(){
        
    }
    
    void* CaravelTypeLoader::openPluginSO(std::filesystem::path so_path) {
        void* caravelPkgType = dlopen(so_path.string().c_str(), RTLD_NOW);
        if (!caravelPkgType) {
            std::cerr << "Error loading Caravel Package Type: " << dlerror() <<  std::endl;
            return nullptr;
        }
        dlerror();
        return caravelPkgType;
    }
    
     CaravelPackageType CaravelTypeLoader::getPackageType(std::string packageType){
        auto it = packageTypes.find(packageType);
        if (it != packageTypes.end()){
            return it->second;
        }
        return CaravelPackageType();
    }
    
    void* CaravelTypeLoader::getPackageTypeSO(std::string packageType){
            return packageSOs[packageType];
    }
    
    void CaravelTypeLoader::destroyPackageType(std::string packageType){
           dlclose(getPackageTypeSO(packageType));
    }
    
    void CaravelTypeLoader::loadAllTypes(){
      for (const std::filesystem::directory_entry& dir_entry : std::filesystem::directory_iterator{std::filesystem::path(std::string("/usr/lib/caravel-plugins/"))}) {
          if (dir_entry.path().extension() == ".so"){
                void* so_obj = openPluginSO(dir_entry.path());
                if(so_obj){
                    CaravelPackageType pt = CaravelPackageType();
                    {
                        caravel_type_t creator = (caravel_type_t)((dlsym(so_obj, "type_name")));
                        if (!creator) {
                            std::cerr << "Cannot load symbol type_name: " << dlerror() << '\n';
                            continue;
                        };
                        pt.name = std::string(creator());
                    }
                    dlerror();
                    {
                        caravel_type_validate_t validate = (caravel_type_validate_t)((dlsym(so_obj, "validate")));
                        if (!validate) {
                            std::cerr << "Cannot load symbol validate: " << dlerror() << '\n';
                            continue;
                        };

                        pt.validate = validate;
                    }
                    dlerror();
                    {
                        caravel_type_installu_t process_iu = (caravel_type_installu_t)((dlsym(so_obj, "process_iu")));
                        if (!process_iu) {
                            std::cerr << "Cannot load symbol process_iu: " << dlerror() << '\n';
                            continue;
                        };
                       pt.processInstallUninstaller = process_iu;
                    }
                    dlerror();
                    {
                        caravel_type_files_t files = (caravel_type_files_t)((dlsym(so_obj, "process_files")));
                        if (!files) {
                            std::cerr << "Cannot load symbol process_files: " << dlerror() << '\n';
                            continue;
                        };
                        pt.processFiles = files;

                    }
                    dlerror();
                     {
                        caravel_type_verdir_t ver_dir = (caravel_type_verdir_t)((dlsym(so_obj, "ver_dir")));
                        if (!ver_dir) {
                            std::cerr << "Cannot load symbol ver_dir: " << dlerror() << '\n';
                            continue;
                        };
                        pt.ver_dir = ver_dir;

                    }
                    dlerror();
                    {
                        caravel_type_download_dir_t download_dir = (caravel_type_download_dir_t)((dlsym(so_obj, "download_dir")));
                        if (!download_dir) {
                            std::cerr << "Cannot load symbol download_dir: " << dlerror() << '\n';
                            continue;
                        };
                        pt.download_dir = download_dir;

                    }
                    dlerror();
                    packageTypes.insert(std::make_pair(pt.name,pt));
                    packageSOs.insert(std::make_pair(pt.name, so_obj));

                }
                

          }
      }
    }
    
    CaravelTypeLoader::~CaravelTypeLoader(){
       
    }
}
