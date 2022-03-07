#pragma once

#include <sqlite3.h>
#include <string>
#include <vector>
#include <fstream>

namespace CaravelPM {

  struct CaravelPackageInfo {
    std::string PackageName;
    std::string PackageCategory;
    std::string PackageType;
    std::string Description;
    std::string Namespace;
    int Id;
  };
  
  struct CaravelPackageId {
    std::string PackageId;
    int Id;
  };
  
  struct CaravelInstalledPackage {
        std::string name;
        CaravelInstalledPackage() = default;
        CaravelInstalledPackage(const CaravelInstalledPackage& other) = default;
        
  };
  
  class CaravelDBContext {
  public:
    CaravelDBContext(std::string databaseFile, bool useTemp = false);
    ~CaravelDBContext();
    void Init(bool temp = false, bool debug = false);
    std::string FindNamespace(std::string packageName);
    std::string FindType(std::string packageName);
    std::string FindCategory(std::string packageName);
    CaravelPackageId* FindPackageId(CaravelPackageInfo packageInfo);
    std::vector<CaravelPackageInfo> GetPackageRecords();
    std::vector<CaravelPackageInfo> GetAllPackages();
    void FindPackagesOfType(std::string type);
    void FindPackagesInNamespace(std::string pNamespace);
    void FindPackagesInCategory(std::string category);
    std::string GetPackageLink(std::string packageName);
    std::vector<CaravelPackageInfo> FindPackagesFromNameQuery(std::string query);
    std::vector<std::vector<std::string>>  FindPackageIdsFromNameQuery(std::string query);
    std::vector<CaravelInstalledPackage> GetInstalledPackages();
  public:
    void ClearPackageRecords();
    void AddRecord(CaravelPackageInfo info);
    void MarkDone();
  public:
    static void InitDB(std::string databaseFile, bool useTemp = false);
    static CaravelDBContext* GetDB();
    static void Cleanup();
  private:
    static CaravelDBContext* s_CurrentContext;

  private:
    std::string m_dbFile;
    sqlite3* m_DB;
    bool m_Done;
    std::vector<CaravelPackageInfo> m_PackageRecords;
    std::ofstream caravelLog;
  };
};
