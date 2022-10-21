load("@rules_proto_grpc//cpp:defs.bzl", "cpp_grpc_library")

load("@com_grail_bazel_compdb//:defs.bzl", "compilation_database")
load("@com_grail_bazel_output_base_util//:defs.bzl", "OUTPUT_BASE")

compilation_database(
    name = "compdb",
    targets = [
        "//kv:server",
        "//kv:client",
    ],
    # OUTPUT_BASE is a dynamic value that will vary for each user workspace.
    # If you would like your build outputs to be the same across users, then
    # skip supplying this value, and substitute the default constant value
    # "__OUTPUT_BASE__" through an external tool like `sed` or `jq` (see
    # below shell commands for usage).
    output_base = OUTPUT_BASE,
)

load("@rules_foreign_cc//foreign_cc:defs.bzl", "cmake")
cmake(
    name = "wiredtiger",
    lib_source = "@com_mongodb_wiredtiger//:all_srcs",
    lib_name = "libwiredtiger",
    generate_args = ["-DENABLE_STATIC=1"],
) 

cpp_grpc_library(
    name = "kv_cpp_grpc_lib",
    protos = ["//kv/protos:kv_proto"],
)

cc_binary(
    name = "server",
    srcs = ["kvserver.cpp"],
    deps = [":kv_cpp_grpc_lib"],
)

cc_library(
    name = "client_lib",
    srcs = ["kvclient.cpp"],
    hdrs = ["kvclient.hpp"],
    deps = [":kv_cpp_grpc_lib"],
)

cc_library(
    name = "client_async_lib",
    srcs = ["kvclientasync.cpp"],
    hdrs = ["kvclientasync.hpp", "kvclient.hpp"],
    deps = [":kv_cpp_grpc_lib"],
)

cc_binary(
    name = "client",
    srcs = ["kvclientmain.cpp"],
    deps = [":client_lib", ":client_async_lib", ":kv_cpp_grpc_lib"],
)

cc_test(
  name = "bench",
  srcs = ["kvbench.cpp"],
  deps = ["@com_google_benchmark//:benchmark_main", ":client_lib"],
)

cc_binary(
    name = "wtcpp",
    srcs = ["wiredtiger.cpp"],
    deps = [":wiredtiger"]
)