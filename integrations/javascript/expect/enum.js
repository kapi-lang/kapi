const Enum = {
  is(value, enumerator) {
    if (typeof enumerator === 'function')
      return value.$typeof === enumerator;

    return value === enumerator;
  },
  value(value, at) {
    if (typeof value === "object")
      return value['arg' + at];

    return value;
  }
};

const Result = {
  Ok(arg0, arg1) {
    return {
      $typeof: Result.Ok,
      arg0,
      arg1,
    };
  },
  Error(arg0, arg1) {
    return {
      $typeof: Result.Error,
      arg0,
      arg1,
    };
  }
};

var option = Result.Ok(200, "Success");

console.log(
  Enum.is(option, Result.Ok),

  Enum.value(option, 0),

  Enum.value(option, 1),
);
