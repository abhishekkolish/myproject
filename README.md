# Library Architecture Scanner

This program scans a folder for `.so` library files and detects their CPU architecture type 
(armeabi-v7a, arm64-v8a, x86, x86-64, mips).

## Build Instructions

```bash
make


Run
./libscanner <folder_path>
Example: ./libscanner sample_libs