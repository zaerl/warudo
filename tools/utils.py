import sys


# Function to generate a new file.
def wrd_generate_file(path, start, end, replacement):
    with open(path, "r") as f:
        content = f.read()

    f.close()

    try:
        start_pos = content.find(start)
        end_pos = content.find(end, start_pos)
    except ValueError:
        print(f"Could not find the start/end tests declarations in {path}.")
        sys.exit(1)

    start_pos += len(start) + 1

    new_file_start = content[:start_pos]
    new_file_end = content[end_pos:]
    new_file = new_file_start + replacement + new_file_end

    with open(path, "w") as f:
        f.write(new_file)

    f.close()
