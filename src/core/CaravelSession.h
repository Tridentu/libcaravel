#pragma once

#include "repository/CaravelRepository.h"
#include <string>
#include <iostream>
#include <vector>
#include "packages/CaravelTypeLoader.hpp"
#include "packages/CaravelReader.h"
#include "CaravelLogger.h"
namespace CaravelPM {
    class CaravelSession {
    
    public:
        CaravelSession();
    public:
        /**
         * Retrieves the url for which packages are retrieved.
         */
        std::string GetDownloadUrl(std::string urlStub);
        std::string GetRepoUrl();
        /**
         * Retrieves the "download directory" for the given package type.
         */
        std::string GetPackagePath(std::string packageType);
    public:
        CaravelPackageType getPackageType(std::string typeName);
        void Create(std::string packageDir, CaravelPackageType packageType, std::map<std::string, std::string> propMap);
        bool ReadAndInstall(std::string pathString, std::string packageFile);
        CaravelReader* getReader(std::string pathString, std::string packageFile);
        void writeToLog(CaravelPM::LogLevel level, std::string msg);
        void uninstallPackage(std::string uninstallScriptPath);
    protected:
        void Load();
    private:
        CaravelPM::CaravelRepository* m_Repository;
        CaravelPM::CaravelTypeLoader* loader;
        CaravelPM::Logger* logger;
        
    };
};
