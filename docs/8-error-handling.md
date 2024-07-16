# Error Handling

## Raise and Capture Exception

```
try {
  raise Exception("Unknown Error");
} catch (Exception ex) {
  print("[UnknownError]: " + ex);
}
```

## Custom Exception

```
exception HttpError(Int, String);

try {
  raise HttpError(500, "Internal Server Error");
}
catch (HttpError ex) {
  print("[HttpError] status(" + String(Exception.get(ex, 0)) + ")");

  print("message: " + Exception.get(ex, 1));
}
catch (Exception ex) {
  print("[UnknownError]: " + ex);
}
```
