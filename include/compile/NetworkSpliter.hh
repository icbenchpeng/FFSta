#pragma once

#include "utility/AliasMap.hh"
#include "Sta.hh"

namespace sta {
  class FastSta;
};

namespace fsta {

typedef sta::FastSta FastSta;

class NetworkSpliter : public sta::StaState {
  typedef sta::Vertex Vertex;
  typedef unsigned    Id;
  typedef std::unordered_map<Vertex*, Id> NetworkIdMap;
  typedef std::vector<Vertex*> SubNetwork;
  struct SubNetworks : public std::vector<std::shared_ptr<SubNetwork>> {
	std::shared_ptr<SubNetwork> get(Id const & id) {
      if (id < size()) return at(id);
      assert(size() == id);
      auto p = std::make_shared<SubNetwork>();
      push_back(p);
      return p;
	}
  };

  struct BfsBkwdEdgeIterator : public BfsBkwdIterator {
	using sta::BfsBkwdIterator::enqueueAdjacentVertices;
	BfsBkwdEdgeIterator(BfsIndex i, SearchPred *p, NetworkSpliter *s)
	 : BfsBkwdIterator(i, p, s), spliter(s) {}
	virtual void enqueueAdjacentVertices(Vertex* vertex, SearchPred* search_pred, Level to_level) {
	  iterVertexEdge(vertex, to_level);
	};
  protected:
    bool ableToEnqueue(SearchPred* p, Vertex* v, Level const& l, Edge* e) {
      return v->level() >= l && p->searchFrom(v) && p->searchThru(e) &&
             v->isDriver(network_) && !search_->isClock(v);
    }
	void iterVertexEdge(Vertex* v, Level to_level) {
	  if (search_pred_->searchTo(v)) {
		VertexInEdgeIterator edge_iter(v, graph_);
		while (edge_iter.hasNext()) {
		  Edge* e = edge_iter.next();
		  Vertex* pv = e->from(graph_);
		  if (ableToEnqueue(search_pred_, pv, to_level, e)) {
			spliter->process(e);
		    enqueue(pv);
		  }
		}
	  }
	}
	NetworkSpliter* spliter;
  };
  void process(Edge* e) {
	auto& idmap = *pIdMap;
	Vertex* t = e->to(graph_);
	Vertex* f = e->from(graph_);
	bool et = idmap.count(t);
	bool ef = idmap.count(f);
	if (!et && !ef) idmap[f] = idmap[t] = idCount++;
	else if (et && !ef) idmap[f] = idmap[t];
	else if (!et && ef) idmap[t] = idmap[f];
	else {
	  Id idf = idmap[f];
	  Id idt = idmap[t];
	  (*pAlias)(idf, idt);
	}
  }
public:
  NetworkSpliter(FastSta* s) : idCount(0), sta(s), pAlias(nullptr), pIdMap(nullptr) { copyState((StaState*)sta); }
  void split() {
	AliasMap<Id> aliasMap;
	NetworkIdMap  idmap;
	pIdMap = &idmap;
	pAlias  = &aliasMap;
	SearchPredNonReg2 srch_non_reg(this);
	BfsBkwdEdgeIterator bkwd_iter(BfsIndex::other, &srch_non_reg, this);
	VertexSet* ends = search_->endpoints();
	for (Vertex* v : *ends) {
	  bkwd_iter.enqueueAdjacentVertices(v);
	}
	while (bkwd_iter.hasNext()) {
	  Vertex* v = bkwd_iter.next();
	  if (isRegisterOrTopLevelPort(v)) continue;
	  if (search_->isClock(v)) continue;
      bkwd_iter.enqueueAdjacentVertices(v);
	}
	idCount = 0;
	std::map<Id, Id> ids;
	for (auto& p : *pIdMap) {
      Id c = collect(ids, (*pAlias)[p.second]);
      p.second = c;
      subNetworks.get(c)->push_back(p.first);
	}
  }
  bool isRegisterOrTopLevelPort(Vertex* v) const {
	Pin* p = v->pin();
	if (network_->isTopLevelPort(p)) return true;
    LibertyPort* port;
	LibertyCell* cell;
	return (port = network_->libertyPort(p)) && (cell = port->libertyCell()) && cell->hasSequentials();
  }

  size_t        size() const { return idCount; }
  SubNetworks   subNetworks;
  Id            idCount;
private:
  Id collect(std::map<Id, Id>& ids, Id id) {
    auto it = ids.find(id);
    if (it != ids.end())
      return it->second;
    Id ret = idCount++;
    ids.insert({id, ret});
    return ret;
  }
  FastSta*      sta;
  AliasMap<Id>* pAlias;
  NetworkIdMap* pIdMap;

};

} // end namespace fsta
