# New Programming Langue

## TODO

### Expressions

- ObjectExpression
- ArrayExpression
- MemberExpression
- UpdateExpression ( a++ or ++a or --a or a--)
- LogicalExpression ( && or || ) => and, or
- SequenceExpression ( Expression,Expression )

### Statements

- EmptyStatement ( <empty line>; )
- BlockStatement

Control Flow

- ReturnStatement
- BreakStatement
- ContinueStatement

Choice

- IfStatement
- SwitchStatement

Loop

- DoWhileStatement
- WhileStatement
- ForOfStatement
- ForStatement

Exceptions

- ThrowStatement
- TryStatement

## Objective

- Output: Javascript
- Types: Strong
- Complexity: Less than Typescript

## Definitions

### Primitives

#### Nil

javacript output: `null` or `undefined`

#### Integer or Float

javacript output: `number` or `BigInt`

#### Boolean

javacript output: `boolean`

### Struct

input:

```
struct Person {
  name: String;
  age: Integer;
}

var person = Person {
  name = "Equal",
  age = 29,
};
```

javacript output:

```
var person = {
  name = "Equal",
  age = 29,
};
```

### Function

input:

```
void greet(Person person) {
  print(person.name + ", " + person.age);
}

void greet(String person) {
  print(person);
}

Integer sum(Integer a, Integer b) {
  return a + b;
}

Float sum(Float a, Float b) {
  return a + b;
}
```

javacript output:

```
function greet(person) {
  if (typeof person === 'string')
    console.log(person);
  else
    console.log(person.name + ", " + person.age);
}

function sum(a, b) {
  return a + a;
}
```

### Control Flow

input:

```
if (1 eq 1) {
 // true
} else {
  // false
}

if (1 not 2) {
 // true
}

if (2 gt 1) {
 // true
}

if (2 gte 1) {
 // true
}

if (1 lt 2) {
 // true
}

if (1 lte 2) {
 // true
}

if ((1 eq 1) and (true eq true)) {
 // true
}

if ((1 eq 1) or (true eq true)) {
 // true
}

```

javacript output:

```
if (1 === 1) {
 // true
} else {
  // false
}

if (1 !== 2) {
 // true
}

if (2 > 1) {
 // true
}

if (2 >= 1) {
 // true
}

if (1 < 2) {
 // true
}

if (1 <= 2) {
 // true
}

if ((1 === 1) && (true === true)) {
 // true
}

if ((1 === 1) || (true === true)) {
 // true
}
```
