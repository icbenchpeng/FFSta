#pragma once

#include <unordered_map>
#include <vector>
#include <cassert>
#include "runtime/VirtualStack.hh"
#include "DataModel.h"

namespace fsta {

struct FuncBase {
  virtual ~FuncBase() {}
  virtual void call(Stack& s) = 0;
};

template<typename F>
struct Func : public FuncBase {
  Func(F* f) : func(f) {}

  F* func;
  void call(Stack& s) { s.call(func); }
};

struct FuncManager {
  typedef std::unordered_map<void*, CalcType> IdMap;
  struct FuncInfo {
    FuncBase*   fb;
    const char* name;
  };
  typedef std::vector<FuncInfo>              Funcs;

  template< typename F >
  CalcType registor(F* f, const char* n) {
	auto r = idmap.insert({(void*)f, idmap.size()});
	if (r.second) funcs.push_back({new Func<F>(f), n});
    return r.first->second;
  }
  template< typename F >
  CalcType find(F* f) {
    assert(idmap.count((void*)f));
    return idmap[(void*)f];
  }
  const FuncInfo& get(CalcType ty) { return funcs[ty]; }

  ~FuncManager() { for(auto f : funcs) { delete f.fb; } }
  IdMap idmap;
  Funcs funcs;
};

static const FuncManager& initFuncManager();

} // end namespace fsta
