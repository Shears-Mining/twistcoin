#!/usr/bin/env bash
# Creates twistcoin-named symlinks next to the compiled bitcoin* binaries,
# without touching the build system or source code that produces them
# (deliberately - see CLAUDE.md at the project workspace root for why the
# underlying targets are still named bitcoind/bitcoin-cli/etc.).
#
# Run after building, e.g.:
#   cmake -B build -DBUILD_GUI=OFF -DWITH_ZMQ=ON -DENABLE_WALLET=ON
#   cmake --build build
#   contrib/rename-binaries.sh
#
# Usage: contrib/rename-binaries.sh [build-dir]   (default: build)

set -euo pipefail

build_dir="${1:-build}"
bin_dir="$build_dir/bin"

if [ ! -d "$bin_dir" ]; then
    echo "error: '$bin_dir' not found - build first, or pass the build dir as \$1" >&2
    exit 1
fi

renames=(
    "bitcoind:twistcoind"
    "bitcoin-cli:twistcoin-cli"
    "bitcoin:twistcoin"
    "bitcoin-tx:twistcoin-tx"
    "bitcoin-util:twistcoin-util"
    "bitcoin-wallet:twistcoin-wallet"
    "bitcoin-qt:twistcoin-qt"
)

for pair in "${renames[@]}"; do
    src="${pair%%:*}"
    dst="${pair##*:}"
    if [ -e "$bin_dir/$src" ] && [ ! -e "$bin_dir/$dst" ]; then
        ln -s "$src" "$bin_dir/$dst"
        echo "linked $bin_dir/$dst -> $src"
    fi
done
