

#pragma once

#include <gpgme.h>
#include <string>
#include <filesystem>
#include <curl/curl.h>
#include "../CaravelUtils.h"



namespace CaravelPM {
    class CaravelPackageChecker  {
    public:
        CaravelPackageChecker(std::string packageName, bool isLocalFile = false, std::string packageName_Global = "");
        ~CaravelPackageChecker();
    public:
        void LoadSignatureAndContents(std::string repo, bool downloadFirst = false,  std::string arch = "x86_64");
        bool Verify();
        
    private:
        std::string m_packageName;
        std::filesystem::path m_packagePath;
        std::string m_signaturePath;
        CURL* m_HandleHttp;
        FILE* m_SigHandle;
        FILE* m_SigHandleDown;
        FILE* m_ContentHandle;
        gpgme_ctx_t m_Context;
        gpgme_data_t m_SigData;
        gpgme_data_t m_FileData;
    };
}
