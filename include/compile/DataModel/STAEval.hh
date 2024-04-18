#pragma once

#include "STAEvalVirtual.hh"
#include "STAEvalIds.hh"
#include "STAEvalOffsets.hh"
#include "utility/BitStream.hh"
#include "runtime/InsModel/Functions.hh"
#include "DataModel.h"

namespace fsta {

class Eval;
typedef STAEvalVir<Eval> STAEval;

class Eval : public STAEvalBase::ImplBase {
public:
  virtual void dump(BitStream& s) = 0;
  virtual size_t nArgs() const { return 0; }
  virtual STAEval arg(size_t n) const { assert(0); return this; }
  typedef uint32_t Word;

  template< typename T >
  T* userObj() const { return dynamic_cast<T*>(user); }
  template< typename T, typename ... Args >
  void setUserObj(Args && ... args) { user = std::make_shared<T>(std::forward<Args>(args)...); }
  template< typename T >
  void setUserObj(T* passOwnership) { user = std::shared_ptr<T>(passOwnership); }
};

template<typename T>
class Load : public Eval, public Ids {
public:
  StLoadStore code;
  Load(T* d) : code({LD, extractType(d), extractId(d)}), data(d) {}
  T* data;
};
template<typename T>
using STALd = STAEvalCon<Load<T>>;

template<typename T>
class Store : public Eval, public Ids {
  StLoadStore code;
  T* data;
};
template<typename T>
using STASt = STAEvalCon<Store<T>>;

template<typename T, JumpType JmpTy>
class Jump : public Eval {
  typedef ShortAddr Differ;
  StJump code;
  Differ diff;
  Jump(Differ d = 0) : diff(d) {
	code.op   = JP;
	code.type = JmpTy;
	code.free = 0;
	code.addr = diff;
  }
  void dump(BitStream& s) {
	s.write((char*)&code, sizeof(code));
  }
};

template<typename T, JumpType JTy >
using STAJmp = STAEvalCon<Jump<T, JTy>>;

template<typename T, JumpType JTy >
class LongJump : public Jump<T, JTy>, public Offsets {
  typedef Jump<T,JTy> Super;
  StJump code;
  // Offset jumpTo
  BasicBlockIdBase* bb;
  LongJump(T const & to) : Super(), bb(bbmap.set(to)) {}

  void dump(BitStream& s) {
    Super::dump(s);
	patches.set(&s);
    patches.patch(bb);
  }
};

template<typename T, JumpType JTy>
using STALJmp = STAEvalCon<LongJump<T, JTy>>;

template<typename T>
class BasicBlock : public Eval, public Offsets {
public:
  BasicBlock(T uuid) {} //T can be { TimingArc, Vertex } { Vertex only }
  void add(STAEval const & s) { stmts.push_back(s); }
protected:
  std::vector<STAEval> stmts;
};
template<typename T>
using STABB = STAEvalCon<BasicBlock<T>>;

template< typename F >
class Calc : public Eval {
public:
  Calc(F f, PopOp pop) {
	code.op   = CA;
	code.pop  = pop;
	code.type = initFuncManager().find(f);
  }
  StCalc code;

  void dump(BitStream& s) {
	s.write((char*)&code, sizeof(code));
  }
};
template<typename Func>
using STACalc = STAEvalCon<Calc<Func>>;

/**
BasicBlock bbv(Vertex*);
Load loadV(Vertex*);
bbv.add(loadV);
BasicBlock cgd = makeCalcGateDelay(Cell, Arc, Slew, Cap); // fill arrival* of all arcs
bbv.add(cgd);
BasicBlock arrivals = sinkArrivlas(Arrival*s); // load arc arrival*s and use minmax to get min/max final arrival*
bbv.add(arrivals);
for (pin : LoadPins)
BasicBlock wd = makeWireDelay(Arrival*, Pin); // sched next basicblock is required. thie requires min/max final arrival*
bbv.add(wd);
**/

} // end namespace fsta
