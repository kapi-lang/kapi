# String

## Single-ling

```
var text: String = "Hello, World!";
```

## Multi-line

```
var text: String = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Duis consectetur elementum volutpat. Sed efficitur neque sollicitudin leo laoreet, et dignissim ex lobortis. Phasellus non hendrerit elit. Curabitur scelerisque libero tempus mi tristique fermentum. Curabitur in orci non arcu tempus ullamcorper eget efficitur arcu. Phasellus in neque elit. Mauris porta id lorem molestie aliquet. Suspendisse ullamcorper ac enim id varius. Vestibulum magna quam, cursus ac bibendum non, vehicula ac nulla. Nullam justo libero, placerat eget vulputate eu, hendrerit efficitur est. Donec non libero blandit, efficitur erat id, semper leo. Maecenas libero erat, posuere in diam eu, pellentesque laoreet tellus. In hac habitasse platea dictumst.

Nunc blandit ex sit amet urna pharetra tincidunt. Fusce sit amet erat quis nulla semper imperdiet. Aenean egestas, lacus vel rhoncus finibus, risus ante finibus elit, et ultrices dui sem sed tortor. Donec dolor velit, malesuada vitae tortor ac, varius porta quam. Aliquam et felis in felis aliquam pulvinar. Nam nibh massa, fermentum eget auctor quis, posuere eget quam. Ut sit amet orci consequat, tincidunt mauris id, molestie quam. Pellentesque sit amet nibh sem. Donec non elit at tortor fringilla pharetra.";
```

## Mutability
```
var text: String = "Hello";

text += ", World!";
```

## Concatenation

```
var name: String = "World";

var text: String = "Hello, " + name + "!";
```

## Comparision

```
var a: String = "text";
var b: String = "text";

if (a eq b) {
  print("same value!");
}
```
