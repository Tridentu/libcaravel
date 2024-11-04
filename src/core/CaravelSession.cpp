#include "CaravelSession.h"
#include <INIReader.h>
#include "CaravelUtils.h"
#include "CaravelAuthor.h"
#include "CaravelContext.h"

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

bool CaravelPM::CaravelSession::ReadAndInstall(std::string pathString, std::string packageFile)
{
     CaravelPM::CaravelReader* reader = new CaravelPM::CaravelReader(pathString,packageFile, loader);
          if(!reader->Extract()){
            std::cerr << "Can't extract package." << std::endl;
            return false;
          }
          if(reader->hasHybridType()){
             std::cout << "Hybrid package detected. Caravel will reinstall this package." << std::endl;
             std::string uninstallScript(std::string(std::string("/tmp/ccontainer/uninstall.lua")));
             std::filesystem::path ulPath(uninstallScript);

             CaravelPM::CaravelContext* context = new CaravelPM::CaravelContext(ulPath.string());
             context->Run();
             delete context;
          }
          if(!reader->Install()){
            std::cerr << "Can't install package " << reader->GetMetadata("name") << std::endl;
            return false;
          }
          std::cout << "Done." << std::endl;
          delete reader;
          return true;
}

CaravelPM::CaravelReader * CaravelPM::CaravelSession::getReader(std::string pathString, std::string packageFile)
{
    return new CaravelPM::CaravelReader(pathString, packageFile, loader);
}


