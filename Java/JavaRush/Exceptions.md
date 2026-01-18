```ascii
            +----- Throwable -----+
            |                     |
        Error           +---------+--------------+
                        |                        |
                RuntimeExceptions     (other Exceptions)
```
Sintax `try-catch`
```java
try {
    // Code that can fail
    System.out.println(10 / 0);
}
catch (ArithmeticException e) { // Name of the Exception that should be caught
    System.out.println("Short message: " + e.getMessage());
    System.out.println("Long message : " + e.printStackTrace());
}
```
Sintax `try-catch-finally`
```java
try {
    // Code that can fail
    System.out.println(10 / 0);
}
catch (ArithmeticException e) { // Name of the Exception that should be caught
    System.out.println("Short message: " + e.getMessage());
    System.out.println("Long message : " + e.printStackTrace());
}
finally {
    System.out.println("This part will work anyway!");
}
```

## Checked / Unchecked Exceptions
- #### Checked Exceptions:
    `IOException, SQLException` - compiler requires processing or trowing them.
- #### Unchecked Exceptions:
    `NullPointerException, IllegalArgumentException` - compiler does NOT require processing, no need to show in `throws`.

### Mistakes:
- If a method can throw `checked exception`, but you didn't declare it in `throws` and didn't process it with `try-catch` - compiler gives an ERROR.

- If you don't what to do with the exception don't write `catch (Exception e)`. Give it to the next better.

- If your method can only throw `FileNotFoundException` don't write `throws Exception`.

- Throw unchecked exceptions in `throws`. No need to write `throws NullPointerException`, compiler doesn't require this, and it won't help.
iasdfa   asdf[]
