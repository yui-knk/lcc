#!/bin/bash

set -x

if [[ -z "$BUILD_DIR" ]]; then
  echo "You should set BUILD_DIR."
  exit 1
fi

cmd="${BUILD_DIR}/lcc"
test_dir="${BUILD_DIR}/test"

mkdir -p "$test_dir"

try() {
  expected="$1"
  input="$2"

  s="${test_dir}/tmp.s"
  e="${test_dir}/tmp"

  "./${cmd}" "$input" > "$s"
  gcc -o "$e" "$s"
  "./${e}"
  actual="$?"

  if [[ "$actual" != "$expected" ]]; then
    echo "$expected expected, but got $actual"
    exit 1
  fi
}

try 0 0
try 42 42

echo "OK"
