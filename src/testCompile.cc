#include "TestFramework.hh"
#include "compile/DataModel/STAEvalVirtual.hh"
#include "compile/DataModel/STAEval.hh"

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

Test*
fsta_compile_test() {
  TestGroup* group = new TestGroup("compile");
  group->add(new evalbase);
  return group;
}

} // end namespace sta
