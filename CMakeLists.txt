
cmake_minimum_required(VERSION 3.10)
project(CaravelLib VERSION 0.2.0 LANGUAGES CXX DESCRIPTION "Caravel-PM's backend.")

find_package(PkgConfig REQUIRED)
find_package(CURL REQUIRED)
find_Package(LibArchive REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(Boost REQUIRED)

option(WITH_BACKEND "Builds the Caravel PackageKit backend." OFF)

option(WITH_BIN_TYPE "Builds the Caravel Binary Type Plugin." OFF)
option(WITH_CONFIG_TYPE "Builds the Caravel Dotfile Type Plugin." OFF)
option(WITH_ASSETS_TYPE "Builds the Caravel Assets Type Plugin." OFF)
option(WITH_CARAVELSCRIPT "Builds the CaravelScript Interpreter." ON)


include_directories("./src")
include_directories(${CMAKE_INSTALL_PREFIX}/include/LuaCpp)
include_directories(${CMAKE_INSTALL_PREFIX}/include/mast_tk/core)
include_directories(${SQLITE3_INCLUDE_DIR})
include_directories(${CMAKE_INSTALL_PREFIX}/include/readline)
file(GLOB CVL_SRCS src/core/*.cpp)
file(GLOB CVL_SRCS_REP  src/core/repository/*.cpp)
file(GLOB CVL_SRCS_PACK src/core/packages/*.cpp)

file(GLOB CVL_HDRS
src/core/*.hpp
src/core/*.h

)
file(GLOB CVL_HDRS_PACK src/core/packages/*.h
src/core/packages/*.hpp

)
file(GLOB CVL_HDRS_REP
src/core/repository/*.h
src/core/repository/*.hpp

)

set(CMAKE_CXX_STANDARD 17)
set(CM_FOLDER "cmake-3.22")
set(caraveltarget CaravelLib)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_library(CaravelLib SHARED ${CVL_SRCS} ${CVL_PA_SRCS} ${CVL_SRCS_PACK} ${CVL_SRCS_REP})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})


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
	dl
	${Boost_LIBRARIES}
	gpgme
)

if (WITH_BACKEND)
    find_package(GTK2 REQUIRED)
    file(GLOB CVL_PK_SRCS src/pk/*.cpp)
    file(GLOB CVL_PK_HDRS src/pk/*.h src/pk/*.hpp) 
    include_directories(${GTK2_INCLUDE_DIRS})
    include_directories(${CMAKE_INSTALL_PREFIX}/include/PackageKit)
    add_library(pk_backend_caravel SHARED ${CVL_PK_SRCS})
    target_link_libraries(pk_backend_caravel
        PRIVATE
        CaravelLib
        PUBLIC
        -lpackagekit-glib2
    )
        
endif()

if (WITH_CARAVELSCRIPT)
    find_package(GTK2 REQUIRED)
    file(GLOB CVL_CS_SRCS src/caravelscript/*.cpp)
    include_directories(${GTK2_INCLUDE_DIRS})
    add_executable(caravelscript ${CVL_CS_SRCS})
    target_link_libraries(caravelscript
        PRIVATE
        CaravelLib
    )
        
endif()

if (WITH_BIN_TYPE)
    set(CVL_BINARY_SRC src/core/plugins/BinariesType.cpp)
    add_library(caravel_binary_type SHARED ${CVL_BINARY_SRC})
    target_include_directories(caravel_binary_type PUBLIC $<INSTALL_INTERFACE:include/caravel>)
    target_link_libraries(caravel_binary_type
        PRIVATE
        CaravelLib
    )
endif()

if (WITH_CONFIG_TYPE)
    set(CVL_CONFIG_SRC src/core/plugins/ConfigType.cpp)
    add_library(caravel_config_type SHARED ${CVL_CONFIG_SRC})
    target_include_directories(caravel_config_type PUBLIC $<INSTALL_INTERFACE:include/caravel>)
    target_link_libraries(caravel_config_type
        PRIVATE
        CaravelLib
    )
endif()

if (WITH_ASSETS_TYPE)
    set(CVL_ASSETS_SRC src/core/plugins/AssetsType.cpp)
    add_library(caravel_assets_type SHARED ${CVL_ASSETS_SRC})
    target_include_directories(caravel_assets_type PUBLIC $<INSTALL_INTERFACE:include/caravel>)
    target_link_libraries(caravel_assets_type
        PRIVATE
        CaravelLib
    )
endif()

install(TARGETS CaravelLib  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${CVL_HDRS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/caravel/)
install(FILES ${CVL_HDRS_PACK} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/caravel/packages)
install(FILES ${CVL_HDRS_REP} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/caravel/repository)

install(FILES build/libcaravel.pc  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig/)
install(FILES build/FindCaravelLib.cmake  DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${CM_FOLDER}/Modules)
if(WITH_BACKEND)
    install(TARGETS pk_backend_caravel DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/packagekit-backend)
endif()
if(WITH_BIN_TYPE)
    install(TARGETS caravel_binary_type DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/caravel-plugins)
endif()
if(WITH_CONFIG_TYPE)
    install(TARGETS caravel_config_type DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/caravel-plugins)
endif()
if(WITH_ASSETS_TYPE)
    install(TARGETS caravel_assets_type DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/caravel-plugins)
endif()
if(WITH_CARAVELSCRIPT)
    install(TARGETS caravelscript DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif()
