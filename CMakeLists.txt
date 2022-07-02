cmake_minimum_required(VERSION 3.5)
project(wRATter VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT 6.24 CONFIG REQUIRED)
if(NOT ROOT_FOUND)
    message(STATUS "ROOT package not found.")
    return()
endif()

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# ## NOT NEEDED FOR DICT
include_directories(
        ${PROJECT_SOURCE_DIR}/include
        ${ROOT_INCLUDE_DIRS}
        $ENV{RATROOT}/include
)

#----------------------------------------------------------------------------
# Locate RATROOT Libs
#
LINK_DIRECTORIES($ENV{G4LIB}/$ENV{G4SYSTEM})
LINK_DIRECTORIES($ENV{RATROOT}/lib)

add_library(wRATter src/wRATter/Wrapper.cc)
target_link_libraries(wRATter ROOT::Physics ROOT::Hist ROOT::RIO ROOT::Tree RATEvent)
