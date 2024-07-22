import { Enum } from "@kapi-lang/std";

/*

Input:

struct Data;

enum Result {
  Ok(Int, Data),
  Err(Int, String),
}

Output:
*/
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
