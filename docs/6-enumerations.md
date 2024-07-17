# Enumerations

## Numeric

```
enum Types {
  A,
  B,
  C,
}

Enum.value(Types.A);   # 1
Enum.value(Types.B);   # 2
Enum.value(Types.C);   # 3
```

## String

```
enum Types {
  A = "A",
  B = "B",
  C = "C",
}

Enum.value(Types.A);   # "A"
Enum.value(Types.B);   # "B"
Enum.value(Types.C);   # "C"
```

## With Arguments

```
struct Data {
  message: String;
}

enum Result {
  Ok(Int, Data),
  Error(Int, String)
}

var ok: Result = Result.Ok(200, data);

var error: Result = Result.Error(500, "error message");

Enum.value(ok, 0); # 200

Enum.value(ok, 1); # data

Enum.value(error, 0); # 500

Enum.value(error, 1); # error message
```

## Comparision


### Numeric

```
enum Types {
  A,
  B,
  C,
}

var option: Types = Types.A;

Enum.is(option, Types.A);   # true

Enum.is(option, Types.B);   # false
```

### With Arguments

```
enum Result {
  Ok(Int),
  Error(Int)
}

var option: Result = Result.Ok(200);

Enum.is(option, Result.Ok);      # true

Enum.is(option, Result.Error);   # false
```
