#pragma once

#include "CaravelRepository.h"
#include <string>
#include <iostream>
#include <vector>
#include "CaravelTypeLoader.hpp"
#include "CaravelReader.h"

namespace CaravelPM {
    class CaravelSession {
    
    public:
        CaravelSession();
    public:
        std::string GetDownloadUrl(std::string urlStub);
        CaravelPackageType getPackageType(std::string typeName);
        void Create(std::string packageDir, CaravelPackageType packageType, std::map<std::string, std::string> propMap);
        bool ReadAndInstall(std::string pathString, std::string packageFile);
        CaravelReader* getReader(std::string pathString, std::string packageFile);
    protected:
        void Load();
    private:
        CaravelPM::CaravelRepository* m_Repository;
        CaravelPM::CaravelTypeLoader* loader;
        
    };
};
