import { defineConfig } from 'rollup';
import generate from '@babel/generator';
import * as path from "node:path";
import { spawnSync } from 'node:child_process';
import { cwd, stdout } from 'node:process';
// import postcss from "postcss";
// import cssnano from "cssnano";
// import autoprefixer from "autoprefixer";

const ROOT = path.join(cwd(), '..', '..');

export default defineConfig({
  input: {
    index: path.join(ROOT, 'examples', 'playground.kapi'),
  },
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
        const kapi = spawnSync(path.join(ROOT, 'build', 'kapi'), ['javascript', id]);

        const kapi_stdout = kapi.stdout.toString('utf-8')
          .split(/(\r?\n)/g);

        const ast_text = kapi_stdout
          .find((content) => content.startsWith('{'));

        if (!ast_text) {
          this.error(new Error(kapi.stderr.toString('utf-8')));

          return "";
        }

        const ast = JSON.parse(ast_text);

        const file = generate.default(ast);

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


// const css_ast = postcss.parse(`
//   ::placeholder {
//     color: gray;
//   }

//   #root {
//     color: red;
//   }
// `);

// const css_processor = postcss()
//   .use(autoprefixer)
//   .use(cssnano);


// let css_result = '';

// postcss.stringify(css_ast, (chunk, node, type) => {
//   css_result += chunk;
// });

// const r = await css_processor.process(css_result, {
//   from: undefined,
// });

// console.log(r.css);
