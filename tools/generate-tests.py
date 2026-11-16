import glob
import os
from utils import generate_file

# Use glob to find all .cpp files.
c_files = glob.glob("src/*.cpp")
c_files += glob.glob("cli/*.cpp")

# The main.cpp file don't need to be tested.
if "src/main.cpp" in c_files:
    c_files.remove("src/main.cpp")

if "cli/main.cpp" in c_files:
    c_files.remove("cli/main.cpp")

names: list[str] = []

for file in c_files:
    # Path relative to the source root, e.g. "http".
    root = "cli" if file.startswith("cli/") else "src"
    name = os.path.splitext(os.path.relpath(file, root))[0]
    c_name = os.path.basename(name).replace("-", "_")

    names.append(name)

    dest_filename = f"tests/{name}.cpp"

    # Tests register themselves with ATT_TEST, only the source list is generated.
    if not os.path.exists(dest_filename):
        new_file = f'#include "test.h"\n\nATT_TEST({c_name}) {{\n}}\n'

        with open(dest_filename, "w") as f:
            f.write(new_file)

            print(f"Created {dest_filename}")

names.append("test")
names.append("test-utils")
names.sort()

start_line = "set(TEST_SOURCES\n   "
replacement = ".cpp\n    ".join(names) + ".cpp\n"
generate_file("tests/CMakeLists.txt", start_line, ")\n", replacement)
