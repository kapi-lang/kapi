import * as util from 'node:util';
import { TokenType } from "./token.mjs";
import { error_create } from "./tools.mjs";

let types = {};
let declarations = {};
let errors = [];

function typecheck_error(msg, token) {
  return error_create(`[typecheck] ${msg}`, token);
}

/**
 * @typedef {import("./parser.mjs").Statement} Stmt
 * @typedef {Stmt['type']} StmtType
 *
 * @typedef {Partial<{[P in StmtType]: (stmt: Extract<Stmt, {type: P}>) => void}>} Visitor
 *
 * @param {import("./parser.mjs").Statement | import("./parser.mjs").Statement[]} stmt
 * @param {Visitor} map
 */
function traverse(stmt, map) {
  if (Array.isArray(stmt)) {
    for (const item of stmt) {
      traverse(item, map);
    }

    return;
  }

  const visitor = map[stmt.type];

  if (!visitor) return;

  switch (stmt.type) {
    case 'VariableDeclaration': {
      visitor(stmt);
      break;
    }
    case 'FunctionDeclaration': {
      visitor(stmt);
      break;
    }
    case 'LiteralExpression': {
      visitor(stmt);
      break;
    }
    case 'UnaryExpression': {
      visitor(stmt);
      break;
    }
    case 'BinaryExpression': {
      visitor(stmt);
      break;
    }
    case 'VariableExpression': {
      visitor(stmt);
      break;
    }
    case 'AssignmentExpression': {
      visitor(stmt);
      break;
    }
    case 'ObjectExpression': {
      visitor(stmt);
      break;
    }
    case 'GroupExpression': {
      visitor(stmt);
      break;
    }
    case 'CallExpression': {
      visitor(stmt);
      break;
    }
  }

}

