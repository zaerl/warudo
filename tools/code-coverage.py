import functools
import re
import subprocess

type Coverage = dict[str, int]


def run_process(cmd: str) -> str:
    output = subprocess.run(cmd, shell=True, capture_output=True, text=True)

    if output.returncode != 0:
        print(f"Error running: {cmd}")

    return output.stdout


def comparator(a: str, b: str) -> int:
    if coverage[a] and coverage[b] or not coverage[a] and not coverage[b]:
        if a < b:
            return -1
        elif a > b:
            return 1
        else:
            return 0

    if coverage[a] and not coverage[b]:
        return -1

    if not coverage[a] and coverage[b]:
        return 1


def valid_input_line(index: int, lines: list[str]) -> bool:
    line = lines[index]

    return not (
        line == ""
        or line.startswith("Match #")
        or line.startswith("Binding for")
        or index == len(lines) - 2
    )


def analyze_output(output: str, verbose=False) -> Coverage:
    lines = output.split("\n")
    pattern = r"\b(\w+)\s+\*?\b(\w+)\s*\("
    coverage = {}

    for i, line in enumerate(lines):
        if valid_input_line(i, lines):
            match = re.search(pattern, line)

            if match:
                if verbose:
                    print(f"{line}\n    > {match.group(2)}")

                coverage[match.group(2)] = 0
            else:
                if verbose:
                    print(line)

    return coverage


def analyze_test(output: str, coverage: Coverage, verbose=False) -> Coverage:
    lines = output.split("\n")
    pattern = r"\_Generic\(\(0\s,\s(.+)\),\schar: att_assert_c"

    for i, line in enumerate(lines):
        if valid_input_line(i, lines):
            match = re.search(pattern, line)

            if not match:
                continue

            if verbose:
                print(match.group(1))

            match_2 = re.search(r"([a-z0-9_]+)\(.*\)", match.group(1))

            if match_2:
                if verbose:
                    print(f"    > {match_2.group(1)}")

                if match_2.group(1) not in coverage:
                    coverage[match_2.group(1)] = 0

                coverage[match_2.group(1)] += 1

    return coverage


output = run_process("clang-query -f tools/src-query.txt ./src/*.h")
coverage = analyze_output(output)

output = run_process("clang-query -f tools/test-query.txt ./test/*.c")
coverage = analyze_test(output, coverage)

total = 0
max_name_length = 0

for key, value in coverage.items():
    total += value
    max_name_length = max(max_name_length, len(key))


sorted_keys = sorted(coverage.keys(), key=functools.cmp_to_key(comparator))
coverage = {key: coverage[key] for key in sorted_keys}
bar = "-" * max_name_length
post = " " * (max_name_length - 5)
post_2 = " " * 5

with open("TESTS.md", "w") as f:
    print("# Test coverage\n", file=f)
    print(f"| Test {post} | Coverage |", file=f)
    print(f"| {bar} | -------- |", file=f)

    for key, value in coverage.items():
        post = " " * (max_name_length - len(key))
        print(f"| {key}{post} | {value}", end="", file=f)

        post = " " * (8 - len(str(value)))
        print(f"{post} |", file=f)

    post = " " * (max_name_length - 6)
    print(f"| Total {post} | {total}", end="", file=f)

    post = " " * (8 - len(str(total)))
    print(f"{post} |", file=f)
