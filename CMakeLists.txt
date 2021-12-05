
cmake_minimum_required(VERSION 3.10)
project(CaravelLib)

find_package(PkgConfig REQUIRED)
find_package(CURL REQUIRED)
find_Package(LibArchive REQUIRED)
find_package(SQLite3 REQUIRED)

include_directories("./src")
include_directories(${CMAKE_INSTALL_PREFIX}/include/LuaCpp)
include_directories(${CMAKE_INSTALL_PREFIX}/include/mast_tk/core)
include_directories(${SQLITE3_INCLUDE_DIR})
include_directories(${CMAKE_INSTALL_PREFIX}/include/readline)
file(GLOB MTK_SRCS src/*.cpp)
file(GLOB MTK_HDRS src/*.h src/*.hpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_library(CaravelLib SHARED ${MTK_SRCS})

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
install(FILES ${MTK_HDRS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/caravel)

