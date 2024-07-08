/**
 * @typedef {{
 *   type: 'CallExpression';
 *   name: import('./token.mjs').Token;
 *   args: Expression[];
 * }} CallExpression
 */

/**
 * @typedef {{
 *   type: 'GroupExpression';
 *   expression: Expression;
 * }} GroupExpression
 */

/**
 * @typedef {{ key: import('./token.mjs').Token, value: Expression; }} ObjectProperty
 *
 * @typedef {{
 *   type: 'ObjectExpression';
 *   kind: import('./token.mjs').Token;
 *   properties: ObjectProperty[];
 * }} ObjectExpression
 */

/**
 * @typedef {{
 *   type: 'VariableExpression';
 *   name: import('./token.mjs').Token;
 * }} VariableExpression
 */

/**
 * @typedef {{
 *   type: 'AssignmentExpression';
 *   left: import('./token.mjs').Token;
 *   operator: import('./token.mjs').Token;
 *   right: Expression;
 * }} AssignmentExpression
 */

/**
 *
 * @typedef {keyof typeof import('./token.mjs').TokenType} TokenKeys
 * @typedef {typeof import('./token.mjs').TokenType[TokenKeys]} TokenValues
 *
 * @typedef {{
 *   type: 'LiteralExpression';
 *   kind: TokenValues;
 *   value: import('./token.mjs').Token;
 * }} LiteralExpression
 */

/**
 * @typedef {{
 *   type: 'UnaryExpression';
 *   operator: import('./token.mjs').Token;
 *   right: import('./token.mjs').Token;
 * }} UnaryExpression
 */

/**
 * @typedef {{
 *   type: 'BinaryExpression';
 *   left: Expression;
 *   operator: import('./token.mjs').Token;
 *   right: Expression;
 * }} BinaryExpression
 */

/**
 * @typedef { LiteralExpression | UnaryExpression | BinaryExpression | VariableExpression | AssignmentExpression | ObjectExpression | GroupExpression | CallExpression } Expression
 */

/**
 * @template {Expression['type']} T
 * @template {Extract<Expression, { type: T }>} Return
 * @param {T} type
 * @param {*} options
 * @returns {Return}
 */
export function expression_create(type, ...options) {
  if (type === 'LiteralExpression') {
    return /** @type {Return} */({
      type,
      kind: options[0],
      value: options[1],
    });
  }

  if (type === 'UnaryExpression') {
    return /** @type {Return} */({
      type,
      operator: options[0],
      right: options[1],
    });
  }

  if (type === 'BinaryExpression' || type === 'AssignmentExpression') {
    return /** @type {Return} */({
      type,
      left: options[0],
      operator: options[1],
      right: options[2],
    });
  }

  if (type === 'VariableExpression') {
    return /** @type {Return} */({
      type,
      name: options[0],
    });
  }

  if (type === 'ObjectExpression') {
    return /** @type {Return} */({
      type,
      kind: options[0],
      properties: options[1],
    });
  }

  if (type === 'GroupExpression') {
    return /** @type {Return} */({
      type,
      expression: options[0],
    });
  }

  if (type === 'CallExpression') {
    return /** @type {Return} */({
      type,
      name: options[0],
      args: options[1],
    });
  }

  throw new Error('invalid type of expression (' + type + ')');
}
