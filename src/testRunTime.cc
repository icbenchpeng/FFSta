#include <initializer_list>
#include "runtime/FastSchedQueue.hh"
#include "runtime/VirtualStack.hh"
#include "TestFramework.hh"
#include "runtime/InsModel/Functions.hh"

namespace sta {

using namespace fsta;

class sched_queue : public Test {
public:
  struct PureData {
    unsigned id;
    unsigned lvl;
    unsigned nexts;
    bool     sched:1;
  };
  struct Data : public PureData {
	enum {
	  baseOffset = sizeof(PureData),
	  nextOffset = sizeof(unsigned)
	};
    unsigned level() { return lvl; }

    void* operator new(size_t _, unsigned lv, const std::initializer_list<int>& xx) {
      size_t nxs = xx.size();
      size_t sz = baseOffset + nextOffset * nxs;
      void* ptr = new char[sz];
      memset(ptr, 0, sz);
      Data* ret = (Data*)ptr;
      ret->id = count_id++;
      ret->lvl = lv;
      ret->nexts = nxs;
      ret->sched = false;
      unsigned* p = (unsigned*)(((char*)ptr) + baseOffset);
      for (auto x : xx) {
    	*p = x;
    	++p;
      }
      return ret;
    }

    void operator delete (void* ptr) {
      char* p = (char*) ptr;
      delete [] p;
    }
    unsigned& operator[](size_t i) {
      char* p = ((char*)this) + baseOffset + nextOffset * i;
      return *(unsigned*)p;
    }
    void log(Logger* logger) const {
      logger->warn("id=%d\n", id);
    }
    static unsigned count_id;
  };
  sched_queue() : Test(__FUNCTION__) {}
  int run() {
	size_t nLevel = 10;
	std::vector<Data*> events = {
	  new (0, {1, 2}) Data, // 0->1, 2

	  new (1, {3, 4}) Data, // 1->3, 4
	  new (1, {3, 4}) Data, // 2->3, 4

	  new (2, {5, 6}) Data, // 3->5, 6
	  new (2, {6, 7}) Data, // 4->6, 7

	  new (3, {9}   ) Data, // 5->9
	  new (3, {8}   ) Data, // 6->8
	  new (3, {10}  ) Data, // 7->10

	  new (4, {9,10}) Data, // 8->9, 10

	  new (5, {11,12})Data, // 9->11,12
	  new (5, {13}  ) Data, // 10->13

	  new (6, {}) Data, // 11
	  new (6, {}) Data, // 12
	  new (6, {}) Data  // 13
	};

	FastSchedQueue<Data> queue(nLevel, true);
	queue.sched(events[0]);
	while (!queue.empty()) {
	  Data* d = queue.get();
	  d->log(logger());
	  for (unsigned i = 0; i < d->nexts; ++i)
		queue.sched(events[(*d)[i]]);
	}

	for (auto & x : events)
	  delete x;
    return 0;
  }
};

unsigned
sched_queue::Data::count_id = 0;

class stack : public Test {
public:
  stack() : Test(__FUNCTION__) {}

  struct Load {
	Load(long long vv) : v(vv) {}
    void eval(Stack& s) {
      long long& b = s.push<long long>();
      b = v;
    }
    long long v;
  };
  struct Store {
    Store(long long& rr) : r(rr) {}
    void eval(Stack& s) {
      r = s.pop<long long>();
    }
    long long & r;
  };
  struct Incr {
	void eval(Stack& s) {
      s.call(incr);
      s.push<long long>();
	}
    static void incr(long long& v) {
      v++;
    }
  };
  struct Add {
	void eval(Stack& s) {
	  s.call(add);
	}
    static long long add(long long const & a, long long & b) {
      return a + b;
    }
  };
  int run() {
    Stack stack(4096);
    Load b(1000); b.eval(stack);
    Load a(1); a.eval(stack);
    Incr incr; incr.eval(stack);
    Add  add;  add.eval(stack);
    long long r;
    Store s(r); s.eval(stack);
    logger()->warn("%d", r);
    return 0;
  }
};

static long long add(long long a, long long b) {
  return a + b;
}

class funcmanager : public Test {
public:
  struct Load {
	Load(long long vv) : v(vv) {}
	void eval(Stack& s) {
	  long long& b = s.push<long long>();
	  b = v;
	}
	long long v;
  };
  struct Store {
	Store(long long& rr) : r(rr) {}
	void eval(Stack& s) {
	  r = s.pop<long long>();
	}
	long long & r;
  };
  struct Call {
    Call(CalcType ty, FuncManager* mgr) : func(mgr->get(ty).fb){}
    void eval(Stack& s) {
      func->call(s);
    }
    FuncBase* func;
  };
  funcmanager() : Test(__FUNCTION__) {}
  int run() {
    FuncManager mgr;
    CalcType ct = mgr.registor(add, "add");
    logger()->warn("%d\n", mgr.find(add));
    Stack stack(4096);
    Load b(1000);       b.eval(stack);
	Load a(1);          a.eval(stack);
	Call c(ct, &mgr);   c.eval(stack);
	long long r;
	Store d(r);         d.eval(stack);
	logger()->warn("%d\n", r);
    return 0;
  }
};

Test*
fsta_runtime_test() {
  TestGroup* group = new TestGroup("runtime");
  group->add(new sched_queue);
  group->add(new stack);
  group->add(new funcmanager);
  return group;
}

} // end namespce sta
