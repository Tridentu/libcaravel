
#pragma once

#include <string>
#include <vector>
#include "CaravelRepository.h"
#include "../CaravelUtils.h"

namespace CaravelPM {
    class CaravelRepoManager {
    public:
        CaravelRepoManager();
    public:
        void addRepository(std::string name, std::string title, std::string url);
        void saveRepositories();
        void clearPendingRepos();
    private:
        std::vector<CaravelRepository*> pendingRepos;
    };
};
