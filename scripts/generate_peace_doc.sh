#!/bin/bash

current_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd ${current_dir}/../docs/peace/

pandoc terrain-system.md --latex-engine=xelatex --from=markdown --to=latex -o "${current_dir}/../projects/peace/Assets/Peace/documentation.pdf"

