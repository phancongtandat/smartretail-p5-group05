#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p bin
${CXX:-g++} -std=c++17 -Wall -Wextra -Werror src/main.cpp -o bin/p5
python3 tests/verify.py
