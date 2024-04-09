#include "compile/DataModel/STAEvalOffsets.hh"

namespace fsta {

thread_local Offsets::BBMap
Offsets::bbmap;

thread_local Offsets::PatchRequires
Offsets::patches;

} // end namespace fsta
