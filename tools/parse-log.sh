#!/bin/bash
# cat file.log | ./tools/parse-log.sh --debug

current_dir="$(dirname "$0")"
php_script="$current_dir/parse-log.php"
// warudo_cli="$current_dir/../bin/warudo"
warudo_cli="$current_dir/../build/cli/warudo-cli"

# Check if a log file name was provided, or use "-" as the default
if [ $# -eq 0 ]; then
    argument="-"
else
    argument="$1"
fi

pwd

# Call the PHP script
php "$php_script" "-" | $warudo_cli "$@"
