#pragma once

#include "FastSta.hh"
#include "sta/Graph.hh"


namespace sta {

typedef long long TaggedJumpOffset;

extern "C" {

struct OutDelayCalc {
  TimingArc* arc;
  Edge* edge;
  const PathAnalysisPt* path_ap;
  bool to_propagates_clk;
};

struct TaggedJump {
  TaggedJumpOffset offset;  // ?
};

struct TaggedJumpFanin : public TaggedJump {
  OutDelayCalc delayCalc;
};

struct TaggedJumpFanout : public TaggedJump {};

struct TaggedData {
  TaggedVertex tagged;
  Arrival* p_arrival;
  Required* p_require;
  size_t fanouts_count;  // ?
  size_t fanins_count;   // ?
  bool sched;
  // TaggedJumpFanin fanouts[cfanouts];
  // TaggedJumpFanout fanins[cfanins];
};

}  // extern "C"

struct TaggedDataCmp {
  bool operator()(TaggedData* const& a, TaggedData* const& b) const {
    if (a->tagged.v->level() == b->tagged.v->level())  return a < b;
    else return a->tagged.v->level() < b->tagged.v->level();
  }
};

};  // namespace sta
