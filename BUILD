load("@rules_foreign_cc//foreign_cc:defs.bzl", "cmake")

cmake(
    name = "urdfdom",
    env = {
        "CMAKE_BUILD_TYPE": "Release",
    },
    lib_source = "@urdfdom//:all_srcs",
    out_include_dir = "include/urdfdom",
    out_shared_libs = [
        "liburdfdom_world.so",
    ],
    targets = [
        "install",
    ],
    visibility = ["//visibility:public"],
)