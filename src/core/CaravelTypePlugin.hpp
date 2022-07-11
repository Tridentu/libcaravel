#pragma once

#include <filesystem>
#include <string>
#include <functional>



extern "C" {
    typedef  const char* (*caravel_type_t)();
    typedef  bool (*caravel_type_validate_t)(std::string);
    typedef  bool (*caravel_type_installu_t)(std::string, std::filesystem::path);
    typedef  void (*caravel_type_files_t)(std::filesystem::path, std::function<void(const char*, const char*)>);
    typedef  const char* (*caravel_type_verdir_t)();

}
