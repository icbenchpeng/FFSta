#pragma once

#include "utility/Meta.hh"
#include "compile/DataModel/ByteCodeStream.hh"
#include <unordered_map>
#include <set>
#include <cassert>

namespace fsta {

class Offsets {
public:
  typedef uint32_t Offset;
  enum { InvalidOffset = (Offset)-1 };

  struct BasicBlockIdBase {
    virtual ~BasicBlockIdBase() {}
    virtual bool eq(BasicBlockIdBase* o) const = 0;
    virtual size_t hash() const = 0;
  };
  template< class T >
  struct BasicBlockId : public BasicBlockIdBase, public T {
	template<typename ... Args>
	BasicBlockId(Args&& ... args) : T(std::forward<Args>(args) ...) {}
	BasicBlockId(T const & o) : T(o) {}

	bool operator==(BasicBlockId const & o) const { return T::operator==(o); }
	template< typename U >
	bool operator==(BasicBlockId<U> const & ) const { return false; }
	size_t hash() const { return T::hash(); }
	bool eq(BasicBlockIdBase* o) const {
	  auto p = dynamic_cast<BasicBlockId*>(o);
	  if (!p) return false;
	  return *this == *p;
	}
  };
  template< typename T >
  struct BasicBlockId<T*> : public BasicBlockIdBase {
    T* value;
    BasicBlockId(T* v) : value(v) {}
    bool operator==(BasicBlockId const & o) const { return value == o.value; }
    template< typename U >
    bool operator==(BasicBlockId<U> const &) const { return false; }
    size_t hash() const { return (size_t)value; }
    bool eq(BasicBlockIdBase* o) const {
      auto p = dynamic_cast<BasicBlockId*>(o);
      if (!p) return false;
      return *this == *p;
    }
  };
  // assume no old c type should exist for BasicBlockId marking

  struct BasicBlockIdBasePtrHash {
    size_t operator()(BasicBlockIdBase* p) const { return p->hash(); }
  };
  struct BasicBlockIdBasePtrEq {
    size_t operator()(BasicBlockIdBase* o0, BasicBlockIdBase* o1) const {
      return o0->eq(o1);
    }
  };
  struct BBMap : public std::unordered_map<BasicBlockIdBase*, Offset, BasicBlockIdBasePtrHash, BasicBlockIdBasePtrEq> {
	typedef std::unordered_map<BasicBlockIdBase*, Offset, BasicBlockIdBasePtrHash, BasicBlockIdBasePtrEq> Super;

	BasicBlockIdBase* set(BasicBlockIdBase* key, Offset off = InvalidOffset) {
	  BasicBlockIdBase* ret = nullptr;
	  auto r = find(key);
	  assert(r != end());
	  assert(r->second == InvalidOffset);
	  r->second = off;
	  ret = r->first;
	  if (off != InvalidOffset)
	    patches.patchAll(ret);
	  return ret;
	}
	template< typename T, std::enable_if_t< !std::is_convertible<T, BasicBlockIdBase*>::value, int > = 0 >
    BasicBlockIdBase* set(T const & v, Offset off = InvalidOffset) {
      BasicBlockId<T> key(v);
      BasicBlockIdBase* ret = nullptr;
      auto r = find(&key);
      if (r == end()) {
    	ret = Super::insert(Super::value_type(new BasicBlockId<T>(v), off)).first->first;
      } else {
    	assert(r->second == InvalidOffset);
    	r->second = off;
    	ret = r->first;
      }
      if (off != InvalidOffset)
    	patches.patchAll(ret);
      return ret;
    }

    Offset operator[](BasicBlockIdBase* key) const {
	  auto r = find(key);
	  if (r == end()) return InvalidOffset;
	  return r->second;
	}
    template< typename T, std::enable_if_t< !std::is_convertible<T, BasicBlockIdBase*>::value, int > = 0 >
	Offset operator[](T const & v) const {
      BasicBlockId<T> key(v);
      return operator[](&key);
	}

	void clear() { for (auto & x : *this) { delete x.first; } Super::clear(); }
	~BBMap() { clear(); }
  };

  struct PatchRequires : public std::unordered_map<BasicBlockIdBase*, std::set<Offset>*, BasicBlockIdBasePtrHash, BasicBlockIdBasePtrEq > {
	typedef std::unordered_map<BasicBlockIdBase*, std::set<Offset>*, BasicBlockIdBasePtrHash, BasicBlockIdBasePtrEq> Super;

	void patchAll(BasicBlockIdBase* bb) {
	  auto r = find(bb);
	  if (r == end()) return;
	  auto off = bbmap[bb];
	  assert(off != InvalidOffset);
	  for (auto v : *(r->second))
		stream->write(v, (char*)(&off), sizeof(Offset));
	  delete r->second;
	  erase(r);
	}
	void patch(BasicBlockIdBase* bb, Offset pos = InvalidOffset) {
	  if (pos == InvalidOffset)
	  	pos = stream->tellp();
	  auto off = bbmap[bb];
	  stream->write(pos, (char*)&off, sizeof(Offset));
	  if (off == InvalidOffset)
		addRequire(bb, pos);
	}
	void set(BitStream* s) { stream = s; }

	void addRequire(BasicBlockIdBase* bb, Offset pos) {
      auto r = find(bb);
      if (r == end()) r = insert({bb, new std::set<Offset>}).first;
      r->second->insert(pos);
	}
	void clear() { for (auto & x : *this) { delete x.second; } Super::clear(); }
	~PatchRequires() { clear(); }
	BitStream* stream;
  };

  static thread_local BBMap bbmap;
  static thread_local PatchRequires patches;
};

} // end namespace fsta
