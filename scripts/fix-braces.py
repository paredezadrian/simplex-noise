#!/usr/bin/env python3
"""
Systematic fix for braces around single-line if statements
"""

import re


def fix_braces_in_file(filename):
    with open(filename, "r") as f:
        content = f.read()

    # Pattern to match single-line if statements without braces
    # Matches: if (condition)\n        statement;
    pattern = r"if\s*\([^)]+\)\s*\n\s+([^{][^;]*;)\s*\n"

    def add_braces(match):
        condition = match.group(0).split("\n")[0]
        statement = match.group(1).strip()
        return f"{condition} {{\n        {statement}\n    }}\n"

    # Apply the fix
    new_content = re.sub(pattern, add_braces, content)

    # Write back if changed
    if new_content != content:
        with open(filename, "w") as f:
            f.write(new_content)
        print(f"Fixed braces in {filename}")
        return True
    return False


if __name__ == "__main__":
    files = [
        "src/simplex_noise.c",
        "src/simplex_image.c",
        "tests/test_basic.c",
        "tests/test_config.c",
        "tests/test_performance.c",
        "examples/example_2d.c",
        "examples/example_3d.c",
        "examples/example_config.c",
        "examples/example_fractal.c",
        "examples/example_image.c",
    ]

    fixed_count = 0
    for file in files:
        try:
            if fix_braces_in_file(file):
                fixed_count += 1
        except FileNotFoundError:
            print(f"File {file} not found, skipping")

    print(f"Fixed braces in {fixed_count} files")
