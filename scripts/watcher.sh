#!/bin/bash

COMMAND=${1}

PLATFORM="javascript"

ROOT="integrations/${PLATFORM}"

KAPI_BUILD="mkdir -p build && cd build && cmake .. && make"
KAPI_BIN="build/kapi"
KAPI_INPUT="examples/playground.kapi"
KAPI_OUTPUT="${ROOT}/.kapi/${PLATFORM}.ast"

COLORED_CAT="bat -p -pp -m '*.ast:JSON'"

if [[ "$COMMAND" = "kapi-input" ]]; then
  bash -c "reset && cat ${KAPI_INPUT}"
  fswatch -0 -o $KAPI_INPUT | xargs -0 -n1 -I{} bash -c "reset && cat ${KAPI_INPUT}"
fi

if [[ "$COMMAND" = "kapi-output" ]]; then
  bash -c "reset && ${COLORED_CAT} ${KAPI_OUTPUT}"
  fswatch -0 -o $KAPI_OUTPUT | xargs -0 -n1 -I{} bash -c "reset && ${COLORED_CAT} ${KAPI_OUTPUT}"
fi

if [[ "$COMMAND" = "js-output" ]]; then
  bash -c "reset && ${COLORED_CAT} ${ROOT}/out/index.js"
  fswatch -0 -or ${ROOT}/out | xargs -0 -n1 -I{} bash -c "reset && ${COLORED_CAT} ${ROOT}/out/index.js"
fi

if [[ "$COMMAND" = "kapi-build" ]]; then
  bash -c "reset && $KAPI_BUILD"
  fswatch -0 -or src -o CMakeLists.txt | xargs -0 -n1 -I{} bash -c "reset && $KAPI_BUILD"
fi

if [[ "$COMMAND" = "js-generate" ]]; then
  bash -c "reset && cd ${ROOT} && pnpm rollup -c"
  fswatch -0 -o $KAPI_BIN -o $KAPI_INPUT -o $ROOT/rollup.config.mjs | xargs -0 -n1 -I{} bash -c "reset && cd ${ROOT} && pnpm rollup -c"
fi
