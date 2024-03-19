#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <exception>
#include <string>
#include <cassert>

namespace sta {
namespace private_fsta {

inline std::string operator""_str(const char* v, size_t sz) { return std::string(v); }

struct Error : public std::exception {
  template<typename ... Arg>
  Error(const char* msg, Arg ... args) {
	size_t sz = snprintf(0, 0, msg, args...);
	message = new char[sz];
	snprintf(message, sz, msg, args...);
  }
  ~Error() { delete [] message; }
  char* message;
  const char* what() const noexcept { return message; }
};

template<typename T> T convertTo(const char* v);
template<> inline bool convertTo<bool>(const char* v) {
  if ("true"_str == v) return true;
  if ("false"_str == v) return false;
  throw new Error("Error: converting '%s' to boolean, please use 'true' or 'false'", v);
}

#define CONVERT_TO_OLDCTYPE(TYPE, CHECKER, CONVERTOR)      \
template<> inline TYPE convertTo<TYPE>(const char* v) {    \
  CHECKER checker;                                         \
  if (!checker(v))                                         \
	throw new Error("Error: converting '%s' to "#TYPE, v); \
  TYPE ret = CONVERTOR(v);                                 \
  return ret;                                              \
}

using integerValueChecker = decltype([](const char* v) -> bool {
  while (*v) {
	if (!isdigit(*v)) return false;
	++v;
  };
  return true;
});
using floatValueChecker   = decltype([](const char* v) -> bool {
  bool hasDot = false;
  while (*v) {
	if (isdigit(*v)) { ++v; continue; }
	if (*v == '.' && !hasDot) { hasDot = true; ++v; continue; }
	return false;
  }
  return true;
});
CONVERT_TO_OLDCTYPE(int,   integerValueChecker, atoi)
CONVERT_TO_OLDCTYPE(long,  integerValueChecker, atol)
CONVERT_TO_OLDCTYPE(float, floatValueChecker,   atof)



} // end namespace private_fsta

class Option {
public:
  Option(const char* n) : name(n) {}
  const char* name;
  virtual void set(const char* value) = 0;
  virtual ~Option() {}
};

template<typename T>
class OptionT : public Option {
public:
  OptionT(const char* n) : Option(n) {}
  virtual void set(const char* v) {
    value = private_fsta::convertTo<T>(v);
  }
  T value;
};

class Options {
public:
  void registor(Option* opt) {
	map.insert({opt->name, opt});
  }
  Option* parse(const char* name) const {
    auto it = map.find(name);
    if (it == map.end())
      throw new private_fsta::Error("Error: '%s' is not an option of faststa", name);
    return it->second;
  }

  template<typename T>
  T get(const char* name) {
	OptionT<T>* opt = nullptr;
	try {
	  opt = dynamic_cast<OptionT<T>*>(parse(name));
	} catch (private_fsta::Error* e) {
      assert(!e);
	}
	assert(opt);
	return opt->value;
  }

  void setOptValue(const char* name, const char* val);
private:
  typedef std::unordered_map<const char*, Option*> OptionMap;
  OptionMap map;
};

} // end namespace sta
