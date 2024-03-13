sta::define_cmd_args "test_fsta" { case_name }

proc cd_eden { } {
  set curdir [pwd]
  set edendir [string range $curdir 0 [string first /Eden/ $curdir]]
  cd ${edendir}Eden/
}

proc source_design { design_load_tcl } {
  set ::env(TEST_CASE_PATH) [file dirname $design_load_tcl]
  source $design_load_tcl
  cd_eden	
}

proc test_fsta { args } {
  set case_name [lindex $args 0]
  sta::test_faststa_cmd $case_name
}