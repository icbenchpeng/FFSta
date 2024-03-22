#include "utility/BitStream.hh"

namespace fsta {

static size_t
fileLength(const char* fn) {
  std::fstream len_test(fn, std::ios::in);
  len_test.seekg(0, std::ios::end);
  auto l = len_test.tellg();
  return l == -1 ? 0 : (size_t)l;
}

void
touchFile(const char* fn) {
  std::fstream(fn, std::ios::app);
}

BitStream::BitStream(std::string const & filename, bool write) : fstream() {
  static const ios_base::openmode write_flags = fstream::in | fstream::out | fstream::binary;
  static const ios_base::openmode  read_flags = fstream::in | fstream::binary;
  const char* fn = filename.c_str();
  original_file_end = fileLength(fn);
  if (write) {
	touchFile(fn);
    fstream::open(fn, write_flags);
  } else {
    fstream::open(fn, read_flags);
  }
}

void
BitStream::virtualWrite(char* value, size_t bytes) {
  while (bytes) {
    char buf;
    fstream::read(&buf, 1);
    if (buf != *value) {
	  fstream::seekp((size_t)fstream::tellp() - 1);
	  fstream::write(value, 1);
    }
    --bytes;
    ++value;
  }
  fstream::flush();
}

void
BitStream::write(char* value, size_t bytes) {
  write_end = fstream::tellp();
  if (write_end + bytes <= original_file_end)
    virtualWrite(value, bytes);
  else
    fstream::write(value, bytes);
}

void
BitStream::write(size_t offset, char* value, size_t bytes) {
  write_end = fstream::tellp();
  if (offset < write_end) {
    fstream::seekp(offset);
    virtualWrite(value, bytes);
    fstream::seekp(write_end);
  } else {
    write(value, bytes);
  }
}

} // end namespace sta
