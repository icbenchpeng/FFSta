#pragma once

#include <cassert>
#include <cstring>

#include "sta/Hash.hh"

namespace sta {

class BitMap {
  typedef unsigned Word;
  enum {
    wordBits = sizeof(Word) * 8,
  };
  inline size_t allocSize(size_t size) const { return (size - 1) / wordBits + 1; }
  inline size_t wordIndex(size_t idx) const { return idx / wordBits; }
  inline size_t bitIndex(size_t idx) const { return idx % wordBits; }

 public:
  enum {
	invalidIndex = (size_t)-1,
  };
  BitMap(size_t size) : words(nullptr), count(size), allocSz(allocSize(count)) {
    assert(size != 0);
    words = new Word[allocSz];
    memset(words, 0, allocSz * sizeof(Word));
  }
  ~BitMap() { delete[] words; }
  bool operator[](size_t index) const {
    if (index >= count) return false;
    return (words[wordIndex(index)] >> bitIndex(index)) & 0x1;
  }
  class Bit {
   public:
    Bit(Word* p, size_t i) : ref(p), mask(1 << i) {}
    bool operator=(bool val) {
      if (val)
        *ref |= mask;
      else
        *ref &= ~mask;
      return val;
    }
    operator bool() const { return (*ref & mask) ? true : false; }

   private:
    Word* ref;
    Word mask;
  };
  Bit operator[](size_t index) {
    assert(index < count);
    return Bit(words + wordIndex(index), bitIndex(index));
  }
  size_t next(size_t cur = invalidIndex) const {
    ++cur;
    if (cur >= count) return invalidIndex;
    unsigned cbit = bitIndex(cur);
    Word* start = words + wordIndex(cur);
    Word mask = (Word)-1 << cbit;
    Word bits = *start & mask;
    if (!bits) {
      cbit = 0;
      start++;
      while (!*start) {
        if (start >= (words + allocSz)) return invalidIndex;
        ++start;
      }
      bits = *start;
    }
    size_t ret = wordBits * (start - words) + firstIndexOf(bits);
    if (ret < count) return ret;
    return invalidIndex;
  }

 private:
  size_t firstIndexOf(Word bits) const {
    assert(bits);
    size_t testBits = wordBits;
    size_t result = 0;
    do {
      testBits /= 2;
      if (testBits == 0) break;
      Word lomask = ((Word) 1 << testBits) - 1;
      if (lomask & bits) {
        bits &= lomask;
        continue;
      }
      result += testBits;
      bits >>= testBits;
    } while (1);
    return result;
  }
  BitMap& operator=(BitMap const&) = delete;
  Word* words;
  size_t count;
  size_t allocSz;
};

}  // end namespace sta
