project (faststa
  LANGUAGES CXX
  )

set(FASTSTA_SOURCE
  src/FastSta.cc
  )

add_library(faststa
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
