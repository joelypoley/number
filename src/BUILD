cc_library(
  name = "integer",
  srcs = ["integer.cpp", ],
  hdrs = ["integer.h", ],
  #copts=["-Weverything"],
)

cc_test(
  name = "integer_test",
  srcs = ["integer_test.cpp", ],
  copts=['-Iexternal/gtest/include'],
  deps = [
        ":integer",
        "@gtest//:main",
    ],
)