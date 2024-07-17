# Concurrency


### Defining and Calling a Task

```
struct Data {
  message: String;
}

enum Result {
  Ok(Int, Data),
  Error(Int, String)
}

async fn do_request(String url) Result {
  if (Task.cancelled) {
    return Result.Error(0, "Cancelled");
  }

  var data: Data = {
    message = "Success",
  };

  return Result.Ok(200, data);
}

var result: Task<Result> = do_request("url");

var result: Result = await do_request("url");

if (Enum.is(result, Result.Ok)) {
  print(status);
}
```

### Waiting multiple Tasks

```
var result_1: Task<Result> = do_request("url-1");
var result_2: Task<Result> = do_request("url-2");
var result_3: Task<Result> = do_request("url-3");

var results: Result[] = await [result_1, result_2, result_3];
```

### Cancelling Task

```
var result: Task<Result> = do_request("url");

Task.cancel(result);
```
