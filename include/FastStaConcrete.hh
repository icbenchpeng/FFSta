#pragma once
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_set>

#include "TaggedData.hh"
#include "FastSchedQueue.hh"
#include "sta/Fuzzy.hh"
#include "sta/Graph.hh"
#include "sta/GraphClass.hh"
#include "sta/Tag.hh"
#include "sta/Search.hh"
#include "sta/TagGroup.hh"
#include "sta/PathAnalysisPt.hh"

namespace sta {

class FastSchedQueue;
/*
{
  typedef std::set<TaggedData*, TaggedDataCmp> Queue;
 public:
  Queue queue;

 public:
  void sched(TaggedData* d) {
    assert(d);
    queue.insert(d);
  }
};
*/

class FastStaConcrete : public FastSta {
 public:
  FastStaConcrete(StaState* sta);
  void update(Vertex* v);
  static StaState* s_sta;

  struct RTaggedData;
  struct CTaggedVertex;
  struct CTaggedVertexHash;
 protected:
  struct RTaggedDataForm;
//   using RTaggedDataForm = std::unordered_map<CTaggedVertex, RTaggedData*, CTaggedVertexHash>;


 public: // compile
  struct CTaggedVertex : public TaggedVertex {
	CTaggedVertex(TaggedVertex const & o) : TaggedVertex(o) {}
    size_t index() const {
      assert(s_sta);
      return ((std::uint64_t)(s_sta->graph()->id(v)) << 32) + tag->index();
    }
    bool operator<(const CTaggedVertex& rhs) const {
      if (v->level() != rhs.v->level()) return v->level() < rhs.v->level();
      else return index() < rhs.index();
    }
    bool operator==(const CTaggedVertex& rhs) const {
      return index() == rhs.index();
    }
  };
  struct CTaggedVertexHash {
    size_t operator()(const CTaggedVertex& v) const { return v.index(); }
  };
  struct COutDelayCalc : public OutDelayCalc {
    // bool operator==(const COutDelayCalc&) const { return true; }
  };
  struct CTaggedData : public TaggedData {
    CTaggedData(CTaggedVertex tagged_vertex) {
      tagged = tagged_vertex;
    }
    CTaggedData() = default;
    std::map<CTaggedVertex, COutDelayCalc> fanins;
    std::set<CTaggedVertex> fanouts;

    size_t byteSize() const {
      return fanins.size() * (sizeof(TaggedVertex) + sizeof(TaggedJumpFanin)) + fanouts.size() * (sizeof(TaggedVertex) + sizeof(TaggedJumpFanout)) + sizeof(TaggedData);
    }
  };

  void compileDelayCalc(const TaggedVertex& from, const TaggedVertex& to, TimingArc* arc, Edge* edge, bool to_propagates_clk, const PathAnalysisPt* path_ap) override {
    auto&& from_data = c_tagged_data_builder.getOrAssign(from);
    auto&& to_data = c_tagged_data_builder.getOrAssign(to);

    from_data.fanouts.insert(to);
    to_data.fanins.insert_or_assign(from, COutDelayCalc{arc, edge, path_ap, to_propagates_clk});
  }
  void compilePath(const TaggedVertex& from, const TaggedVertex& to) override {
    c_tagged_data_builder.compileFromTagVertexToTagVertex(from, to);
  }

  void compile() {}
  void compileTestBuilder() {
    // toRuntime();
    std::cout << c_tagged_data_builder.toString() << std::endl;
  }

 protected:
  struct CTaggedDataBuilder {
    CTaggedData& getOrAssign(CTaggedVertex tag_v) {
      auto&& [iter, _] = tagged_vertexs.emplace(tag_v, tag_v);
      return iter->second;
    }

    void compileFromTagVertexToTagVertex(CTaggedVertex from, CTaggedVertex to) {
    //   auto&& from_data = get(from);
    //   auto&& to_data = get(to);
    // //   from_data.fanouts.insert(to);
    //   to_data.fanins.insert(from);
    }

    RTaggedDataForm buildOutRTaggedData() {
      RTaggedDataForm r_tagged_data_form;
      size_t sum_size = 0;
      for (auto&& [tagged_vertex, tagged_data] : tagged_vertexs) {
        sum_size += tagged_data.byteSize();
      }
      r_tagged_data_form.start = (RTaggedData*)malloc(sum_size);
      r_tagged_data_form.siz = sum_size;
      std::int8_t* data_start = (std::int8_t*)r_tagged_data_form.start;

      for (auto&& [tagged_vertex, tagged_data] : tagged_vertexs) {
        RTaggedData* p = new (data_start, tagged_data) RTaggedData;
        r_tagged_data_form.insert(tagged_vertex, p);
      }
      for (auto&& [tagged_vertex, tagged_data] : tagged_vertexs) {
        RTaggedData* data = r_tagged_data_form.get(tagged_vertex);
        for (int i = 0; auto&& [in_tagged_vertex, delay_calc] : tagged_data.fanins) {
          RTaggedData* in_address = r_tagged_data_form.get(in_tagged_vertex);
          auto fanin_env = data->faninEnv(i);
          fanin_env->offset = (std::int8_t*)in_address - (std::int8_t*)data;
          fanin_env->delayCalc = delay_calc;
          assert(data->fanin(i) == in_address);
          i++;
        }
        for (int i = 0; auto&& out_tagged_vertex : tagged_data.fanouts) {
          RTaggedData* out_address = r_tagged_data_form.get(out_tagged_vertex);
          auto fanout_env = data->fanoutEnv(i);
          fanout_env->offset = (std::int8_t*)out_address - (std::int8_t*)data;
          assert(data->fanout(i) == out_address);
          i++;
        }
      }
      tagged_vertexs.clear();
      return r_tagged_data_form;
    }

