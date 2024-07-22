export const Enum = {
  is(value, enumerator) {
    if (typeof enumerator === 'function') {
      return value.$typeof === enumerator;
    }

    return value === enumerator;
  },
  value(value, at) {
    if (typeof value === "object") {
      return value['arg' + at];
    }

    return value;
  }
};
