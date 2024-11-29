
#include "CaravelContext.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <LuaCpp/LuaCpp.hpp>
#include <filesystem>
#include <mast_tk/core/LineUtils.hpp>

using namespace LuaCpp;
using namespace LuaCpp::Engine;
using namespace LuaCpp::Registry;
static std::string  RunCommand(const char* command){
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command, "r"), pclose);
    if (!pipe){
      throw std::runtime_error("popen() failed!");
    }
    while(fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr){
      result += buffer.data();
    }
    return result;								      
}

extern "C" {

 
  
  static int _caravel_ctx_install(lua_State* ls){
    std::stringstream cmd;
        std::string homeDir("/tmp/ccontainer/");
    cmd << "install -vDm " << lua_tonumber(ls, 1) << " ";
    std::filesystem::path source(homeDir);
    source /= std::string(lua_tostring(ls, 2));
    cmd << source.string() << " ";
    std::filesystem::path dest(std::string(lua_tostring(ls, 3)));
    cmd << dest.string();
    RunCommand(cmd.str().c_str());
    return 0;
  }

  static int _caravel_ctx_update_fcache(lua_State* ls){
    std::stringstream cmd;
    RunCommand("fc-cache -fv");
    return 0;
  }
  
   static int _caravel_ctx_install_font(lua_State* ls){
    std::stringstream cmd;
    std::string homeDir("/tmp/ccontainer/");
    cmd << "install -vDm 0644 ";
    std::filesystem::path source(homeDir);
    source /= std::string(lua_tostring(ls, 1));
    cmd << source.string() << " ";
    std::filesystem::path dest("/usr/share/fonts/" + source.filename().string());
    cmd << dest.string();
    RunCommand(cmd.str().c_str());
    RunCommand("fc-cache -fv");
    return 0;
  }

  static int _caravel_ctx_installdcpy(lua_State* ls){
    std::stringstream cmd;
    std::string homeDir("/tmp/ccontainer/");
    cmd << "cp -r " << " ";
    std::filesystem::path source(homeDir);
    source /= std::string(lua_tostring(ls, 1));
    cmd << source.string() << " ";
    std::filesystem::path dest(std::string(lua_tostring(ls, 2)));
    cmd << dest.string();
    RunCommand(cmd.str().c_str());
    return 0;
  }

   static int _caravel_ctx_installd(lua_State* ls){
    std::stringstream cmd;
    cmd << "mkdir -pv ";
    cmd << std::string(lua_tostring(ls, 1));
    RunCommand(cmd.str().c_str());
    return 0;
  }

   static int _caravel_ctx_install_info(lua_State* ls){
    std::stringstream cmd;
    cmd << "install-info  ";
    cmd << std::string(lua_tostring(ls, 1));
    cmd << " /usr/share/info/dir";
    RunCommand(cmd.str().c_str());
    return 0;
  }

   static int _caravel_ctx_install_link(lua_State* ls){
    std::stringstream cmd;
    cmd << "ln -sfv  ";
    cmd << std::string(lua_tostring(ls, 1));
    cmd << " " << std::string(lua_tostring(ls, 2));
    RunCommand(cmd.str().c_str());
    return 0;
  }
  
  static int _caravel_ctx_uninstall(lua_State* ls){
    std::stringstream cmd;
    cmd << "rm " << lua_tostring(ls, 1);
    RunCommand(cmd.str().c_str());
    return 0;
  }

   static int _caravel_ctx_install_pipp(lua_State* ls){
    std::stringstream cmd;
    cmd << "pip install " << lua_tostring(ls, 1);
    RunCommand(cmd.str().c_str());
    return 0;
  }


  static int _caravel_ctx_uninstalld(lua_State* ls){
    std::stringstream cmd;
    cmd << "rm -rf " << lua_tostring(ls, 1);
    RunCommand(cmd.str().c_str());
    return 0;
  }

   static int _caravel_ctx_install_terminfo(lua_State* ls){
    std::stringstream cmd;
    cmd << "tic -xe  ";
    cmd << std::string(lua_tostring(ls, 1));
    cmd <<  "/tmp/ccontainer/" << std::string(lua_tostring(ls, 2));
    RunCommand(cmd.str().c_str());
    return 0;
  }

   static int _caravel_ctx_patch_mode(lua_State* ls){
    std::stringstream cmd;
    cmd << "chmod +" << lua_tostring(ls, 1) << " " << lua_tostring(ls, 2);
    RunCommand(cmd.str().c_str());
    return 0;
  }

  static int _caravel_ctx_print(lua_State* ls){
    std::cout << lua_tostring(ls, 1) << std::endl;
    return 0;
  }

  static int _caravel_ctx_confirm(lua_State* ls){
    bool ans = MastTDE::LineIO::Confirm(std::string(lua_tostring(ls, 1)));
    lua_pushboolean(ls, (ans) ? 1 : 0);
    return 1;
  }
}
namespace CaravelPM {
  CaravelContext::CaravelContext(std::string file){

    m_CaravelLib = std::make_shared<LuaLibrary>("caravelInstall");
    m_CaravelLib->AddCFunction("install", _caravel_ctx_install);
    m_CaravelLib->AddCFunction("installFont", _caravel_ctx_install_font);
    m_CaravelLib->AddCFunction("updateFontCache", _caravel_ctx_update_fcache);

    m_CaravelLib->AddCFunction("installLink", _caravel_ctx_install_link);
    m_CaravelLib->AddCFunction("installd", _caravel_ctx_installd);
    m_CaravelLib->AddCFunction("uninstall", _caravel_ctx_uninstall);
    m_CaravelLib->AddCFunction("uninstall_dir", _caravel_ctx_uninstalld);
    m_CaravelLib->AddCFunction("uninstalld", _caravel_ctx_uninstalld);
    m_CaravelLib->AddCFunction("installInfo", _caravel_ctx_install_info);
    m_CaravelLib->AddCFunction("installPipPkg", _caravel_ctx_install_pipp);
    m_CaravelLib->AddCFunction("installTermInfo", _caravel_ctx_install_terminfo);
    m_CaravelLib->AddCFunction("installDCpy", _caravel_ctx_installdcpy);
    m_CaravelLib->AddCFunction("patchMode", _caravel_ctx_patch_mode);
    m_CaravelLib->AddCFunction("print", _caravel_ctx_print);
    m_CaravelLib->AddCFunction("confirm", _caravel_ctx_confirm);
    std::shared_ptr<Engine::LuaTString> homeDir = std::make_shared<Engine::LuaTString>(std::string(getenv("HOME")));
    lua.AddGlobalVariable("homeDir", homeDir);
    lua.AddLibrary(m_CaravelLib);
    // Extract file contents here
    {
      std::ifstream luaIn;
      luaIn.open(file);
      std::stringstream luaSS;
      std::string line;
      while(std::getline(luaIn, line)){
	luaSS << line << std::endl;
      };
      m_FileContents = luaSS.str();
      luaIn.close();
    }
  }

  bool  CaravelContext::Run(){
    lua.CompileString("manager",m_FileContents);
    try {
      lua.Run("manager");
    } catch (std::runtime_error& e) {
      std::cerr << "Lua script failed: " << e.what() << std::endl;
      return false;
    }
    return true;
  }

  CaravelContext::~CaravelContext(){
    
  }
  
}
