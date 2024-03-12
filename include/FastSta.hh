#pragma once

#include "tcl.h"
#include <cassert>
#include "sta/Fuzzy.hh"
#include "sta/Graph.hh"
#include "sta/GraphClass.hh"
#include "sta/Tag.hh"
#include "sta/Search.hh"
#include "sta/TagGroup.hh"
#include "sta/PathAnalysisPt.hh"

namespace sta {

class StaState;
class TimingArc;
class Edge;
class PathAnalysisPt;
class Tag;
class Vertex;

extern "C" {

struct TaggedVertex {
  Vertex* v;
  Tag* tag;
};

} // end extern "C"

class FastSta : public StaState {
public:
  FastSta() : StaState() {}
  virtual ~FastSta() {}
  virtual void compileDelayCalc(const TaggedVertex& from, const TaggedVertex& to, TimingArc* arc, Edge* edge, bool to_propagates_clk, const PathAnalysisPt* path_ap) = 0;
  virtual void compilePath(const TaggedVertex& from, const TaggedVertex& to) = 0;
  virtual void update(Vertex* v) = 0;
  virtual void compileTestBuilder() = 0;
  virtual void findAllArrivals() = 0;
  static FastSta* create(StaState* sta);

  static Tcl_Interp* tcl_interp;
  template <typename T>
  static T* getPtr(std::string const& tclVar) {
    const char* tcl_obj;
    tcl_obj = Tcl_GetVar(tcl_interp, tclVar.c_str(), TCL_GLOBAL_ONLY);
    void* p;
    convertPtrFromString(tcl_obj, &p);
    return reinterpret_cast<T*>(p);
  }

  static const char* unpack_data(const char* c, void* ptr, size_t sz) {
    unsigned char* u = (unsigned char*) ptr;
    const unsigned char* eu = u + sz;
    for (; u != eu; ++u) {
      char d = *(c++);
      unsigned char uu;
      if ((d >= '0') && (d <= '9'))
        uu = (unsigned char) ((d - '0') << 4);
      else if ((d >= 'a') && (d <= 'f'))
        uu = (unsigned char) ((d - ('a' - 10)) << 4);
      else
        return (char*) 0;
      d = *(c++);
      if ((d >= '0') && (d <= '9'))
        uu |= (unsigned char) (d - '0');
      else if ((d >= 'a') && (d <= 'f'))
        uu |= (unsigned char) (d - ('a' - 10));
      else
        return (char*) 0;
      *u = uu;
    }
    return c;
  }

  static void convertPtrFromString(const char* c, void** ptr) {
    assert(*c == '_');
    c++;
    c = unpack_data(c, ptr, sizeof(void*));
  }
};

} // end namespace sta
