from sys import argv
import utils as ut

if len(argv) < 2:
    print("Usage: python3 generate-config.py h|c|conf")
    exit(1)

type Configuration = list[str | map]

cfg_map: Configuration = ut.get_data_from_json_file("tools/configs.json")

# Configuration file.
confs: list[str] = []

# H header file.
defines: list[str] = []
names: list[str] = []
statuses: list[str] = []
structs: list[str] = []

# C file.
init_configs: list[str] = []
free_configs: list[str] = []
db_loads: list[str] = []
env_loads: list[str] = []

# Server file.
logs: list[str] = []

struct_count = sum(isinstance(item, dict) for item in cfg_map)
statuses.append("// Configuration statuses.")
structs.append(f"char config_status[{struct_count}];\n")
init_configs.append("// Initialize to defaults.")
init_configs.append(
    f"memset(config->config_status, WRD_DEFAULT_CONFIG, {struct_count});\n"
)

# Loop configs.json.
for value in cfg_map:
    comment = None

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
        confs.append(f"// {value["comment"]}")

    entry_name = value["name"]
    entry_value = value["default"]
    enum_values = value.get("enum", [])
    is_int = isinstance(entry_value, int) and not value.get("type") == "mixed"
    define_name = entry_name.upper()
    define = f"WRD_DEFAULT_{define_name}"
    has_enum = len(enum_values)

    if is_int:
        entry_type = "int "
        env_function = "LOAD_ENV_CONFIG_INT"
        env_cast = "(int*)"
    else:
        entry_type = "char *"
        env_function = "LOAD_ENV_CONFIG_STRING"
        env_cast = ""

    # Configuration file.
    conf_entry_value = ut.value_to_conf(entry_value)
    confs.append(f"{entry_name}: {conf_entry_value},")

    # An enum.
    if has_enum:
        entry_type = f"wrd_{entry_name}"
        defines.append("typedef enum {")
        enums = []

        for enum_value in enum_values:
            enums.append(f"WRD_{entry_name.upper()}_{enum_value.upper()}")

        defines.append(",\n".join(ut.add_indent(enums)))
        defines.append(f"}} {entry_type};\n")
        entry_type += " "

    c_entry_value = ut.value_to_c(entry_value, is_int)
    define_name = f"WRD_{define_name}"

    # h file.
    if has_enum:
        defines.append(f"#define {define} {enums[c_entry_value]}")
    else:
        defines.append(f"#define {define} {c_entry_value}")

    structs.append(f"{entry_type}{entry_name};")
    names.append(f"{define_name},")

    # c file.
    init_configs.append(f"config->{entry_name} = {define};")
    config_name = entry_name.split("_")
    config_name = [s.capitalize() for s in config_name]
    config_name = " ".join(config_name)

    if is_int:
        db_loads.append(f"LOAD_DB_CONFIG_INT({define_name}, {entry_name})")
        logs.append(
            f'wrd_log_info(config, u8"{config_name}: %d [%c]\\n", '
            f"config->{entry_name}, wrd_get_config_status(config, "
            f"{define_name}));"
        )
    else:
        db_loads.append(f"LOAD_DB_CONFIG_STR({define_name}, {entry_name})")
        logs.append(
            f'wrd_log_info(config, u8"{config_name}: %s [%c]\\n", '
            f"config->{entry_name}, wrd_get_config_status(config, "
            f"{define_name}));"
        )

    if not is_int:
        free_statement = f"FREE_CONFIG_STRING({define_name}, {entry_name})"
        free_configs.append(free_statement)

    env_loads.append(f"{env_function}({define_name}, {entry_name})")

defines: list[str] = (
    ["// Configuration names.", "typedef enum {"]
    + ut.add_indent(names)
    + ["} wrd_config_name;"]
    + defines
)

type File = dict[str, str | list[dict[str, str]]]

files: File = {
    "h": {
        "file": "src/warudo.h",
        "start": "// Configurations.",
        "end": "\n#include <",
        "text": "\n".join(defines) + "\n",
        "additionals": [
            {
                "end": "\n    // Network",
                "text": "\n" + "\n".join(ut.add_indent(structs)) + "\n",
            },
        ],
    },
    "c": {
        "file": "src/conf.c",
        "start": "// Configurations.",
        "end": "\n    return WRD_OK;",
        "text": "\n" + "\n".join(ut.add_indent(init_configs)) + "\n",
        "additionals": [
            {
                "end": "\n    return WRD_OK;",
                "text": "\n" + "\n".join(ut.add_indent(env_loads)) + "\n",
            },
            {
                "end": "\n    return WRD_OK;",
                "text": "\n" + "\n".join(ut.add_indent(free_configs)) + "\n",
            },
            {
                "end": "\n    wrd_load_config_env",
                "text": "\n" + "\n".join(ut.add_indent(db_loads)) + "\n",
            },
        ],
    },
    "conf": {
        "file": "warudo.conf.default",
        "start": "{",
        "end": "}",
        "text": "\n".join(ut.add_indent(confs)) + "\n",
        "additionals": [],
    },
    "server": {
        "file": "src/server.c",
        "start": "// Configurations.",
        "end": "ret = wrd_db_init(config->db_path, config);",
        "text": "\n".join(ut.add_indent(logs)) + "\n\n    ",
        "additionals": [],
    },
}

for i, arg in enumerate(argv[1:]):
    f = files[arg]

    with open(f["file"], "r") as file:
        content = file.read()

    file.close()
    res = ut.generate_content(content, f["start"], f["end"], f["text"])

    for additional in f["additionals"]:
        res = ut.generate_content(
            res["text"],
            f["start"],
            additional["end"],
            additional["text"],
            res["start_pos"],
        )

    with open(f["file"], "w") as file:
        file.write(res["text"])

    print(f"Generated {f['file']}")
