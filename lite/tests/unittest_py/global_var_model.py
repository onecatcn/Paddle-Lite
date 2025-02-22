# Copyright (c) 2021 PaddlePaddle Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import pickle
from pathlib import Path
import os

statics_data = {
    "targets": set(),
    "all_test_ops": {
        "Host": set(),
        "X86": set(),
        "ARM": set(),
        "OpenCL": set(),
        "Metal": set()
    },
    "success_ops": {
        "Host": set(),
        "X86": set(),
        "ARM": set(),
        "OpenCL": set(),
        "Metal": set()
    },
    "out_diff_ops": {
        "Host": set(),
        "X86": set(),
        "ARM": set(),
        "OpenCL": set(),
        "Metal": set()
    },
    "not_supported_ops": {
        "Host": set(),
        "X86": set(),
        "ARM": set(),
        "OpenCL": set(),
        "Metal": set()
    },
}
static_file = Path("./statics_data")
static_file_path_str = "./statics_data"


# coding=utf-8
def set_value(kind, target, op):
    if not static_file.exists():
        global statics_data
    else:
        with open(static_file_path_str, "rb") as f:
            statics_data = pickle.load(f)

    statics_data["targets"].add(target)
    statics_data[kind][target].add(op)

    with open(static_file_path_str, "wb") as f:
        pickle.dump(statics_data, f)


def set_all_test_ops(target, op):
    set_value("all_test_ops", target, op)


def set_success_ops(target, op):
    set_value("success_ops", target, op)


def set_out_diff_ops(target, op):
    set_value("out_diff_ops", target, op)


def set_not_supported_ops(target, op):
    set_value("not_supported_ops", target, op)


def display():
    print("----------------------Unit Test Summary---------------------")
    with open("./statics_data", "rb") as f:
        statics_data = pickle.load(f)
        targets = statics_data["targets"]

    for target in targets:
        all_test_ops = statics_data["all_test_ops"][target]
        not_supported_ops = statics_data["not_supported_ops"][target]
        out_diff_ops = statics_data["out_diff_ops"][target]
        success_ops = statics_data["success_ops"][
            target] - not_supported_ops - out_diff_ops

        print("Target =", target)
        print("Number of test =", len(all_test_ops))
        print("Number of success =", len(success_ops))
        print("Number of not supported =", len(not_supported_ops))
        print("Number of output diff =", len(out_diff_ops))
        print("\nDetails:")
        print("Success:")
        print(list(success_ops))
        print("\nNot supported:")
        print(list(not_supported_ops))
        print("\nOutput diff:")
        print(list(out_diff_ops))
        print("\n")


if __name__ == "__main__":
    display()
