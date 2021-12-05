

#include <curl/curl.h>
#include <string>

namespace CaravelPM {
  class CaravelDownloader {

  public:
    CaravelDownloader(std::string pkg, bool useDatabase = false);
    CaravelDownloader(std::string pkg, std::string pkgUrl);
    ~CaravelDownloader();
    void Run();
  private:
    CURL* m_HandleHttp;
    int m_Running = 1;
    std::string m_URL;
    bool isDatabase;
    std::string m_pkgName;
  };
}