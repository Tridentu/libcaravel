
#include "CaravelSigner.hpp"
#include <iostream>
#include <errno.h>
#include <string>
#include <mast_tk/core/LineUtils.hpp>

namespace CaravelPM {
    

    
    
    CaravelSigner::CaravelSigner(const std::string& packageName)
    {
        m_packageName = packageName;
        setlocale(LC_ALL, "");
        m_VersionString = (char*)gpgme_check_version(NULL);
        gpgme_set_locale(NULL, LC_CTYPE, setlocale(LC_CTYPE, NULL));
    }
    
    void CaravelSigner::Lockdown(bool shouldLockDown)
    {
            gpgme_set_armor(m_Context, (int)shouldLockDown);
    }

    
    
    void CaravelSigner::SetProtocol(CaravelPM::CaravelSigningProtocol protocol)
    {
        switch(protocol){
            case CaravelSigningProtocol::OpenPGP:
                CheckGPGMEError(gpgme_engine_check_version(GPGME_PROTOCOL_OpenPGP));
                break;
            case CaravelSigningProtocol::Assuan:
                CheckGPGMEError(gpgme_engine_check_version(GPGME_PROTOCOL_ASSUAN));
                break;
            case CaravelSigningProtocol::GnuPG:
                CheckGPGMEError(gpgme_engine_check_version(GPGME_PROTOCOL_DEFAULT));
                break;
            default:
                std::cerr <<  "Invalid protocol specified." << std::endl;
                exit(1);
                break;
        }
        CheckGPGMEError(gpgme_new(&m_Context));
        gpgme_set_textmode(m_Context, 1);
        switch(protocol){
            case CaravelSigningProtocol::Assuan:
                        CheckGPGMEError(gpgme_set_protocol(m_Context, GPGME_PROTOCOL_ASSUAN));
                        break;
            case CaravelSigningProtocol::OpenPGP:
                        CheckGPGMEError(gpgme_set_protocol(m_Context, GPGME_PROTOCOL_OpenPGP));
                        break;
            case CaravelSigningProtocol::GnuPG:
                        CheckGPGMEError(gpgme_set_protocol(m_Context, GPGME_PROTOCOL_DEFAULT));
                        break;
        }
    }
    
    gpgme_key_t  CaravelSigner::GenerateKey(std::string signerName)
    {
            CheckGPGMEError(gpgme_op_createkey(m_Context, signerName.c_str(), "default", 0, 0, NULL, GPGME_CREATE_SIGN));
    
        CaravelSigningParams params = CaravelSigningParams();
        params.email = signerName;
        params.keyType = "default";
        params.subKeyType = "default";
        params.comment = MastTDE::LineIO::Prompt("Enter a comment: ");
        params.signerName = MastTDE::LineIO::Prompt("Enter your full name: ");
        CheckGPGMEError(gpgme_op_genkey(m_Context, params.Generate().c_str(), NULL, NULL));
        gpgme_genkey_result_t key_result = gpgme_op_genkey_result(m_Context);
        gpgme_key_t key;
        CheckGPGMEError(gpgme_get_key(m_Context, key_result->fpr, &key, 0));
        return key;
    }

    
    void CaravelSigner::AddSigner(std::string signerName,  bool addToContext, bool getExistingKey)
    {
        if(m_potentialSigners.find(signerName) == m_potentialSigners.end()){
            if (getExistingKey) {
                gpgme_key_t key;
                CheckGPGMEError(gpgme_op_keylist_start(m_Context, signerName.c_str(), 0));
                CheckGPGMEError(gpgme_op_keylist_next(m_Context,  &key));
                m_potentialSigners.insert(std::make_pair(signerName, key));
            } else
                m_potentialSigners.insert(std::make_pair(signerName, GenerateKey(signerName)));
        }
        if (addToContext) {
            CheckGPGMEError(gpgme_signers_add(m_Context, m_potentialSigners[signerName]));
        }
        
    }
    
    void CaravelSigner::SignFile(const std::string outputFile,  CaravelSigningMode mode)
    {
        switch(mode){
            case CaravelSigningMode::Clear:
                m_SignMode = GPGME_SIG_MODE_CLEAR;
                break;
            case CaravelSigningMode::Detach:
                m_SignMode = GPGME_SIG_MODE_DETACH;
                break;
        }
        CheckGPGMEError(gpgme_data_new_from_file(&m_in, m_packageName.c_str(), 1));
        CheckGPGMEError(gpgme_data_new(&m_out));
        CheckGPGMEError(gpgme_op_sign(m_Context, m_in, m_out, m_SignMode));
        m_Output = fopen(outputFile.c_str(), "w+");
        int ret = gpgme_data_seek(m_out, 0, SEEK_SET);
        if(ret)
            CheckGPGMEError(gpg_err_code_from_errno(errno));
        
        while ((ret = gpgme_data_read(m_out, m_buf, CARAVEL_SIGN_BUF_SIZE)) > 0){
            fwrite(m_buf, ret, 1, m_Output);
        }
        
        if (ret < 0)
            CheckGPGMEError(gpgme_err_code_from_errno(errno));
        
        fclose(m_Output);
        
        delete this;
    }

    CaravelSigner::~CaravelSigner()
    {
        gpgme_data_release(m_in);
        gpgme_data_release(m_out);
        gpgme_release(m_Context);
    }

    
    

    
    
}
