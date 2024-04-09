project (faststa
  LANGUAGES CXX
  )

execute_process(COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/include/.DS_Store/.gencode.py ${CMAKE_CURRENT_SOURCE_DIR}/include/datamodel)

set(FASTSTA_SOURCE
  src/FastSta.cc
  src/TestFramework.cc
  src/MakeFastSta.cc
  src/testFastSta.cc
  src/testUtility.cc
  src/testRunTime.cc
  src/testCompileInfra.cc
  src/compile/DataModel/STAEvalBase.cc
  src/compile/DataModel/STAEvalIds.cc
  src/compile/DataModel/STAEvalOffsets.cc
  src/utility/Options.cc
  src/utility/BitStream.cc
  )

swig_lib(NAME faststa
NAMESPACE sta
I_FILE src/FastSta.i
SCRIPTS src/FastSta.tcl
)

target_sources(faststa
  PRIVATE
  ${FASTSTA_SOURCE}
  )

target_compile_options(faststa
  PRIVATE
  $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -pedantic -Wcast-qual -Wredundant-decls -Wformat-security> -DABC_NAMESPACE=abc
  $<$<CXX_COMPILER_ID:AppleClang>:-Wall -Wextra -pedantic -Wcast-qual -Wredundant-decls -Wformat-security -Wno-gnu-zero-variadic-macro-arguments> -DABC_NAMESPACE=abc
  $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -pedantic -Wcast-qual -Wredundant-decls -Wformat-security -Wno-gnu-zero-variadic-macro-arguments> -DABC_NAMESPACE=abc
  )

target_include_directories(faststa
  PUBLIC
  include
  ../include
)

target_link_libraries(faststa
  PUBLIC
    odb
    dbSta
)

messages(
  TARGET faststa
  OUTPUT_DIR ..
)
