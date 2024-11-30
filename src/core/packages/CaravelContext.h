
#pragma once

#include <LuaCpp/LuaCpp.hpp>
#include "../CaravelLogger.h"

using namespace LuaCpp;
using namespace LuaCpp::Registry;
using namespace LuaCpp::Engine;


namespace CaravelPM {
    class CaravelContext {
    public:
      CaravelContext(std::string file);
      ~CaravelContext();
      bool Run(CaravelPM::Logger* logger);
    private:
      LuaContext lua;
      std::shared_ptr<LuaLibrary> m_CaravelLib;
      std::string m_FileContents;
    };
}

//  = std::make_shared<LuaLibrary>("caravelInstall");
