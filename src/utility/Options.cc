#include "utility/Options.hh"
#include "sta/Error.hh"

namespace sta {

void
Options::setOptValue(const char* name, const char* val) {
  try {
    parse(name)->set(val);
  } catch (private_fsta::Error* e) {
	throw ExceptionMsg(e->what());
	delete e;
  }
}

} // end namespace sta
