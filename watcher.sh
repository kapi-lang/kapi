#!/bin/bash

COMMAND=${1}

BUILD_COMMAND="mkdir -p build && cd build && cmake .. && make"
RUN_COMMAND="build/js playground.eq"

if [[ "$COMMAND" = "build" ]]; then
  # while sleep 0.3; do
  ls src/*.c src/*.h build/compile_commands.json | entr bash -c "reset && $BUILD_COMMAND"
  # done
fi

if [[ "$COMMAND" = "run" ]]; then
  # while sleep 0.3; do
  ls playground.eq build/js | entr bash -c "reset && $RUN_COMMAND"
  # done
fi
