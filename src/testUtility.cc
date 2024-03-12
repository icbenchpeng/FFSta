#include "TestFramework.hh"

namespace sta {

class bitmap_ut : public Test {
public:
  bitmap_ut() : Test(__FUNCTION__) {}
  int run() {
	logger()->warn("OK");
	return 0;
  }
};

Test*
fsta_utility_test() {
  TestGroup* group = new TestGroup("utility");
  group->add(new bitmap_ut);
  return group;
}

} // end namespace sta
