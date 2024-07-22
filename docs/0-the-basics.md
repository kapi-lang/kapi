# The Basics

## Semicolons

The semicolon(`;`) after each statement is required.

## Parentheses & Braces

The parentheses(`(` and `)`) and braces(`{` and `}`) are required.

## Comments

Comments are ignored when the code is compiled and are single-line, starting with a hash(`#`) character.

## Empty Value

Empty value is defined with the keyword `nil`, instead of `undefined` or `null` in javascript.

## Types

- `Int` is an integer value
- `Float` is a floating-point value
- `Bool` is a boolean value
- `String` is a string value

## Number

The number types is a [double-precision 64-bit binary format IEEE 754](https://en.wikipedia.org/wiki/Double_precision_floating-point_format). The types `Int` and `Float` is just for improve the readability and maintenability.

- The minimum is `-9007199254740991` (-9,007,199,254,740,991, or about -9 quadrillion).
- The maximum is `+9007199254740991` (9,007,199,254,740,991, or ~9 quadrillion).

## Numeric Literals

The underscore `_` is usable to improve the number readability.

```
var one_million = 1_000_000;

var just_over_one_million = 1_000_000.000_000_1;
```

## Integer Conversion

This convertion accepts:

- boolean
- float
- string
- `nil`

```
var value_integer: Int = Int(true); # 1

var value_integer: Int = Int(false); # 0

var value_integer: Int = Int(1.2); # 1

var value_integer: Int = Int("123"); # 123

var value_integer: Int = Int(nil); # 0

```

## Float Conversion

This convertion accepts:

- boolean
- integer
- string
- `nil`

```
var value_float: Float = Float(true); # 1.0

var value_float: Float = Float(false); # 0.0

var value_float: Float = Float(1); # 1.0

var value_float: Float = Float("123"); # 123.0

var value_float: Float = Float(nil); # 0.0
```

## Boolean Conversion

This convertion accepts:

- integer
- float
- string
- `nil`

```
var value_bool: Bool = Bool(1); # true

var value_bool: Bool = Bool(0); # false

var value_bool: Bool = Bool(1.0); # true

var value_bool: Bool = Bool(0.0); # false

var value_bool: Bool = Bool("Hello, World!"); # true

var value_bool: Bool = Bool(""); # false

var value_bool: Bool = Bool(nil); # false
```

## String Conversion

This convertion accepts:

- integer
- float
- boolean
- `nil`

```
var value_string: String = String(1); # "1"

var value_string: String = String(1_000); # "1000"

var value_string: String = String(1.5); # "1.5"

var value_string: String = String(true); # "true"

var value_string: String = String(false); # "false"

var value_string: String = String(nil); # "nil"
```

## Declaring variables

When declare a variable is necessary define the type and value.

```
var value_integer: Int = 1;

var value_float: Float = 1.5;

var value_boolean: Bool = true;

var value_string: String = "Hello, World!";
```

But, is possible too create a variable without a value using the optional operator, now the variable could be `nil` or the type defined.

```
var value_integer: Int?;

var value_float: Float?;

var value_boolean: Bool?;

var value_string: String?;
```
