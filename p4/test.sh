#!/bin/bash
# Tests
./booga_load
cat /proc/driver/booga

./test-booga 0 64 read
./test-booga 0 64 write
./test-booga 1 64 read
./test-booga 1 64 write
./test-booga 2 64 read
./test-booga 2 64 write
./test-booga 3 64 read
./test-booga 3 64 write

cat /proc/driver/booga
./booga_unload
