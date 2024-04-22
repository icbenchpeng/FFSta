#include "TestFramework.hh"
#include "compile/DataModel/STAEvalVirtual.hh"
#include "compile/DataModel/STAEval.hh"
#include "utility/ByteCodeStream.hh"

using namespace fsta;

namespace sta {
namespace test_evalbase {

struct V : public STAEvalBase::ImplBase {
  V(int v) : valueV(v) { hashValue = valueV; }
  int valueV;
};

typedef STAEvalVir<test_evalbase::V> Vn;

struct E : public V {
  int valueE;
  E(int v, int e) : V(v), valueE(e) { hashValue *= valueE; }
  bool operator==(const E& e) const {
    return valueV == e.valueV && valueE == e.valueE;
  }
  bool eq(ImplBase* i) const {
	auto v = dynamic_cast<E*>(i);
	if (!v) return false;
	return *this == *v;
  }
};

typedef STAEvalCon<test_evalbase::E> En;

struct F : public V {
  En valueF;
  F(int v, En const & e) : V(v), valueF(e) {}
  bool operator==(const F& e) const {
    return valueV == e.valueV && valueF == e.valueF;
  }
  bool eq(ImplBase* i) const {
    auto v = dynamic_cast<F*>(i);
    if (!v) return false;
    return *this == *v;
  }
};

typedef STAEvalCon<test_evalbase::F> Fn;

}

class evalbase : public Test {
public:
  typedef test_evalbase::Vn V;
  typedef test_evalbase::En E;
  typedef test_evalbase::Fn F;

  void print(V const & v) const {
    logger()->warn("%d\n", v->valueV);
  }
  evalbase() : Test(__FUNCTION__) {}
  int run() {
    E e(0, 9);
    print(e);
    F f(4, e);
    print(f);
    return 0;
  }
};

class evalids : public Test {
public:
  evalids() : Test(__FUNCTION__) {}
  int run() {
	Ids ids;

	sta::Pin* p = (sta::Pin*)9999L;
	logger()->warn(",ty:%d", ids.extractType(p));
	logger()->warn(",id:%d", ids.extractId(p));

    return 0;
  }
};

class evaloffsets : public Test {
public:
  struct II {
	int v;
	int d;
	bool operator==(II const & o) const { return v == o.v && d == o.d; }
	size_t hash() const { return v ^ d; }
  };

  struct Jump : public Offsets {
    Jump(II const to) : bb(bbmap.set(to)) {}
    void dump(BitStream& s) const {
      patches.set(&s);
      s.write((char*)code.c_str(), code.size());
      patches.patch(bb);
    }
    BasicBlockIdBase* bb;
    std::string code = "Good looks\n";
  };
  struct BasicBlock : public Offsets {
	BasicBlock(II const id) : bb(bbmap.set(id)) {}
	BasicBlockIdBase* bb;
	void dump(BitStream& s) const {
	  patches.set(&s);
      bbmap.set(bb, s.tellp());
      s.write((char*)code.c_str(), code.size());
	}
	std::string code = "The rain has let up\n";
  };

  evaloffsets() : Test(__FUNCTION__) {}
  int run() {
	Offsets offsets;

	II i {2, 3};
	logger()->warn("%d\n", offsets.bbmap[i]);
	auto key1 = offsets.bbmap.set(i);
	logger()->warn("%d\n", offsets.bbmap[i]);
	auto key2 = offsets.bbmap.set(i, 9999);
	if (key1 == key2)
	  logger()->warn("reset:");
	logger()->warn("%d\n", offsets.bbmap[i]);

	BitStream s("/tmp/kirinji.rainyrunway.music", true);
	char uppercode [] = "Step out\nThough it keeps raining\n";
	s.write(uppercode, sizeof(uppercode));
	II bbTag {1, 2};
	Jump j(bbTag);
	j.dump(s);
	char badcode [] = "......zzzz.zzzzzzzz..zz....";
	s.write(badcode, sizeof(badcode));
	BasicBlock bb(bbTag);
	bb.dump(s);
	s.close();

	// check result
	BitStream ss("/tmp/kirinji.rainyrunway.music");
	while (ss.good()) {
	  char c = ss.get();
	  if (ss.good())
	    logger()->binary(&c, 1);
	}

    return 0;
  }
};

Test*
fsta_compile_infra_test() {
  TestGroup* group = new TestGroup("compile");
  group->add(new evalbase);
  group->add(new evalids);
  group->add(new evaloffsets);
  return group;
}

} // end namespace sta
