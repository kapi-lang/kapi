# Control Flow


## Loops

### For
```
var arr: Int[] = [1, 2, 3];

for (item in arr) {
  print("Value: " + String(item));
}

# Value: 1
# Value: 2
# Value: 3

for (item in 0 to 3) {
  print("Value: " + String(item));
}

# Value: 1
# Value: 2
# Value: 3

for (item in 3 downto 0) {
  print("Value: " + String(item));
}

# Value: 3
# Value: 2
# Value: 1
```

### While

```
var i: Int = 10;

while (i > 0) {
  i--;
}
```

## Conditional Statements

### If

```
var n: Int = 10;

if (n eq 10) {
  # ...
} else if (n gt 100) {
  # ...
} else {
  # ...
}
```
