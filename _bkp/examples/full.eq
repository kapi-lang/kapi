var a = 1;
var a2 = 1.2;

if(a eq 1) {
  print("One");
}

if(a2 gt 1) {
  print("One++");
}

print(a + a2);

var b = true;
var b2 = false;

if(b) {
  print("True");
}

if(b2 eq false) {
  print("False");
}

var c = "Equal";
var c2 = nil;

# Type Person
struct Person {
  name: String;
  age: Integer;
  title ?: String;
}

var person: Person {
  name = "Equal",
  age = 29,
}

void person_to_string(person: Person) {
  print("Person{ name: " + person.name + ", age: " + person.age + " }");
}

String person_get_name(person: Person) {
  return person.name;
}

Integer person_get_age(person: Person) {
  return person.age;
}

print(person_get_name(person));

print(person_get_age(person));

print(person_to_string(person));
