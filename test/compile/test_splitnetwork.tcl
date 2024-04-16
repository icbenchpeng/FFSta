source ../helpers.tcl
cd ../../../../../

set test_path "./src/sta/faststa/test/cases"

# define_corners ff ss

# read_liberty -corner ff $test_path/pdk/umc110/libs/ff/ua11lscef15bdrll_165c-40_ff.lib
read_liberty $test_path/pdk/umc110/libs/ss/ua11lscef15bdrll_135c125_ss.lib
#read_liberty $test_path/pdk/umc110/libs/ss/ua11lscef15bdrll_135c125_ss.lib
read_lef $test_path/pdk/umc110/lefs/ua11lscef15bdrll_6m2t.lef
read_lef $test_path/pdk/umc110/lefs/ua11lscef15bdrll.lef
read_def $test_path/gcd/place_dp.def
read_sdc $test_path/gcd/place_dp.sdc
source $test_path/pdk/umc110/setRC.tcl

sta::set_crpr_enable 0
estimate_parasitics -placement
set_delay_calculator simple_rc

test_fsta compile/splitnetwork
