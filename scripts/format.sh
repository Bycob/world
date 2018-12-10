#!/bin/bash

current_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cd $current_dir/..

clang-format -i -style=file projects/world/*/*
