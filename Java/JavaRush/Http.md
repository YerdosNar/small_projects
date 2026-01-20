## HttpClient:
This class supports GET, POST and etc.
Example:
```java
URI uri = URI.create("https://raw.githubusercontent.com/YerdosNar/YerdosNar/master/assets/arch.png");
HttpClient client = HttpClient.newHttpClient();
HttpRequest req = HttpRequest.newBuilder(uri).build();

HttpResponse<byte[]> res = client.send(req, HttpResponse.BodyHandlers.ofByteArray());
Files.write(Path.of("test_file"), res.body());
```
Creating Client `HttpClient`.\
Creating request `HttpRequest`.\
Sending request and receiving byte array `HttpResponse.BodyHandlers.ofByteArray()`.\
Writing that array into `test_file`.

## Checking status:
```java
if (res.statusCode() == 200) {
    Files.write(Path.of("test_file"), res.body());
}
else {
    System.out.println("ERROR: code " + res.statusCode());
}
```
Now we know for sure that we saved the requested file.

## Headers:
```java
String type = res.header().firstValue("Content-Type".orElse("unknown"));
System.out.println("Type of content: " + type);
```

## Timeout:
```java
HttpRequest req = HttpRequest.newBuilder(uri)
                             .timeout(Duration.ofSeconds(5)) // Do not wait indefinitely
                             .build();
```

## Redirects:
```java
HttpClient client = HttpClient.newBuilder()
                              .followRedirects(HttpClient.Redirect.NORMAL)
                              .build();
```
In case URL redirects to somewhere else.

## Checking file size:
To get file size in the header:
```java
String length = res.headers().firstValue("Content-Length").orElse("unknown size");
System.out.println("Size: " + length + "bytes");
```
To get file size in the body:
```java
int length = res.body().length;
```
