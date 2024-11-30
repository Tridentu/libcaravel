#include "CaravelSession.h"
#include <INIReader.h>
#include "CaravelUtils.h"
#include "packages/CaravelAuthor.h"
#include "packages/CaravelContext.h"

CaravelPM::CaravelSession::CaravelSession()
{
    loader = CaravelPM::InitLoader();
    Load();
}

void CaravelPM::CaravelSession::Load()
{
    INIReader reader("/etc/caravel-repos/main.ini");
    if(reader.HasSection("general")){
        std::string repository = reader.GetString("general","repository","cmr");
        m_Repository = new CaravelPM::CaravelRepository(repository);
    }
    loader->loadAllTypes();
    logger = new CaravelPM::Logger("/var/log/caravel.log");
}

std::string CaravelPM::CaravelSession::GetPackagePath(std::string packageType)
{
    auto package_t = loader->getPackageType(packageType);
    std::string download = package_t.download_dir();
    return download;
}

std::string CaravelPM::CaravelSession::GetDownloadUrl(std::string urlStub)
{
    if(!m_Repository)
        return "";
    return  m_Repository->DownloadURI(urlStub);
}

CaravelPM::CaravelPackageType CaravelPM::CaravelSession::getPackageType(std::string typeName)
{
    return loader->getPackageType(typeName);
}

void CaravelPM::CaravelSession::Create(std::string packageDir, CaravelPM::CaravelPackageType packageType, std::map<std::string, std::string> propMap)
{
    CaravelPM::CaravelAuthor::CreatePackage(packageDir,packageType.name,propMap,loader);
}
void CaravelPM::CaravelSession::uninstallPackage(std::string uninstallScriptPath){
    std::string uninstallScript = uninstallScriptPath;
    std::filesystem::path ulPath(uninstallScript);

    CaravelPM::CaravelContext* context = new CaravelPM::CaravelContext(ulPath.string());
    context->Run(logger);
    delete context;
}



bool CaravelPM::CaravelSession::ReadAndInstall(std::string pathString, std::string packageFile)
{
     CaravelPM::CaravelReader* reader = new CaravelPM::CaravelReader(pathString,packageFile, loader, logger);
          if(!reader->Extract()){
               writeToLog(CaravelPM::LogLevel::ERROR, "Can't extract package.");
            return false;
          }
          if(reader->hasHybridType()){
              writeToLog(CaravelPM::LogLevel::INFO, "Hybrid package detected. Caravel will reinstall this package.");
             uninstallPackage("/tmp/ccontainer/uninstall.lua");
          }
          if(!reader->Install()){
              writeToLog(CaravelPM::LogLevel::ERROR, "Can't install package " + reader->GetMetadata("name"));
            return false;
          }
          writeToLog(CaravelPM::LogLevel::INFO, "Done.");
          delete reader;
          return true;
}

CaravelPM::CaravelReader * CaravelPM::CaravelSession::getReader(std::string pathString, std::string packageFile)
{
    return new CaravelPM::CaravelReader(pathString, packageFile, loader, logger);
}

std::string CaravelPM::CaravelSession::GetRepoUrl()
{
    return m_Repository->DownloadURI();
}

void CaravelPM::CaravelSession::writeToLog(CaravelPM::LogLevel level, std::string msg)
{
    return logger->write(level,msg);
}

