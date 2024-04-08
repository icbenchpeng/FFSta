#pragma once

#include "STAEvalBase.hh"

namespace fsta {

template<typename T >
struct STAEvalVir : public STAEvalBase {
protected:
  STAEvalVir() {}
public:
  STAEvalVir(ImplBase* i) { impl = i; refCountIncr(impl); }
  STAEvalVir(ImplBase const * i) { impl = (ImplBase*)i; refCountIncr(impl); }
  STAEvalVir(STAEvalBase const & base) { impl = base.impl; refCountIncr(impl); }
  STAEvalVir& operator=(STAEvalBase const & base) {
    refCountDecr(impl);
    impl = base.impl;
    refCountIncr(impl);
    return *this;
  }
  const T* operator->() const { return (T*)impl; }
  ~STAEvalVir() { refCountDecr(impl); }
};

template<typename T>
struct STAEvalCon : public STAEvalVir<T> {
protected:
  typedef STAEvalVir<T> Super;
public:
  STAEvalCon(typename Super::ImplBase* i) { Super::impl = i; refCountIncr(Super::impl); }
  STAEvalCon(typename Super::ImplBase const * i) { Super::impl = (typename Super::ImplBase*)i; refCountIncr(Super::impl); }
  STAEvalCon(STAEvalCon const & o) { Super::impl = o.impl; Super::refCountIncr(Super::impl); }
  T* operator->() { return (T*)Super::impl; }
  template<typename ... Args>
  STAEvalCon(Args&& ... args) {
	Super::createManager();
    Super::impl = new T(std::forward<Args>(args) ...);
    auto r = Super::manager->used.insert(Super::impl);
    if (!r.second) {
      delete Super::impl;
      Super::impl = *r.first;
    } else
    Super::impl->id = ++Super::manager->idUsed;
    Super::impl->rtti = &typeid(T);
    Super::refCountIncr(Super::impl);
  }
  ~STAEvalCon() { Super::refCountDecr(Super::impl); }
};

} // end namespace fsta
