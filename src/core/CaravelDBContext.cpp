#include "CaravelDBContext.h"
#include <string>
#include <vector>
#include <regex>
#include <iostream>
#include <map>
#include <sqlite3.h>
#include <filesystem>
#include "CaravelDownloader.h"

static int dbCallback(void* data, int argc, char **argv, char **colName);
static int dbCallbackPkgId(void* data, int argc, char **argv, char **colName);
static int dbCallbackPkgGroup(void* data, int argc, char **argv, char **colName);

static bool done = false;
static std::vector<CaravelPM::CaravelPackageInfo> infos;
static std::vector<CaravelPM::CaravelPackageId> ids;
static std::vector<CaravelPM::CaravelPackageGroup> groups;

namespace CaravelPM {

  CaravelDBContext* CaravelDBContext::s_CurrentContext = nullptr;

  void CaravelDBContext::InitDB(std::string databaseFile, bool useTemp){
    s_CurrentContext = new CaravelDBContext(databaseFile, useTemp);
  }

  CaravelDBContext* CaravelDBContext::GetDB(){
    return s_CurrentContext;
  }
  
  CaravelDBContext::CaravelDBContext(std::string databaseFile, bool useTemp){
    m_dbFile = databaseFile;
    Init(useTemp);
  }

  CaravelDBContext::~CaravelDBContext(){
    sqlite3_close(m_DB);
    
  }

  void CaravelDBContext::Init(bool temp, bool debug){
    std::string dbPath = m_dbFile;
    if(m_dbFile.find("https") != std::string::npos){
      if(debug){
          std::cout << "Downloading Repository Database..." << std::endl;
      }
      CaravelDownloader* downl = new CaravelDownloader("database", true, temp);
      downl->Run();

      if(temp)
          dbPath = "/tmp/pman.caraveldb";
      else {
        dbPath = std::string(getenv("HOME"));
        dbPath += "/pman.caraveldb";
      }
      m_dbFile = dbPath;
      if(debug){
         std::cout << "Database Downloaded." << std::endl;
      }
    }
    int rc = sqlite3_open(m_dbFile.c_str(), &m_DB);
    if (rc){
      if(debug) {
          std::cerr << "Can't use manifest." << std::endl;
      }
      exit(0);
    } else {
      if(debug){
          std::cout << "Opened repo manifest successfully." << std::endl;
      }
    }
  }

