

#include <sstream>
#include <iostream>
#include <filesystem>
#include "CaravelDownloader.h"
#include "../CaravelUtils.h"

namespace CaravelPM {

  

  
  CaravelDownloader::CaravelDownloader(std::string pkg, std::string repo, bool useDatabase, bool useTempFolder, std::string pkgSpace){
    m_pkgName = pkg;
    std::stringstream urlS;
    m_UseTemp = useTempFolder;
    isDatabase = useDatabase;
    if (!useDatabase)
      urlS <<  repo << pkgSpace << pkg << ".caravel";
    else
      urlS << repo << "/pman.caraveldb";
    m_HandleHttp = curl_easy_init();
    curl_easy_setopt(m_HandleHttp,CURLOPT_URL,urlS.str().c_str());
    
  };

  CaravelDownloader::CaravelDownloader(std::string pkg,  std::string pkgUrl){
    m_pkgName = pkg;
    isDatabase = false;
    m_HandleHttp = curl_easy_init();
    curl_easy_setopt(m_HandleHttp,CURLOPT_URL,pkgUrl.c_str());
  };
  
    size_t proginfo(void* p, size_t dltotal, size_t dlnow, size_t ultotal, size_t ulnow){
        std::cout << std::flush;
        if (dltotal > 0){
            struct CaravelDLTInfo* info = (struct CaravelDLTInfo*)p;
            if(info->bar.is_completed()){
                std::cout << std::flush;
                return 0;
            }
            info->currProgress = (float)((double)dlnow/(double)dltotal);
            info->bar.set_progress((info->currProgress * 100.0));
        }
        return 0;
    }
  

    void  CaravelDownloader::Run(){
	FILE* fp;
	std::stringstream filenameS;
	if(isDatabase){
        if(!m_UseTemp)
            filenameS << getenv("HOME") <<  "/pman.caraveldb";
        else
            filenameS << "/tmp/pman.caraveldb";
    } else {
      if(!m_UseTemp)
        filenameS << getenv("HOME") <<  "/" <<  m_pkgName << ".caravel";
      else
        filenameS << "/tmp" << "/" <<  m_pkgName << ".caravel";
    }
	CURLcode res;
	fp = fopen(filenameS.str().c_str(), "wb");
    indicators::show_console_cursor(false);
    struct CaravelDLTInfo statinfo;
    statinfo.curl = m_HandleHttp;
    
	curl_easy_setopt(m_HandleHttp,  CURLOPT_WRITEFUNCTION, writeData);
	curl_easy_setopt(m_HandleHttp, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(m_HandleHttp, CURLOPT_XFERINFODATA, &statinfo);
    curl_easy_setopt(m_HandleHttp, CURLOPT_XFERINFOFUNCTION, &proginfo);
    curl_easy_setopt(m_HandleHttp, CURLOPT_NOPROGRESS, 0L);
	res = curl_easy_perform(m_HandleHttp);
    indicators::show_console_cursor(true);
	fclose(fp);
	delete this;
       
  }
  
  CaravelDownloader::~CaravelDownloader(){
    curl_easy_cleanup(m_HandleHttp);
  }

  
  
}
