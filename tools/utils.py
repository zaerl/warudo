import sys


# Function to generate a new file.
def generate_file(
    path: str, start: str, end: str, replacement: str, start_pos=0, save=True
) -> dict[str, int | str]:
    with open(path, "r") as f:
        content = f.read()

    f.close()

    res = generate_content(content, start, end, replacement, start_pos)

    if save:
        with open(path, "w") as f:
            f.write(res["text"])

        f.close()

    return res


# Function to generate a new content.
def generate_content(
    content: str, start: str, end: str, replacement: str, start_pos=0
) -> dict[str, int | str]:
    start_pos = content.find(start, start_pos)

    if start_pos == -1:
        print(f"Could not find start marker: {repr(start)}")
        sys.exit(1)

    end_pos = content.find(end, start_pos + len(start))

    if end_pos == -1:
        print(f"Could not find end marker: {repr(end)} after position {start_pos}")
        sys.exit(1)

    start_pos += len(start) + 1

    new_file_start = content[:start_pos]
    new_file_end = content[end_pos:]
    new_file = new_file_start + replacement + new_file_end

    return {
        "start_pos": start_pos,
        "text": new_file,
    }
