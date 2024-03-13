#include "TestFramework.hh"
#include "utility/BitMap.hh"

namespace sta {

class bitmap_ut : public Test {
public:
  bitmap_ut() : Test(__FUNCTION__) {}
  void printAllSet(BitMap const & bm) const {
	logger()->warn("index:\n");
	size_t i = BitMap::invalidIndex;
    do {
     i = bm.next(i);
     logger()->warn("%d\n", (int)i);
    } while(i != BitMap::invalidIndex);
  }
  int run() {
    BitMap bm(100);
    logger()->warn("%b\n", (bool)bm[0]);
    bm[0] = true;
    logger()->warn("%b\n", (bool)bm[0]);
    printAllSet(bm);
    bm[0] = false;
    bm[32] = true;
    bm[45] = true;
    bm[46] = true;
    bm[47] = true;
    bm[63] = true;
    printAllSet(bm);
	return 0;
  }
};

class network_ptr_access : public Test {
public:
  network_ptr_access() : Test(__FUNCTION__) {}
  int run() {
	auto p = getPtr<sta::Pin>("pin");
	logger()->warn("%s\n", network()->name(p));
    return 0;
  }
};

Test*
fsta_utility_test() {
  TestGroup* group = new TestGroup("utility");
  group->add(new bitmap_ut);
  group->add(new network_ptr_access);
  return group;
}

} // end namespace sta
