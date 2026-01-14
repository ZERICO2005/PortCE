We welcome contributions to PortCE.

Most of the coding style is similar to the CE C/C++ toolchain.

# Style guide

- C/C++: space indentation, 4 spaces per indent
- assembly: tab indentation, 8 spaces per indent
- Try to keep things to less than 120 columns

Function formatting:
```c
int func(int a, int b, int c);

int func(
    int a,
    int b,
    int c
);
```

Please avoid this:
```c
int func(int a,
         int b,
         int c);
```

# Pull Requests:

To minimize whitespace in Pull Requests:
- Trim trailing whitespace
- LF `\n` line endings
- Files end with exactly one newline
