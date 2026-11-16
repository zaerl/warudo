import glob
import os
from utils import generate_file

# Use glob to find all .cpp files.
c_files = glob.glob("src/*.cpp")
c_files += glob.glob("src/app/*.cpp")
c_files += glob.glob("cli/*.cpp")

# The main.cpp file don't need to be tested.
if "src/main.cpp" in c_files:
    c_files.remove("src/main.cpp")

if "cli/main.cpp" in c_files:
    c_files.remove("cli/main.cpp")

functs: list[str] = []
names: list[str] = []
run_macro: list[str] = []

for file in c_files:
    # Path relative to the source root, e.g. "http" or "app/db".
    root = "cli" if file.startswith("cli/") else "src"
    name = os.path.splitext(os.path.relpath(file, root))[0]
    c_name = os.path.basename(name).replace("-", "_")
    func = f"void *test_{c_name}(void *arg)"

    # Code to generate the test function.
    functs.append(func)
    names.append(name)
    run_macro.append(f"    RUN_TEST({c_name})")

    dest_filename = f"tests/{name}.cpp"

    if not os.path.exists(dest_filename):
        new_file = f'#include "test.h"\n\n{func} {{\n    return NULL;\n}}\n'

        with open(dest_filename, "w") as f:
            f.write(new_file)

            print(f"Created {dest_filename}")

names.append("test")
names.append("test-utils")

functs.sort()
names.sort()
run_macro.sort()

start_line = "// Start tests declarations."
generate_file("tests/test.h", start_line, "\n\n", ";\n".join(functs) + ";")
generate_file("tests/test.cpp", start_line, "\n\n", "\n".join(run_macro))

start_line = "set(TEST_SOURCES\n   "
replacement = ".cpp\n    ".join(names) + ".cpp\n"
generate_file("tests/CMakeLists.txt", start_line, ")\n", replacement)
