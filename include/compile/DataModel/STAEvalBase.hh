#pragma once

#include <cassert>
#include <cstddef>
#include <typeinfo>
#include <unordered_set>

namespace fsta {

class STAEvalBase {
public:
  struct ImplBase {
    ImplBase() : refCount(0), id(-1), hashValue(-1) {}
    virtual ~ImplBase() {}
    unsigned refCount;
    unsigned id;
    const std::type_info* rtti;
    size_t hashValue;

    size_t hash() const { return hashValue; }
    virtual bool eq(ImplBase* i) const = 0;
  }* impl;

  static void refCountDecr(ImplBase* i) {
    if (!i) return;
    if (--i->refCount == 0) {
      assert(manager);
      assert(manager->used.count(i));
      manager->used.erase(i);
      delete i;
    }
  }
  static void refCountIncr(ImplBase* i) {
    if (!i) return;
    assert(manager);
    assert(manager->used.count(i) && "Currently no duplication of manager is needed");
    ++i->refCount;
  }
  friend struct Manager;
  bool operator==(STAEvalBase const & o) const { return impl == o.impl; }
public:
  struct ImplPtrHash { size_t operator()(ImplBase* i) const { return i->hash(); } };
  struct ImplPtrEq { bool operator()(ImplBase* r, ImplBase* l) const { if (r->rtti != l->rtti) return false; return r->eq(l); } };
  struct Manager {
    std::unordered_set<ImplBase*, ImplPtrHash, ImplPtrEq> used;
    unsigned idUsed;
    Manager() : idUsed(0) {}
    ~Manager() { assert(used.empty()); }
  };
  static void createManager();
  static thread_local Manager* manager;
};

} // end namespace fsta
