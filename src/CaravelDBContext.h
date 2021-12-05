#pragma once

#include <sqlite3.h>
#include <string>
#include <vector>

namespace CaravelPM {

  struct CaravelPackageInfo {
    std::string PackageName;
    std::string PackageCategory;
    std::string PackageType;
    std::string Description;
    std::string Namespace;
  };
  
  class CaravelDBContext {
  public:
    CaravelDBContext(std::string databaseFile);
    ~CaravelDBContext();
    void Init();
    std::string FindNamespace(std::string packageName);
    std::string FindType(std::string packageName);
    std::string FindCategory(std::string packageName);
    std::vector<CaravelPackageInfo> GetPackageRecords();
    void FindPackagesOfType(std::string type);
    void FindPackagesInNamespace(std::string pNamespace);
    void FindPackagesInCategory(std::string category);
    std::string GetPackageLink(std::string packageName);
    void ClearPackageRecords();
    void AddRecord(CaravelPackageInfo info);
    void MarkDone();
  public:
    static void InitDB(std::string databaseFile);
    static CaravelDBContext* GetDB();
    static void Cleanup();
  private:
    static CaravelDBContext* s_CurrentContext;
  private:
    std::string m_dbFile;
    sqlite3* m_DB;
    bool m_Done;
    std::vector<CaravelPackageInfo> m_PackageRecords;
  };
};
