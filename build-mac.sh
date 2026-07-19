#!/bin/bash
#
# Build the custom 4coder layer (custom_4coder.so) on macOS.
#
# Usage:
#   ./build-mac.sh <path-to-4coder-custom-dir> [output-dir]
#   FCODER_CUSTOM=<path-to-4coder-custom-dir> ./build-mac.sh
#
# <path-to-4coder-custom-dir> is the `custom` directory of a 4coder source
# tree / distribution (the one that ships 4coder_default_include.cpp,
# 4coder_metadata_generator.cpp and the generated/ headers). See README.md.

set -e

REPO="$(cd "$(dirname "$0")" && pwd)"
SOURCE="$REPO/custom/4coder_config.cpp"

CODE_HOME="${1:-$FCODER_CUSTOM}"
if [ -z "$CODE_HOME" ]; then
    echo "usage: $0 <path-to-4coder/code/custom> [output-dir]"
    echo "   or: FCODER_CUSTOM=<path> $0"
    exit 1
fi
CODE_HOME="$(cd "$CODE_HOME" && pwd)"

OUT="${2:-$REPO/build}"
mkdir -p "$OUT"
cd "$OUT"

opts="-Wno-write-strings -Wno-null-dereference -Wno-comment -Wno-switch -Wno-missing-declarations -Wno-logical-op-parentheses -g -DOS_MAC=1 -DOS_WINDOWS=0 -DOS_LINUX=0"
arch=-m64
preproc=4coder_command_metadata.i

clang++ -I"$CODE_HOME" -DMETA_PASS $arch $opts -std=c++11 "$SOURCE" -E -o "$preproc"
clang++ -I"$CODE_HOME" $opts -std=c++11 "$CODE_HOME/4coder_metadata_generator.cpp" -o ./metadata_generator
./metadata_generator -R "$CODE_HOME" "$OUT/$preproc"
clang++ -I"$CODE_HOME" $arch $opts -std=c++11 "$SOURCE" -shared -o custom_4coder.so -fPIC

rm -f ./metadata_generator "$preproc"
echo "Built $OUT/custom_4coder.so"
