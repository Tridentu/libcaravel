
#include <archive.h>
#include <archive_entry.h>
#include "CaravelUtils.h"
#include "CaravelWriter.h"
#include <sstream>
#include <vector>
#include <filesystem>
#include <stdio.h>
#include <string>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

namespace CaravelPM {
  CaravelWriter::CaravelWriter(std::string pkgName, std::string type){
    m_Name = pkgName;
    m_Type = type;
    m_Archive = archive_write_new();
    archive_write_add_filter_xz(m_Archive);
    archive_write_set_compression_xz(m_Archive);
    archive_write_set_format_gnutar(m_Archive);
      int flags;
    
    flags = ARCHIVE_EXTRACT_PERM;
    flags |= ARCHIVE_EXTRACT_ACL;
    flags |= ARCHIVE_EXTRACT_FFLAGS;
    archive_write_disk_set_options(m_Archive,flags);
    std::stringstream fileSS;
    fileSS << std::filesystem::current_path().c_str() << "/" <<  pkgName << ".caravel";
    archive_write_open_filename(m_Archive,fileSS.str().c_str());
  }

  void CaravelWriter::SetMetadata(std::string key, std::string value){
    m_Metadata[key] = value;
  }
  
  CaravelWriter::~CaravelWriter(){
    if(m_Archive){
      archive_write_close(m_Archive);
      archive_write_free(m_Archive);
    }  
  }

  
  void CaravelWriter::AddFile(std::string fileName,std::string name){
    struct stat st;
    std::stringstream fileSS;
    char buff[8192];
    int len;
    const char* filename_pkg = fileName.c_str();
    std::filesystem::path filePath(filename_pkg);
    stat(filename_pkg, &st);
    struct archive_entry *entry = archive_entry_new();
    archive_entry_set_pathname(entry,name.c_str());
    archive_entry_copy_stat(entry, &st);
    std::filesystem::path p(fileName);
    if(std::filesystem::is_directory(p)){
      archive_entry_set_filetype(entry,AE_IFDIR);
      
    } else if (std::filesystem::is_symlink(p)){
      std::filesystem::path p2 = std::filesystem::read_symlink(p);
      if(std::filesystem::is_regular_file(p2)){
	archive_entry_set_filetype(entry,AE_SYMLINK_TYPE_FILE);
      } else if (std::filesystem::is_directory(p2)){
	archive_entry_set_filetype(entry,AE_SYMLINK_TYPE_DIRECTORY);
      } else {
	archive_entry_set_filetype(entry, AE_SYMLINK_TYPE_UNDEFINED);
      }
     
    } else if (std::filesystem::is_regular_file(p)) {
      archive_entry_set_filetype(entry,AE_IFREG);
      archive_entry_set_mode(entry,st.st_mode);
    }
      
    archive_entry_set_uname(entry, "root");
    int r = archive_write_header(m_Archive, entry);
    if (r != ARCHIVE_OK){
      std::cerr << archive_error_string(m_Archive) << std::endl;
      return;
    }
    int fd = open(filename_pkg, O_RDONLY);
    len = read(fd, buff, sizeof(buff));
    while(len > 0 ){
      archive_write_data(m_Archive, buff, len);
      len = read(fd, buff, sizeof(buff));
    }
    close(fd);
    archive_entry_free(entry);
  }

void CaravelWriter::AddFiles(std::vector<std::string> files){
  for (int i = 0; i < files.size(); ++i){
    std::filesystem::path stemPath = std::filesystem::path(files.at(i));
      AddFile(files.at(i), stemPath.stem().string());
  }
}

  void CaravelWriter::AddManifest(){
    SetMetadata("name",m_Name);
    std::ofstream manifest;
    std::string manifestPath( std::filesystem::current_path().c_str());
    manifest.open(manifestPath + "/manifest.ini");
    manifest << "[caravel]" <<  std::endl;
    for (auto it = m_Metadata.begin(); it != m_Metadata.end(); ++it){
      manifest << it->first << "=" << it->second << std::endl;
    }
    manifest.close();
    AddFile(manifestPath + "/manifest.ini","manifest.ini");   
  }
  
}

