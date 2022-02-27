
cmake_minimum_required(VERSION 3.10)
project(CaravelLib VERSION 0.2.0 LANGUAGES CXX DESCRIPTION "Caravel-PM's backend.")

find_package(PkgConfig REQUIRED)
find_package(CURL REQUIRED)
find_Package(LibArchive REQUIRED)
find_package(SQLite3 REQUIRED)

include_directories("./src")
include_directories(${CMAKE_INSTALL_PREFIX}/include/LuaCpp)
include_directories(${CMAKE_INSTALL_PREFIX}/include/mast_tk/core)
include_directories(${SQLITE3_INCLUDE_DIR})
include_directories(${CMAKE_INSTALL_PREFIX}/include/readline)
file(GLOB CVL_SRCS src/core/*.cpp)
file(GLOB CVL_HDRS src/core/*.h src/core/*.hpp)

set(CMAKE_CXX_STANDARD 17)
set(CM_FOLDER "cmake-3.22")
set(caraveltarget CaravelLib)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_library(CaravelLib SHARED ${CVL_SRCS})

configure_file(libcaravel.pc.in libcaravel.pc @ONLY)
configure_file(FindCaravelLib.cmake.in FindCaravelLib.cmake @ONLY)

target_link_libraries(
    CaravelLib
    PUBLIC
	curl
	archive
	luacpp
	INIReader
	mast_tk_core
	sqlite3
	readline
)

install(TARGETS CaravelLib  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${CVL_HDRS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/caravel/)
install(FILES build/libcaravel.pc  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig/)
install(FILES build/FindCaravelLib.cmake  DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${CM_FOLDER}/Modules)

