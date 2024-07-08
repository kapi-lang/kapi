/**
 * @typedef {{
 *   type: 'VariableDeclaration';
 *   name: import('./token.mjs').Token;
 *   init: import('./expression.mjs').Expression;
 * }} VariableDeclaration
 */

/**
 * @typedef {{
 *   type: 'FunctionDeclaration';
 *   name: import('./token.mjs').Token;
 *   body: import('./expression.mjs').Expression[];
 * }} FunctionDeclaration
 */

/**
 * @typedef { VariableDeclaration | FunctionDeclaration } Declaration
 */

/**
 * @template {Declaration['type']} T
 * @template {Extract<Declaration, { type: T }>} Return
 * @param {T} type
 * @param {import('./token.mjs').Token} name
 * @param {*} data
 * @returns {Return}
 */
export function declaration_create(type, name, data) {
  if (type === 'VariableDeclaration') {
    return /** @type {Return} */({
      type,
      name,
      init: data,
    });
  }

  if (type === 'FunctionDeclaration') {
    return /** @type {Return} */({
      type,
      name,
      body: data,
    });
  }

  throw new Error('invalid type of declaration');
}
