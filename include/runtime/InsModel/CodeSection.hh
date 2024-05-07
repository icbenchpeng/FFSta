#pragma once

#include <unordered_map>
#include "utility/ByteCodeStream.hh"

namespace fsta {

struct CodeSectionBase {
  enum Type {
	Ids,
	Funcs,
	NumOfSecs
  };
  typedef size_t Offset;
  struct Header {
	Type     type;
	uint32_t free_packing;
	Offset   offset;
  };
  enum {
	patchOffsetOfHeader = offsetof(Header, offset)
  };
};

struct CodeSection : public CodeSectionBase {
  CodeSection(Header const & h, ByteCodeStream& stream) : header(h), pcode(stream[header.offset]) {}
  virtual ~CodeSection() {}
  virtual bool load() = 0;
  typedef ByteCodeStream::CodeLoader CodeLoader;

  Header     header;
  CodeLoader pcode;
};

struct IdSection : public CodeSection {
  IdSection(Header const & h, ByteCodeStream& s) : CodeSection(h, s) {}
  bool load() { return true; }
};

struct FuncSection : public CodeSection {
  FuncSection(Header const & h, ByteCodeStream& s) : CodeSection(h, s) {}
  bool load() { return true; }
};

struct SectionOrganizer : protected std::unordered_map<CodeSectionBase::Type, CodeSection*>, public CodeSectionBase {
  typedef std::unordered_map<Type, CodeSection*> Super;
  SectionOrganizer(ByteCodeStream& s) : stream(s) {
	auto pcode = stream[0];
	for (size_t i = 0; i < NumOfSecs; ++i) {
	  Header header = *(Header*)(const Word*)pcode;
	  pcode += ((sizeof(Header) + (sizeof(Word) - 1)) / sizeof(Word));
	  CodeSection* sec = nullptr;
	  switch (header.type) {
	  case Ids:   sec = new   IdSection(header, stream); break;
	  case Funcs: sec = new FuncSection(header, stream); break;
	  default: assert(0); break;
	  }
	  Super::insert({header.type, sec});
	}
  }
  CodeSection* operator[](Type const ty) const { return Super::at(ty); }
  ByteCodeStream& stream;
};

} // end namespace fsta

