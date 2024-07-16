# Basic Operators

## Assignment Operator

```
var a: Int = 1;
var b: Float = 2.5;
```

## Arithmetic Operators

```
1 + 2       # equals 3
5 - 3       # equals 2
2 * 3       # equals 6
10.0 / 2.5  # equals 4.0
```

### String

Using `+` and a `String` value is possible to do a concatenation.

```
"hello, " + "world"  # equals "hello, world"
```

## Bitwise Operators

```
1 ^ 1;
1 << 1;
1 & 1;
1 >> 1;
1 >>> 1;
1 | 1;
```

## Remainder Operator

```
9 % 4    # equals 1
-9 % 4   # equals -1
```

## Unary Operator


### Minus
```
var three: Int = 3
var minusThree: Int = -three       # minusThree equals -3
var plusThree: Int = -minusThree   # plusThree equals 3, or "minus minus three"
```

### Bitwise NOT
```
var value: Int = ~3
```

## Update Operator

```
var i: Int = 3

i++;
++i;

i--;
--i;
```

## Compound Assignment Operators

```
x = 1;
x += 1;
x -= 1;
x *= 1;
x /= 1;
x %= 1;
x **= 1;
x <<= 1;
x >>= 1;
x >>>= 1;
x &= 1;
x ^= 1;
x |= 1;
```

## Comparison Operators

```
1 eq 1   # true because 1 is equal to 1
2 not 1   # true because 2 isn't equal to 1
2 gt 1    # true because 2 is greater than 1
1 lt 2    # true because 1 is less than 2
1 gte 1   # true because 1 is greater than or equal to 1
2 lte 1   # false because 2 isn't less than or equal to 1
```

## Logical Operators

### AND

```
var enteredDoorCode: Bool = true;
var passedRetinaScan: Bool = false;

if (enteredDoorCode and passedRetinaScan) {
  print("Welcome!");
} else {
  print("ACCESS DENIED");
}
```

### OR

```
var hasDoorKey: Bool = false;
var knowsOverridePassword: Bool = true;

if (hasDoorKey or knowsOverridePassword) {
  print("Welcome!");
} else {
  print("ACCESS DENIED");
}
```

### NOT

```
var allowedEntry: Bool = false;

if (not allowedEntry) {
  print("ACCESS DENIED");
}
```

## Explicit Parentheses

Parentheses could be useful to guarentee the expected result of complex expressions.

```
if (
  (enteredDoorCode && passedRetinaScan)
  || hasDoorKey
  || knowsOverridePassword
) {
  print("Welcome!");
} else {
  print("ACCESS DENIED");
}
```
