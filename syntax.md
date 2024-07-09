Rascunho explicação:

```
====
C -> C++
JS -> EQ -> TS
====

Dores:
- Typescript muito complexo
- Typescript é burlável (any everywhere)
- TS ou JS build para versões antigas add muito polyfill
- Polyfill não é performático
- Core-JS não é otimizado para build (treeshake)
- Bundle de 1 MB+
- Muito disposito com browser antigo (Safari 5, Opera 9 ...)

Possíveis soluções
- Core-JS com build otimizado (ESM code)
- Processo de build mais inteligente no Polyfill
  - Core-JS de muitos níveis (Polyfill Base, Polyill Base + allSettled)
- Criar uma linguagem que gera JS otimizado (podendo incluir polyfill ou não)

// Input
[].forEach((item, index)=> {

})

// Babel Input: for (const item of array)
// Output
for (var i = 0; i < arr.length); i++) {

}
```

# Language Definitions

## TODO

### Type-Checker

### Expressions

- ArrayExpression
- MemberExpression
- SequenceExpression ( Expression,Expression )

### Statements

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
