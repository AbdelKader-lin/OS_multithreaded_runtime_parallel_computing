#!/bin/sh

clear

echo "Compiling :"
make

echo "Testing with pi.c :"
./tests/pi.run
echo
echo "----------------------------------------"
echo

echo "Testing with simple_tests_outputs.c :"
./tests/simple_test_outputs.run
echo
echo "----------------------------------------"
echo 


echo "Testing with simple_tests.c :"
./tests/simple_test.run
echo
echo "----------------------------------------"
echo 

make $expr clean
