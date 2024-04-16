#include "TestFramework.hh"
#include "compile/DataModel/STAEvalVirtual.hh"
#include "compile/DataModel/STAEval.hh"
#include "compile/DataModel/ByteCodeStream.hh"
#include "compile/NetworkSpliter.hh"

using namespace fsta;

namespace sta {

struct splitnetwork : public Test {
  splitnetwork() : Test(__FUNCTION__) {}
  int run () {
	auto dbSta = ord::OpenRoad::openRoad()->getSta();
	auto network = dbSta->network();
	auto fastSta = network->search()->faststa();
	NetworkSpliter s(fastSta);
	s.split();
	unsigned idCount = 0;
	for (auto p : s.subNetworks) {
	  logger()->warn("%d\n", idCount++);
	  for (auto v : *p)
        logger()->warn("  %s\n", network->name(v->pin()));
	}

	for (auto p : s.idmap2) {
      logger()->warn("%s:%d\n", network->name(p.first->pin()), p.second);
	}
    return 0;
  }
};

Test*
fsta_compile_test() {
  TestGroup* group = new TestGroup("compile");
  group->add(new splitnetwork);
  return group;
}

}
