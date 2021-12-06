#include "CaravelDBContext.h"
#include <string>
#include <vector>
#include <regex>
#include <iostream>
#include <map>
#include <sqlite3.h>
#include "CaravelDownloader.h"

static int dbCallback(void* data, int argc, char **argv, char **colName);
static bool done = false;
static std::vector<CaravelPM::CaravelPackageInfo> infos;
namespace CaravelPM {

  CaravelDBContext* CaravelDBContext::s_CurrentContext = nullptr;

  void CaravelDBContext::InitDB(std::string databaseFile){
    s_CurrentContext = new CaravelDBContext(databaseFile);
  }

  CaravelDBContext* CaravelDBContext::GetDB(){
    return s_CurrentContext;
  }
  
  CaravelDBContext::CaravelDBContext(std::string databaseFile){
    m_dbFile = databaseFile;
    Init();
  }

  CaravelDBContext::~CaravelDBContext(){
    sqlite3_close(m_DB);
    
  }

  void CaravelDBContext::Init(){
    std::string dbPath = m_dbFile;
    if(m_dbFile.find("https") != std::string::npos){
      std::cout << "Downloading Repository Database..." << std::endl;
      CaravelDownloader* downl = new CaravelDownloader("database", true);
      downl->Run();
      dbPath = std::string(getenv("HOME"));
      dbPath += "/pman.caraveldb";
      m_dbFile = dbPath;
      std::cout << "Database Downloaded." << std::endl;
    }
    int rc = sqlite3_open(m_dbFile.c_str(), &m_DB);
    if (rc){
      std::cerr << "Can't use manifest." << std::endl;
      exit(0);
    } else {
      std::cout << "Opened repo manifest successfully." << std::endl;
    }
  }

  std::string CaravelDBContext::FindNamespace(std::string packageName){
    infos.clear();
    std::string data = std::string("CALLBACK FUNCTION").c_str();
    char* errMsg = 0;
    std::string result;
    std::string sql = "SELECT * FROM packageinfo WHERE name=\'" + packageName + "\';";
    int rc = sqlite3_exec(m_DB, sql.c_str(), dbCallback, (void*)data.c_str(), &errMsg);
    if(rc != SQLITE_OK ){
      std::cerr << "SQL error detected: " << errMsg << std::endl;
      sqlite3_free(errMsg);
      return result;
    } else {
      int i = 0;
      while(!done)
	i++;
      if(infos.size() < 1)
        result = "";
      CaravelPackageInfo record;
      for (int i = 0; i < infos.size(); ++i){
	if (!infos.at(i).Namespace.empty()){
	  record = infos.at(i);
	  break;
	}
      }
      result = record.Namespace;
    }
    return result;
  }

  std::string CaravelDBContext::FindType(std::string packageName){
    infos.clear();
    std::string data = std::string("CALLBACK FUNCTION").c_str();
    char* errMsg = 0;
    std::string result;
    std::string sql = "SELECT * FROM packageinfo WHERE name=\'" + packageName + "\';";
    int rc = sqlite3_exec(m_DB, sql.c_str(), dbCallback, (void*)data.c_str(), &errMsg);
    if(rc != SQLITE_OK ){
      std::cerr << "SQL error detected: " << errMsg << std::endl;
      sqlite3_free(errMsg);
      return result;
    } else {
      int i = 0;
       while(!done)
	i++;
      if(infos.empty())
	return result;
      CaravelPackageInfo record;
      for (int i = 0; i < infos.size(); ++i){
	if (!infos.at(i).Namespace.empty()){
	  record = infos.at(i);
	  break;
	}
      }
      result = record.PackageType;
    }
    return result;
  }

  std::string CaravelDBContext::FindCategory(std::string packageName){
    infos.clear();
    std::string data = std::string("CALLBACK FUNCTION").c_str();
    char* errMsg = 0;
    std::string result;
    std::string sql = "SELECT * FROM packageinfo WHERE name=\'" + packageName + "\';";
    int rc = sqlite3_exec(m_DB, sql.c_str(), dbCallback, (void*)data.c_str(), &errMsg);
    if(rc != SQLITE_OK ){
      std::cerr << "SQL error detected: " << errMsg << std::endl;
      sqlite3_free(errMsg);
      return result;
    } else {
       int i = 0;
       while(!done)
	i++;
      if(infos.empty())
	return result;
      CaravelPackageInfo record;
       for (int i = 0; i < infos.size(); ++i){
	if (!infos.at(i).Namespace.empty()){
	  record = infos.at(i);
	  break;
	}
      }
      result = record.PackageCategory;
    }
    return result;
  }

