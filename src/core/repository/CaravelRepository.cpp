#pragma once

#include "CaravelRepository.h"


CaravelPM::CaravelRepository::CaravelRepository(std::string repoName)
{
      INIReader  reader("/etc/caravel-repos/" + repoName + ".ini"); 
      if(reader.HasSection("metadata")){
        m_name = reader.GetString("metadata", "name", repoName);
        m_title = reader.GetString("metadata", "title", repoName);
        m_description = reader.GetString("metadata", "description", "");
        m_url = reader.GetString("metadata", "url", "");
      }
}

CaravelPM::CaravelRepository::CaravelRepository(std::string repoName, bool new_repo)
{
    if (new_repo){
        m_name = repoName;
    } else {
        INIReader  reader("/etc/caravel-repos/" + repoName + ".ini");
        if(reader.HasSection("metadata")){
            m_name = reader.GetString("metadata", "name", repoName);
            m_title = reader.GetString("metadata", "title", repoName);
            m_description = reader.GetString("metadata", "description", "");
            m_url = reader.GetString("metadata", "url", "");
        }
    }

}

std::string CaravelPM::CaravelRepository::DownloadURI() const
{
    return m_url;
    
}

std::string CaravelPM::CaravelRepository::DownloadURI(std::string urlStub) const
{
    return DownloadURI() + "/" + urlStub;
}


std::string CaravelPM::CaravelRepository::Title() const
{
    return m_title;
}


bool CaravelPM::CaravelRepository::IsEnabled() const
{
    return m_enabled;
}

void CaravelPM::CaravelRepository::SetEnabled(bool enabled)
{
    m_enabled = enabled;
}

void CaravelPM::CaravelRepository::SetTitle(std::string title)
{
    m_title = title;
}

std::string  CaravelPM::CaravelRepository::Name() const
{
    return m_name;
}


void CaravelPM::CaravelRepository::SetUrl(std::string url)
{
    m_url = url;
}





