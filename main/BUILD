cc_binary(
    name = "cart",
    srcs = ["main.c"],
    deps = [
        "//cart:cart_lib"
    ],
    copts = ["-I/usr/include/libxml2"],
    linkopts = ["-lxml2"],
    visibility = ["//visibility:public"],
)