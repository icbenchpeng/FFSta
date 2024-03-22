#pragma once

#include <fstream>

namespace fsta {

class BitStream : public std::fstream {
  typedef std::fstream fstream;
  size_t write_end = 0;
  size_t original_file_end = 0;
  void virtualWrite(char* value, size_t bytes);
public:
  BitStream(std::string const & filename, bool write = false);
  void write(size_t offset, char* value, size_t bytes);
  void write(char* value, size_t bytes);
};

} // end namespace fsta
