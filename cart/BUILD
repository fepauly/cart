cc_library(
    name = "cart_lib",
    srcs = [
        "src/commands.c",
        "src/deadline.c",
        "src/feature.c",
        "src/init.c",
        "src/meta.c",
    ],
    hdrs = [
        "src/commands.h",
        "src/deadline.h",
        "src/feature.h",
        "src/init.h",
        "src/meta.h",
    ],
    deps = [
        "//cart/cart_handler:cart_handler_lib",
    ],
    includes = ["src"],
    copts = ["-I/usr/include/libxml2"],
    linkopts = ["-lxml2"],
    visibility = ["//visibility:public"],
)