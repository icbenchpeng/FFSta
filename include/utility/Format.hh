#pragma once

#include <cstdio>
#include <string>

namespace fsta {

struct StringFormat {
  StringFormat(std::string const & s) : format(s) {}
  template<typename ... Args >
  StringFormat operator()(Args ... args) const {
	snprintf(buf, 4096, format.c_str(), args ...);
	return StringFormat(buf);
  }
  StringFormat operator*(unsigned v) const {
	std::string buf;
	while(v--) buf += format;
	return StringFormat(buf.c_str());
  }
  StringFormat operator+(std::string const & s) const { return StringFormat(format + s); }
  inline const char* c_str() { return format.c_str(); }
  inline operator const char*() { return c_str(); }
  inline operator std::string() { return format; }
  std::string format;
  mutable char buf[4096];
  template<typename T, typename ... Args>
  StringFormat join(T const arg0, Args ... args) const { return StringFormat(std::string(arg0) + format + join(args...)); }
  template<typename T>
  StringFormat join(T const arg0) const { return StringFormat(arg0); }
};

inline StringFormat operator""_f(const char* str, size_t n) {
  return StringFormat(str);
}

} // end namespace fsta
