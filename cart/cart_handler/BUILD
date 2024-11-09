cc_library(
    name = "cart_handler_lib",
    srcs = [
        "cart.c",
    ],
    hdrs = [
        "cart.h",
    ],
    deps = ["//cart/utils:utils_lib"],
    includes = ["."],
    copts = ["-I/usr/include/libxml2"],
    linkopts = ["-lxml2"],
    visibility = ["//visibility:public"],
)