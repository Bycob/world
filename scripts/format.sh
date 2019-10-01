#!/bin/bash

current_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cd $current_dir/..

# TODO replace these commands with a "find"-based one
clang-format -i -style=file projects/world/*/*.h projects/world/*/*.cpp projects/world/*/*.inl
clang-format -i -style=file projects/vkworld/*.h projects/vkworld/*.cpp projects/vkworld/*/*.h projects/vkworld/*/*.cpp
clang-format -i -style=file projects/world3D/*.cpp projects/world3D/*.h
clang-format -i -style=file projects/worldgui/*.cpp projects/worldgui/*.h
clang-format -i -style=file tests/*.cpp tests/world/*.cpp tests/peace/*.cpp
