

#include "CaravelFS.hpp"

namespace CaravelPM {
  int CaravelFS::GetAttributes(const char* path, stat* stBuf){
    if(!s_ArchiveName)
      return;
    s_CurrentArchive = archive_read_new();
    archive_read_support_filter_xz(s_CurrentArchive);
    archive_read_support_format_tar(s_CurrentArchive);
    int res = 0;
    memset(stBuf, 0, sizeof(stat));
    archive_entry* entry;
    archive_read_
    int r = archive_read_next_header(s_CurrentArchive,&entry);
    if (r == ARCHIVE_OK){
      archive_entry_copy_stat(entry,stBuf);
    } else {
      res = -ENOENT;
    }

    return res;
  }
}

static int getattr(const char* path, stat* stBuf){
  return CaravelPM::CaravelFS::GetAttributes(path,stBuf);
}
