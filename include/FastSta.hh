#pragma once

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
};

} // end namespace sta
