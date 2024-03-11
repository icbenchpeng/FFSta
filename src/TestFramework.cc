#include "TestFramework.hh"
#include <fstream>

namespace sta {

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

} // end namespace sta