  std::string CaravelDBContext::FindNamespace(std::string packageName){
    infos.clear();
    std::string data = std::string("CALLBACK FUNCTION").c_str();
    char* errMsg = 0;
    std::string result = "";
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
    std::string result = "";
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
  
  CaravelPackageId* CaravelDBContext::FindPackageId(CaravelPackageInfo packageInfo){
    ids.clear();
    std::string data = std::string("CALLBACK FUNCTION").c_str();
    char* errMsg = 0;
    CaravelPackageId* result;
    std::stringstream ss;
    ss << "SELECT * FROM packagekit_ids WHERE package_table_id=" << packageInfo.Id << ";";
    std::string sql = ss.str();
    std::cout << sql << std::endl;
    int rc = sqlite3_exec(m_DB, sql.c_str(), dbCallbackPkgId, (void*)data.c_str(), &errMsg);
    if(rc != SQLITE_OK ){
      std::cerr << "SQL error detected: " << errMsg << std::endl;
      sqlite3_free(errMsg);
      return nullptr;
    } else {
      int i = 0;
      while(!done)
        i++;
      if(ids.empty())
        return nullptr;
      CaravelPackageId* record;
      for (int i = 0; i < ids.size(); i++){
        if (!ids.at(i).PackageId.empty()){
            record = &ids.at(i);
            break;
        }
      }
      result = record;
    }
    return result;
  }

  std::string CaravelDBContext::FindCategory(std::string packageName){
    infos.clear();
    std::string data = std::string("CALLBACK FUNCTION").c_str();
    char* errMsg = 0;
    std::string result = "";
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
    std::string result = "";
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
    std::string result = "";
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
    std::string result = "";
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

   std::vector<CaravelPackageInfo> CaravelDBContext::GetAllPackages(){
    infos.clear();
    std::string data = std::string("CALLBACK FUNCTION").c_str();
    char* errMsg = 0;
    std::string result = "";
    std::string sql = "SELECT * FROM packageinfo;";
    int rc = sqlite3_exec(m_DB, sql.c_str(), dbCallback, (void*)data.c_str(), &errMsg);
    if(rc != SQLITE_OK ){
      std::cerr << "SQL error detected: " << errMsg << std::endl;
      sqlite3_free(errMsg);
      return infos;
    } else {
      int i = 0;
      while(!done)
        i++;
    }
    return infos;
  }
  
  std::vector<CaravelInstalledPackage> CaravelDBContext::GetInstalledPackages()
    {
       std::vector<CaravelPackageInfo> packageInfos =  GetAllPackages();
       std::vector<CaravelInstalledPackage> packages;
       for (const CaravelPackageInfo& package : packageInfos){
        if (std::filesystem::exists(std::filesystem::path("/usr/share/caravel-uninstall/" + package.PackageName + ".lua"))){
           CaravelInstalledPackage pack = CaravelInstalledPackage();
           pack.name = package.PackageName;
           packages.push_back(pack);
        }
       }
       return packages;
    }

  
  std::vector<CaravelPackageInfo> CaravelDBContext::FindPackagesFromNameQuery(std::string query){
    infos.clear();
    std::string data = std::string("CALLBACK FUNCTION").c_str();
    char* errMsg = 0;
    std::string result = "";
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
  
   CaravelPackageGroup* CaravelDBContext::GetPackageGroup(std::string groupName){
            groups.clear();
            std::string data = std::string("CALLBACK FUNCTION").c_str();
            char* errMsg = 0;
            std::string result = "";
            std::string sql = "SELECT * FROM packagegroups WHERE package_group LIKE '%" + groupName + "%';";
            int rc = sqlite3_exec(m_DB, sql.c_str(), dbCallbackPkgGroup, (void*)data.c_str(), &errMsg);
            if(rc != SQLITE_OK ){
                std::cerr << "SQL error detected: " << errMsg << std::endl;
                sqlite3_free(errMsg);
                return nullptr;
            } else {
                int i = 0;
                while(!done)
                    i++;;
            }
            return &groups[0];
    }
  
  std::vector<std::vector<std::string>>  CaravelDBContext::FindPackageIdsFromNameQuery(std::string query){
    std::vector<std::vector<std::string>> packageIds;
    std::vector<CaravelPackageInfo> pids = FindPackagesFromNameQuery(query);
    for (int i = 0; i < pids.size(); i++){
        CaravelPackageId* res = FindPackageId(pids.at(i));
        if (res){
            std::vector<std::string> data = {res->PackageId, pids.at(i).Description};
            packageIds.push_back(data);
        }
    }
    return packageIds;
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
  
  std::string CaravelPackageGroup::ToPackage()
  {
        return Group + "-" +  Latest;
  }

  
}

static int dbCallback(void* data, int argc, char **argv, char **colName){
  std::map<std::string, std::string> propMap;
  int packageRecIndex = 0;
  done = false;
  for(int i = 0; i < argc; i++){
    std::string column(colName[i]);
    if(column == "name" || column == "description" || column == "pkgType" || column == "category" || column ==  "architecture" || column == "pkgId"){
      propMap[column] = std::string(argv[i] ? argv[i] : "NULL");
      packageRecIndex++;
    }
    if(packageRecIndex % 6 == 0 && packageRecIndex > 0){
      CaravelPM::CaravelPackageInfo info;
      info.Namespace = propMap["architecture"];
      info.PackageType = propMap["pkgType"];
      info.Description = propMap["description"];
      info.PackageCategory = propMap["category"];
      info.PackageName = propMap["name"];
      info.Id = std::stoi(propMap["pkgId"]);
      infos.push_back(info);
      propMap.clear();
    }
    
  }
  done = true;
  return 0;
}

static int dbCallbackPkgGroup(void* data, int argc, char **argv, char **colName){
  std::map<std::string, std::string> propMap;
  int packageRecIndex = 0;
  done = false;
  for(int i = 0; i < argc; i++){
    std::string column(colName[i]);
    if(column == "package_group" || column == "latest" || column == "id"){
      propMap[column] = std::string(argv[i] ? argv[i] : "NULL");
      packageRecIndex++;
    }
    if(packageRecIndex % 3 == 0 && packageRecIndex > 0){
      CaravelPM::CaravelPackageGroup info;
      info.Group = propMap["package_group"];
      info.Latest = propMap["latest"];
      info.Id = std::stoi(propMap["id"]);
      groups.push_back(info);
      propMap.clear();
    }
    
  }
  done = true;
  return 0;
}

static int dbCallbackPkgId(void* data, int argc, char **argv, char **colName){
  std::map<std::string, std::string> propMap;
  int packageRecIndex = 0;
  done = false;
  for(int i = 0; i < argc; i++){
    std::string column(colName[i]);
    if(column == "package_table_id" || column == "package_id" || column == "record_id"){
      propMap[column] = std::string(argv[i] ? argv[i] : "NULL");
      packageRecIndex++;
    }
    if(packageRecIndex % 3 == 0 && packageRecIndex > 0){
      CaravelPM::CaravelPackageId info;
      info.PackageId = propMap["package_id"];
      info.Id = std::stoi(propMap["package_table_id"]);
      ids.push_back(info);
      propMap.clear();
    }
    
  }
  done = true;
  return 0;
}
