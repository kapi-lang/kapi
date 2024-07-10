#!/bin/bash

COMMAND=${1}

C_BUILD="mkdir -p build && cd build && cmake .. && make"
C_PROGRAM="build/js playground.eq"
BUNDLER_INPUT="cat playground.eq"
BUNDLER_BUILD="cd integration && pnpm rollup -c"
BUNDLER_OUTPUT="cat integration/out/index.js"

if [[ "$COMMAND" = "c-run" ]]; then
  ls playground.eq build/js | entr bash -c "reset && $C_PROGRAM"
fi

if [[ "$COMMAND" = "c-build" ]]; then
  ls -R src/*.c src/*.h build/compile_commands.json | entr bash -c "reset && $C_BUILD"
fi

if [[ "$COMMAND" = "js-generate" ]]; then
  ls ast.json | entr bash -c "reset && $BUNDLER_BUILD"
fi

if [[ "$COMMAND" = "js-output" ]]; then
  ls integration/out/index.js | entr bash -c "reset && $BUNDLER_OUTPUT"
fi

if [[ "$COMMAND" = "eq-input" ]]; then
  ls playground.eq | entr bash -c "reset && $BUNDLER_INPUT"
fi
