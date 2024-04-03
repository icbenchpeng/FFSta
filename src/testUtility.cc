#include "TestFramework.hh"
#include "utility/BitMap.hh"
#include "utility/Options.hh"
#include "utility/BitStream.hh"

namespace sta {

using namespace fsta;

class bitstream_ut : public Test {
public:
  bitstream_ut() : Test(__FUNCTION__) {}
  int run() {
	const char* filename = "/tmp/testbitstream.log";
    BitStream s(filename, true);
    assert(s.is_open());
    char buffer [] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    s.write(buffer, sizeof(buffer));
    s.write(10, "aaaaabaaaa", 10);
    s.close();

    BitStream ss(filename);
    char buffer2[sizeof(buffer)] = {0};
    ss.read(buffer2, 20);
    ss.close();
    logger()->warn("%s\n", buffer2);
    return 0;
  }
};

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

class option_ut : public Test {
public:
  option_ut() : Test(__FUNCTION__) {}
  int run() {
    Options opts;
    opts.registor(new OptionT<bool>("has_set_value"));
    opts.parse("has_set_value")->set("true");
    logger()->warn("%b\n", opts.get<bool>("has_set_value"));
    opts.parse("has_set_value")->set("false");
    logger()->warn("%b\n", opts.get<bool>("has_set_value"));

    opts.registor(new OptionT<int>("int_val"));
    opts.parse("int_val")->set("100");
    logger()->warn("%d\n", opts.get<int>("int_val"));

    opts.registor(new OptionT<float>("float_val"));
    opts.parse("float_val")->set("10.1030");
    logger()->warn("%f\n", opts.get<float>("float_val"));

    //opts.setOptValue("float_val", "1.1.1");
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
  group->add(new bitstream_ut);
  group->add(new bitmap_ut);
  group->add(new network_ptr_access);
  group->add(new option_ut);
  return group;
}

} // end namespace sta
