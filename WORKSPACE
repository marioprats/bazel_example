workspace(name = "bazel_example")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Rules to be able to build cmake projects.
http_archive(
    name = "rules_foreign_cc",
    sha256 = "2a4d07cd64b0719b39a7c12218a3e507672b82a97b98c6a89d38565894cf7c51",
    strip_prefix = "rules_foreign_cc-0.9.0",
    url = "https://github.com/bazelbuild/rules_foreign_cc/archive/refs/tags/0.9.0.tar.gz",
)
load("@rules_foreign_cc//foreign_cc:repositories.bzl", "rules_foreign_cc_dependencies")
rules_foreign_cc_dependencies()

# URDFDOM.
_all_content = """\
filegroup(
    name = "all_srcs",
    srcs = glob(["**"]),
    visibility = ["//visibility:public"],
)
"""
http_archive(
    name = "urdfdom",
    build_file_content = _all_content,
    strip_prefix = "urdfdom-3.0.0",
    url = "https://github.com/ros/urdfdom/archive/refs/tags/3.0.0.zip",
)

# Abseil.
http_archive(
  name = "com_google_absl",
  urls = ["https://github.com/abseil/abseil-cpp/archive/98eb410c93ad059f9bba1bf43f5bb916fc92a5ea.zip"],
  strip_prefix = "abseil-cpp-98eb410c93ad059f9bba1bf43f5bb916fc92a5ea",
)