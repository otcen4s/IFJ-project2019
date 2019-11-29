#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

run_test() {
    echo \# \# \# "$filename" \# \# \#
    cat ifj19.py "$filename" > temp.src
    ./../bin/main < "$filename" > temp.code

    code=$?

    expCode=0
    if [ "$(head -n 1 "$filename" | cut -c 1-2)" == "##" ]; then
        expCode=$(head -n 1 "$filename" | cut -c 3-)
    fi

    if [ "$code" == "0" ]; then

        inFile="${filename%.*}.in"
        if [ -f "$inFile" ]; then
            ./ic19int temp.code < "$inFile" > test.out
            code=$?
        else
            ./ic19int temp.code > test.out
            code=$?
        fi

        if [ "$expCode" == "$code" ]; then
            printf "${GREEN}RETURN CODE OK${NC}\n"
        else
            printf "${RED}RETURN CODE BAD${NC}\n"
            echo "EXPECTED $expCode WAS $code"
            exit 1
        fi

        if [ "$code" == "0" ]; then
            if [ -f "$inFile" ]; then
                python3 temp.src < "$inFile" > vzor.out
                code=$?
            else
                python3 temp.src > vzor.out
                code=$?
            fi

            diff=$(diff -s vzor.out test.out)

            if [ "$diff" == "Files vzor.out and test.out are identical" ]; then
                printf "${GREEN}DIFF OK${NC}\n"
            else
                printf "${RED}DIFF BAD${NC}\n"
                diff vzor.out test.out
                exit 1
            fi
        fi

    else
        if [ "$expCode" == "$code" ]; then
            printf "${GREEN}RETURN CODE OK${NC}\n"
        else
            printf "${RED}RETURN CODE BAD${NC}\n"
            echo "EXPECTED $expCode WAS $code"
            exit 1
        fi
    fi

    echo
}

make main -C ../

if [ "$1" != "" ]; then
    if [ "${1: -1}" == "/" ]; then
        for filename in "$1"*.src; do
            run_test
        done
    else
        filename=$1
        run_test
    fi
else
    for filename in tests/*.src; do
        run_test
    done
fi