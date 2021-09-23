```sh
cmake -B .build .
cmake --build .build/
.build/test
```

Without using `to_err`.
```
benchmark_dynamic_cast: 153104 uS, 0.0510347 uS
benchmark_typeinfo: 130062 uS, 0.043354 uS
```

When `to_err` is in use
```
benchmark_dynamic_cast: 180976 uS, 0.0603253 uS
benchmark_typeinfo: 132591 uS, 0.044197 uS
```