    std::string toString();
   private:
    std::map<CTaggedVertex, CTaggedData> tagged_vertexs;
  } c_tagged_data_builder;

 public:  // runtime
  void toRuntime() {
    tagged_data_form = c_tagged_data_builder.buildOutRTaggedData();
  }

  struct RTaggedData : public TaggedData {
    void* operator new (size_t _, std::int8_t*& pos, CTaggedData const& c_tagged_data) {
      RTaggedData& data = *(RTaggedData*)pos;
      auto siz = c_tagged_data.byteSize();
      memset(&data, 0, siz);
      data.tagged = c_tagged_data.tagged;
    //   memcpy(&data, &c_tagged_data, sizeof(TaggedData));
      data.fanins_count = c_tagged_data.fanins.size();
      data.fanouts_count = c_tagged_data.fanouts.size();
      pos += siz;
      return (void*)&data;
    }

    size_t numFanouts() const { return fanouts_count; }

    RTaggedData* fanout(size_t i) { return (RTaggedData*) (((std::int8_t*) this) + fanoutOffset(i)); }
    RTaggedData* fanin(size_t i) { return (RTaggedData*) (((std::int8_t*) this) + faninOffset(i)); }
    TaggedJumpFanout* fanoutEnv(size_t i) { assert(i < fanouts_count); return ((TaggedJumpFanout*)((std::int8_t*)this + sizeof(TaggedData))) + i; }
    TaggedJumpFanin* faninEnv(size_t i) { assert(i < fanins_count); return ((TaggedJumpFanin*)fanoutEnvEnd()) + i; }
    TaggedJumpFanout* fanoutEnvEnd() { return ((TaggedJumpFanout*)((std::int8_t*)this + sizeof(TaggedData))) + fanouts_count; }
   private:
    TaggedJumpOffset fanoutOffset(size_t i) { return (*(((TaggedJumpFanout*) ((std::int8_t*) this + sizeof(TaggedData))) + i)).offset; }
    TaggedJumpOffset faninOffset(size_t i) { return (*(((TaggedJumpFanin*) fanoutEnvEnd()) + i)).offset; }
  };

  FastSchedQueue* arrival_queue;
  FastSchedQueue* required_queue;  // arrivals that will not send sched should be add into requiredQUeue
  void schedArrival(RTaggedData* data) {
    arrival_queue->sched(data);
  }
  void schedRequired(RTaggedData*);

  void schedArrival(Vertex *v) {
    auto r_tagged_datas = tagged_data_form.get(v);
    for (auto data : r_tagged_datas) {
      schedArrival(data);
    }
  }
  void schedArrival(CTaggedVertex v) {
    schedArrival(tagged_data_form.get(v));
  }
  void ensureStaPointers() {
    tagged_data_form.stitchStaEnv();
  }

 protected:
  struct RTaggedDataForm {
    RTaggedData* get(CTaggedVertex tagged_vertex) const {
      return address.at(tagged_vertex);
    }

    std::vector<RTaggedData*> get(Vertex* vertex) const {
      std::vector<RTaggedData*> res;
      auto tags = s_sta->search()->tagGroup(vertex);
      for (auto [tag, arrival_index] : *(tags->arrivalMap())) {
        res.push_back(get(TaggedVertex{vertex, tag}));
      }
      return res;
    }

    void stitchStaEnv() {
      for (auto&& [c_tagged_vertex, r_tagged_data] : address) {
        Arrival* arrivals = s_sta->graph()->arrivals(c_tagged_vertex.v);
        Arrival* requireds = s_sta->graph()->requireds(c_tagged_vertex.v);
        int arrival_index = 0;
        bool exists = false;
        s_sta->search()->tagGroup(c_tagged_vertex.v)->arrivalIndex(c_tagged_vertex.tag, arrival_index, exists);
        assert(!exists);
        r_tagged_data->p_arrival = arrivals + arrival_index;
        r_tagged_data->p_require = requireds + arrival_index;
      }
    }
   protected:
    std::unordered_map<CTaggedVertex, RTaggedData*, CTaggedVertexHash> address;
    RTaggedData* start;
    size_t siz;

    void insert(CTaggedVertex tagged_vertex, RTaggedData* p) {
      address[tagged_vertex] = p;
    }
    friend CTaggedDataBuilder;
  } tagged_data_form;
};

}  // end namespace sta
