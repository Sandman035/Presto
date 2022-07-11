#!/bin/bash
set echo on

mkdir -p bin

# Get a list of all the .cpp files.
cFilenames=$(find . -type f -name "*.cpp")

# echo "Files:" $cFilenames

assembly="presto"
compilerFlags="" 
includeFlags="-Isrc"
linkerFlags="-lstdc++ -lxcb -lxcb-keysyms -lxcb-randr -lm"
defines=""

echo "Building $assembly..."
echo clang $cFilenames $compilerFlags -o bin/$assembly $defines $includeFlags $linkerFlags
clang $cFilenames $compilerFlags -o bin/$assembly $defines $includeFlags $linkerFlags
