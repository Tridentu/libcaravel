#pragma once

#include <filesystem>
#include <string>
#include <functional>



extern "C" {
    typedef  const char* (*caravel_type_t)();
    typedef  bool (*caravel_type_validate_t)(std::string);
    typedef  bool (*caravel_type_installu_t)(std::string);
    typedef  void (*caravel_type_files_t)(std::filesystem::path, std::function<void(const char*, const char*)>);

}