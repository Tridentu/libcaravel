
#pragma once 
#include <curl/curl.h>
#include <string>
#include <indicatorslib.hpp>

namespace CaravelPM {
    
    struct CaravelDLTInfo {
        CURL* curl;
        curl_off_t lastTick = 0;
        float currProgress = 0.0f;
        indicators::ProgressBar bar{
            indicators::option::BarWidth{50},
            indicators::option::Start{"["},
            indicators::option::Fill{"="},
            indicators::option::Lead{">"},
            indicators::option::Remainder{" "},
            indicators::option::End{" ]"},
            indicators::option::PostfixText{"Downloading"},
            indicators::option::ForegroundColor{indicators::Color::green},
            indicators::option::FontStyles{std::vector<indicators::FontStyle>{ indicators::FontStyle::bold}}
        };
    };
    
  class CaravelDownloader {

  public:
    CaravelDownloader(std::string pkg, std::string repo, bool useDatabase = false, bool useTempFolder = false, std::string pkgSpace = "/packages/x86_64/");
    CaravelDownloader(std::string pkg, std::string pkgUrl);
    ~CaravelDownloader();
    void Run();
  private:
    CURL* m_HandleHttp;
    int m_Running = 1;
    std::string m_URL;
    bool isDatabase;
    bool m_UseTemp = true;
    std::string m_pkgName;
  };
}
