#include "FastSta.hh"
#include <string>
#include "sta/Corner.hh"
#include "sta/Fuzzy.hh"
#include "sta/Network.hh"
#include "sta/TimingArc.hh"
#include "sta/Sta.hh"
#include "sta/PathAnalysisPt.hh"
#include "sta/Transition.hh"
#include "sta/Search.hh"

namespace sta {

StaState*
FastSta::s_sta = nullptr;

FastSta::FastSta(StaState* sta) {
  s_sta = sta;
}

std::string
FastSta::CTaggedDataBuilder::toString() {
  std::string res;
  res += "tagged_vertexs count\n";
  res += std::to_string(tagged_vertexs.size());
  res += "\n";
  struct AnalysisVertex {
    Vertex* v;
    RiseFall* rf;
    PathAnalysisPt* path_ap;
  };
//   std::map<AnalysisVertex, Arrival> arrivals;
  for (auto&& [_, data] : tagged_vertexs) {
    auto tag_v_from = data.tagged;
    auto rf = RiseFall::find(tag_v_from.tag->trIndex());
    auto path_ap = tag_v_from.tag->pathAnalysisPt(s_sta);
    auto from_arrival = Sta::sta()->vertexArrival(tag_v_from.v, rf, path_ap);
    assert(path_ap->corner()->findPathAnalysisPt(path_ap->pathMinMax()) == path_ap);
    // arrivals[AnalysisVertex{tag_v_from.v, rf, path_ap}] = from_arrival;
  }
  for (auto&& [_, data] : tagged_vertexs) {
    auto tag_v_from = data.tagged;
    auto to_arrival = Sta::sta()->vertexArrival(tag_v_from.v, RiseFall::find(tag_v_from.tag->trIndex()), tag_v_from.tag->pathAnalysisPt(s_sta));
    res += std::string("now tagged_vertex name :") + tag_v_from.v->name(s_sta->network()) + ' ' + tag_v_from.tag->asString(s_sta) + '\n';
    res += std::to_string(to_arrival * 1e9);
    res += '\n';
    res += "fanins :" + std::to_string(data.fanins.size()) + '\n';
    for (auto&& [tagged_vertex, delay_calc] : data.fanins) {
      res += std::string("name = ") + tagged_vertex.v->name(s_sta->network()) + '\n';
      res += std::string("tag = ") + tagged_vertex.tag->asString(s_sta) + '\n';

      assert(delay_calc.edge->to(s_sta->graph()) == data.tagged.v);
      assert(delay_calc.edge->from(s_sta->graph()) == tagged_vertex.v);
      auto tag_v_from = tagged_vertex;
      auto rf = RiseFall::find(tag_v_from.tag->trIndex());
      auto path_ap = tag_v_from.tag->pathAnalysisPt(s_sta);
      auto from_arrival = Sta::sta()->vertexArrival(tag_v_from.v, rf, path_ap);
      auto arc_delay = s_sta->search()->deratedDelay(tag_v_from.v, delay_calc.arc, delay_calc.edge, delay_calc.to_propagates_clk, delay_calc.path_ap);
      if (fuzzyEqual(to_arrival, from_arrival + arc_delay)) {
        res += "\nok\n";
      }
      res += '\n';
    }
    
    res += "fanouts :" + std::to_string(data.fanouts.size()) + '\n';
    for (auto&& tagged_vertex : data.fanouts) {
      res += std::string("name = ") + tagged_vertex.v->name(s_sta->network()) + '\n';
      res += std::string("tag = ") + tagged_vertex.tag->asString(s_sta) + '\n';
      
      res += '\n';
    }

    res += "\n";
  }
  res += "\n\n";
  decltype(tagged_vertexs) temp;
  tagged_vertexs.swap(temp);
  return res;
}

void
FastSta::update(Vertex* v) {
  ensureStaPointers();
  schedArrival(v);
  while (!arrival_queue.queue.empty()) {
    RTaggedData* r_tagged_data = (RTaggedData*) (*(arrival_queue.queue.begin()));
    arrival_queue.queue.erase(arrival_queue.queue.begin());
    auto min_max = r_tagged_data->tagged.tag->pathAnalysisPt(s_sta)->pathMinMax();
    auto arrival_before = *(r_tagged_data->p_arrival);
    auto arrival_res = min_max->initValue();
    for (size_t i = 0; i < r_tagged_data->fanins_count; i++) {
      auto fanin_tagged_data = r_tagged_data->fanin(i);
      auto delay_calc = r_tagged_data->faninEnv(i)->delayCalc;
      auto fanin_arrival = *(fanin_tagged_data->p_arrival);
      auto arc_delay = s_sta->search()->deratedDelay(fanin_tagged_data->tagged.v, delay_calc.arc, delay_calc.edge, delay_calc.to_propagates_clk, delay_calc.path_ap);
      auto arrival = fanin_arrival + arc_delay;
      if (min_max->compare(arrival, arrival_res)) { arrival_res = arrival; }
    }
    if (!min_max->compare(arrival_res, arrival_before)) {
      for (size_t i = 0; i < r_tagged_data->fanouts_count; i++) {
        auto fanout_tagged_data = r_tagged_data->fanout(i);
        schedArrival(fanout_tagged_data);
      }
    } else {
      std::cout << "arrival not pass, vertex name : " << r_tagged_data->tagged.v->name(s_sta->network()) << "  tag : " << r_tagged_data->tagged.tag->asString(s_sta) << '\n';
      std::cout << "arrival before : " << (arrival_before * 1e9) << " "
                << "arrival res : " << (arrival_res * 1e9) << '\n';
    }
  }
  // compile and update all relative map
  // sched v relative fanin tagged data
}
}
