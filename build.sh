#!/bin/bash
set echo on

mkdir -p bin

# Get a list of all the .cpp files.
cFilenames=$(find . -type f -name "*.cpp")

# echo "Files:" $cFilenames

assembly="presto"
compilerFlags="-g" 
includeFlags="-Isrc"
linkerFlags="-lstdc++ -lxcb"
defines=""
#defines="-DIMPORT"

echo "Building $assembly..."
echo clang $cFilenames $compilerFlags -o bin/$assembly $defines $includeFlags $linkerFlags
clang $cFilenames $compilerFlags -o bin/$assembly $defines $includeFlags $linkerFlags
