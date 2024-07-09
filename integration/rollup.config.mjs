import { defineConfig } from 'rollup';
import generate from '@babel/generator';
import * as fs from "node:fs";

export default defineConfig({
  input: "index.js",
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
  return {
    name: "equal-script",
    transform(source, id) {
      try {
        const ast = fs.readFileSync("../ast.json", { encoding: "utf8" });

        const file = generate.default(JSON.parse(ast));

        return file;
      } catch (error) {
        if (error.line !== undefined)
          this.error(error, { line: error.line, column: error.column });
        else
          this.error(error);
      }
    }
  };
}
