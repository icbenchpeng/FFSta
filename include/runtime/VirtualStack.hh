#pragma once

namespace sta {

class Stack {
  typedef uint32_t Word;
  enum {
    wordSz = sizeof(Word),
    largeMemory = 1024 * 1024 * 1024
  };
  Word* memory;
  Word* top;
  size_t size;
public:
  Stack(size_t initial_size) : memory(nullptr), top(nullptr), size(initial_size) {
    top = memory = (Word*)::malloc(size * wordSz);
    ::memset(memory, 0, size * wordSz);
  }
  ~Stack() { ::free(memory); }

  template<typename RetTy, typename ... Args>
  inline void call(RetTy (*func)(Args ...)) {
    push<RetTy>() = func(pop<std::remove_cvref_t<Args>>()...);
  }

  template<typename T>
  inline T& push() {
    size_t sz = toWords(sizeof(T));
    if (full(sz)) realloc(sz);
    T* ret = (T*)top;
    top += sz;
    return *ret;
  }
  template<typename T>
  inline T& pop() {
    size_t sz = toWords(sizeof(T));
    top -= sz;
    return *(T*)top;
  }
private:
  inline size_t toWords(size_t sz) const { return (sz + wordSz - 1) / wordSz; }
  inline bool full(size_t sz) const { return (memory + size - top) < sz; }
  void realloc(size_t demand) {
    size_t enlarge = demand;
    if (size < largeMemory)
      enlarge = size;
    size += enlarge;
    size_t d = top - memory;
    memory = (Word*)::realloc(memory, size * wordSz);
    ::memset(top, 0, enlarge * wordSz);
    top = memory + d;
  }
};

}
