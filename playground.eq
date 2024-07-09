struct Person {
  title?: String;
  name: String;
  age: Integer;
}

var a = Person {
  title = nil,
  name = "Equal",
  age = 29,
};

--a;
++a;
a--;
a++;

; # empty statement

{
  var a = 1;

  {
    var a = 1;
  }

}
