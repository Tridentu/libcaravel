
#pragma once 

#include <gpgme.h>
#include <string>
#include <sstream>
#include <map>

namespace CaravelPM {
    
    enum class CaravelSigningProtocol {
      OpenPGP,
      Assuan,
      GnuPG
    };
    
    enum class CaravelSigningMode {
            Clear,
            Detach
    };
    
    const int CARAVEL_SIGN_BUF_SIZE = 512;
    
    struct CaravelSigningParams {
      
        std::string keyType;
        std::string subKeyType;
        std::string signerName;
        std::string comment;
        std::string email;
        int expireDate = 0;
        std::string passphrase;
        CaravelSigningParams() {};
        CaravelSigningParams(const CaravelSigningParams& other) = default;
        std::string Generate() {
         std::stringstream ss;
         ss << "<GnupgKeyParms format=\"internal\">" << std::endl;
         ss << "Key-Type: " << keyType << std::endl;
         ss << "Subkey-Type: " << subKeyType << std::endl;
         ss << "Name-Real: " << signerName << std::endl;
         ss << "Name-Comment: " << comment << std::endl;
         ss << "Name-Email: " << email << std::endl;
         ss << "Expire-Date: " << std::to_string(expireDate) << std::endl;
         ss << "</GnupgKeyParms>" << std::endl;
         return ss.str();
        }
    };
    
    
    void CheckGPGMEError(gpgme_error_t err, std::string message = "A GPG Error occurred.");
 
    class CaravelSigner {
    public:
        CaravelSigner(const std::string& packageName);
        ~CaravelSigner();
    public:
        void SetProtocol(CaravelSigningProtocol protocol);
        void Lockdown(bool shouldLockDown);
        void AddSigner(std::string signerName, bool addToContext = false, bool getExistingKey = false);
        void SignFile(const std::string outputFile, CaravelSigningMode mode);
        
    private:
        gpgme_key_t GenerateKey(std::string signerName);
    private:
        std::string m_packageName;
        gpgme_ctx_t m_Context;
        gpgme_data_t m_in, m_out;
        char* m_VersionString;
        gpgme_sig_mode_t m_SignMode;
        FILE* m_Output;
        char m_buf[CARAVEL_SIGN_BUF_SIZE]; 
        std::map<std::string, gpgme_key_t> m_potentialSigners;
    };
    
}
