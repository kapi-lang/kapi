#!/bin/bash

COMMAND=${1}

C_BUILD="mkdir -p build && cd build && cmake .. && make"
C_PROGRAM="build/kapi javascript examples/playground.kapi"
BUNDLER_INPUT="cat examples/playground.kapi"
BUNDLER_BUILD="cd integrations/javascript && pnpm rollup -c"
BUNDLER_OUTPUT="cat integrations/javascript/out/index.js"

if [[ "$COMMAND" = "c-run" ]]; then
  fswatch -0 -o examples/playground.kapi -o build/kapi | xargs -0 -n1 -I{} bash -c "reset && $C_PROGRAM"
fi

if [[ "$COMMAND" = "c-build" ]]; then
  fswatch -0 -or src | xargs -0 -n1 -I{} bash -c "reset && $C_BUILD"
fi

if [[ "$COMMAND" = "js-generate" ]]; then
  fswatch -0 -o .kapi/javascript.ast.json | xargs -0 -n1 -I{} bash -c "reset && $BUNDLER_BUILD"
fi

if [[ "$COMMAND" = "js-output" ]]; then
  fswatch -0 -or integrations/javascript/out | xargs -0 -n1 -I{} bash -c "reset && $BUNDLER_OUTPUT"
fi

if [[ "$COMMAND" = "eq-input" ]]; then
  fswatch -0 -o examples/playground.kapi | xargs -0 -n1 -I{} bash -c "reset && $BUNDLER_INPUT"
fi
