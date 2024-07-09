import generate from "@babel/generator";
import * as t from "@babel/types";
import { TokenType } from "./token.mjs";

function variable_declaration(name, value) {
  return t.variableDeclaration('var', [
    t.variableDeclarator(
      t.identifier(name),
      expression(value),
    )
  ]);
}

function expression(expr) {
  if (expr.type === 'UnaryExpression')
    return t.unaryExpression(
      expr.operator.lexeme,
      expression(expr.right),
      true,
    );

  if (expr.type === 'BinaryExpression')
    return t.binaryExpression(
      expr.operator.lexeme,
      expression(expr.left),
      expression(expr.right),
    );

  if (expr.type === 'AssignmentExpression')
    return t.expressionStatement(
      t.assignmentExpression(
        expr.operator.lexeme,
        t.identifier(expr.left.lexeme),
        expression(expr.right),
      )
    );

  if (expr.type === 'LiteralExpression')
    return literal_expression(expr);

  if (expr.type === 'ObjectExpression') {
    return t.objectExpression(
      expr.properties.map((prop) =>
        t.objectProperty(
          t.identifier(prop.key.lexeme),
          expression(prop.value),
        )
      )
    );
  }

  if (expr.type === 'VariableExpression')
    return t.identifier(expr.name.lexeme);

  if (expr.type === 'GroupExpression')
    return t.parenthesizedExpression(expression(expr.expression));

  if (expr.type === 'CallExpression')
    return t.callExpression(
      expr.name.lexeme === 'print' ?
        t.memberExpression(t.identifier('console'), t.identifier('log'))
        : t.identifier(expr.name.lexeme),
      expr.args.map(expression)
    );

  throw new Error('unknown expression(' + expr.type + ')');
}

function literal_expression(literal) {
  switch (literal.kind) {
    case TokenType.integer:
    case TokenType.float:
      return t.numericLiteral(literal.value.literal);

    case TokenType.bool:
      return t.booleanLiteral(literal.value.literal);

    case TokenType.string:
      return t.stringLiteral(literal.value.literal);

    default:
      return t.buildUndefinedNode();
  }
}

export function js_from(ast) {
  /** @type {t.Statement[]} */
  const body = [];

  for (const stmt of ast.statements) {
    switch (stmt.type) {
      case 'VariableDeclaration': {
        body.push(variable_declaration(stmt.name.lexeme, stmt.init));
        break;
      }
      case 'FunctionDeclaration': {
        break;
      }
      default: {
        body.push(expression(stmt));
        break;
      }
    }
  }

  const program = t.program(body);

  t.addComment(program, 'inner', ` gid: ${Math.floor(Math.random() * 1_000_000)} `);

  // @ts-ignore
  const file = generate.default(t.file(program));

  return file;
}
