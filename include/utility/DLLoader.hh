#pragma once

#include <dlfcn.h>
#include <unordered_map>
#include <cassert>
#include <memory>
#include "Format.hh"

namespace fsta {

struct DLConfig {
  static const char* cpp_file_path;
  static const char* so_file_path;
};

class DynamicLinker : public DLConfig {
  typedef std::unordered_map<const char*, void*> HandlerMap;
  HandlerMap handlers;
public:
  void* dynLibOpen(const char* filename_os) {
    auto iter = handlers.find(filename_os);
    if (iter != handlers.end()) return iter->second;

	std::string path = so_file_path;
	std::string cmd = "mkdir -p "; cmd += path;
	int cr = std::system(cmd.c_str());
	(void)cr;
	path += filename_os;
	void* handle = dlopen(path.c_str(), RTLD_LAZY);
	assert(handle);
	dlerror();
	handlers.insert({filename_os, handle});
	return handle;
  }
  void* dynLibSym(void* handle, const char* func) { return dlsym(handle, func); }
  void* dynLibSym(const char* filename_os, const char* func) { return dynLibSym(dynLibOpen(filename_os), func); }
  void  dynLibClose(const char* filename_os) {
    auto iter = handlers.find(filename_os);
    assert(iter != handlers.end());
    dlclose(iter->second);
    handlers.erase(iter);
  }
  void dynLibClose(void* handle) {
#ifndef NDEBUG
	bool found = false;
	for (auto x:handlers) if (x.second == handle) {
	  found = true; handlers.erase(x.first); break;
	}
	assert(found);
#endif
	dlclose(handle);
  }
  ~DynamicLinker() {
	for (auto x:handlers) dlclose(x.second);
  }
};

struct DLJitCompiler : DynamicLinker {
  void* compile(std::string const & prog) {
    std::string filename = std::string("dljit_%zu"_f(fileCount));
    ++fileCount;

    std::string mkdir = "mkdir -p ";
    std::string cppmkdir = mkdir + cpp_file_path;
    std::string somkdir  = mkdir + so_file_path;
    int cr0 = std::system(cppmkdir.c_str()); (void)cr0;
    int cr1 = std::system(somkdir.c_str());  (void)cr1;

    std::string cpppath = cpp_file_path + filename + ".cpp";
    std::string sopath = so_file_path + filename + ".so";
    auto fh = fopen(cpppath.c_str(), "w");
	fwrite(prog.c_str(), prog.size(), 1, fh);
    fclose(fh);
    std::string compile_cmd = "g++ -shared -o %s %s"_f(sopath.c_str(), cpppath.c_str());
    int cr2 = std::system(compile_cmd.c_str()); (void)cr2;

    filename += ".so";
    last = dynLibOpen(filename.c_str());
    return last;
  }

  template < typename F >
  F* sym(const char* name, void* handle = nullptr) {
	if (handle) last = handle;
    return (F*)dynLibSym(last, name);
  }
private:
  void* last = nullptr;
  static size_t fileCount;
};

} // end namespace fsta
