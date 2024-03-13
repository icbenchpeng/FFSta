#include "tcl.h"
#include "FastSta.hh"
#include "../MakeFastSta.h"
#include "ord/OpenRoad.hh"
#include "sta/StaMain.hh"

namespace sta {
extern const char* faststa_tcl_inits[];
}

extern "C" {
extern int Faststa_Init(Tcl_Interp* interp);
}

namespace ord {

void initFastSta(OpenRoad* ord) {
  Tcl_Interp* interp = ord->tclInterp();
  Faststa_Init(interp);
  sta::evalTclInit(interp, sta::faststa_tcl_inits);
  sta::FastSta::tcl_interp = interp;
}

} // end namespace ord
