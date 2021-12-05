

#include <sstream>
#include <iostream>
#include <filesystem>
#include "CaravelDownloader.h"

namespace CaravelPM {

  size_t writeData(void* ptr, size_t size, size_t nmemb, FILE *stream){
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
  };
  
  CaravelDownloader::CaravelDownloader(std::string pkg, bool useDatabase){
    m_pkgName = pkg;
    std::stringstream urlS;
    isDatabase = useDatabase;
    if (!useDatabase)
      urlS << "https://tridentu.github.io/acquirium/packages/x86_64/" << pkg << ".caravel";
    else
      urlS << "https://tridentu.github.io/acquirium/pman.caraveldb";
    m_HandleHttp = curl_easy_init();
    curl_easy_setopt(m_HandleHttp,CURLOPT_URL,urlS.str().c_str());
  };

  CaravelDownloader::CaravelDownloader(std::string pkg, std::string pkgUrl){
    m_pkgName = pkg;
    isDatabase = false;
    m_HandleHttp = curl_easy_init();
    std::cout << pkgUrl << std::endl;
    curl_easy_setopt(m_HandleHttp,CURLOPT_URL,pkgUrl.c_str());
  };

 void  CaravelDownloader::Run(){
	FILE* fp;
	std::stringstream filenameS;
	if(isDatabase)
	  filenameS << getenv("HOME") <<  "/pman.caraveldb";
	else
	  filenameS << getenv("HOME") << "/" <<  m_pkgName << ".caravel";
	CURLcode res;
	fp = fopen(filenameS.str().c_str(), "wb");
	curl_easy_setopt(m_HandleHttp,  CURLOPT_WRITEFUNCTION, writeData);
	curl_easy_setopt(m_HandleHttp, CURLOPT_WRITEDATA, fp);
	res = curl_easy_perform(m_HandleHttp);
	fclose(fp);
	delete this;
       
  }
  
  CaravelDownloader::~CaravelDownloader(){
    curl_easy_cleanup(m_HandleHttp);
  }

  
  
}
