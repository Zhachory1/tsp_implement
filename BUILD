filegroup(
    name = "test_files",
    srcs = glob(["*.txt"]),
)

cc_binary(
    name = "tsp_main",
    deps = [
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/flags:usage",
        "@com_google_absl//absl/status:status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:strings",
        "@com_github_google_glog//:glog",
    ],
    srcs = ["test.cc"],
    data = [":test_files"]
)