#!/usr/bin/env python3
import os

OUTPUT_FOLDER = "build"
BUILD_GENERATOR = "Ninja Multi-Config"
ENABLE_COMPILE_COMMANDS = 1
CXX_COMPILER = "clang++"


if __name__ == "__main__":
    ret = os.system(
        f"cmake -B {OUTPUT_FOLDER}" +
        f" -G \"{BUILD_GENERATOR}\"" +
        f" -DCMAKE_EXPORT_COMPILE_COMMANDS={ENABLE_COMPILE_COMMANDS}" +
        f" -DCMAKE_CXX_COMPILER={CXX_COMPILER}"
    )

    if ret == 0:
        print("Setup complete")
    else:
        print("Setup failed")
