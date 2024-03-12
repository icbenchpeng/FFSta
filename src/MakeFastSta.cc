#include "tcl.h"
#include "../MakeFastSta.h"
#include "ord/OpenRoad.hh"

namespace sta {
extern const char* FastSta_tcl_inits[];
}

extern "C" {
extern int Faststa_Init(Tcl_Interp* interp);
}

namespace ord {

void initFastSta(OpenRoad* ord) {
  Faststa_Init(ord->tclInterp());
}

} // end namespace ord
