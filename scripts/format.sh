#!/bin/bash

current_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cd $current_dir/..

paths=("projects/world" "projects/vkworld" "projects/world3D" "projects/worldgui" "projects/peace/csrc" "tests")
files=$(find "${paths[@]}" \( -name "*.cpp" -o -name "*.h" -o -name "*.inl" \))
clang-format -i -style file ${files[@]}
