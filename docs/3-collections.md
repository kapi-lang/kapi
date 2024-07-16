# Collections

## List

```
var arr: Int[] = [1, 2, 3];
```

### Add item

```
var arr: Int[] = [1, 2, 3];
var value: Int = 4;

List.add(arr, value); # [1, 2, 3, 4]
```

### Add item at

```
var arr: Int[] = [1, 2, 3];
var index: Int = 0;
var value: Int = 0;

List.add(arr, value, index); # [0, 1, 2, 3]
```

### Remove item

```
var arr: Int[] = [1, 2, 3];
var index: Int = 0;

List.remove(arr, index); # [2, 3]
```

### Accessing

```
var arr: Int[] = [1, 2, 3];

List.at(arr, 0); # 1
List.at(arr, -1); # 3
```

## Dictionary

```
var dict: Dict = {
  key = "value"
};
```

### Keys

```
var dict: Dict = {
  key = "value"
};

Dict.keys(dict); # ["key"]
```

### Values

```
var dict: Dict = {
  key = "value"
};

Dict.values(dict); # ["value"]
```

### Get

```
var dict: Dict = {
  key = "value"
};

Dict.get(dict, "key"); # "value"
```

### Set

```
var dict: Dict = {
  key = "value"
};

Dict.set(dict, "key", "new-value");

Dict.get(dict, "key"); # "new-value"
```

### Set

```
var dict: Dict = {
  key = "value"
};

Dict.remove(dict, "key");

Dict.keys(dict); # []
```