  std::vector<CaravelPackageInfo> CaravelDBContext::GetPackageRecords(){
    return m_PackageRecords;
  }

  void CaravelDBContext::FindPackagesOfType(std::string type){
    infos.clear();
    std::string data = std::string("CALLBACK FUNCTION").c_str();
    char* errMsg = 0;
    std::string result;
    std::string sql = "SELECT * FROM packageinfo WHERE pkgType=\'" + type + "\';";
    int rc = sqlite3_exec(m_DB, sql.c_str(), dbCallback, (void*)data.c_str(), &errMsg);
    if(rc != SQLITE_OK ){
      std::cerr << "SQL error detected: " << errMsg << std::endl;
      sqlite3_free(errMsg);
      return;
    } else {
      int i = 0;
       while(!done)
	i++;
      if(infos.empty())
	return;
      
    }
  }

  void CaravelDBContext::Cleanup(){
    delete s_CurrentContext;
  }

   void CaravelDBContext::FindPackagesInNamespace(std::string pNamespace){
     infos.clear();
     std::string data = std::string("CALLBACK FUNCTION").c_str();
    char* errMsg = 0;
    std::string result;
    std::string sql = "SELECT * FROM packageinfo WHERE architecture=\'" + pNamespace + "\';";
    int rc = sqlite3_exec(m_DB, sql.c_str(), dbCallback, (void*)data.c_str(), &errMsg);
    if(rc != SQLITE_OK ){
      std::cerr << "SQL error detected: " << errMsg << std::endl;
      sqlite3_free(errMsg);
      return;
    } else {
      int i = 0;
       while(!done)
	i++;
      if(infos.empty())
	return;
    }
  }

   void CaravelDBContext::FindPackagesInCategory(std::string category){
     infos.clear();
     std::string data = std::string("CALLBACK FUNCTION").c_str();
    char* errMsg = 0;
    std::string result;
    std::string sql = "SELECT * FROM packageinfo WHERE category=\'" + category + "\';";
    int rc = sqlite3_exec(m_DB, sql.c_str(), dbCallback, (void*)data.c_str(), &errMsg);
    if(rc != SQLITE_OK ){
      std::cerr << "SQL error detected: " << errMsg << std::endl;
      sqlite3_free(errMsg);
      return;
    } else {
       int i = 0;
       while(!done)
	i++;
      if(infos.empty())
	return;
    }
  }

  std::vector<CaravelPackageInfo> CaravelDBContext::FindPackagesFromNameQuery(std::string query){
    infos.clear();
    std::string data = std::string("CALLBACK FUNCTION").c_str();
    char* errMsg = 0;
    std::string result;
    std::string sql = "SELECT * FROM packageinfo WHERE name LIKE '%" + query + "%';";
    int rc = sqlite3_exec(m_DB, sql.c_str(), dbCallback, (void*)data.c_str(), &errMsg);
    if(rc != SQLITE_OK ){
      std::cerr << "SQL error detected: " << errMsg << std::endl;
      sqlite3_free(errMsg);
      return infos;
    } else {
      int i = 0;
      while(!done)
        i++;;
    }
    return infos;
  }


  std::string CaravelDBContext::GetPackageLink(std::string packageName){
    std::string pNamespace = FindNamespace(packageName);
    return "https://tridentu.github.io/acquirium/packages/" + pNamespace + "/" + packageName + ".caravel";
  }
  
  void CaravelDBContext::ClearPackageRecords(){
    infos.clear();
  }

  void CaravelDBContext::AddRecord(CaravelPackageInfo info){
    infos.push_back(info);
  }

  void CaravelDBContext::MarkDone(){
    m_Done = true;
  }
  
}

static int dbCallback(void* data, int argc, char **argv, char **colName){
  std::map<std::string, std::string> propMap;
  int packageRecIndex = 0;
  done = false;
  for(int i = 0; i < argc; i++){
    std::string column(colName[i]);
    if(column == "name" || column == "description" || column == "pkgType" || column == "category" || column ==  "architecture"){
      propMap[column] = std::string(argv[i] ? argv[i] : "NULL");
      packageRecIndex++;
    }
    if(packageRecIndex % 5 == 0 && packageRecIndex > 0){
      CaravelPM::CaravelPackageInfo info;
      info.Namespace = propMap["architecture"];
      info.PackageType = propMap["pkgType"];
      info.Description = propMap["description"];
      info.PackageCategory = propMap["category"];
      info.PackageName = propMap["name"];
      infos.push_back(info);
      propMap.clear();
    }
    
  }
  done = true;
  return 0;
}
