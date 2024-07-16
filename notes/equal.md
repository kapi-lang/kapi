# TODO

- Type-Checker

- New Features

  - Type Check
    - `if (a is Bool)`
    - `if (a is String)`
    - `if (a is Int)`
    - `if (a is Float)`
    - `if (a is Number)`
    - `if (a is Person)`
    - `if (a is List)`
    - `if (a is Dict)`
    - `if (a is nil)`
  - ArrayExpression
    - Tuples `[Float, Float]`
    - Type `Person[]` or `Person[][]`
    - Init `[ Person {} ]`
    - `List.add(list, value, at?)`
    - `List.remove(list, at)`
    - `List.at(list, index: posititive or negative)`
  - Dictionary
    - Type `Dict`
    - Init `{ "key" = "value" }`
    - `Dict.keys(dict)`
    - `Dict.values(dict)`
    - `Dict.get(dict, key)`
    - `Dict.set(dict, key, value)`
    - `Dict.remove(dict, key)`
  - Module/Packages (ImportExpression)
  - Async
    - Keywords: `async`, `await`
  - AssertStatement `expect(a eq 1, "Oops!");`
  - Enum
    - (Swift Associated-Values)[https://docs.swift.org/swift-book/documentation/the-swift-programming-language/enumerations#Associated-Values]
    - ? (ReScript Constructor Arguments)[https://rescript-lang.org/docs/manual/latest/variant#constructor-arguments]
  - Function Overloading
  - ThrowStatement
  - TryStatement

  - SwitchStatement (Must be Exaustive or Default Case)
  - ? Constant Values
  - ? Assignment with:
    - BlockStatement + ReturnStatement
    - IfStatement
    - SwitchStatement
  - ? JS Intergration

    - ReScript
      ```rescript
      @val
      external fetchMessage: string => promise<string> = "global.fetchMessage"
      ```
