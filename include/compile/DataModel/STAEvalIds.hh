#pragma once

#include <cstddef>
#include <unordered_map>
#include <typeinfo>
#include "Sta.hh"
#include "DataModel.h"

namespace fsta {

struct Ids {
  struct IdMapBase {
	virtual ~IdMapBase() {}
	virtual size_t getId(void* t) = 0;
  };
  template< typename T >
  struct IdMap : public IdMapBase, public std::unordered_map<void*, size_t> {
	size_t getId(void* t) {
	  return insert({t, size()}).first->second;
	}
  };
  struct IdManager : public std::unordered_map<const std::type_info*, IdMapBase* > {
	typedef std::unordered_map<const std::type_info*, IdMapBase* > Super;
	template<typename T>
	size_t operator[](T* t) {
	  auto ty = &typeid(T*);
	  auto it = find(ty);
	  if (it == end()) it = Super::insert({ty, new IdMap<T*>}).first;
	  return it->second->getId((void*)t);
	}
	void clear() { for (auto x : *this) delete x.second; }
	~IdManager() { clear(); }
  };

  IdType extractType(sta::Pin*) const { return IdTy_Pin; }
  Id     extractId(sta::Pin* p) { return manager[p]; }
  static thread_local IdManager manager;
};

// needs two process:
// prepareIds() // all graph nodes (pin, vertex must be aliased)
// dumpIds()    // used before dump code

} // end namespace fsta
