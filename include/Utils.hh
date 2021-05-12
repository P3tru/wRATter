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
#include <utility>
#include <vector>
#include <sys/stat.h>
#include <csignal>
#include <thread>

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

namespace wRATter {

volatile std::sig_atomic_t gSignalStatus;
void signal_handler(int signal) {
  std::cout << std::endl;
  std::cout << "Receive Ctrl+C" << std::endl;
  std::cout << "Will process the last threads and then exit" << std::endl;
  gSignalStatus = signal;
}

typedef struct ThreadPool {

  std::vector<std::thread> vThreads;

  explicit ThreadPool(const std::size_t &nCores = std::thread::hardware_concurrency()) { vThreads.resize(nCores); };

  std::thread &operator[](std::size_t idx) { return vThreads[idx]; };
  const std::thread &operator[](std::size_t idx) const { return vThreads[idx]; };

  bool IsNThreadsFull(const int &i) const {
	return i % vThreads.size() == 0;
  }

  void LoopAndJoin() {
	for (auto &t: vThreads) {
	  if (t.joinable())
		t.join();
	}
  }

  std::size_t GetNThreads() const {
	return vThreads.size();
  }

} ThreadPool;

class Args {
 protected:
  std::vector<std::string> vFilenames;
  bool isVerbose = false;
 public:
  Args() = default;
  Args(std::vector<std::string> v_filenames, bool is_verbose)
	  : vFilenames(std::move(v_filenames)), isVerbose(is_verbose) {}

  virtual void ShowUsage(const std::string &name) {

	std::cerr << "Usage: " << name << " <option(s)> -i (--input) FILE.root" << std::endl
			  << "Options:\n"

			  << "\t-h\tShow this help message\n"
			  << "\t-v\tSet verbose mode (display progress bar)\n"

			  << std::endl;

  }

  virtual void ProcessArgs(TApplication &theApp) {

	// Reading user input parameters
	if (theApp.Argc() < 2) {
	  ShowUsage(theApp.Argv(0));
	  exit(0);
	}

	for (int i = 1; i < theApp.Argc(); i++) {
	  std::string arg = theApp.Argv(i);
	  if ((arg == "-h") || (arg == "--help")) {
		ShowUsage(theApp.Argv(0));
		exit(0);
	  } else if (boost::iequals(arg, "-v")) {
		isVerbose = true;
	  } else if (boost::iequals(arg, "-i") || boost::iequals(arg, "--input")) {
		vFilenames.emplace_back(theApp.Argv(++i));
	  } else {
		std::cout << "Unkown parameter" << std::endl;
		continue;
	  }
	}

	if (vFilenames.empty()) {
	  std::cerr << "ERROR: No input file provided!" << std::endl;
	  exit(EXIT_FAILURE);
	}

  };

};

}
#endif //_WRATTER_UTILS_HH_
