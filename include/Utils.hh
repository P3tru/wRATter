//
// Created by zsoldos on 6/18/20.
//

#ifndef _WRATTER_UTILS_HH_
#define _WRATTER_UTILS_HH_

// ####################################### //
// #### #### ####   C/C++   #### #### #### //
// ####################################### //
#include <iostream>
#include <string>
#include <sys/stat.h>

/////////////////////////   BOOST   /////////////////////////
#include <boost/filesystem/path.hpp>

std::string ExtractFilenameFromPath(std::string pathname){
  boost::filesystem::path p(pathname);
  return p.filename().string();
}

std::string RemoveFilenameExtension(std::string filename){
  boost::filesystem::path path(filename);
  return path.stem().string();
}

bool IsFileExist(const char* name) {
  struct stat buffer{};
  return (stat (name, &buffer) == 0);
}

#endif //_WRATTER_UTILS_HH_
