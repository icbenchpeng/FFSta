#ifndef   FastSta_TestFramework_hh_includeded
#define   FastSta_TestFramework_hh_includeded

#include "tcl.h"
#include <cassert>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <atomic>
#include "sta/Sta.hh"
#include "sta/Search.hh"
#include "sta/Network.hh"
#include "ord/OpenRoad.hh"
#include "db_sta/dbSta.hh"

namespace sta {

class BaseSystem {
protected:
  // typedef utl::Logger Logger;
  class Logger {
  public:
    // if defined log file path, redirect output to it
    Logger(): stream(&std::cout), destruct(false) {}
    Logger(std::string const& log_filename): destruct(true) {
	  stream = new std::ofstream();
      ((std::ofstream*)stream)->open(log_filename.c_str());
      assert(((std::ofstream*)stream)->is_open());
    }
    virtual ~Logger() {
	  if (destruct){
	    ((std::ofstream*)stream)->close();
	    delete stream;
	  }
    }

    template<typename ...Args>
    void warn(std::string const& fmt, Args... args) {
	  size_t sz = 1 + snprintf(nullptr, 0, fmt.c_str(), args...);
	  char buffer[sz];
	  snprintf(buffer, sz, fmt.c_str(), args...);
	  log(buffer);
    }
    void log(std::string const & s) { *stream << s << std::flush; }
    operator std::ostream& () { return *stream; }
    void binary(char* buffer, size_t sz) { stream->write(buffer, sz); *stream << std::flush; }
  protected:
    std::ostream* stream;
    bool destruct: 1;
  };
  struct Impl {
    Impl() : refcount(0) {}
    Logger* logger;
    std::atomic<size_t> refcount;
  };
  static Impl* singleton;

public:
  BaseSystem() {
    if (singleton) {
      ++singleton->refcount;
    } else {
      singleton = new Impl;
    }
  }
  ~BaseSystem() {
    if (singleton && --singleton->refcount == 0) {
      delete singleton;
      singleton = nullptr;
    }
  }
  void setLogger(Logger* logger) { singleton->logger = logger; }
  Logger* logger()  const { return singleton->logger;  }
};

struct TestEnv {
  template <typename T>
  static T* getPtr(std::string const& tclVar) {
	const char* tcl_obj;
	tcl_obj = Tcl_GetVar(tcl_interp, tclVar.c_str(), TCL_GLOBAL_ONLY);
	void* p;
	convertPtrFromString(tcl_obj, &p);
	return reinterpret_cast<T*>(p);
  }
  static void setTclInterp(Tcl_Interp* i) { tcl_interp = i; }
  static dbSta*   dbsta() { return ord::OpenRoad::openRoad()->getSta(); }
  static Graph*   graph() { return dbsta()->graph(); }
  static Network* network() { return dbsta()->network(); }
private:
  static Tcl_Interp* tcl_interp;

  static const char* unpack_data(const char* c, void* ptr, size_t sz);
  static void convertPtrFromString(const char* c, void** ptr);
};

class Test : public BaseSystem, public TestEnv {
public:
  Test(std::string const & n) : name(n), parent(nullptr) {}
  virtual ~Test() = default;
  virtual int run() = 0;
  int diff(std::string const & fn); // read fn.ok fn.log and compare
  virtual std::string path() const {
    std::string res;
    if (parent) res += parent->path() + "/";
    res += name;
    return res;
  }
  virtual Test* findCase(std::string const & case_name) { return this; }
  virtual int flow() {
	std::string fn = path();
    int failed = 0;
	{
      Logger logger(fn + ".log");
      Logger* oldlogger = BaseSystem::logger();
      setLogger(&logger);
	  failed = run();
      if (!failed) failed = diff(fn);
	    setLogger(oldlogger);
	}
	// needs elaborate failure information
    if (failed) {
      logger()->warn("%s\n", fn.c_str());
      return 1;
    }
	return 0;
  }
  std::string name;
  Test* parent;
};

class TestGroup : public Test {
public:
  TestGroup(std::string const & n) : Test(n) {}
  virtual ~TestGroup() { for (auto x : tests) delete x; }
  void add(Test* t) { tests.push_back(t); t->parent = this; }
  int run() { return 0; }
  Test* findCase(std::string const & case_name) {
    if (case_name.empty())
      return this;
    std::size_t pos = case_name.find("/");
    std::string fsec = case_name;
    if (pos != std::string::npos) fsec = case_name.substr(0, pos);
    for (auto x : tests) {
      if (x->name == fsec) {
        if (pos && pos != std::string::npos) {
          Test* t = x->findCase(case_name.substr(pos + 1));
          if (t) return t;
        } else {
          return x;
        }
      }
    }
    return nullptr;
  }
  int flow() {
	  std::string fn = path();
    int sum = 0;
    for (auto x : tests) sum += x->flow();
    return sum;
  }
  std::vector<Test*> tests;
};

class TestSuite : public Test {
public:
  TestSuite(std::string const & n) : Test(n) {}
  std::string path() const override {
    return Test::path() + (subcase_name.empty() ? "" : "/") + subcase_name;
  }
  Test* findCase(std::string const & case_name) override {
    subcase_name = case_name;
    return this;
  }

  int flow() override {
    int is_ok = Test::flow();
    subcase_name = "";
    return is_ok;
  }
  std::string subcase_name;
};

class TestFramework : TestGroup {
public:
   // n is the name of the path
  TestFramework(std::string const & n) : TestGroup(n) { setLogger(new Logger()); }
  virtual ~TestFramework() { delete logger(); setLogger(nullptr); } ;
  using TestGroup::add;
  Test* findCase(std::string const & case_name) {
    return TestGroup::findCase(case_name);
  }
  void summary(int failed_cnt) {
    if (failed_cnt > 0) {
      logger()->warn("%d failed\n", failed_cnt);
    } else {
      logger()->warn("all passed\n");
    }
  }
  int test(const char* case_name) {
    Test* t = findCase(case_name);
    if (t) {
      int failed_cnt = t->flow();
      summary(failed_cnt);
      return failed_cnt;
    };
    logger()->warn("not found case \"%s\"\n", case_name);
    return 0; // false
  }
};

void fsta_test(TestFramework* tf);

} // end namespace sta-test

#endif /*FastSta_TestFramework_hh_includeded*/
