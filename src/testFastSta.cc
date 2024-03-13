#include "TestFramework.hh"
#include "FastSta.hh"

#include "ord/OpenRoad.hh"
#include "sta/PortDirection.hh"
#include "sta/Sta.hh"

// #include "db_sta/dbNetwork.hh"
#include "db_sta/dbSta.hh"
// #include "sta/dbSta.hh"
#include "sta/Search.hh"
#include "sta/Network.hh"

namespace sta {

class compile_and_sched_arrival_test : public Test {
public:
  compile_and_sched_arrival_test() : Test(__FUNCTION__) {}
  int run() {
    auto pin = FastSta::getPtr<sta::Pin>("pin_to_update_from");
    auto dbSta = ord::OpenRoad::openRoad()->getSta();
    auto graph = dbSta->graph();
    auto network = dbSta->network();
    auto fastSta = network->search()->faststa();
    fastSta->findAllArrivals();
    logger()->warn("do nothing find\n");
    fastSta->update(network->direction(pin)->isInput() ? network->graph()->pinLoadVertex(pin) : network->graph()->pinDrvrVertex(pin));
    logger()->warn("after update do find arrivals\n");
    fastSta->findAllArrivals();
    
    logger()->warn("pin name %s\n", network->name(pin));
    // Laurel::getPtr<sta::Pin>("pin_obj");

    // sta::Sta().search()->faststa()->update();
	return 0;
  }
};

Test*
fsta_faststa_test() {
  TestGroup* group = new TestGroup("faststa");
  group->add(new compile_and_sched_arrival_test);
  return group;
}


}