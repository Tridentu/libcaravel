

#include "CaravelRepoManager.h"
#include <boost/algorithm/string.hpp>    
#include <iostream>
#include <sstream>
#include <fstream>

CaravelPM::CaravelRepoManager::CaravelRepoManager()
{
    
}

void CaravelPM::CaravelRepoManager::clearPendingRepos(){
    pendingRepos.clear();
}


void CaravelPM::CaravelRepoManager::addRepository(std::string name, std::string title, std::string url)
{
    auto repo = new CaravelRepository(name, true);
    repo->SetTitle(title);
    repo->SetUrl(url);
    pendingRepos.push_back(repo);
}

void CaravelPM::CaravelRepoManager::saveRepositories()
{
    for (const auto repo : pendingRepos){
        std::stringstream stream_ss;
        stream_ss << "[metadata]" << std::endl; 
        stream_ss << "name=" << repo->Name() << std::endl;
        stream_ss << "title=" << repo->Title() << std::endl;
        stream_ss << "url=" << repo->DownloadURI() << std::endl;
        std::ofstream stream_fs("/etc/caravel-repos/" + boost::algorithm::to_lower_copy(repo->Name()) + ".ini");
        stream_fs << stream_ss.str();
        stream_fs.close();
    }
}
