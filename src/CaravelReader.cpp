
#include <archive.h>
#include <vector>
#include <map>
#include <array>
#include <string>
#include <archive_entry.h>
#include <filesystem>
#include <INIReader.h>
#include <sstream>
#include <unistd.h>
#include "CaravelUtils.h"
#include "CaravelReader.h"
#include "CaravelContext.h"


static int parse_manifest(void* user, const char* section, const char* name, const char* value){
  CaravelPM::CaravelManifest* man = (CaravelPM::CaravelManifest*)user;
  #define MATCH(s,p) strcmp(section,s) == 0 && strcmp(name,p) == 0
  if (MATCH("caravel","name")){
    man->name = strdup(value);
    
  } else if (MATCH("caravel","type")){
    man->type = strdup(value);
  } else {
    return 0;
  }

  return 1;
}

namespace CaravelPM {

  CaravelReader::CaravelReader(std::string pkgFile, std::string name){
    m_File = pkgFile;
    m_Name = name;
  }

  std::string CaravelReader::RunCommand(const char* command){
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

  bool CaravelReader::Extract(){
    std::string folder = std::string(getenv("HOME"));
    folder += "/ccontainer/";
    

    std::filesystem::path p = std::filesystem::current_path();
    if(!std::filesystem::create_directory(std::filesystem::path(folder))){
      return false;
    }

    std::filesystem::path containerPath(folder);
    
    std::filesystem::rename(m_File , containerPath / m_Name);

    std::filesystem::current_path(containerPath);
    
    std::stringstream cmd;
    cmd <<  (containerPath / m_Name ).string();

    {
      int r;
      archive_entry* entry;
      m_Archive = archive_read_new();
      archive_read_support_format_tar(m_Archive);
      archive_read_support_filter_xz(m_Archive);
      m_ArchiveWrite = archive_write_disk_new();
      archive_write_disk_set_options(m_ArchiveWrite,ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_ACL | ARCHIVE_EXTRACT_PERM |  ARCHIVE_EXTRACT_FFLAGS);
      archive_write_disk_set_standard_lookup(m_ArchiveWrite);
      if (r = archive_read_open_filename(m_Archive,cmd.str().c_str(),10240))
	return false;
      for(;;){
	r = archive_read_next_header(m_Archive,&entry);
	if(r == ARCHIVE_EOF)
	  break;
	if (r < ARCHIVE_OK)
	  std::cerr << archive_error_string(m_Archive) << std::endl;
	if (r < ARCHIVE_WARN)
	  return false;
	r = archive_write_header(m_ArchiveWrite,entry);
	if (r < ARCHIVE_OK)
	  std::cerr << archive_error_string(m_ArchiveWrite) << std::endl;
	else if (archive_entry_size(entry) > 0) {
	  r = CopyFile(m_Archive,m_ArchiveWrite);
	  if (r < ARCHIVE_OK)
	      std::cerr << archive_error_string(m_ArchiveWrite) << std::endl;
	  if (r < ARCHIVE_WARN)
	    return false;
	  
	}
	r = archive_write_finish_entry(m_ArchiveWrite);
	if (r < ARCHIVE_OK)
	    std::cerr << archive_error_string(m_ArchiveWrite) << std::endl;
	if (r < ARCHIVE_WARN)
	    return false;
      }
      archive_read_close(m_Archive);
      archive_read_free(m_Archive);
      archive_write_close(m_ArchiveWrite);
      archive_write_free(m_ArchiveWrite);
    }
    
    std::filesystem::current_path(p);
    return true;
  }


  int CaravelReader::CopyFile(struct archive* a, struct archive *w){
    int r;
    const void* buff;
    size_t size;
    la_int64_t offset;
   while(true){
      r = archive_read_data_block(a,&buff,&size,&offset);
      if (r == ARCHIVE_EOF)
	return (ARCHIVE_OK);
      if (r < ARCHIVE_OK)
	return (r);
      r = archive_write_data_block(w, buff, size, offset);
      if (r < ARCHIVE_OK){
	std::cerr << archive_error_string(w) << std::endl;
	return r;
      }
    }
  }

  bool CaravelReader::Install(){
    std::string folder = std::string(getenv("HOME"));
    folder += "/ccontainer/";
    std::filesystem::path installPath(folder + "install.lua");
    if(!std::filesystem::exists(installPath)){
      std::cerr << "Install script does not exist, exiting..." << std::endl;
      return false;
    }

    CaravelManifest* man;


    std::string pathS1 = std::string(folder + "/manifest.ini");
    const char* pathStr = pathS1.c_str();

    INIReader reader(pathStr);
    if(reader.ParseError() < 0){
      std::cerr << "Can't load manifest." << std::endl;
      return false;
    }

    if (std::string(reader.GetString("caravel","type","binaries")) == "binaries") {
      m_Type = CaravelPkgType::Binaries;
      
    } else if (std::string(reader.GetString("caravel","type","binaries")) == "config"){
      m_Type = CaravelPkgType::DotFiles;
    } else if (std::string(reader.GetString("caravel","type","binaries"))  == "assets"){
      m_Type = CaravelPkgType::Assets;
    } else if (std::string(reader.GetString("caravel","type","binaries")) == "sources"){
      m_Type = CaravelPkgType::Source;
    }
        
    CaravelContext* ctx = new CaravelContext(installPath.string());
    ctx->Run();

    if(m_Type == CaravelPkgType::Binaries){
      std::filesystem::path uninstallPath("/usr/share/caravel-uninstall/");
      std::string uninstallFile (reader.GetString("caravel","name","unknown"));
      uninstallFile += ".lua";
      uninstallPath /= uninstallFile;
      if(std::filesystem::exists(uninstallPath)){
	std::filesystem::remove(uninstallPath);
      }
      std::filesystem::copy_file(folder + "uninstall.lua",uninstallPath);
    } else {
      return false;
    }

    return true;
    
  }


  CaravelReader::~CaravelReader(){
    const std::filesystem::path tmp = std::filesystem::path(std::string(getenv("HOME")));
    std::uintmax_t contents = std::filesystem::remove_all(tmp / "ccontainer");
    std::cout << "Either installed " << contents - 3 << " files or nothing happened.";
    
  }


  std::string CaravelReader::GetMetadata(std::string key){
    return m_Metadata[key];
  }

  
  
}
