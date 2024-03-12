#include "TestFramework.hh"

namespace sta {

class bitmap : public Test {
public:
  bitmap() : Test(__FUNCTION__) {}
  int run() {
	return 0;
  }
};

Test*
fsta_utility_test() {
  TestGroup* group = new TestGroup("utility");
  group->add(new bitmap);
  return group;
}

} // end namespace sta
