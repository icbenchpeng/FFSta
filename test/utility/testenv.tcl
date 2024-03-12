source_design "../cases/gcd/load.tcl"

set pin [get_pins "dpath/a_mux/U7/Z"]

test_fsta utility/network_ptr_access
