[![Build Status](https://travis-ci.org/zjhmale/lc3.svg?branch=master)](https://travis-ci.org/zjhmale/lc3)

## Build, Test and Run

```bash
./run.sh
```

## Turing complete

```rust
let factorial = fn (n) {
  if (n > 0) {
    return n * factorial(n - 1);
  } else {
    return 1;
  }
};
```

## Meta programming

```rust
let unless = macro(condition, consequence, alternative) {
  quote(if (!(unquote(condition))) {
          unquote(consequence);
        } else {
          unquote(alternative);
        });
};
```
