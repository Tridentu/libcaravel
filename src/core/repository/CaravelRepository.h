#pragma once

#include <string>
#include <INIReader.h>

namespace CaravelPM {
class CaravelRepository {
public:
    CaravelRepository(std::string repoName);
    CaravelRepository(std::string repoName, bool new_repo);

// Methods
public:
    // Downloads a file from the given string path (i.e. the database).
    std::string DownloadURI() const;
    std::string DownloadURI(std::string urlStub) const;
    std::string Title() const;
    bool IsEnabled() const;
    std::string Name() const;
public:
    void SetEnabled(bool enabled);
    void SetTitle(std::string title);
    void SetUrl(std::string url);
private:
        std::string m_name;
        std::string m_url;
        std::string m_title;
        std::string m_description;
        bool m_enabled;        
};
}
