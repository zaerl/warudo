import sys


# Function to generate a new file.
def generate_file(path, start, end, replacement, start_pos=0, save=True):
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
def generate_content(content, start, end, replacement, start_pos=0):
    try:
        start_pos = content.find(start, start_pos)
        end_pos = content.find(end, start_pos)
    except ValueError:
        print("Could not find the start/end tests declarations in file.")
        sys.exit(1)

    start_pos += len(start) + 1

    new_file_start = content[:start_pos]
    new_file_end = content[end_pos:]
    new_file = new_file_start + replacement + new_file_end

    return {
        "start_pos": start_pos,
        "text": new_file,
    }


# Function to get a value from a list safely.
def safe_get(lst, index, default=None):
    if isinstance(lst, list) and 0 <= index < len(lst):
        return lst[index]

    return default


# Transform a value to a configuration file value.
def value_to_conf(value):
    if value is None:
        return "null"

    if isinstance(value, int):
        return value

    return "'" + value.replace("'", "\\'") + "'"


# Add indentation to a list of values.
def add_indentation(values, indentation=1):
    indentation = " " * indentation * 4

    return [f"{indentation}{value}" for value in values]


# Transform a value to a C file value.
def value_to_c(value):
    if value is None:
        return "NULL"

    if isinstance(value, int):
        return value

    return '"' + value.replace('"', '\\"') + '"'
