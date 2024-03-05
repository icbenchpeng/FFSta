#pragma once

#include <set>
#include <cassert>
#include "sta/Graph.hh"
#include "sta/GraphClass.hh"
#include "sta/Tag.hh"

namespace sta {

extern "C" {

struct TaggedVertex {
  Vertex*   v;
  Tag*      tag;
};

struct TaggedData {
  TaggedVertex          tagged;
  TimingArc*            arc;
  Edge*                 edge;
  const PathAnalysisPt* path_ap;
  bool                  to_propagates_clk;
  Arrival*              p_arrival;
  Required*             p_require;
  size_t                c_fanouts;  // ?
  size_t                c_fanins;   // ?
  // TaggedJump         fanouts[cfanouts];
  // TaggedJump         fanins[cfanins];
};

struct TaggedJump {
  size_t                offset; // ?
};

} // extern "C"

class FastSchedQueue {
  struct Less {
    bool operator()(TaggedData*, TaggedData*) const;
  };
  typedef std::set<TaggedData*, Less> Queue;
  Queue queue;
public:
  void sched(TaggedData* d) { assert(d); queue.insert(d); }

};

class FastSta : public StaState {
public:
  FastSta(StaState* sta) { s_sta = sta; }
  void update(Vertex* v) {
    // compile and update all relative map
	// sched v relative fanin tagged data
  }
  static StaState* s_sta;
public: // compile
  struct CTaggedVertex : public TaggedVertex {
	size_t index() const { assert(s_sta); return s_sta->graph()->id(v) << 32 + tag->index(); }
  };
  struct CTaggedData : public TaggedData {
  };

  void compileDelayCalc(CTaggedVertex from, TimingArc* arc, Edge* edge, bool to_propagates_clk, PathAnalysisPt* path_ap) {

  }
  void compilePath(CTaggedVertex from, CTaggedVertex to) {

  }

public: // runtime
  struct RTaggedData : public TaggedData {
  };

  FastSchedQueue arrivalQueue;
  FastSchedQueue requiredQueue; // arrivals that will not send sched should be add into requiredQUeue
  void schedArrival(RTaggedData*);
  void schedRequired(RTaggedData*);
};

} // end namespace sta
