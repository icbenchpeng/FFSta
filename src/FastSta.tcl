sta::define_cmd_args "test_fsta" { case_name }

proc test_fsta { args } {
  set case_name [lindex $args 0]
  sta::test_faststa_cmd $case_name
}