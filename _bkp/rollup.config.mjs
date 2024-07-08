import * as util from 'node:util';
import { defineConfig } from 'rollup';
import { js_from, lexer_get_tokens, parser_ast_from, typecheck_from } from './src/index.mjs';
// import { nodeResolve } from '@rollup/plugin-node-resolve';
// import commonjs from '@rollup/plugin-commonjs';

export default defineConfig({
  input: process.env.INPUT,
  output: {
    format: 'esm',
    generatedCode: 'es5',
    dir: 'out',
  },
  treeshake: false,
  plugins: [
    equal_script(),
  ]
});

/**
* @return {import('rollup').Plugin}
*/
function equal_script() {

  const debug_options = {
    colors: true,
    compact: false,
    numericSeparator: true,
    depth: Infinity,
    maxArrayLength: Infinity,
    maxStringLength: Infinity,
  };

  return {
    name: "equal-script",
    transform(source, id) {
      try {

        const tokens = lexer_get_tokens(source);

        console.log('='.repeat(process.stdout.columns));
        console.log('Tokens:', util.inspect(tokens, debug_options));
        console.log('='.repeat(process.stdout.columns));

        const ast = parser_ast_from(tokens);

        console.log('='.repeat(process.stdout.columns));
        console.log('AST:', util.inspect(ast, debug_options));
        console.log('='.repeat(process.stdout.columns));

        const errors = typecheck_from(ast);

        for (const error of errors) throw error;

        const code = js_from(ast);

        console.log('='.repeat(process.stdout.columns));
        console.log(code);
        console.log('='.repeat(process.stdout.columns));

        return code;
      } catch (error) {
        if (error.line !== undefined)
          this.error(error, { line: error.line, column: error.column });
        else
          this.error(error);
      }
    }
  };
}
