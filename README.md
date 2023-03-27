# arithmetic operators

## fixed-point operation
add, sub, mul, div

## bit operation
add, sub, mul, div

## soft float operation
add, sub, mul, div

## bitwise and mod operation
bitwise and/or/xor
remainder

## Build and Run
```bash
cmake -S . -B build
cmake --build build/
cd build/ && ctest
```
## Test precision of different methods
```bash
cd build
cmake -DCMAKE_C_FLAGS="-D TEST_PRECISION" ..
make
./arithmetic_operator
```

```bash
sudo bash env.sh
cd build
cmake -DCMAKE_C_FLAGS="-D TEST_TIME_CONSUMPTION -D BENCHMARK_SUITE_DOUBLE" ..
make
taskset 0x1 perf stat -B ./arithmetic_operator if=/dev/zero of=/dev/null count=1000000

cmake -DCMAKE_C_FLAGS="-D TEST_TIME_CONSUMPTION -D BENCHMARK_SUITE_SOFT_FLOAT" ..
make
taskset 0x1 perf stat -B ./arithmetic_operator if=/dev/zero of=/dev/null count=1000000

cmake -DCMAKE_C_FLAGS="-D TEST_TIME_CONSUMPTION -D BENCHMARK_SUITE_ROUND" ..
make
taskset 0x1 perf stat -B ./arithmetic_operator if=/dev/zero of=/dev/null count=1000000

cmake -DCMAKE_C_FLAGS="-D TEST_TIME_CONSUMPTION -D BENCHMARK_SUITE_FIXEDPOINT" ..
make
taskset 0x1 perf stat -B ./arithmetic_operator if=/dev/zero of=/dev/null count=1000000
```
