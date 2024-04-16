#include <fstream>
#include "TestFramework.hh"

namespace sta {

Tcl_Interp*
TestEnv::tcl_interp = nullptr;

const char*
TestEnv::unpack_data(const char* c, void* ptr, size_t sz) {
unsigned char* u = (unsigned char*) ptr;
  const unsigned char* eu = u + sz;
  for (; u != eu; ++u) {
    char d = *(c++);
    unsigned char uu;
    if ((d >= '0') && (d <= '9'))
	  uu = (unsigned char) ((d - '0') << 4);
    else if ((d >= 'a') && (d <= 'f'))
	  uu = (unsigned char) ((d - ('a' - 10)) << 4);
    else
	  return (char*) 0;
    d = *(c++);
    if ((d >= '0') && (d <= '9'))
	  uu |= (unsigned char) (d - '0');
    else if ((d >= 'a') && (d <= 'f'))
	  uu |= (unsigned char) (d - ('a' - 10));
    else
	  return (char*) 0;
    *u = uu;
  }
  return c;
}

void
TestEnv::convertPtrFromString(const char* c, void** ptr) {
  assert(*c == '_');
  c++;
  c = unpack_data(c, ptr, sizeof(void*));
}

BaseSystem::Impl*
BaseSystem::singleton = nullptr;

bool compareFiles(const std::string& p1, const std::string& p2)
{
  std::ifstream f1(p1, std::ifstream::binary | std::ifstream::ate);
  std::ifstream f2(p2, std::ifstream::binary | std::ifstream::ate);

  if (f1.fail() || f2.fail())
    return false;  // file problem

  if (f1.tellg() != f2.tellg())
    return false;  // size mismatch

  // seek back to beginning and use std::equal to compare contents
  f1.seekg(0, std::ifstream::beg);
  f2.seekg(0, std::ifstream::beg);
  return std::equal(std::istreambuf_iterator<char>(f1.rdbuf()),
                    std::istreambuf_iterator<char>(),
                    std::istreambuf_iterator<char>(f2.rdbuf()));
}

int Test::diff(std::string const & fn)
{
  std::string golden = fn + ".ok";
  std::string revised = fn + ".log";
  return compareFiles(golden, revised) ? 0 : 1;
}



extern Test* fsta_utility_test();
extern Test* fsta_faststa_test();
extern Test* fsta_runtime_test();
extern Test* fsta_compile_test();
extern Test* fsta_compile_infra_test();

void
fsta_test(TestFramework* tf) {
  tf->add(fsta_utility_test());
  tf->add(fsta_compile_infra_test());
  tf->add(fsta_compile_test());
  tf->add(fsta_faststa_test());
  tf->add(fsta_runtime_test());
}

} // end namespace sta
