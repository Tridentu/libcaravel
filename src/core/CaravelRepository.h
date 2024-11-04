#pragma once

#include <string>
#include <INIReader.h>

namespace CaravelPM {
class CaravelRepository {
public:
    CaravelRepository(std::string repoName);
// Methods
public:
    // Downloads a file from the given string path (i.e. the database).
    std::string DownloadURI() const;
    std::string DownloadURI(std::string urlStub) const;
    std::string Title() const;
    bool IsEnabled() const;
public:
    void SetEnabled(bool enabled);
private:
        std::string m_name;
        std::string m_url;
        std::string m_title;
        std::string m_description;
        bool m_enabled;        
};
}
