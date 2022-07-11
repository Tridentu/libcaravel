
#include "CaravelPackageChecker.hpp"
#include <sstream>

namespace CaravelPM {
   
    CaravelPackageChecker::CaravelPackageChecker(std::string packageName, bool isLocalFile, std::string packageName_Global)
    {
        gpgme_check_version (NULL);
        setlocale (LC_ALL, "");
        gpgme_set_locale (NULL, LC_CTYPE, setlocale (LC_CTYPE, NULL));
        m_packageName = packageName;
        m_packagePath = (!isLocalFile) ? (std::string(getenv("HOME")) + "/ccontainer/" + std::string(m_packageName + ".caravel")) : m_packageName;
        m_signaturePath = std::string(std::string(getenv("HOME")) + "/"  + std::string(m_packagePath.stem().string() + ".caravel.sig"));
        if(isLocalFile){
            m_packageName = packageName_Global;
            m_signaturePath = std::string(std::string(getenv("HOME")) + "/"  + std::string(packageName_Global + ".caravel.sig"));
        }
    }

    void CaravelPackageChecker::LoadSignatureAndContents(bool downloadFirst, std::string arch)
    {
        
        if (downloadFirst){
            std::stringstream url;
            url << "https://tridentu.github.io/acquirium/sigs/" << arch << "/" << m_packageName << ".caravel.sig";
            m_HandleHttp = curl_easy_init();
            curl_easy_setopt(m_HandleHttp, CURLOPT_URL, url.str().c_str());
            m_SigHandleDown = fopen(m_signaturePath.c_str(), "w+");
            curl_easy_setopt(m_HandleHttp,  CURLOPT_WRITEFUNCTION, writeData);
            curl_easy_setopt(m_HandleHttp, CURLOPT_WRITEDATA, m_SigHandleDown);
            CURLcode res = curl_easy_perform(m_HandleHttp);
            fclose(m_SigHandleDown);
            curl_easy_cleanup(m_HandleHttp);
        }
        
        m_SigHandle = fopen(m_signaturePath.c_str(), "rb");
        if(!m_SigHandle){
            gpgme_error_t err = gpgme_error_from_syserror();
            std::cout << "The signature file could not be opened (" << gpgme_strerror(err) << "). Aborting..." << std::endl ;
            exit(1);
        }
        m_ContentHandle = fopen(m_packagePath.c_str(), "rb");
        if(!m_ContentHandle){
            gpgme_error_t err = gpgme_error_from_syserror();
            std::cout << "The package could not be opened (" << gpgme_strerror(err) << "). Aborting..." << std::endl ;
            exit(1);
        }
        CheckGPGMEError(gpgme_new(&m_Context), "An error occurred while verifying the given package.");
        CheckGPGMEError(gpgme_engine_check_version(GPGME_PROTOCOL_OpenPGP), "An error occurred while verifying the given package.");
        CheckGPGMEError(gpgme_set_protocol(m_Context, GPGME_PROTOCOL_OpenPGP), "An error occurred while verifying the given package.");
        CheckGPGMEError(gpgme_data_new_from_stream(&m_SigData, m_SigHandle),"An error occurred while verifying the given package.");
        CheckGPGMEError(gpgme_data_new_from_stream(&m_FileData, m_ContentHandle),"An error occurred while verifying the given package.");
    }
    
    bool CaravelPackageChecker::Verify(){
            CheckGPGMEError(gpgme_op_verify(m_Context, m_SigData, m_FileData, NULL), "An error occurred while verifying the given package.");
            gpgme_verify_result_t result = gpgme_op_verify_result(m_Context);
            return (bool)(result != NULL);
    }

    CaravelPackageChecker::~CaravelPackageChecker(){
            gpgme_data_release (m_FileData);
            gpgme_data_release (m_SigData);
            gpgme_release (m_Context);
            fclose(m_ContentHandle);
            fclose(m_SigHandle);
            std::filesystem::remove(std::filesystem::path(m_signaturePath));
    }
}
