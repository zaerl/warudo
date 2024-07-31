import glob
import os
from utils import wrd_generate_file

# Use glob to find all .c files.
c_files = glob.glob("src/*.c")
c_files += glob.glob("cli/*.c")

# The main.c file don't need to be tested.
if "src/main.c" in c_files:
    c_files.remove("src/main.c")

if "cli/main.c" in c_files:
    c_files.remove("cli/main.c")

functs = []
names = []
run_macro = []

for file in c_files:
    base_name = os.path.basename(file)
    name = os.path.splitext(base_name)[0]
    c_name = name.replace("-", "_")
    func = f"void *test_{c_name}(void *arg)"

    # Code to generate the test function
    functs.append(func)
    names.append(name)
    run_macro.append(f"    RUN_TEST({c_name})")

    dest_filename = f"test/{name}.c"

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
wrd_generate_file("test/test.h", start_line, "\n\n", ";\n".join(functs) + ";")
wrd_generate_file("test/test.c", start_line, "\n\n", "\n".join(run_macro))

start_line = "attractor/attractor.c\n   "
replacement = ".c\n    ".join(names) + ".c\n"
wrd_generate_file("test/CMakeLists.txt", start_line, ")\n", replacement)
