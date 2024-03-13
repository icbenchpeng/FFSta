set path $::env(TEST_CASE_PATH)

define_corners ff ss
read_liberty -corner ff $path/../pdk/umc110/libs/ff/ua11lscef15bdrll_165c-40_ff.lib
read_liberty -corner ss $path/../pdk/umc110/libs/ss/ua11lscef15bdrll_135c125_ss.lib

read_lef $path/../pdk/umc110/lefs/ua11lscef15bdrll_6m2t.lef
read_lef $path/../pdk/umc110/lefs/ua11lscef15bdrll.lef

read_def $path/place_dp.def
read_sdc $path/place_dp.sdc

source $path/../pdk/umc110/setRC.tcl
