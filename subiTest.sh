#!/usr/bin/bash

test_failed=0;
for i in {1..4095}; do
    sed -i "s/subi r0, .*/subi r0, $i/" ./test-suite/subi.tk
    ./build.sh
    outputa=$(./assembler ./test-suite/subi.tk)
    outputs=$(./simulator ./test-suite/subi.tko)

    if [[ $outputs != *"-$i"* ]]; then
        test_failed=1
        echo "Test failed on i=$i"
        exit 1
    fi 
done
if [[ $test_failed == "0" ]]; then
    echo "Test success"
else 
    echo "Test failed"
fi


