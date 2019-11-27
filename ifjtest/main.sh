#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

run_test() {
    echo \# \# \# "$filename" \# \# \#
    cat ifj19.py "$filename" > temp.src
    if [ -f $(basename $filename .src).in ]; then
        fileIn=< $(basename $filename .src).in
    fi
    ./../bin/main < $filename > temp.code

    code=$?

    expCode=0
    if [ "$(head -n 1 $filename | cut -c 1-2)" == "##" ]; then
        expCode=$(head -n 1 $filename | cut -c 3-)
    fi

    if [ $expCode == $code ]; then
        printf "${GREEN}RETURN CODE OK${NC}\n"
    else
        printf "${RED}RETURN CODE BAD${NC}\n"
        echo "EXPECTED $expCode WAS $code"
        exit 1
    fi

    if [ "$code" == "0" ]; then
        python3 temp.src $fileIn > vzor.out
        ./ic19int temp.code $fileIn > test.out
        diff=$(diff -s vzor.out test.out)

        if [ "$diff" == "Files vzor.out and test.out are identical" ]; then
            printf "${GREEN}DIFF OK${NC}\n"
        else
            printf "${RED}DIFF BAD${NC}\n"
            echo $diff
            exit 1
        fi
    fi

    echo
}

make main -C ../

if [ $1 != "" ]; then
    filename=$1
    run_test
else
    for filename in tests/*.src; do
        run_test
    done
fi