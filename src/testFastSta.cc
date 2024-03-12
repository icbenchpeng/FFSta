#include "TestFramework.hh"
#include "FastSta.hh"

#include "ord/OpenRoad.hh"
#include "sta/Sta.hh"
#include "sta/Search.hh"


namespace sta {

class compile_and_sched_arrival_test : public Test {
public:
  compile_and_sched_arrival_test() : Test(__FUNCTION__) {}
  int run() {
    // Laurel::getPtr<sta::Pin>("pin_obj");
    // ord::OpenRoad::openRoad()->getSta();

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