# Functions

## Defining and Calling

```
fn greet(String person) String {
  var greeting: String = "Hello, " + person + "!";

  return greeting;
}

greet("World"); # Hello, World!
```

## With Parameters

```
fn greet(String person) String {
  var greeting: String = "Hello, " + person + "!";

  return greeting;
}
```

## Without Parameters

```
fn greet() String {
  var person: String = "World";
  var greeting: String = "Hello, " + person + "!";

  return greeting;
}
```

## Without Return Values

```
fn greet() Void {
  var person: String = "World";
  var greeting: String = "Hello, " + person + "!";

  print(greeting);
}
```
