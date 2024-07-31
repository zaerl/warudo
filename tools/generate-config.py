from sys import argv
import utils as ut

if len(argv) < 2:
    print("Usage: python3 generate-config.py h|c|conf")
    exit(1)

# List of configurations.
map = [
    "Database",
    ["db_path", "file:warudo.db"],
    [
        "log_level",
        0,
        ["no_log", "info", "error", "debug"],
        "Log level can be one of the following [0, 1, 2, 3]: no_log, info, "
        "error, debug",
    ],
    "Net",
    ["access_origin", None],
    ["listen_backlog", 1024],
    ["max_columns", 64],
    ["net_buffer_size", 1],
    ["net_headers_buffer_size", 4096],
    ["net_input_buffer_size", 1],
    ["socket_port", 6251],
    ["timing", 1],
    "Server",
    ["worker_processes", "auto"],
]

# Configuration file.
confs = []

# H header file.
defines = []
names = []
statuses = []
structs = []

# C file.
init_configs = []
free_configs = []
db_loads = []
env_loads = []

# Server file.
logs = []

struct_count = sum(isinstance(item, list) for item in map)
statuses.append("// Configuration statuses.")
structs.append(f"char config_status[{struct_count}];")
structs.append("")
init_configs.append("// Initialize to defaults.")
init_configs.append(
    f"memset(config->config_status, WRD_DEFAULT_CONFIG, {struct_count});\n"
)

for value in map:
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

    if not isinstance(value, list) or not len(value):
        continue

    if len(value) == 4:
        confs.append(f"// {value[3]}")

    entry_name = ut.safe_get(value, 0)
    entry_value = ut.safe_get(value, 1)
    enum_values = ut.safe_get(value, 2, [])
    is_int = isinstance(entry_value, int)
    define_name = entry_name.upper()
    define = f"WRD_DEFAULT_{define_name}"

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
    if len(enum_values):
        entry_type = f"wrd_{entry_name}"
        defines.append("typedef enum {")
        enums = []

        for enum_value in enum_values:
            enums.append(f"WRD_{entry_name.upper()}_{enum_value.upper()}")

        defines.append(",\n".join(ut.add_indentation(enums)))
        defines.append(f"}} {entry_type};\n")
        entry_type += " "

    c_entry_value = ut.value_to_c(entry_value)
    define_name = f"WRD_{define_name}"

    # h file.
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

defines = (
    ["// Configuration names.", "typedef enum {"]
    + ut.add_indentation(names)
    + ["} wrd_config_name;"]
    + defines
)

files = {
    "h": {
        "file": "src/warudo.h",
        "start": "// Configurations.",
        "end": "\n#include <",
        "text": "\n".join(defines) + "\n",
        "additionals": [
            {
                "end": "\n    // Network",
                "text": "\n" + "\n".join(ut.add_indentation(structs)) + "\n",
            },
        ],
    },
    "c": {
        "file": "src/conf.c",
        "start": "// Configurations.",
        "end": "\n    return WRD_OK;",
        "text": "\n" + "\n".join(ut.add_indentation(init_configs)) + "\n",
        "additionals": [
            {
                "end": "\n    return WRD_OK;",
                "text": "\n" + "\n".join(ut.add_indentation(env_loads)) + "\n",
            },
            {
                "end": "\n    return WRD_OK;",
                "text": "\n" + "\n".join(ut.add_indentation(free_configs)) + "\n",
            },
            {
                "end": "\n    wrd_load_config_env",
                "text": "\n" + "\n".join(ut.add_indentation(db_loads)) + "\n",
            },
        ],
    },
    "conf": {
        "file": "warudo.conf.default",
        "start": "{",
        "end": "}",
        "text": "\n".join(ut.add_indentation(confs)) + "\n",
        "additionals": [],
    },
    "server": {
        "file": "src/server.c",
        "start": "// Configurations.",
        "end": "ret = wrd_db_init(config->db_path, config);",
        "text": "\n".join(ut.add_indentation(logs)) + "\n\n    ",
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
