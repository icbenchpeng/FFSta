%{

#include "TestFramework.hh"

%}

%include "../../../Exception.i"
%inline %{

namespace sta {

void 
test_faststa_cmd(const char* case_name) {
  TestFramework* tf = new TestFramework("./src/sta/faststa/test");
  fsta_test(tf);
  tf->test(case_name);
  delete tf;
}

} // end namespace sta

%} // inline