export function typecheck_from(ast) {
  types = {}, declarations = {}, errors = [];

  for (const type of ast.types) {
    if (type.name.lexeme in types) {
      typecheck_add_error(`can't re-declare the type '${type.name.lexeme}'`, type.name);
    } else {
      types[type.name.lexeme] = type;
    }
  }

  const is_numeric = (v) => v === TokenType.integer || v === TokenType.float;

  const assignment_visitor = (variable, is_declaration = false) => {
    /** @type {Visitor} */
    const visitor = {
      LiteralExpression(literal) {
        if (literal.kind === TokenType.nil) {
          typecheck_add_error(`can't assign '${literal.kind}' to a variable`, variable.name);

          return;
        }

        if (variable.name.lexeme in declarations) {
          if (is_numeric(declarations[variable.name.lexeme]) && !is_numeric(literal.kind)) {
            typecheck_add_error(`can't create a binary expression with '${declarations[variable.name.lexeme]}' and '${literal.kind}'`, variable.name);
          }

          return;
        };

        declarations[variable.name.lexeme] = literal.kind;
      },
      BinaryExpression(bin) {
        traverse(bin.left, {
          LiteralExpression: visitor.LiteralExpression,
          VariableExpression({ name }) {
            if (!(name.lexeme in declarations)) {
              typecheck_add_error(`can't assing undeclared variable '${name.lexeme}'`, name);

              return;
            }

            if (variable.name.lexeme in declarations) {
              if (is_declaration && variable.name.lexeme === name.lexeme) {
                typecheck_add_error(`can't re-declare a variable '${variable.name.lexeme}'`, variable.name);

                return;
              }
              return;
            };

            declarations[variable.name.lexeme] = declarations[name.lexeme];
          },
          BinaryExpression: visitor.BinaryExpression,
        });

        traverse(bin.right, visitor);
      },
      ObjectExpression(obj) {
        if (obj.kind.lexeme in types) {
          const struct = types[obj.kind.lexeme];

          const checked_prop = [];
          for (const prop of obj.properties) {

            const attribute = struct.attributes.find((attr) => attr.name.lexeme === prop.key.lexeme);

            if (attribute)
              traverse(prop.value, {
                LiteralExpression(literal) {
                  if (literal.kind !== attribute.type) {
                    typecheck_add_error(`invalid type`, literal.value);
                  }
                }
              });


            checked_prop.push(prop.key.lexeme);
          }

          const missing_props = obj.properties
            .filter((prop) => !checked_prop.includes(prop.key.lexeme))
            .map((prop) => prop.key.lexeme).join(', ');

          if (missing_props.length > 0) {
            typecheck_add_error(`missing attribute(s) '${missing_props}' of type(${obj.kind.lexeme})`, variable.name);
          }

          return;
        }

        typecheck_add_error(`can't assign a variable with an undeclared type(${obj.kind.lexeme})`, variable.name);
      }
    };

    return visitor;
  };

  traverse(ast.statements, {
    AssignmentExpression(assign) {
      if (!(assign.left.lexeme in declarations)) {
        typecheck_add_error(`can't assing undeclared variable '${assign.left.lexeme}'`, assign.left);

        return;
      }


      traverse(assign.right, assignment_visitor({ name: assign.left }));
    },
    VariableDeclaration(variable) {
      if (variable.name.lexeme in declarations) {
        typecheck_add_error(`can't re-declare a variable '${variable.name.lexeme}'`, variable.name);

        return;
      }

      traverse(variable.init, assignment_visitor(variable, true));
    },
  });


  console.log('='.repeat(process.stdout.columns));
  console.log('Type-Checker:', util.inspect({
    types,
    declarations,
  }, {
    colors: true,
    compact: false,
    numericSeparator: true,
    depth: Infinity,
    maxArrayLength: Infinity,
    maxStringLength: Infinity,
  }));
  console.log('='.repeat(process.stdout.columns));

  // for (const statement of ast.statements) {
  //   if (statement.kind === 'variable') variables[statement.name] = statement;
  //   if (statement.kind === 'call') calls.push(statement);
  // }

  // for (const variable_name in variables) {
  //   const variable = variables[variable_name];
  //   const type = types[variable.type];

  //   switch (variable.type) {
  //     case TokenType.integer:
  //     case TokenType.float:
  //     case TokenType.bool:
  //     case TokenType.string:
  //       continue;
  //     case TokenType.nil:
  //       typecheck_add_error(`variable can't be initialized with 'nil'`, variable.token);
  //       continue;
  //   }

  //   let filled_attr = [];
  //   for (const attr of variable.value) {
  //     const ref = type.attributes.find((t_attr) => t_attr.name === attr.name);

  //     if (!ref) {
  //       typecheck_add_error(`invalid attribute(${attr.name}) on type(${type.name})`, attr.token);
  //     }

  //     filled_attr.push(attr.name);

  //     switch (ref.type) {
  //       case TokenType.integer:
  //       case TokenType.float:
  //         if (typeof attr.value !== 'number') {
  //           typecheck_add_error(`invalid value for '${attr.name}', expected '${ref.type}'`, attr.token);
  //         }
  //         break;
  //       case TokenType.bool:
  //         if (typeof attr.value !== 'boolean')
  //           typecheck_add_error(`invalid value for '${attr.name}', expected '${ref.type}'`, attr.token);
  //         break;
  //       case TokenType.string:
  //         if (typeof attr.value !== 'string')
  //           if (ref.optional) {
  //             if (attr.value !== null)
  //               typecheck_add_error(`invalid value for '${attr.name}', expected '${ref.type}' or 'nil'`, attr.token);
  //           }
  //           else {
  //             typecheck_add_error(`invalid value for '${attr.name}', expected '${ref.type}'`, attr.token);
  //           }
  //         break;
  //     }
  //   }

  //   const missing_attr = type.attributes.filter((attr) => !filled_attr.includes(attr.name)).map((attr) => attr.name);

  //   if (missing_attr.length > 0)
  //     typecheck_add_error(`missing attributes '${missing_attr}' on type(${type.name})`, variable.token);
  // }

  return errors;
}

function typecheck_add_error(msg, token) {
  errors.push(error_create(msg, token));
}
