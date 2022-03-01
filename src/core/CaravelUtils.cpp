
#include "CaravelUtils.h"
namespace CaravelPM {
     size_t writeData(void* ptr, size_t size, size_t nmemb, FILE *stream){
        size_t written = fwrite(ptr, size, nmemb, stream);
        return written;
    }
  
   void CheckGPGMEError(gpgme_error_t err, std::string message)
    {
        if (err != GPG_ERR_NO_ERROR){
            std::cerr << message << std::endl;
            std::cerr << gpgme_strerror(err) << std::endl;
            exit(1);
        }
    }

}
