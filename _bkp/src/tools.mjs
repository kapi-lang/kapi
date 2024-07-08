export function is_digit(c) {
  return c >= '0' && c <= '9';
}

export function is_alpha(c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c === '_'
}

export function is_alphanumeric(c) {
  return is_digit(c) || is_alpha(c);
}

export function error_create(msg, { line, column }) {
  const error = /** @type {Error & {line: number; column: number;}} */(new Error(`${msg} at ${line + 1}:${column + 1}`));

  error.line = line + 1;
  error.column = column + 1;
  error.stack = ""

  return error;
}
