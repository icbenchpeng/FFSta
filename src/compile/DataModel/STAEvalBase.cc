#include <stdlib.h>
#include "compile/DataModel/STAEvalBase.hh"

namespace fsta {

thread_local STAEvalBase::Manager*
STAEvalBase::manager = nullptr;

static void deleteManager() {
  delete STAEvalBase::manager;
}

void
STAEvalBase::createManager() {
  if (manager) return;
  ::atexit(deleteManager);
  STAEvalBase::manager = new Manager;
}

} // end namespace fsta
