#include "runtime/InsModel/Functions.hh"

namespace fsta {

static FuncManager*
sFuncManager = nullptr;

static void deleteFuncManager() {
  delete sFuncManager;
}

const FuncManager&
initFuncManager() {
  if (!sFuncManager) {
	sFuncManager = new FuncManager;
	atexit(deleteFuncManager);
#define REGISTOR_FUNC(FUNC) sFuncManager->registor(FUNC, #FUNC);

	//REGISTOR_FUNC();

#undef  REGISTOR_FUNC
  }
  return *sFuncManager;
}

}
