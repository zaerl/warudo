import json
import sys

CONFIGS_PATH = "tools/configs.json"


# Add indentation to a list of values.
def add_indent(values: list[str], indentation: int = 1) -> list[str]:
    prefix = " " * indentation * 4

    return [f"{prefix}{value}" for value in values]


# Transform a value to a C file value.
def value_to_c(value: str | int | None, is_int: bool = False) -> str:
    if value is None:
        return "NULL"

    if is_int:
        return str(value)

    return '"' + str(value).replace('"', '\\"') + '"'


# Replace the content between the start and end markers.
def generate_content(
    content: str, start: str, end: str, replacement: str, start_pos: int = 0
) -> tuple[str, int]:
    start_pos = content.find(start, start_pos)

    if start_pos == -1:
        sys.exit(f"Could not find start marker: {start!r}")

    end_pos = content.find(end, start_pos + len(start))

    if end_pos == -1:
        sys.exit(f"Could not find end marker: {end!r} after position {start_pos}")

    start_pos += len(start) + 1

    return content[:start_pos] + replacement + content[end_pos:], start_pos


if len(sys.argv) < 2:
    sys.exit("Usage: python3 generate-config.py h|c|conf|server ...")

with open(CONFIGS_PATH, "r") as file:
    cfg_map: list[str | dict] = json.load(file)

# Configuration file.
confs: list[str] = []

# H header file.
defines: list[str] = []
names: list[str] = []
structs: list[str] = []

# C file.
init_configs: list[str] = []
free_configs: list[str] = []
db_loads: list[str] = []
env_loads: list[str] = []

# Server file.
logs: list[str] = []

struct_count = sum(isinstance(item, dict) for item in cfg_map)
structs.append(f"char config_status[{struct_count}];\n")
init_configs.append("// Initialize to defaults.")
init_configs.append(
    f"memset(config->config_status, WRD_DEFAULT_CONFIG, {struct_count});\n"
)

# Loop configs.json.
for value in cfg_map:
    if isinstance(value, str):
        comment = f"// {value}"

        # Configuration file.
        confs.append(comment)

        # h file.
        defines.append(f"\n{comment}")
        structs.append(comment)

        # c file.
        init_configs.append(comment)
        free_configs.append(comment)
        env_loads.append(comment)

        continue

    if not isinstance(value, dict):
        continue

    if "comment" in value:
        confs.append(f"// {value['comment']}")

    entry_name = value["name"]
    entry_value = value["default"]
    enum_values = value.get("enum", [])
    is_int = isinstance(entry_value, int) and value.get("type") != "mixed"
    define = f"WRD_DEFAULT_{entry_name.upper()}"
    define_name = f"WRD_{entry_name.upper()}"

    if is_int:
        entry_type = "int "
        env_function = "LOAD_ENV_CONFIG_INT"
    else:
        entry_type = "char *"
        env_function = "LOAD_ENV_CONFIG_STRING"

    # Configuration file.
    confs.append(f"{entry_name}: {json.dumps(entry_value, ensure_ascii=False)},")

    c_entry_value = value_to_c(entry_value, is_int)

    # h file.
    if enum_values:
        entry_type = f"wrd_{entry_name}"
        enums = [f"WRD_{entry_name.upper()}_{e.upper()}" for e in enum_values]

        defines.append("typedef enum {")
        defines.append(",\n".join(add_indent(enums)))
        defines.append(f"}} {entry_type};\n")
        defines.append(f"#define {define} {enums[entry_value]}")
        entry_type += " "
    else:
        defines.append(f"#define {define} {c_entry_value}")

    structs.append(f"{entry_type}{entry_name};")
    names.append(f"{define_name},")

    # c file.
    init_configs.append(f"config->{entry_name} = {define};")
    config_name = " ".join(s.capitalize() for s in entry_name.split("_"))

    if is_int:
        db_loads.append(f"LOAD_DB_CONFIG_INT({define_name}, {entry_name})")
        log_format = "%d"
    else:
        db_loads.append(f"LOAD_DB_CONFIG_STR({define_name}, {entry_name})")
        free_configs.append(f"FREE_CONFIG_STRING({define_name}, {entry_name})")
        log_format = "%s"

    logs.append(
        f'wrd_log_info(config, u8"{config_name}: {log_format} [%c]\\n", '
        f"config->{entry_name}, wrd_get_config_status(config, "
        f"{define_name}));"
    )

    env_loads.append(f"{env_function}({define_name}, {entry_name})")

defines = (
    ["// Configuration names.", "typedef enum {"]
    + add_indent(names)
    + ["} wrd_config_name;"]
    + defines
)

files: dict[str, dict] = {
    "h": {
        "file": "src/warudo.h",
        "start": "// Configurations.",
        "end": "\n// End Configurations.",
        "text": "\n".join(defines) + "\n",
        "additionals": [
            {
                "end": "\n    // End warudo configurations.",
                "text": "\n" + "\n".join(add_indent(structs)) + "\n",
            },
        ],
    },
    "c": {
        "file": "src/conf.c",
        "start": "// Configurations.",
        "end": "\n    return WRD_OK;",
        "text": "\n" + "\n".join(add_indent(init_configs)) + "\n",
        "additionals": [
            {
                "end": "\n    return WRD_OK;",
                "text": "\n" + "\n".join(add_indent(env_loads)) + "\n",
            },
            {
                "end": "\n    return WRD_OK;",
                "text": "\n" + "\n".join(add_indent(free_configs)) + "\n",
            },
            {
                "end": "\n    wrd_load_config_env",
                "text": "\n" + "\n".join(add_indent(db_loads)) + "\n",
            },
        ],
    },
    "conf": {
        "file": "warudo.conf.default",
        "start": "{",
        "end": "}",
        "text": "\n".join(add_indent(confs)) + "\n",
        "additionals": [],
    },
    "server": {
        "file": "src/server.c",
        "start": "// Configurations.",
        "end": "ret = wrd_db_init(config->db_path, config);",
        "text": "\n".join(add_indent(logs)) + "\n\n    ",
        "additionals": [],
    },
}

for arg in sys.argv[1:]:
    if arg not in files:
        sys.exit(f"Unknown target: {arg}. Valid targets: {', '.join(files)}")

    f = files[arg]

    with open(f["file"], "r") as file:
        content = file.read()

    text, start_pos = generate_content(content, f["start"], f["end"], f["text"])

    for additional in f["additionals"]:
        text, start_pos = generate_content(
            text, f["start"], additional["end"], additional["text"], start_pos
        )

    with open(f["file"], "w") as file:
        file.write(text)

    print(f"Generated {f['file']}")
