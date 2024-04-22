#pragma once

#include <vector>
#include <mutex>
#include "BitStream.hh"

namespace fsta {

struct ByteCodeStream {
  typedef uint32_t Word;
  enum {
    wordSz = sizeof(Word)
  };
  typedef uint32_t Offset;

  class CodeLoader {
  public:
    CodeLoader(const Offset& offset, const ByteCodeStream* s) : start(offset), current(start), bytecodestream(s), loaded(4096, 0) { reload(start); }
    operator const Word* () const { return &loaded[(curpos() / wordSz)]; }
    size_t bufCount() const { return loaded.size() - (curpos() / wordSz); }
    size_t size() const { return stream()->originalFileEnd() - current; }
    CodeLoader& operator+=(size_t v) { current += (v * wordSz); if (curpos() > loaded.size()) reload(current); return *this; }
    CodeLoader& operator-=(size_t v) { assert(current >= (v * wordSz)); current-= (v * wordSz); if (curpos() < 0) reload(current); return *this; }
    inline CodeLoader& operator++() { return (*this += 1); }
    inline CodeLoader& operator--() { return (*this -= 1); }
    inline CodeLoader& operator++(int) { CodeLoader ret = *this; ++*this; return ret; }
    inline CodeLoader& operator--(int) { CodeLoader ret = *this; --*this; return ret; }
    inline CodeLoader operator-(size_t v) const { CodeLoader ret = *this; ret -= v; return ret; }
    inline CodeLoader operator+(size_t v) const { CodeLoader ret = *this; ret += v; return ret; }
  private:
    inline size_t previous_start() const { return start < 4096 ? 0 : start - 4096; }
    inline ptrdiff_t curpos() const { return current - start; }
    void reload(const Offset& offset) {
      std::lock_guard<std::mutex> lock(bytecodestream->mutex);
      assert(offset >= 0);
      start = offset;
      auto oldpos = stream()->tellg();
      stream()->seekg(offset);
      if (loaded.size() < 4096) loaded.resize(4096);
      size_t sz = stream()->readsome((char*)&loaded[0], 4096 * sizeof(Word));
      sz /= sizeof(Word);
      if (sz < 4096) loaded.resize(sz);
      assert(stream()->good());
      stream()->seekg(oldpos);
    }
    BitStream* stream() const { return bytecodestream->stream; }
    Offset            start;
    Offset            current;
    const ByteCodeStream* bytecodestream;
    std::vector<Word> loaded;
  };

  ByteCodeStream(const char* filename) : stream(new BitStream(filename, false)) {}
  CodeLoader operator[] (const Offset& offset) const { return CodeLoader(offset, this); }
private:
  mutable std::mutex mutex;
  BitStream* stream;
};

} // end namespace fsta
