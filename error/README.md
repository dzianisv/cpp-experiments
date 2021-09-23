```sh
cmake -B .build .
cmake --build .build/
.build/test
```

```
.build/test
benchmark_dynamic_cast: 153104 uS, 0.0510347 uS
benchmark_typeinfo: 130062 uS, 0.043354 uS
```